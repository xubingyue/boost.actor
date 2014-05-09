/******************************************************************************\
 *           ___        __                                                    *
 *          /\_ \    __/\ \                                                   *
 *          \//\ \  /\_\ \ \____    ___   _____   _____      __               *
 *            \ \ \ \/\ \ \ '__`\  /'___\/\ '__`\/\ '__`\  /'__`\             *
 *             \_\ \_\ \ \ \ \L\ \/\ \__/\ \ \L\ \ \ \L\ \/\ \L\.\_           *
 *             /\____\\ \_\ \_,__/\ \____\\ \ ,__/\ \ ,__/\ \__/.\_\          *
 *             \/____/ \/_/\/___/  \/____/ \ \ \/  \ \ \/  \/__/\/_/          *
 *                                          \ \_\   \ \_\                     *
 *                                           \/_/    \/_/                     *
 *                                                                            *
 * Copyright (C) 2011-2013                                                    *
 * Dominik Charousset <dominik.charousset@haw-hamburg.de>                     *
 *                                                                            *
 * This file is part of libcppa.                                              *
 * libcppa is free software: you can redistribute it and/or modify it under   *
 * the terms of the GNU Lesser General Public License as published by the     *
 * Free Software Foundation; either version 2.1 of the License,               *
 * or (at your option) any later version.                                     *
 *                                                                            *
 * libcppa is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 * See the GNU Lesser General Public License for more details.                *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with libcppa. If not, see <http://www.gnu.org/licenses/>.            *
\******************************************************************************/


#ifndef BOOST_ACTOR_FWD_HPP
#define BOOST_ACTOR_FWD_HPP

#include <cstdint>

namespace boost {

template<class> class intrusive_ptr;

namespace actor {

// classes
class actor;
class group;
class channel;
class node_id;
class behavior;
class resumable;
class message;
class actor_addr;
class actor_proxy;
class scoped_actor;
class execution_unit;
class abstract_actor;
class abstract_group;
class blocking_actor;
class message_header;
class message_handler;
class uniform_type_info;
class event_based_actor;

// structs
struct anything;

// enums
enum primitive_type : unsigned char;
enum class atom_value : std::uint64_t;

// intrusive pointer typedefs
typedef intrusive_ptr<abstract_group>      abstract_group_ptr;
typedef intrusive_ptr<actor_proxy>         actor_proxy_ptr;
typedef intrusive_ptr<node_id>             node_id_ptr;

// convenience typedefs
typedef const message_header& msg_hdr_cref;

} // namespace actor
} // namespace boost

#endif // BOOST_ACTOR_FWD_HPP