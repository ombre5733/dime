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

#ifndef DIME_PATTERNMATCHING_HPP
#define DIME_PATTERNMATCHING_HPP

#include "code.hpp"

#include <memory>


namespace dime
{
namespace dime_detail
{

//! Checks if \p c is a digit.
constexpr
bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

//! Checks if \p c is an uppercase ASCII character.
constexpr
bool isUppercase(char c)
{
    return c >= 'A' && c <= 'Z';
}

//constexpr
//bool isAlnum(char c)
//{
//    return isDigit(c) || isUppercase(c);
//}

constexpr
bool match(const char* pattern, const char* text)
{
    return *pattern == 0 || *text == 0
           ? *pattern == *text
           : (*pattern == '*'
              ? true
              : (*pattern == '?'
                 ? match(pattern + 1, text + 1)
                 : *pattern == *text && match(pattern + 1, text + 1)
                )
             );
}



class PatternMatcher
{
public:
    enum Type
    {
        Exact,
        //Range
    };

    virtual
    ~PatternMatcher();

    virtual
    bool matches(const Code& id) const = 0;

    virtual
    Type type() const = 0;

    virtual
    bool moreSpecificThan(const PatternMatcher& other) const = 0;
};

std::unique_ptr<PatternMatcher> compilePattern(const char* pattern);

} // namespace dime_detail
} // namespace dime

#endif // DIME_PATTERNMATCHING_HPP
