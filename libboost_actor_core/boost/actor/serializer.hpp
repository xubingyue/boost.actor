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

#ifndef BOOST_ACTOR_SERIALIZER_HPP
#define BOOST_ACTOR_SERIALIZER_HPP

#include <string>
#include <cstddef> // size_t

#include "boost/actor/uniform_typeid.hpp"
#include "boost/actor/uniform_type_info.hpp"
#include "boost/actor/primitive_variant.hpp"

namespace boost {
namespace actor {

class actor_namespace;
class uniform_type_info;

/// @ingroup TypeSystem
/// Technology-independent serialization interface.
class serializer {
public:
  serializer(const serializer&) = delete;
  serializer& operator=(const serializer&) = delete;

  /// @note `addressing` must be guaranteed to outlive the serializer
  serializer(actor_namespace* addressing = nullptr);

  virtual ~serializer();

  /// Begins serialization of an object of type `uti`.
  virtual void begin_object(const uniform_type_info* uti) = 0;

  /// Ends serialization of an object.
  virtual void end_object() = 0;

  /// Begins serialization of a sequence of size `num`.
  virtual void begin_sequence(size_t num) = 0;

  /// Ends serialization of a sequence.
  virtual void end_sequence() = 0;

  /// Writes a single value to the data sink.
  /// @param value A primitive data value.
  virtual void write_value(const primitive_variant& value) = 0;

  /// Writes a raw block of data.
  /// @param num_bytes The size of `data` in bytes.
  /// @param data Raw data.
  virtual void write_raw(size_t num_bytes, const void* data) = 0;

  inline actor_namespace* get_namespace() {
    return namespace_;
  }

  template <class T>
  serializer& write(const T& val) {
    write_value(val);
    return *this;
  }

  template <class T>
  serializer& write(const T& val, const uniform_type_info* uti) {
    uti->serialize(&val, this);
    return *this;
  }

private:
  actor_namespace* namespace_;
};

/// Serializes a value to `s`.
/// @relates serializer
template <class T>
typename std::enable_if<
  detail::is_primitive<T>::value,
  serializer&
>::type
operator<<(serializer& sink, const T& value) {
  return sink.write(value);
}

/// Serializes a value to `s`.
/// @relates serializer
template <class T>
typename std::enable_if<
  ! detail::is_primitive<T>::value,
  serializer&
>::type
operator<<(serializer& sink, const T& value) {
  return sink.write(value, uniform_typeid<T>());
}

template <class T>
void operator&(serializer& sink, const T& value) {
  sink << value;
}

} // namespace actor
} // namespace boost

#endif // BOOST_ACTOR_SERIALIZER_HPP
