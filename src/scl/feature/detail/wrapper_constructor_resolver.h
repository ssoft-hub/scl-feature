#pragma once

#include <scl/feature/type_traits/wrapper.h>
#include <scl/feature/wrapper_guard.h>
#include <scl/utility/type_traits/forward_like.h>

#include <type_traits>
#include <utility>

namespace scl::feature::detail
{
    /// @internal
    /// @brief Primary template of wrapper_constructor_resolver — intentionally undefined.
    ///
    /// Only the partial specialisation
    /// `wrapper_constructor_resolver<wrapper<V,E>, RightRefer>` is defined.
    /// Attempting to instantiate the primary template is a hard compile error,
    /// which guards against accidental misuse.
    template <typename Left, typename Right>
    class wrapper_constructor_resolver;

    /// @internal
    /// @brief RAII helper that guards a wrapper and delegates resolution to
    ///        wrapper_constructor_resolver for the inner value.
    ///
    /// Used by `wrapper_constructor_resolver::resolve()` when the right-hand
    /// side is itself a wrapper.  Stores the `wrapper_guard` as a member so
    /// that `guard()`/`unguard()` bracket the entire resolution, not just the
    /// moment the inner value reference is obtained.
    ///
    /// Construction order:
    ///   1. `m_guard` is initialised — calls `executor_type::guard()` if present.
    ///   2. `m_guard.value()` returns a reference into the guarded executor.
    ///   3. `m_resolver` is initialised with that reference and resolves further.
    ///
    /// Destruction order (reverse):
    ///   1. `m_resolver` is destroyed.
    ///   2. `m_guard` is destroyed — calls `executor_type::unguard()` if present.
    ///
    /// @tparam LeftWrapper  The target `wrapper<V,E>` being constructed.
    /// @tparam RightRefer   cv-ref-qualified reference to the source wrapper.
    template <typename LeftWrapper, typename RightRefer>
        requires ::scl::feature::is_wrapper_v<RightRefer>
    class guarded_wrapper_constructor_resolver
    {
        using guard_type = ::scl::wrapper_guard<RightRefer>;
        using inner_refer_t = decltype(::std::declval<guard_type &>().value());
        using inner_resolver_t = wrapper_constructor_resolver<LeftWrapper, inner_refer_t>;

    public:
        constexpr explicit guarded_wrapper_constructor_resolver(RightRefer right)
            : m_guard{::std::forward<RightRefer>(right)}
            , m_resolver{m_guard.value()}
        {}

        constexpr decltype(auto) resolve() { return m_resolver.resolve(); }

    private:
        guard_type m_guard;
        inner_resolver_t m_resolver;
    };

    /// @internal
    /// @brief Resolves the correct constructor argument for `wrapper<LeftValue, LeftExecutor>`
    ///        given a source of type `RightRefer`.
    ///
    /// Used inside the forwarding constructor of `wrapper` to determine what to pass
    /// to `LeftExecutor<LeftValue>` during construction.  `resolve()` never returns
    /// a wrapper object — it always returns a reference suitable for direct use as
    /// a constructor argument to the executor.
    ///
    /// **Resolution strategies** (evaluated in order):
    ///
    /// | # | Condition | resolve() returns |
    /// |---|-----------|-------------------|
    /// | 1 | `is_compatible_with_v<Left, Right>` | `executor_type cv-ref` of right |
    /// | 2 | `is_compatible_with_part_of_v<Left, Right>` | `RightRefer` (right wrapper itself) |
    /// | 3 | `is_wrapper_v<Right>` | Recursive: guard right, resolve its inner value |
    /// | 4 | otherwise | `RightRefer` (plain value, forwarded as-is) |
    ///
    /// **Strategy 1** — compatible wrappers (`left ≈ right`):
    /// @code{.cpp}
    ///   using W = wrapper<int, plain>;
    ///   W src{42};
    ///   wrapper_constructor_resolver<W, W &> r{src};
    ///   // r.resolve() → plain<int> & — the executor of src
    /// @endcode
    ///
    /// **Strategy 2** — right is compatible with an inner part of left:
    /// @code{.cpp}
    ///   using W  = wrapper<int,  plain>;
    ///   using WW = wrapper<W,    uninit>;  // WW wraps W
    ///   W src{42};
    ///   wrapper_constructor_resolver<WW, W &> r{src};
    ///   // r.resolve() → W & — src itself, to be used as the value arg of WW's executor
    /// @endcode
    ///
    /// **Strategy 3** — right is a wrapper, peel one layer and recurse:
    /// @code{.cpp}
    ///   using W  = wrapper<int, plain>;
    ///   using WW = wrapper<W,   uninit>;
    ///   WW src{42};
    ///   wrapper_constructor_resolver<W, WW &> r{src};
    ///   // r.resolve() → plain<int> & — executor of the inner W inside src
    /// @endcode
    ///
    /// **Strategy 4** — right is a plain (non-wrapper) value:
    /// @code{.cpp}
    ///   using W = wrapper<int, plain>;
    ///   int x = 42;
    ///   wrapper_constructor_resolver<W, int &> r{x};
    ///   // r.resolve() → int & — x itself
    /// @endcode
    ///
    /// @tparam LeftValue     Value type of the wrapper being constructed.
    /// @tparam LeftExecutor  Executor template of the wrapper being constructed.
    /// @tparam RightRefer    cv-ref-qualified type of the construction source.
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions, bugprone-branch-clone)
    template <typename LeftValue, template <typename> class LeftExecutor, typename RightRefer>
    class wrapper_constructor_resolver<wrapper<LeftValue, LeftExecutor>, RightRefer>
    {
        using left_type = wrapper<LeftValue, LeftExecutor>;
        using right_type = ::std::remove_cvref_t<RightRefer>;

    public:
        explicit constexpr wrapper_constructor_resolver(RightRefer right) noexcept
            : m_right{::std::forward<RightRefer>(right)}
        {}

        constexpr decltype(auto) resolve()
        {
            if constexpr (::scl::feature::is_compatible_with_v<left_type, right_type>)
            {
                // strategy 1: same wrapper type — forward the executor directly
                return executor_access::get(::std::forward<RightRefer>(m_right));
            }
            else if constexpr (::scl::feature::is_compatible_with_part_of_v<left_type, right_type>)
            {
                // strategy 2: right fits into a part of left — forward the right wrapper
                return ::std::forward<RightRefer>(m_right);
            }
            else if constexpr (::scl::feature::is_wrapper_v<right_type>)
            {
                // strategy 3: right is a wrapper — peel one layer and recurse
                using right_executor = typename right_type::executor_type;
                using right_executor_refer = ::scl::forward_like_t<RightRefer, right_executor>;

                constexpr bool has_unguard = ::scl::feature::has_unguard_v<right_executor, right_executor_refer>;

                if constexpr (has_unguard)
                {
                    // unguard exists — need RAII wrapper_guard for guard/unguard bracketing
                    return guarded_wrapper_constructor_resolver<left_type, RightRefer>{
                        ::std::forward<RightRefer>(m_right)}
                        .resolve();
                }
                else
                {
                    // no unguard — intentional: guard() without unguard() is a valid executor
                    // contract (e.g. a one-shot initializer).  RAII is unnecessary because
                    // there is nothing to release; call guard() if present, then read value.
                    decltype(auto) executor = executor_access::get(::std::forward<RightRefer>(m_right));

                    if constexpr (::scl::feature::has_guard_v<right_executor, right_executor_refer>)
                        right_executor::guard(::std::forward<right_executor_refer>(executor));

                    decltype(auto) inner = right_executor::template access<right_executor_refer>(
                        ::std::forward<decltype(executor)>(executor));

                    return wrapper_constructor_resolver<left_type, decltype(inner)>{
                        ::std::forward<decltype(inner)>(inner)}
                        .resolve();
                }
            }
            else
            {
                // strategy 4: plain value — forward as-is
                return ::std::forward<RightRefer>(m_right);
            }
        }

    private:
        RightRefer m_right;
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions, bugprone-branch-clone)

} // namespace scl::feature::detail
