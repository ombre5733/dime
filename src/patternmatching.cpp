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

#include "patternmatching.hpp"

using namespace dime;
using namespace dime_detail;


PatternMatcher::~PatternMatcher()
{
}



class MatcherImpl : public PatternMatcher
{
public:
    MatcherImpl(const char* pattern)
        : m_pattern(pattern),
          m_next(nullptr)
    {
    }

    virtual
    bool matches(const Code& id) const override
    {
        auto stringId = id.toString();
        return dime_detail::match(m_pattern, stringId.c_str());
    }

    virtual
    Type type() const override
    {
        return Exact;
    }

    virtual
    bool moreSpecificThan(const PatternMatcher& other) const override
    {
        switch (other.type())
        {
        case Exact:
            return false;
        }
        // TODO: assert unreachable
        return false;
    }

private:
    const char* m_pattern;
    PatternMatcher* m_next;
};


std::unique_ptr<PatternMatcher> dime::dime_detail::compilePattern(const char* pattern)
{
    /*
    bool hasWildcard = false;
    int numClassWildcards = 0;
    for (const char* iter = pattern; *iter != 0; ++iter)
    {
        if (*iter == '?')
            hasWildcard = true;
    }
    */

    return std::unique_ptr<PatternMatcher>(new MatcherImpl(pattern));
}
