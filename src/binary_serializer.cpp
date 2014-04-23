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


#include <limits>
#include <string>
#include <iomanip>
#include <cstdint>
#include <sstream>
#include <cstring>
#include <type_traits>

#include "boost/actor/config.hpp"
#include "boost/actor/primitive_variant.hpp"
#include "boost/actor/binary_serializer.hpp"
#include "boost/actor/type_lookup_table.hpp"

#include "boost/actor/detail/ieee_754.hpp"

namespace boost {
namespace actor {

namespace {

using util::grow_if_needed;

class binary_writer : public static_visitor<> {

 public:

    binary_writer(util::buffer* sink) : m_sink(sink) { }

    template<typename T>
    static inline void write_int(util::buffer* sink, const T& value) {
        sink->write(sizeof(T), &value, grow_if_needed);
    }

    static inline void write_string(util::buffer* sink,
                                    const std::string& str) {
        write_int(sink, static_cast<std::uint32_t>(str.size()));
        sink->write(str.size(), str.c_str(), grow_if_needed);
    }

    template<typename T>
    void operator()(const T& value,
                    typename std::enable_if<std::is_integral<T>::value>::type* = 0) const {
        write_int(m_sink, value);
    }

    template<typename T>
    void operator()(const T& value,
                    typename std::enable_if<std::is_floating_point<T>::value>::type* = 0) const {
        auto tmp = detail::pack754(value);
        write_int(m_sink, tmp);
    }

    // the IEEE-754 conversion does not work for long double
    // => fall back to string serialization (event though it sucks)
    void operator()(const long double& v) const {
        std::ostringstream oss;
        oss << std::setprecision(std::numeric_limits<long double>::digits) << v;
        write_string(m_sink, oss.str());
    }

    void operator()(const atom_value& val) const {
        (*this)(static_cast<uint64_t>(val));
    }

    void operator()(const std::string& str) const {
        write_string(m_sink, str);
    }

    void operator()(const std::u16string& str) const {
        write_int(m_sink, static_cast<std::uint32_t>(str.size()));
        for (char16_t c : str) {
            // force writer to use exactly 16 bit
            write_int(m_sink, static_cast<std::uint16_t>(c));
        }
    }

    void operator()(const std::u32string& str) const {
        write_int(m_sink, static_cast<std::uint32_t>(str.size()));
        for (char32_t c : str) {
            // force writer to use exactly 32 bit
            write_int(m_sink, static_cast<std::uint32_t>(c));
        }
    }

 private:

    util::buffer* m_sink;

};

} // namespace <anonymous>

binary_serializer::binary_serializer(util::buffer* buf,
                                     actor_namespace* ns,
                                     type_lookup_table* tbl)
: super(ns, tbl), m_sink(buf) { }

void binary_serializer::begin_object(const uniform_type_info* uti) {
    BOOST_ACTOR_REQUIRE(uti != nullptr);
    auto ot = outgoing_types();
    std::uint32_t id = (ot) ? ot->id_of(uti) : 0;
    std::uint8_t flag = (id == 0) ? 1 : 0;
    binary_writer::write_int(m_sink, flag);
    if (flag == 1) binary_writer::write_string(m_sink, uti->name());
    else binary_writer::write_int(m_sink, id);
}

void binary_serializer::end_object() { }

void binary_serializer::begin_sequence(size_t list_size) {
    binary_writer::write_int(m_sink, static_cast<std::uint32_t>(list_size));
}

void binary_serializer::end_sequence() { }

void binary_serializer::write_value(const primitive_variant& value) {
    apply_visitor(binary_writer(m_sink), value);
}

void binary_serializer::write_raw(size_t num_bytes, const void* data) {
    m_sink->write(num_bytes, data, grow_if_needed);
}

void binary_serializer::write_tuple(size_t size,
                                    const primitive_variant* values) {
    const primitive_variant* end = values + size;
    for ( ; values != end; ++values) {
        write_value(*values);
    }
}

} // namespace actor
} // namespace boost
