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

#ifndef BOOST_ACTOR_TYPED_CONTINUE_HELPER_HPP
#define BOOST_ACTOR_TYPED_CONTINUE_HELPER_HPP

#include "boost/actor/continue_helper.hpp"

#include "boost/actor/detail/type_traits.hpp"

#include "boost/actor/detail/typed_actor_util.hpp"

namespace boost {
namespace actor {

template <class OutputList>
class typed_continue_helper {
public:
  using message_id_wrapper_tag = int;

  typed_continue_helper(message_id mid) : ch_(mid) {
    // nop
  }

  typed_continue_helper(continue_helper ch) : ch_(std::move(ch)) {
    // nop
  }

  message_id get_message_id() const {
    return ch_.get_message_id();
  }

private:
  continue_helper ch_;
};

} // namespace actor
} // namespace boost

#endif // BOOST_ACTOR_TYPED_CONTINUE_HELPER_HPP
