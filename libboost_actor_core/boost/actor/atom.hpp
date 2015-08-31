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

#ifndef BOOST_ACTOR_ATOM_HPP
#define BOOST_ACTOR_ATOM_HPP

#include <string>
#include <functional>
#include <type_traits>

#include "boost/actor/detail/atom_val.hpp"

namespace boost {
namespace actor {

/// The value type of atoms.
enum class atom_value : uint64_t {
  /// @cond PRIVATE
  dirty_little_hack = 31337
  /// @endcond
};

/// Creates an atom from given string literal.
template <size_t Size>
constexpr atom_value atom(char const (&str)[Size]) {
  // last character is the NULL terminator
  static_assert(Size <= 11, "only 10 characters are allowed");
  return static_cast<atom_value>(detail::atom_val(str, 0xF));
}

/// Lifts an `atom_value` to a compile-time constant.
template <atom_value V>
struct atom_constant {
  constexpr atom_constant() {
    // nop
  }
  /// Returns the wrapped value.
  constexpr operator atom_value() const {
    return V;
  }
  /// Returns an instance *of this constant* (*not* an `atom_value`).
  static const atom_constant value;
};

template <atom_value V>
const atom_constant<V> atom_constant<V>::value = atom_constant<V>{};

/// Generic 'ADD' atom for request operations.
using add_atom = atom_constant<atom("ADD")>;

/// Generic 'GET' atom for request operations.
using get_atom = atom_constant<atom("GET")>;

/// Generic 'PUT' atom for request operations.
using put_atom = atom_constant<atom("PUT")>;

/// Generic 'UPDATE' atom, e.g., or signalizing updates in a key-value store.
using update_atom = atom_constant<atom("UPDATE")>;

/// Generic 'DELETE' atom for request operations.
using delete_atom = atom_constant<atom("DELETE")>;

/// Generic 'OK' atom for response messages.
using ok_atom = atom_constant<atom("OK")>;

/// Generic 'ERROR' atom for response messages.
using error_atom = atom_constant<atom("ERROR")>;

/// Marker 'SYS' atom for prefixing messages to a forwarding chain
/// to address an otherwise transparent actor.
using sys_atom = atom_constant<atom("SYS")>;

/// Generic 'JOIN' atom, e.g., for signaling group subscriptions.
using join_atom = atom_constant<atom("JOIN")>;

/// Generic 'LEAVE' atom, e.g., for signaling group unsubscriptions.
using leave_atom = atom_constant<atom("LEAVE")>;

/// Generic 'FORWARD' atom, e.g., for signaling an actor that it
/// should drop the first element and forward the remainder to
/// a list of predefined receivers.
using forward_atom = atom_constant<atom("FORWARD")>;

/// Generic 'FLUSH' atom, e.g., used by `aout`.
using flush_atom = atom_constant<atom("FLUSH")>;

/// Generic 'REDIRECT' atom, e.g., used by `aout`.
using redirect_atom = atom_constant<atom("REDIRECT")>;

/// Generic 'LINK' atom for link requests over network.
using link_atom = atom_constant<atom("LINK")>;

/// Generic 'UNLINK' atom for removing networked links.
using unlink_atom = atom_constant<atom("UNLINK")>;

/// Generic 'PUBLISH' atom, e.g., for publishing actors at a given port.
using publish_atom = atom_constant<atom("PUBLISH")>;

/// Generic 'UNPUBLISH' atom, e.g., for removing an actor/port mapping.
using unpublish_atom = atom_constant<atom("UNPUBLISH")>;

/// Generic 'PUBLISH' atom, e.g., for publishing actors at a given port.
using subscribe_atom = atom_constant<atom("SUBSCRIBE")>;

/// Generic 'UNPUBLISH' atom, e.g., for removing an actor/port mapping.
using unsubscribe_atom = atom_constant<atom("UNSUBSCRIB")>;

/// Generic 'CONNECT' atom, e.g., for connecting to remote CAF instances.
using connect_atom = atom_constant<atom("CONNECT")>;

/// Generic 'OPEN' atom, e.g., for opening a port or file.
using open_atom = atom_constant<atom("OPEN")>;

/// Generic 'CLOSE' atom, e.g., for closing a port or file.
using close_atom = atom_constant<atom("CLOSE")>;

/// Generic 'SPAWN' atom, e.g., for spawning remote actors.
using spawn_atom = atom_constant<atom("SPAWN")>;

/// Atom to signalize an actor to migrate its state to another actor.
using migrate_atom = atom_constant<atom("MIGRATE")>;

} // namespace actor
} // namespace boost

namespace std {

template <>
struct hash<boost::actor::atom_value> {
  size_t operator()(boost::actor::atom_value x) const {
    hash<uint64_t> f;
    return f(static_cast<uint64_t>(x));
  }
};

} // namespace std

#endif // BOOST_ACTOR_ATOM_HPP