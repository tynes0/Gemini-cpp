#pragma once

#ifndef GEMINI_COROUTINE_H
#define GEMINI_COROUTINE_H

#include <version>

#if __cplusplus >= 202002L

#include <future>
#include <coroutine>
#include <thread>

namespace GeminiCPP
{
    struct Task
    {
        struct promise_type {
            Task get_return_object() { return {}; }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void return_void() {}
            [[noreturn]] void unhandled_exception() { std::terminate(); }
        };
    };

    template <typename T>
    struct FutureAwaiter
    {
        std::future<T>& future;

        bool await_ready() const
        { 
            return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready; 
        }

        void await_suspend(std::coroutine_handle<> handle)
        {
            std::thread([this, handle]()
            {
                future.wait();
                handle.resume();
            }).detach();
        }

        T await_resume()
        {
            return future.get();
        }
    };

    template <typename T>
    FutureAwaiter<T> operator co_await(std::future<T>&& f)
    {
        return FutureAwaiter<T>{f};
    }
    
    template <typename T>
    FutureAwaiter<T> operator co_await(std::future<T>& f)
    {
        return FutureAwaiter<T>{f};
    }
}

#endif // C++20 Check
#endif // GEMINI_COROUTINE_H