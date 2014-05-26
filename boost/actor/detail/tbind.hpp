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
 * Copyright (C) 2011 - 2014                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the Boost Software License, Version 1.0. See             *
 * accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt  *
\******************************************************************************/


#ifndef BOOST_ACTOR_TBIND_HPP
#define BOOST_ACTOR_TBIND_HPP

namespace boost {
namespace actor {
namespace detail {

/**
 * @ingroup MetaProgramming
 * @brief Predefines the first template parameter of @p Tp1.
 */
template<template<typename, typename> class Tpl, typename Arg1>
struct tbind {
    template<typename Arg2>
    struct type {
        static constexpr bool value = Tpl<Arg1, Arg2>::value;
    };
};

} // namespace detail
} // namespace actor
} // namespace boost

#endif // BOOST_ACTOR_TBIND_HPP
