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

#ifndef BOOST_ACTOR_IO_EXPERIMENTAL_TYPED_BROKER_HPP
#define BOOST_ACTOR_IO_EXPERIMENTAL_TYPED_BROKER_HPP

#include <map>
#include <vector>
#include <type_traits>

#include "boost/none.hpp"
#include "boost/actor/config.hpp"
#include "boost/actor/make_counted.hpp"
#include "boost/actor/spawn.hpp"
#include "boost/actor/extend.hpp"
#include "boost/actor/typed_actor.hpp"
#include "boost/actor/local_actor.hpp"

#include "boost/actor/detail/logging.hpp"
#include "boost/actor/detail/singletons.hpp"
#include "boost/actor/detail/scope_guard.hpp"
#include "boost/actor/detail/actor_registry.hpp"
#include "boost/actor/detail/sync_request_bouncer.hpp"

#include "boost/actor/io/middleman.hpp"
#include "boost/actor/io/abstract_broker.hpp"

namespace boost {
namespace actor {
namespace io {
namespace experimental {

using minimal_client = typed_actor<reacts_to<new_data_msg>,
                                   reacts_to<connection_closed_msg>>;

using minimal_server =
  minimal_client::extend<reacts_to<new_connection_msg>,
                         reacts_to<acceptor_closed_msg>>;

template <class... Sigs>
class typed_broker;

/// Infers the appropriate base class for a functor-based typed actor
/// from the result and the first argument of the functor.
template <class Result, class FirstArg>
struct BOOST_ACTOR_DEPRECATED infer_typed_broker_base;

template <class... Sigs, class FirstArg>
struct infer_typed_broker_base<typed_behavior<Sigs...>, FirstArg> {
  using type = typed_broker<Sigs...>;
} BOOST_ACTOR_DEPRECATED;

template <class... Sigs>
struct infer_typed_broker_base<void, typed_broker<Sigs...>*> {
  using type = typed_broker<Sigs...>;
} BOOST_ACTOR_DEPRECATED;

/// A typed broker mediates between actor systems and other components in
/// the network.
/// @extends local_actor
template <class... Sigs>
class typed_broker : public abstract_event_based_actor<typed_behavior<Sigs...>,
                                                       false, abstract_broker> {
public:
  using signatures = detail::type_list<Sigs...>;

  using actor_hdl = typed_actor<Sigs...>;

  using behavior_type = typed_behavior<Sigs...>;

  using super = abstract_event_based_actor<behavior_type, false,
                                           abstract_broker>;

  using super::send;
  using super::delayed_send;

  template <class... DestSigs, class... Ts>
  void send(message_priority mp, const typed_actor<DestSigs...>& dest, Ts&&... xs) {
    detail::sender_signature_checker<
      detail::type_list<Sigs...>,
      detail::type_list<DestSigs...>,
      detail::type_list<
        typename detail::implicit_conversions<
          typename std::decay<Ts>::type
        >::type...
      >
    >::check();
    super::send(mp, dest, std::forward<Ts>(xs)...);
  }

  template <class... DestSigs, class... Ts>
  void send(const typed_actor<DestSigs...>& dest, Ts&&... xs) {
    detail::sender_signature_checker<
      detail::type_list<Sigs...>,
      detail::type_list<DestSigs...>,
      detail::type_list<
        typename detail::implicit_conversions<
          typename std::decay<Ts>::type
        >::type...
      >
    >::check();
    super::send(dest, std::forward<Ts>(xs)...);
  }

  template <class... DestSigs, class... Ts>
  void delayed_send(message_priority mp, const typed_actor<DestSigs...>& dest,
                    const duration& rtime, Ts&&... xs) {
    detail::sender_signature_checker<
      detail::type_list<Sigs...>,
      detail::type_list<DestSigs...>,
      detail::type_list<
        typename detail::implicit_conversions<
          typename std::decay<Ts>::type
        >::type...
      >
    >::check();
    super::delayed_send(mp, dest, rtime, std::forward<Ts>(xs)...);
  }

  template <class... DestSigs, class... Ts>
  void delayed_send(const typed_actor<DestSigs...>& dest,
                    const duration& rtime, Ts&&... xs) {
    detail::sender_signature_checker<
      detail::type_list<Sigs...>,
      detail::type_list<DestSigs...>,
      detail::type_list<
        typename detail::implicit_conversions<
          typename std::decay<Ts>::type
        >::type...
      >
    >::check();
    super::delayed_send(dest, rtime, std::forward<Ts>(xs)...);
  }

  /// @cond PRIVATE
  std::set<std::string> message_types() const override {
    return {Sigs::static_type_name()...};
  }

  void initialize() override {
    this->is_initialized(true);
    auto bhvr = make_behavior();
    BOOST_ACTOR_LOG_DEBUG_IF(! bhvr, "make_behavior() did not return a behavior, "
                            << "has_behavior() = "
                            << std::boolalpha << this->has_behavior());
    if (bhvr) {
      // make_behavior() did return a behavior instead of using become()
      BOOST_ACTOR_LOG_DEBUG("make_behavior() did return a valid behavior");
      this->do_become(std::move(bhvr.unbox()), true);
    }
  }

  template <class F, class... Ts>
  typename infer_handle_from_fun<F>::type
  fork(F fun, connection_handle hdl, Ts&&... xs) {
    auto sptr = this->take(hdl);
    BOOST_ACTOR_ASSERT(sptr->hdl() == hdl);
    using impl = typename infer_handle_from_fun<F>::impl;
    static_assert(std::is_convertible<typename impl::actor_hdl,
                                      minimal_client>::value,
                  "Cannot fork: new broker misses required handlers");
    return spawn_functor_impl<no_spawn_options, impl>(
      nullptr, [&sptr](abstract_broker* forked) {
                 sptr->set_parent(forked);
                 forked->add_scribe(sptr);
               },
      std::move(fun), hdl, std::forward<Ts>(xs)...);
  }

  connection_handle add_tcp_scribe(const std::string& host, uint16_t port) {
    static_assert(std::is_convertible<actor_hdl, minimal_client>::value,
                  "Cannot add scribe: broker misses required handlers");
    return super::add_tcp_scribe(host, port);
  }

  connection_handle add_tcp_scribe(network::native_socket fd) {
    static_assert(std::is_convertible<actor_hdl, minimal_client>::value,
                  "Cannot add scribe: broker misses required handlers");
    return super::add_tcp_scribe(fd);
  }

  std::pair<accept_handle, uint16_t>
  add_tcp_doorman(uint16_t port = 0,
                  const char* in = nullptr,
                  bool reuse_addr = false) {
    static_assert(std::is_convertible<actor_hdl, minimal_server>::value,
                  "Cannot add doorman: broker misses required handlers");
    return super::add_tcp_doorman(port, in, reuse_addr);
  }

  accept_handle add_tcp_doorman(network::native_socket fd) {
    static_assert(std::is_convertible<actor_hdl, minimal_server>::value,
                  "Cannot add doorman: broker misses required handlers");
    return super::add_tcp_doorman(fd);
  }

  typed_broker() {
    // nop
  }

  typed_broker(middleman& parent_ref) : abstract_broker(parent_ref) {
    // nop
  }

protected:
  virtual behavior_type make_behavior() {
    if (this->initial_behavior_fac_) {
      auto bhvr = this->initial_behavior_fac_(this);
      this->initial_behavior_fac_ = nullptr;
      if (bhvr)
        this->do_become(std::move(bhvr), true);
    }
    return behavior_type::make_empty_behavior();
  }
};

} // namespace experimental
} // namespace io
} // namespace actor
} // namespace boost

#endif // BOOST_ACTOR_IO_EXPERIMENTAL_TYPED_BROKER_HPP
