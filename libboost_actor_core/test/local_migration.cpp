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

#define BOOST_TEST_MODULE local_migration
#include <boost/test/included/unit_test.hpp>

#include "boost/actor/all.hpp"

#include "boost/actor/detail/actor_registry.hpp"

using boost::none;
using boost::join;
using boost::variant;
using boost::optional;
using boost::is_any_of;
using boost::token_compress_on;
using namespace boost::actor;

using std::endl;

namespace {

struct migratable_state {
  int value = 0;
  static const char* name;
};

const char* migratable_state::name = "migratable_actor";

template <class Archive>
void serialize(Archive& ar, migratable_state& x, const unsigned int) {
  ar & x.value;
}

struct migratable_actor : stateful_actor<migratable_state> {
  behavior make_behavior() override {
    return {
      [=](get_atom) {
        return state.value;
      },
      [=](put_atom, int value) {
        state.value = value;
      }
    };
  }
};

// always migrates to `dest`
behavior pseudo_mm(event_based_actor* self, const actor& dest) {
  return {
    [=](migrate_atom, const std::string& name, std::vector<char>& buf) {
      BOOST_CHECK(name == "migratable_actor");
      self->delegate(dest, sys_atom::value, migrate_atom::value,
                     std::move(buf));
    }
  };
}

} // namespace <anonymous>

BOOST_AUTO_TEST_CASE(migrate_locally) {
  auto a = spawn<migratable_actor>();
  auto b = spawn<migratable_actor>();
  auto mm1 = spawn(pseudo_mm, b);
  scoped_actor self;
  self->send(a, put_atom::value, 42);
  // migrate from a to b
  self->sync_send(a, sys_atom::value, migrate_atom::value, mm1).await(
    [&](ok_atom, const actor_addr& dest) {
      BOOST_CHECK(dest == b);
    }
  );
  self->sync_send(a, get_atom::value).await(
    [&](int result) {
      BOOST_CHECK(result == 42);
      BOOST_CHECK(self->current_sender() == b.address());
    }
  );
  auto mm2 = spawn(pseudo_mm, a);
  self->send(b, put_atom::value, 23);
  // migrate back from b to a
  self->sync_send(b, sys_atom::value, migrate_atom::value, mm2).await(
    [&](ok_atom, const actor_addr& dest) {
      BOOST_CHECK(dest == a);
    }
  );
  self->sync_send(b, get_atom::value).await(
    [&](int result) {
      BOOST_CHECK(result == 23);
      BOOST_CHECK(self->current_sender() == a.address());
    }
  );
  self->send_exit(a, exit_reason::kill);
  self->send_exit(b, exit_reason::kill);
  self->send_exit(mm1, exit_reason::kill);
  self->send_exit(mm2, exit_reason::kill);
  self->await_all_other_actors_done();
}
