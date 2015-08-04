#pragma once

#include <ppl.h>
#include <ppltasks.h>
#include <wrl\wrappers\corewrappers.h>

using namespace concurrency;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;

namespace SimpleGame { namespace Utilities {

void strprintf(std::wstring& sOut, const wchar_t* fmt, ...);
inline float2 GetRectCenter(Rect rect)
{
    return float2(rect.X + rect.Width / 2.0f, rect.Y + rect.Height / 2.0f);
}

inline float lerp(float v0, float v1, float t)
{
    return (1 - t)*v0 + t*v1;
}

template<typename T>
inline T WaitExecution(IAsyncOperation<T>^ asyncOperation)
{
    using namespace Microsoft::WRL::Wrappers;

    Event emptyEvent(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
    if (!emptyEvent.IsValid())
        throw std::bad_alloc();

    task_options options;
    options.set_continuation_context(task_continuation_context::use_arbitrary());

    task<T> asyncTask(asyncOperation);

    asyncTask.then([&](task<T>)
    {
        SetEvent(emptyEvent.Get());
    }, options);

    // waiting before event executed
    auto timeout = 1000 * 5;
    auto waitResult = WaitForSingleObjectEx(emptyEvent.Get(), timeout, true);

    return asyncTask.get();
};

inline void WaitExecution(IAsyncAction^ ayncAction)
{
    using namespace Microsoft::WRL::Wrappers;

    Event emptyEvent(CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
    if (!emptyEvent.IsValid())
        throw std::bad_alloc();

    task_options options;
    options.set_continuation_context(task_continuation_context::use_arbitrary());

    task<void> asyncTask(ayncAction);

    asyncTask.then([&](task<void>)
    {
        SetEvent(emptyEvent.Get());
    }, options);

    // waiting before event executed
    auto timeout = 1000 * 5;
    auto waitResult = WaitForSingleObjectEx(emptyEvent.Get(), timeout, true);

    asyncTask.get();
};

}}