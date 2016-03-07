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

#ifndef DIME_ENGINE_HPP
#define DIME_ENGINE_HPP

#include "config.hpp"
#include "allocator.hpp"
#include "diagnostic.hpp"
#include "patternmatching.hpp"

#include <list>

#ifdef DIME_USE_WEOS
#include <weos/memory.hpp>
#include <weos/mutex.hpp>
#else
#include <memory>
#include <mutex>
#endif // DIME_USE_WEOS


namespace dime
{
class Subscriber;

class Engine : public Allocator
{
    struct FilteredSubscriber
    {
        FilteredSubscriber(DIME_STD::unique_ptr<dime_detail::PatternMatcher> m,
                           Subscriber* s)
            : matcher(DIME_STD::move(m)),
              subscriber(s)
        {
        }

        FilteredSubscriber(FilteredSubscriber&& other)
            : matcher(DIME_STD::move(other.matcher)),
              subscriber(other.subscriber)
        {
        }

        DIME_STD::unique_ptr<dime_detail::PatternMatcher> matcher;
        Subscriber* subscriber;
    };

public:
    // TODO:
    // Engine(std::size_t memorySize);

    template <typename... TArguments>
    void publish(const Descriptor<void(TArguments...)>& spec,
                 TArguments&&... arguments);

    void dispatch(Diagnostic* diagnostic);

    void subscribe(const char* filterPattern, Subscriber* subscriber);

    /*
    template <typename... TPatterns>
    void subscribe(const char* filterPattern, TPatterns... patterns,
                   DiagnosticConsumer* consumer);
    */

    //! \brief Sets a fallback consumer.
    //!
    //! Sets the fallback consumer to \p consumer. This consumer will receive
    //! all diagnostics which have not been filtered before.
    void setFallbackConsumer(Subscriber* consumer) noexcept;

private:
    DIME_STD::mutex m_mutex;

    Subscriber* m_fallbackConsumer = nullptr;

    std::list<FilteredSubscriber> m_list;

    // TODO:
    // - Fallback diagnostic
    // - Common base class for everything allocated in DiagnosticAllocator
};

template <typename... TArguments>
void Engine::publish(const Descriptor<void(TArguments...)>& spec,
                     TArguments&&... arguments)
{
    DIME_STD::lock_guard<DIME_STD::mutex> lock(m_mutex);
    auto diagnostic = Diagnostic::create(*this, spec, DIME_STD::forward<TArguments>(arguments)...);
    dispatch(diagnostic);
}

} // namespace dime

#endif // DIME_ENGINE_HPP
