#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <utility>

#include <scl/utility/type_traits/forward_like.h>

namespace scl::feature::reference
{
    /**
     * @defgroup scl_feature_reference_plain reference::plain
     * @ingroup scl_feature_reference
     * @brief Non-owning observer executor that stores a raw pointer.
     * @{
     */

    /**
     * @brief Non-owning executor that stores a raw pointer to an externally-managed value.
     *
     * Binding to an rvalue is deleted to prevent dangling references. Copy and move
     * preserve the pointer (shallow semantics — both copies observe the same object).
     *
     * @tparam Value  the observed value type
     *
     * @code{.cpp}
     *    std::string s = "hello";
     *    wrapper<std::string, feature::reference::plain> ref{s};
     *    ref.execute([](std::string& v){ v += " world"; });
     *    assert(s == "hello world");
     * @endcode
     */
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions)
    template <typename Value>
    class plain
    {
        using self_type = plain<Value>;

    public:
        /// @brief the observed value type
        using value_type = Value;

    public:
        /// @brief Deleted: binding to rvalues would create a dangling reference.
        plain(Value &&) = delete;

        /// @brief Constructs a plain reference executor observing @p v.
        /// @param v  lvalue to observe; must outlive this executor
        explicit plain(Value & v) noexcept
            : m_ptr{::std::addressof(v)}
        {}

        /// @brief Copies the stored pointer (shallow — both executors observe the same object).
        plain(plain const &) noexcept = default;

        /// @brief Moves the stored pointer (source continues to point to the same object).
        plain(plain &&) noexcept = default;

        /// @brief Copy-assigns the stored pointer.
        plain & operator=(plain const &) noexcept = default;

        /// @brief Move-assigns the stored pointer.
        plain & operator=(plain &&) noexcept = default;

    public:
        /**
         * @brief Invokes @p func with @p args; no guard/unguard overhead.
         * @ingroup scl_feature_reference_plain
         * @tparam Self  cv-ref qualified plain<Value>
         * @tparam Func  callable type
         * @tparam Args  argument types
         * @param  self  executor instance (unused by execute itself)
         * @param  func  callable to invoke
         * @param  args  additional arguments forwarded to @p func
         * @return result of `std::invoke(func, args...)`
         */
        template <typename Self, typename Func, typename... Args>
        static constexpr decltype(auto) execute(Self && self, Func && func, Args &&... args)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> && ::std::invocable<Func, Args && ...>)
        {
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

        /**
         * @brief Returns the observed value with cv-ref qualifiers matching @p Self.
         * @ingroup scl_feature_reference_plain
         * @tparam Self  cv-ref qualified plain<Value>
         * @param  self  executor instance
         * @return cv-ref forwarded reference to the observed value
         */
        template <typename Self>
        static constexpr decltype(auto) value(Self && self)
            requires ::std::same_as<::std::remove_cvref_t<Self>, self_type>
        {
            return ::scl::forward_like<Self>(*self.m_ptr);
        }

    private:
        Value * m_ptr;
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions)

    /// @}

} // namespace scl::feature::reference
