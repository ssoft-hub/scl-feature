#pragma once

#include <utility>

namespace scl::feature::detail
{
    template <typename Value, template <typename> class Executor>
    class wrapper
    {
    public:
        using value_type = Value;
        using executor_type = Executor<value_type>;

    public:
        template <typename... Args>
        constexpr explicit wrapper(Args &&... args)
            : m_executor{::std::forward<Args>(args)...}
        {}

    private:
        executor_type m_executor;

        friend struct executor_access;
    };
} // namespace scl::feature::detail
