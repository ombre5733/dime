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

#ifndef DIME_DIAGNOSTIC_HPP
#define DIME_DIAGNOSTIC_HPP

#include "allocator.hpp"
#include "argument.hpp"
#include "code.hpp"
#include "descriptor.hpp"

#include <chrono>
#include <cstddef>
#include <tuple>
#include <utility>


namespace dime
{

using UniqueId = std::uint32_t;

namespace dime_detail
{

inline
UniqueId createUniqueId()
{
    // TODO
    return UniqueId();
}

} // namespace dime_detail

struct non_droppable_t {};
constexpr non_droppable_t non_droppable = non_droppable_t();

//! \brief A diagnostic message.
//!
//! It consists of
//! - a code,
//! - a unique ID,
//! - a time stamp,
//! - a variable number of arguments.
class alignas(Argument) Diagnostic
{
public:
    using TimePoint = std::chrono::high_resolution_clock::time_point;


    Diagnostic(const Diagnostic&) = delete;
    const Diagnostic& operator=(const Diagnostic&) = delete;


    //! \brief Returns the code of the diagnostic.
    const Code& code() const noexcept
    {
        return m_code;
    }

    //! \brief Returns the time stamp.
    //!
    //! Returns the time stamp at which the diagnostic has been created.
    const TimePoint& timeStamp() const noexcept
    {
        return m_timeStamp;
    }

    //! \brief Checks if the diagnostic is droppable.
    bool droppable() const noexcept
    {
        return m_droppable;
    }

    //! \brief Returns the unique ID.
    UniqueId uniqueId() const noexcept;

    //! \brief Returns the number of arguments.
    //!
    //! Returns the number of arguments, which have been attached to this
    //! diagnostic.
    unsigned numArguments() const noexcept
    {
        return m_numArguments;
    }

    //! \brief Returns the \p index-th argument.
    Argument* arguments() noexcept
    {
        static_assert(alignof(Diagnostic) >= alignof(Argument),
                      "Alignment does not match");
        return reinterpret_cast<Argument*>(this + 1);
    }

//    Argument& argument(unsigned idx) noexcept
//    {
//    }

    template <typename... TArguments>
    static
    Diagnostic* create(Allocator& allocator,
                       const Descriptor<void(TArguments...)>& descriptor,
                       TArguments&&... arguments);

    template <typename... TArguments>
    static
    Diagnostic* create(non_droppable_t,
                       Allocator& allocator,
                       const Descriptor<void(TArguments...)>& descriptor,
                       TArguments&&... arguments);

private:
    //! The code of the message.
    Code m_code;
    //! The time point when the diagnostic was created.
    TimePoint m_timeStamp;
    //! The unique id.
    UniqueId m_uniqueId;
    //! The number of arguments which are stored alongside this diagnostic.
    unsigned m_numArguments : 31;
    //! If set, the diagnostic can be dropped.
    unsigned m_droppable : 1;

    /*std::atomic_int m_referenceCount;
    Diagnostic* m_next;*/


    template <typename... TArguments>
    Diagnostic(const Descriptor<void(TArguments...)>& spec,
               TArguments&&... arguments);

    template <typename TArguments, std::size_t... TIndices>
    void initArguments(TArguments&& arguments, std::integer_sequence<std::size_t, TIndices...>)
    {
        variadicCall(initArgument<TIndices>(std::get<TIndices>(arguments))...);
    }

    template <std::size_t TIndex, typename TArgument>
    int initArgument(TArgument&& argument)
    {
        new (&argumentData()[TIndex]) Argument(std::forward<TArgument>(argument));
        return 0;
    }

    template <typename... T>
    void variadicCall(T...)
    {
    }

    Argument* argumentData()
    {
        return reinterpret_cast<Argument*>(this + 1);
    }
};

template <typename... TArguments>
Diagnostic::Diagnostic(const Descriptor<void(TArguments...)>& desc,
                       TArguments&&... arguments)
    : m_code(desc.m_code),
      m_timeStamp(std::chrono::high_resolution_clock::now()),
      m_uniqueId(dime_detail::createUniqueId()),
      m_numArguments(sizeof...(arguments)),
      m_droppable(true)
{
    initArguments(std::forward_as_tuple(std::forward<TArguments>(arguments)...),
                  std::make_index_sequence<sizeof...(arguments)>());
}

template <typename... TArguments>
Diagnostic* Diagnostic::create(Allocator& allocator,
                               const Descriptor<void(TArguments...)>& descriptor,
                               TArguments&&... arguments)
{
    auto size = sizeof(Diagnostic);
    size += sizeof...(TArguments) * sizeof(Argument);

    void* mem = allocator.allocate(size);
    return new (mem) Diagnostic(descriptor, std::forward<TArguments>(arguments)...);
}

template <typename... TArguments>
Diagnostic* Diagnostic::create(non_droppable_t,
                               Allocator& allocator,
                               const Descriptor<void(TArguments...)>& descriptor,
                               TArguments&&... arguments)
{
    auto size = sizeof(Diagnostic);
    size += sizeof...(TArguments) * sizeof(Argument);

    void* mem = allocator.allocate(size);
    auto diag = new (mem) Diagnostic(descriptor, std::forward<TArguments>(arguments)...);
    diag->m_droppable = false;
    return diag;
}


class DiagnosticPtr
{
public:
    DiagnosticPtr(Diagnostic* diagnostic);
    DiagnosticPtr(const DiagnosticPtr& other);
    DiagnosticPtr(DiagnosticPtr&& other);

    DiagnosticPtr& operator=(const DiagnosticPtr& other);
    DiagnosticPtr&& operator=(DiagnosticPtr&& other);


private:
    Diagnostic* m_diagnostic;
};

} // namespace dime

#endif // DIME_DIAGNOSTIC_HPP
