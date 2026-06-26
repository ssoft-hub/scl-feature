#pragma once

/// @file
/// @brief Copy-on-write executor: heap value in an intrusive, allocator-aware control block.
/// @ingroup scl_feature_executors

#include <atomic>
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include <scl/utility/attribute/no_unique_address.h>
#include <scl/utility/type_traits/forward_like.h>

/// @internal
/// @brief Expands to one shallow-share / steal constructor for the given cv-ref qualification.
///
/// Copy categories (`&`, `const &`, `const &&`) share the control block and bump the
/// reference count; the non-const rvalue category (`&&`) steals the pointer and leaves
/// the source empty.  Defining these explicitly stops the variadic forwarding
/// constructor from greedily capturing same-type arguments.
#define SCL_INDIRECT_CONSTRUCTOR_FOR_SELF_PROTOTYPE(cv_ref)              \
    explicit indirect(self_type cv_ref other) /**/                       \
        noexcept(::std::is_nothrow_copy_constructible_v<allocator_type>) \
        : m_allocator{other.m_allocator}                                 \
        , m_block{adopt(static_cast<self_type cv_ref>(other))}           \
    {}

#define SCL_INDIRECT_CONSTRUCTOR_FOR_SELF                \
    SCL_INDIRECT_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&)       \
    SCL_INDIRECT_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &) \
    SCL_INDIRECT_CONSTRUCTOR_FOR_SELF_PROTOTYPE(&&)      \
    SCL_INDIRECT_CONSTRUCTOR_FOR_SELF_PROTOTYPE(const &&)

namespace scl::feature::implicit
{
    /**
     * @brief Copy-on-write executor: the value lives on the heap and is shared between copies.
     * @ingroup scl_feature_executors
     *
     * Stores @p Value in a single heap allocation — an intrusive control block holding a
     * reference counter next to the value.  Copying the executor shares that block (cheap,
     * no value copy); the value is cloned lazily the first time it is reached for mutation
     * through a **non-const** executor while still shared (counter greater than one).
     * Access through a const executor never clones.
     *
     * Unlike a `std::shared_ptr`-based design this keeps the executor **one pointer wide**
     * (with an empty allocator), performs **one** allocation per distinct value, and lets
     * the reference counter type be chosen — `std::atomic` for a thread-safe count, or a
     * plain integer for single-threaded use with no atomic overhead.
     *
     * The clone is performed by @c guard().  It runs before any mutable access: @c execute()
     * calls it on the reflection dispatch path, and the locking utilities (@c wrapper_guard /
     * @c value_lock) call it on their guard phase.  @c access() itself is a pure accessor and
     * never clones; copy-on-write works because the dispatch resolves @c access() *inside*
     * @c execute(), after @c guard() has detached, so every reflected method call operates on
     * an object owned by no other copy.  (A direct, non-const @c access() that bypasses
     * @c execute()/@c guard() therefore does **not** clone; call @c guard() first, or reach
     * the value through a reflected call, to detach.)
     *
     * @tparam Value      the wrapped value type
     * @tparam Allocator  allocator for the control block; rebound internally (default
     *                    @c std::allocator<Value>)
     * @tparam Counter    reference counter type — any integer-like type supporting
     *                    pre-increment, pre-decrement and comparison; @c std::atomic for a
     *                    thread-safe count (default @c std::atomic<std::size_t>)
     *
     * @note Because an executor cannot observe whether the *method* being called mutates,
     *       any access through a non-const wrapper clones a shared value — including
     *       read-only methods.  Reach the value through a const wrapper (or const reference)
     *       to read without cloning.
     * @note A custom allocator is supplied by binding the template through a one-parameter
     *       alias, e.g. `template <typename T> using my = implicit::indirect<T, my_alloc<T>>;`
     *       then `wrapper<Value, my>`.  A stateful allocator instance can be passed at
     *       construction via the `std::allocator_arg` overload.
     * @warning The clone copy-constructs @c Value.  A polymorphic @c Value holding a derived
     *          object would be sliced; @c Value must be the concrete stored type.  Use
     *          @c implicit::polymorphic for value-semantic polymorphism.
     *
     * @code{.cpp}
     *    wrapper<std::vector<int>, feature::implicit::indirect> a{1, 2, 3};
     *    auto b = a;            // shares the allocation, no value copy
     *    b->push_back(4);       // b clones lazily; a still holds {1, 2, 3}
     * @endcode
     */
    // NOLINTBEGIN(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)
    template <typename Value, typename Allocator = ::std::allocator<Value>, typename Counter = ::std::atomic<::std::size_t>>
    class indirect
    {
        using self_type = indirect<Value, Allocator, Counter>;

    public:
        using value_type = Value;
        using allocator_type = Allocator;
        using counter_type = Counter;

    private:
        struct block
        {
            counter_type count;
            value_type value;
        };

        using block_allocator_type =
            typename ::std::allocator_traits<allocator_type>::template rebind_alloc<block>;
        using block_allocator_traits = ::std::allocator_traits<block_allocator_type>;

    public:
        /// @brief Constructs the held value in-place from @p args using a default allocator
        /// @tparam Args  argument types forwarded to @c Value
        /// @param  args  arguments forwarded to the @c Value constructor
        template <typename... Args>
        explicit indirect(Args &&... args)
            : m_allocator{}
            , m_block{construct_block(m_allocator, ::std::forward<Args>(args)...)}
        {}

        /// @brief Constructs the held value in-place from @p args using @p allocator
        /// @tparam Args      argument types forwarded to @c Value
        /// @param  allocator allocator used for the control block
        /// @param  args      arguments forwarded to the @c Value constructor
        template <typename... Args>
        explicit indirect(::std::allocator_arg_t /*tag*/, allocator_type const & allocator, Args &&... args)
            : m_allocator{allocator}
            , m_block{construct_block(m_allocator, ::std::forward<Args>(args)...)}
        {}

        SCL_INDIRECT_CONSTRUCTOR_FOR_SELF

        indirect & operator=(indirect &) = delete;
        indirect & operator=(indirect const &) = delete;
        indirect & operator=(indirect &&) = delete;
        indirect & operator=(indirect const &&) = delete;

        /// @brief Releases this owner's reference; frees the block when it was the last one
        ~indirect() { release(m_allocator, m_block); }

    public:
        /// @brief No-op guard for const executors — the read path never clones.
        ///
        /// @tparam Self  deduced executor type (const-qualified)
        template <typename Self>
        static void guard(Self &&) noexcept
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> &&
                ::std::is_const_v<::std::remove_reference_t<Self>>)
        {}

        /// @brief Detaches the held value — clones it when shared with another owner.
        ///
        /// The copy-on-write step.  Runs only for a non-const @p self; the const
        /// overload above is a no-op.
        ///
        /// @tparam Self  deduced executor type (non-const)
        /// @param  self  the executor instance to detach
        template <typename Self>
        static void guard(Self && self) /**/
            noexcept(noexcept(construct_block(::std::declval<allocator_type &>(),
                ::std::declval<value_type const &>())))
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> &&
                !::std::is_const_v<::std::remove_reference_t<Self>>)
        {
            if (self.m_block != nullptr && self.m_block->count > 1)
            {
                block * const fresh = construct_block(self.m_allocator, self.m_block->value);
                --self.m_block->count;
                self.m_block = fresh;
            }
        }

        /// @brief Accesses the held value with cv-ref qualifiers propagated from @p Self
        ///
        /// Pure accessor — never clones; detaching is done by @c guard() / @c execute().
        ///
        /// @tparam Self  deduced executor type (carries the desired cv-ref)
        /// @param  self  the executor instance
        /// @return reference to the held value, qualified like @p Self
        ///
        /// @note Not provided for @c volatile executors — @c std::atomic volatile-qualified
        ///       operations were deprecated in C++20 and non-atomic counters provide no
        ///       volatile synchronisation semantics.  Volatile reflected overloads are
        ///       SFINAE-discarded rather than hard-errored.
        template <typename Self>
        static decltype(auto) access(Self && self) noexcept
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> &&
                !::std::is_volatile_v<::std::remove_reference_t<Self>>)
        {
            return ::scl::forward_like<Self>(self.m_block->value);
        }

        /// @brief Assigns @p source to @p self, sharing or stealing the control block.
        ///
        /// Adopts @p source's block before releasing the old one — self-assign safe.
        /// For a non-const rvalue @p source the block is stolen (no ref-count bump);
        /// for lvalue or const sources the block is shared (ref count incremented).
        ///
        /// @tparam Self    deduced executor type (non-const, non-volatile)
        /// @tparam Source  deduced source type (same base as @p self_type or derived)
        /// @param  self    the executor instance to assign into
        /// @param  source  the source executor
        template <typename Self, typename Source>
        static void operator_assign(Self && self, Source && source)
            noexcept(::std::is_nothrow_copy_assignable_v<allocator_type>)
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> &&
                ::std::is_base_of_v<self_type, ::std::remove_cvref_t<Source>> &&
                !::std::is_const_v<::std::remove_reference_t<Self>> &&
                !::std::is_volatile_v<::std::remove_reference_t<Self>>)
        {
            block * const incoming = adopt(::std::forward<Source>(source));
            release(self.m_allocator, self.m_block);
            self.m_block = incoming;
            self.m_allocator = source.m_allocator;
        }

        /// @brief Detaches via @c guard() then invokes @p func with @p args
        ///
        /// @tparam Self  deduced executor type
        /// @tparam Func  callable type
        /// @tparam Args  argument types forwarded to @p func
        /// @param  self  the executor instance
        /// @param  func  callable to invoke
        /// @param  args  arguments forwarded to @p func
        /// @return result of @c ::std::invoke(func, args...)
        template <typename Self, typename Func, typename... Args>
        static decltype(auto) execute(Self && self, Func && func, Args &&... args) /**/
            noexcept(noexcept(guard(::std::declval<Self>())) &&
                noexcept(::std::invoke(::std::declval<Func>(), ::std::declval<Args>()...)))
            requires(::std::same_as<::std::remove_cvref_t<Self>, self_type> && ::std::invocable<Func, Args && ...>)
        {
            guard(self);
            return ::std::invoke(::std::forward<Func>(func), ::std::forward<Args>(args)...);
        }

    private:
        /// Shares (copy categories) or steals (non-const rvalue) @p other's block.
        template <typename Source>
        static block * adopt(Source && other) noexcept
        {
            block * const owned = other.m_block;
            if constexpr (::std::is_rvalue_reference_v<Source &&> &&
                !::std::is_const_v<::std::remove_reference_t<Source>>)
                other.m_block = nullptr;
            else if (owned != nullptr)
                ++owned->count;
            return owned;
        }

        /// Allocates one control block, sets the count to one and constructs the value.
        template <typename... Args>
        static block * construct_block(allocator_type const & allocator, Args &&... args)
        {
            block_allocator_type block_allocator{allocator};
            block * const owned = block_allocator_traits::allocate(block_allocator, 1U);
            try
            {
                block_allocator_traits::construct(block_allocator, ::std::addressof(owned->count),
                    ::std::size_t{1});
                try
                {
                    block_allocator_traits::construct(block_allocator,
                        ::std::addressof(owned->value), ::std::forward<Args>(args)...);
                }
                catch (...)
                {
                    block_allocator_traits::destroy(block_allocator, ::std::addressof(owned->count));
                    throw;
                }
            }
            catch (...)
            {
                block_allocator_traits::deallocate(block_allocator, owned, 1U);
                throw;
            }
            return owned;
        }

        /// Drops one reference; destroys and frees the block when the count reaches zero.
        static void release(allocator_type const & allocator, block * owned) noexcept
        {
            if (owned == nullptr)
                return;
            if (--owned->count == 0U)
            {
                block_allocator_type block_allocator{allocator};
                block_allocator_traits::destroy(block_allocator, ::std::addressof(owned->value));
                block_allocator_traits::destroy(block_allocator, ::std::addressof(owned->count));
                block_allocator_traits::deallocate(block_allocator, owned, 1U);
            }
        }

        SCL_NO_UNIQUE_ADDRESS
        allocator_type m_allocator;
        block * m_block;
    };
    // NOLINTEND(cppcoreguidelines-special-member-functions, cppcoreguidelines-missing-std-forward)
} // namespace scl::feature::implicit

#undef SCL_INDIRECT_CONSTRUCTOR_FOR_SELF
#undef SCL_INDIRECT_CONSTRUCTOR_FOR_SELF_PROTOTYPE
