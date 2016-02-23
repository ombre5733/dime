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

#ifndef DIME_CODE_HPP
#define DIME_CODE_HPP

#include <cstdint>
#include <string>


namespace dime
{
namespace dime_detail
{

//! A table to compress an ASCII sub-set to 6 bit characters.
static constexpr std::uint64_t compressionTable[] = {
    /*        .0  .1  .2  .3  .4  .5  .6  .7  .8  .9  .A  .B  .C  .D  .E  .F */
    /* 0. */  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    /* 1. */  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    /* 2. */  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  0, 99, 99,
    /* 3. */   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 99, 99, 99, 99, 99, 99,
    /* 4. */  99, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    /* 5. */  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 99, 99, 99, 99, 37,
    /* 6. */  99, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
    /* 7. */  53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 99, 99, 99, 99, 99
};

static constexpr char decompressionTable[] = {
    /*        .0   .1   .2   .3   .4   .5   .6   .7   .8   .9   .A   .B   .C   .D   .E   .F */
    /* 0. */  '-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    /* 1. */  'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    /* 2. */  'V', 'W', 'X', 'Y', 'Z', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    /* 3. */  'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

} // namespace dime_detail

//! \brief The code of a diagnostic.
struct Code
{
    constexpr
    std::uint64_t operator[](int idx) const
    {
        return data[idx];
    }

    // TODO: remove
    std::string toString() const
    {
        std::string s(20, ' ');
        for (int n = 0; n < 2; ++n)
            for (int i = 0; i < 10; ++i)
                s[n * 10 + i] = dime_detail::decompressionTable[(data[n] >> (i * 6)) & 0x3f];
        return s;
    }

    std::uint64_t data[2];
};

struct IdentifierTooLongOrWrongChar {}; // TODO: split

namespace dime_detail
{

constexpr
Code orBits(Code x, Code y)
{
    return Code{x[0] | y[0], x[1] | y[1]};
}

constexpr
Code makeCode(const char* str, std::uint64_t index)
{
// TODO: compressionTable[static_cast<unsigned char>(*s)] should go into its own function
    return *str == 0
           ? Code{0, 0}
           : (index >= 20 || compressionTable[static_cast<unsigned char>(*str)] == 99
              ? throw IdentifierTooLongOrWrongChar()
              : orBits(makeCode(str + 1, index + 1),
                       Code{ index < 10  ? compressionTable[static_cast<unsigned char>(*str)] << (index * 6) : 0u,
                             index >= 10 ? compressionTable[static_cast<unsigned char>(*str)] << ((index - 10) * 6) : 0u }));
}

} // namespace dime_detail

//! \brief Creates a diagnosis code.
//!
//! Creates a diagnosis from the \p code string.
constexpr
Code makeCode(const char* code)
{
    return dime_detail::makeCode(code, 0);
}

} // namespace dime

#endif // DIME_CODE_HPP
