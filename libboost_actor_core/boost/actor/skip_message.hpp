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

#ifndef BOOST_ACTOR_SKIP_MESSAGE_HPP
#define BOOST_ACTOR_SKIP_MESSAGE_HPP

#include <ostream>

namespace boost {
namespace actor {

/// Optional return type for functors used in pattern matching
/// expressions. This type is evaluated by the runtime system
/// and can be used to intentionally skip messages.
struct skip_message_t {
  constexpr skip_message_t() {}

};

/// Tells the runtime system to skip a message when used as message
/// handler, i.e., causes the runtime to leave the message in
/// the mailbox of an actor.
constexpr skip_message_t skip_message() {
  return {};
}

// implemented in string_serialization.cpp
std::ostream& operator<<(std::ostream&, skip_message_t);

} // namespace actor
} // namespace boost

#endif // BOOST_ACTOR_SKIP_MESSAGE_HPP
