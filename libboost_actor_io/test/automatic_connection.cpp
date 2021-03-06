/******************************************************************************\
 *                                                                            *
 *           ____                  _        _        _                        *
 *          | __ )  ___   ___  ___| |_     / \   ___| |_ ___  _ __            *
 *          |  _ \ / _ \ / _ \/ __| __|   / _ \ / __| __/ _ \| '__|           *
 *          | |_) | (_) | (_) \__ \ |_ _ / ___ \ (__| || (_) | |              *
 *          |____/ \___/ \___/|___/\__(_)_/   \_\___|\__\___/|_|              *
 *                                                                            *
 *                                                                            *
 *                                                                            *
 * Copyright (C) 2011 - 2015                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#include "boost/actor/config.hpp"

#define BOOST_TEST_MODULE io_automatic_connection
#include <boost/test/included/unit_test.hpp>

#include <set>
#include <thread>
#include <vector>

#include "boost/actor/all.hpp"
#include "boost/actor/io/all.hpp"

#include "boost/actor/experimental/whereis.hpp"

#include "boost/actor/io/network/interfaces.hpp"
#include "boost/actor/io/network/test_multiplexer.hpp"

#include "boost/actor/detail/run_sub_unit_test.hpp"

#ifdef BOOST_ACTOR_USE_ASIO
#include "boost/actor/io/network/asio_multiplexer.hpp"
#endif // BOOST_ACTOR_USE_ASIO

using boost::none;
using boost::join;
using boost::variant;
using boost::optional;
using boost::is_any_of;
using boost::token_compress_on;
using namespace boost::actor;
using namespace boost::actor::io;
using namespace boost::actor::experimental;

using std::string;

using ping_atom = atom_constant<atom("ping")>;
using pong_atom = atom_constant<atom("pong")>;

/*

  This test checks whether automatic connections work as expected
  by first connecting three nodes "in line". In step 2, we send a
  message across the line, forcing the nodes to build a mesh. In step 3,
  we disconnect the node that originally connected the other two and expect
  that the other two nodes communicate uninterrupted.

  1) Initial setup:

    Earth ---- Mars ---- Jupiter

  2) After Jupiter has send a message to Earth:

    Earth ---- Mars
       \       /
        \     /
         \   /
        Jupiter

  3) After Earth has received the message and disconnected Mars:

    Earth ---- Jupiter

*/

std::thread run_prog(const char* arg, uint16_t port, bool use_asio) {
  return detail::run_sub_unit_test(invalid_actor,
                                   boost::unit_test::framework::master_test_suite().argv[0],
                                   0 /* max runtime dummy */,
                                   BOOST_PP_STRINGIZE(BOOST_TEST_MODULE),
                                   use_asio,
                                   {"--port=" + std::to_string(port), arg});
}

// we run the same code on all three nodes, a simple ping-pong client
struct testee_state {
  std::set<actor> buddies;
  uint16_t port = 0;
  const char* name = "testee";
};

behavior testee(stateful_actor<testee_state>* self) {
  return {
    [self](ping_atom, actor buddy, bool please_broadcast) -> message {
      if (please_broadcast)
        for (auto& x : self->state.buddies)
          if (x != buddy)
            send_as(buddy, x, ping_atom::value, buddy, false);
      self->state.buddies.emplace(std::move(buddy));
      return make_message(pong_atom::value, self);
    },
    [self](pong_atom, actor buddy) {
      self->state.buddies.emplace(std::move(buddy));
    },
    [self](put_atom, uint16_t new_port) {
      self->state.port = new_port;
    },
    [self](get_atom) {
      return self->state.port;
    }
  };
}

void run_earth(bool use_asio, bool as_server, uint16_t pub_port) {
  scoped_actor self;
  struct captain : hook {
  public:
    captain(actor parent) : parent_(std::move(parent)) {
      // nop
    }

    void new_connection_established_cb(const node_id& node) override {
      anon_send(parent_, put_atom::value, node);
      call_next<hook::new_connection_established>(node);
    }

    void new_remote_actor_cb(const actor_addr& addr) override {
      anon_send(parent_, put_atom::value, addr);
      call_next<hook::new_remote_actor>(addr);
    }

    void connection_lost_cb(const node_id& dest) override {
      anon_send(parent_, delete_atom::value, dest);
    }

  private:
    actor parent_;
  };
  middleman::instance()->add_hook<captain>(self);
  auto aut = spawn(testee);
  auto port = publish(aut, pub_port);
  BOOST_TEST_MESSAGE("published testee at port " << port);
  std::thread mars_process;
  std::thread jupiter_process;
  // launch process for Mars
  if (! as_server) {
    BOOST_TEST_MESSAGE("launch process for Mars");
    mars_process = run_prog("--mars", port, use_asio);
  }
  BOOST_TEST_MESSAGE("wait for Mars to connect");
  node_id mars;
  self->receive(
    [&](put_atom, const node_id& nid) {
      mars = nid;
      BOOST_TEST_MESSAGE(BOOST_ACTOR_TSARG(mars));
    }
  );
  actor_addr mars_addr;
  uint16_t mars_port;
  self->receive_while([&] { return mars_addr == invalid_actor_addr; })(
    [&](put_atom, const actor_addr& addr) {
      auto hdl = actor_cast<actor>(addr);
      self->sync_send(hdl, sys_atom::value, get_atom::value, "info").await(
        [&](ok_atom, const string&, const actor_addr&, const string& name) {
          if (name != "testee")
            return;
          mars_addr = addr;
          BOOST_TEST_MESSAGE(BOOST_ACTOR_TSARG(mars_addr));
          self->sync_send(actor_cast<actor>(mars_addr), get_atom::value).await(
            [&](uint16_t mp) {
              BOOST_TEST_MESSAGE("mars published its actor at port " << mp);
              mars_port = mp;
            }
          );
        }
      );
    }
  );
  // launch process for Jupiter
  if (! as_server) {
    BOOST_TEST_MESSAGE("launch process for Jupiter");
    jupiter_process = run_prog("--jupiter", mars_port, use_asio);
  }
  BOOST_TEST_MESSAGE("wait for Jupiter to connect");
  self->receive(
    [](put_atom, const node_id& jupiter) {
      BOOST_TEST_MESSAGE(BOOST_ACTOR_TSARG(jupiter));
    }
  );
  actor_addr jupiter_addr;
  self->receive_while([&] { return jupiter_addr == invalid_actor_addr; })(
    [&](put_atom, const actor_addr& addr) {
      auto hdl = actor_cast<actor>(addr);
      self->sync_send(hdl, sys_atom::value, get_atom::value, "info").await(
        [&](ok_atom, const string&, const actor_addr&, const string& name) {
          if (name != "testee")
            return;
          jupiter_addr = addr;
          BOOST_TEST_MESSAGE(BOOST_ACTOR_TSARG(jupiter_addr));
        }
      );
    }
  );
  BOOST_TEST_MESSAGE("shutdown Mars");
  anon_send_exit(mars_addr, exit_reason::kill);
  if (mars_process.joinable())
    mars_process.join();
  self->receive(
    [&](delete_atom, const node_id& nid) {
      BOOST_CHECK(nid == mars);
    }
  );
  BOOST_TEST_MESSAGE("check whether we still can talk to Jupiter");
  self->send(aut, ping_atom::value, self, true);
  std::set<actor_addr> found;
  int i = 0;
  self->receive_for(i, 2)(
    [&](pong_atom, const actor&) {
      found.emplace(self->current_sender());
    }
  );
  std::set<actor_addr> expected{aut.address(), jupiter_addr};
  BOOST_CHECK(found == expected);
  BOOST_TEST_MESSAGE("shutdown Jupiter");
  anon_send_exit(jupiter_addr, exit_reason::kill);
  if (jupiter_process.joinable())
    jupiter_process.join();
  anon_send_exit(aut, exit_reason::kill);
}

void run_mars(uint16_t port_to_earth, uint16_t pub_port) {
  auto aut = spawn(testee);
  auto port = publish(aut, pub_port);
  anon_send(aut, put_atom::value, port);
  BOOST_TEST_MESSAGE("published testee at port " << port);
  auto earth = remote_actor("localhost", port_to_earth);
  send_as(aut, earth, ping_atom::value, aut, false);
}

void run_jupiter(uint16_t port_to_mars) {
  auto aut = spawn(testee);
  auto mars = remote_actor("localhost", port_to_mars);
  send_as(aut, mars, ping_atom::value, aut, true);
}

BOOST_AUTO_TEST_CASE(triangle_setup) {
  uint16_t port = 0;
  uint16_t publish_port = 0;
  auto argv = boost::unit_test::framework::master_test_suite().argv;
  auto argc = boost::unit_test::framework::master_test_suite().argc;
  // skip everything in argv until the separator "--" is found
  auto cmp = [](const char* x) { return strcmp(x, "--") == 0; };
  auto argv_end = argv + argc;
  auto argv_sep = std::find_if(argv, argv_end, cmp);
  if (argv_sep == argv_end) {
    argc = 0;
  } else {
    argv = argv_sep + 1;
    argc = std::distance(argv, argv_end);
  }
  auto r = message_builder(argv, argv + argc).extract_opts({
    {"port,p", "port of remote side (when running mars or jupiter)", port},
    {"mars", "run mars"},
    {"jupiter", "run jupiter"},
    {"use-asio", "use ASIO network backend (if available)"},
    {"server,s", "run in server mode (don't run clients)", publish_port}
  });
  // check arguments
  bool is_mars = r.opts.count("mars") > 0;
  bool is_jupiter = r.opts.count("jupiter") > 0;
  bool has_port = r.opts.count("port") > 0;
  if (((is_mars || is_jupiter) && ! has_port) || (is_mars && is_jupiter)) {
    BOOST_ERROR("need a port when running Mars or Jupiter and cannot "
                   "both at the same time");
    return;
  }
  // enable automatic connections
  anon_send(whereis(atom("ConfigServ")), put_atom::value,
            "global.enable-automatic-connections", make_message(true));
  auto use_asio = r.opts.count("use-asio") > 0;
# ifdef BOOST_ACTOR_USE_ASIO
  if (use_asio) {
    BOOST_TEST_MESSAGE("enable ASIO backend");
    set_middleman<network::asio_multiplexer>();
  }
# endif // BOOST_ACTOR_USE_ASIO
  auto as_server = r.opts.count("server") > 0;
  if (is_mars)
    run_mars(port, publish_port);
  else if (is_jupiter)
    run_jupiter(port);
  else
    run_earth(use_asio, as_server, publish_port);
  await_all_actors_done();
  shutdown();
}
