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
    /**
     * @brief A basic coroutine task type.
     * * Required by the C++20 coroutine machinery to define a coroutine's return type.
     * * This is a minimal implementation primarily to support internal async operations.
     */
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

    /**
     * @brief Awaiter for std::future to allow `co_await future`.
     * * Enables bridging standard C++ std::future based async code with C++20 coroutines.
     * @tparam T The return type of the future.
     */
    template <typename T>
    struct FutureAwaiter
    {
        std::future<T>& future;

        /**
         * @brief Checks if the future is already ready.
         */
        bool await_ready() const
        { 
            return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready; 
        }

        /**
         * @brief Suspends the coroutine and waits for the future in a separate thread.
         * @param handle The coroutine handle to resume when ready.
         */
        void await_suspend(std::coroutine_handle<> handle)
        {
            std::thread([this, handle]()
            {
                future.wait();
                handle.resume();
            }).detach();
        }

        /**
         * @brief Resumes the coroutine and returns the result of the future.
         */
        T await_resume()
        {
            return future.get();
        }
    };

    /**
     * @brief Operator to support `co_await` on an rvalue std::future.
     */
    template <typename T>
    FutureAwaiter<T> operator co_await(std::future<T>&& f)
    {
        return FutureAwaiter<T>{f};
    }
    
    /**
     * @brief Operator to support `co_await` on an lvalue std::future.
     */
    template <typename T>
    FutureAwaiter<T> operator co_await(std::future<T>& f)
    {
        return FutureAwaiter<T>{f};
    }
}

#endif // C++20 Check
#endif // GEMINI_COROUTINE_H