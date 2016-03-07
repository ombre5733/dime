/*******************************************************************************
  Diagnostic messaging

  Copyright (c) 2016, Manuel Freiberger
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef DIME_ARGUMENT_HPP
#define DIME_ARGUMENT_HPP

#include <type_traits>


namespace dime
{

/// TODO: This is a crude hack. Replace it.
template <typename T>
class optional
{
public:
    optional()
        : m_valid(false)
    {
    }

    optional(T v)
        : m_valid(true)
    {
        new (&m_value) T(v);
    }

    optional(const optional& other)
        : m_valid(other.m_valid)
    {
        if (m_valid)
            new (&m_value) T(other.value());
    }

    T& value()
    {
        return *reinterpret_cast<T*>(&m_value);
    }

    const T& value() const
    {
        return *reinterpret_cast<const T*>(&m_value);
    }

private:
    std::aligned_storage_t<sizeof(T)> m_value;
    bool m_valid;
};


//! \brief An enumeration of argument types.
enum class ArgumentKind
{
    SignedInteger,
    UnsignedInteger,
    Float,
    Double,
    LongDouble,
    String
};

//! \brief An argument in a diagnostic.
//!
//! The Argument encapsulates the argument of a diagnostic.
class Argument
{
public:
    Argument(int value)
        : m_integer(value),
          m_kind(ArgumentKind::SignedInteger)
    {
    }

    Argument(unsigned value)
        : m_unsigned(value),
          m_kind(ArgumentKind::UnsignedInteger)
    {
    }

    Argument(float value)
        : m_float(value),
          m_kind(ArgumentKind::Float)
    {
    }

    Argument(double value)
        : m_double(value),
          m_kind(ArgumentKind::Double)
    {
    }

    Argument(long double value)
        : m_longDouble(value),
          m_kind(ArgumentKind::LongDouble)
    {
    }

    Argument(const char* value)
        : m_string(value),
          m_kind(ArgumentKind::String)
    {
    }

    optional<int> toInteger() const
    {
        return m_kind == ArgumentKind::SignedInteger
               ? optional<int>(m_integer)
               : optional<int>();
    }

    optional<float> toFloat() const
    {
        return m_kind == ArgumentKind::Float
               ? optional<float>(m_float)
               : optional<float>();
    }

private:
    union
    {
        int m_integer;
        unsigned m_unsigned;
        float m_float;
        double m_double;
        long double m_longDouble;
        const char* m_string;
    };

    ArgumentKind m_kind;
};

} // namespace dime

#endif // DIME_ARGUMENT_HPP
