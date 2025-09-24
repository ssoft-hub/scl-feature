#pragma once
#ifndef SCL_WRAPPER_TOOL_INPLACE_OPTIONAL_H
#define SCL_WRAPPER_TOOL_INPLACE_OPTIONAL_H

#include <ScL/Feature/Access/HolderGuard.h>
#include <ScL/Feature/MixIn.h>
#include <ScL/Utility/SimilarRefer.h>

#include <exception>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace ScL::Feature::Inplace
{
    /*!
     * Инструмент для формирования значения "по месту", которое
     * по умолчанию не инициализировано (содержит мусор).
     */
    struct Optional
    {
        class BadOptionalAccess : public ::std::exception
        {
        public:
            BadOptionalAccess() {}

            virtual ~BadOptionalAccess() noexcept = default;

            char const * what() const noexcept override { return "Bad optional access"; }
        };

        template <typename _Value>
        struct Holder
        {
            using ThisType = Holder<_Value>;
            using Value = _Value;

            struct Empty
            {};

            union
            {
                Empty m_empty;
                Value m_value;
            };

            bool m_is_exists{};

            constexpr Holder() {}

            template <typename... _Arguments>
            Holder(_Arguments &&... arguments)
            {
                construct(::std::forward<_Arguments>(arguments)...);
            }

            constexpr Holder(ThisType && other)
                noexcept(::std::is_nothrow_constructible_v<Value, Value &&>)
            {
                if (other.m_is_exists)
                {
                    construct(::std::move(other.m_value));
                    other.reset();
                }
            }

            constexpr Holder(ThisType const && other)
                noexcept(::std::is_nothrow_constructible_v<Value, Value const &&>)
            {
                construct(::std::forward<Value const>(other.m_value));
            }

            constexpr Holder(ThisType volatile && other)
                noexcept(::std::is_nothrow_constructible_v<Value, Value volatile &&>)
            {
                if (other.m_is_exists)
                {
                    construct(::std::move(other.m_value));
                    other.reset();
                }
            }

            constexpr Holder(ThisType const volatile && other)
                noexcept(::std::is_nothrow_constructible_v<Value, Value const volatile &&>)
            {
                construct(::std::forward<Value const volatile>(other.m_value));
            }

            constexpr Holder(ThisType & other)
                noexcept(::std::is_nothrow_constructible_v<Value, Value &>)
            {
                construct(other.m_value);
            }

            constexpr Holder(ThisType const & other)
                noexcept(::std::is_nothrow_constructible_v<Value, Value const &>)
            {
                construct(other.m_value);
            }

            constexpr Holder(ThisType volatile & other)
                noexcept(::std::is_nothrow_constructible_v<Value, Value volatile &>)
            {
                construct(other.m_value);
            }

            constexpr Holder(ThisType const volatile & other)
                noexcept(::std::is_nothrow_constructible_v<Value, Value const volatile &>)
            {
                construct(other.m_value);
            }

            template <typename _OtherValue>
            constexpr Holder(Holder<_OtherValue> && other)
                noexcept(::std::is_nothrow_constructible_v<Value, _OtherValue &&>)
            {
                if (other.m_is_exists)
                {
                    construct(::std::move(other.m_value));
                    other.reset();
                }
            }

            template <typename _OtherValue>
            constexpr Holder(Holder<_OtherValue> const && other)
                noexcept(::std::is_nothrow_constructible_v<Value, _OtherValue const &&>)
            {
                construct(::std::forward<typename Holder<_OtherValue const>::Value>(other.m_value));
            }

            template <typename _OtherValue>
            constexpr Holder(Holder<_OtherValue> volatile && other)
                noexcept(::std::is_nothrow_constructible_v<Value, _OtherValue volatile &&>)
            {
                if (other.m_is_exists)
                {
                    construct(::std::move(other.m_value));
                    other.reset();
                }
            }

            template <typename _OtherValue>
            constexpr Holder(Holder<_OtherValue> const volatile && other)
                noexcept(::std::is_nothrow_constructible_v<Value, _OtherValue const volatile &&>)
            {
                construct(::std::forward<typename Holder<_OtherValue const volatile>::Value>(
                    other.m_value));
            }

            template <typename _OtherValue>
            constexpr Holder(Holder<_OtherValue> & other)
                noexcept(::std::is_nothrow_constructible_v<Value, _OtherValue &>)
            {
                construct(other.m_value);
            }

            template <typename _OtherValue>
            constexpr Holder(Holder<_OtherValue> const & other)
                noexcept(::std::is_nothrow_constructible_v<Value, _OtherValue const &>)
            {
                construct(other.m_value);
            }

            template <typename _OtherValue>
            constexpr Holder(Holder<_OtherValue> volatile & other)
                noexcept(::std::is_nothrow_constructible_v<Value, _OtherValue volatile &>)
            {
                construct(other.m_value);
            }

            template <typename _OtherValue>
            constexpr Holder(Holder<_OtherValue> const volatile & other)
                noexcept(::std::is_nothrow_constructible_v<Value, _OtherValue const volatile &>)
            {
                construct(other.m_value);
            }

            ~Holder() { reset(); }

            template <typename... _Arguments>
            auto construct(_Arguments &&... _arguments)
                noexcept(::std::is_nothrow_constructible_v<Value, _Arguments &&...>)
                    -> ::std::enable_if_t<::std::is_constructible_v<Value, _Arguments...>, void>
            {
                static_assert(!::std::is_aggregate_v<::std::string>);
                if constexpr (::std::is_aggregate_v<_Value>)
                    ::new (static_cast<void *>(::std::addressof(m_value)))
                        Value{::std::forward<_Arguments>(_arguments)...};
                else
                    ::new (static_cast<void *>(::std::addressof(m_value)))
                        Value(::std::forward<_Arguments>(_arguments)...);

                //::std::construct_at<Value>( ::std::addressof( m_value ), ::std::forward<
                //:_Arguments >( _arguments ) ... );
                m_is_exists = true;
            }

            auto reset() noexcept(::std::is_nothrow_destructible_v<Value>)
                -> ::std::enable_if_t<::std::is_destructible_v<Value>, void>
            {
                if (m_is_exists)
                {
                    m_is_exists = false;
                    m_value.~Value();
                }
            }

            /*!
             * Access to internal value of Holder for any king of referencies.
             */
            template <typename _HolderRefer>
            static constexpr decltype(auto) value(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::ScL::SimilarRefer<Value, HolderRefer>;
                if (!holder.m_is_exists)
                    throw BadOptionalAccess{};
                return ::std::forward<ValueRefer>(holder.m_value);
            }
        };
    };
} // namespace ScL::Feature::Inplace

#define SCL_OPTIONAL_VALUE_METHOD(refer)                                                      \
    constexpr MixInValue refer value() refer                                                  \
    {                                                                                         \
        using WrapperRefer = MixInWrapper refer;                                              \
        using ValueRefer = MixInValue refer;                                                  \
        if (!static_cast<WrapperRefer>(*this).m_holder.m_is_exists)                           \
            throw ::ScL::Feature::Inplace::Optional::BadOptionalAccess{};                     \
        return ::std::forward<ValueRefer>(static_cast<WrapperRefer>(*this).m_holder.m_value); \
    }

#define SCL_OPTIONAL_VALUE                      \
    SCL_OPTIONAL_VALUE_METHOD(&)                \
    SCL_OPTIONAL_VALUE_METHOD(const &)          \
    SCL_OPTIONAL_VALUE_METHOD(volatile &)       \
    SCL_OPTIONAL_VALUE_METHOD(const volatile &) \
    SCL_OPTIONAL_VALUE_METHOD(&&)               \
    SCL_OPTIONAL_VALUE_METHOD(const &&)         \
    SCL_OPTIONAL_VALUE_METHOD(volatile &&)      \
    SCL_OPTIONAL_VALUE_METHOD(const volatile &&)

namespace ScL::Feature::Detail
{
    template <typename, typename>
    class Wrapper;
} // namespace ScL::Feature::Detail

namespace ScL::Feature
{
    template <typename _Value>
    class ToolAdditionMixIn<
        ::ScL::Feature::Detail::Wrapper<_Value, ::ScL::Feature::Inplace::Optional>>
    {
        using MixInValue = _Value;
        using Tool = ::ScL::Feature::Inplace::Optional;
        using MixInWrapper = ::ScL::Feature::Detail::Wrapper<_Value, Tool>;
        using MixInHolder = ::ScL::Feature::Inplace::Optional::Holder<_Value>;

    public:
        SCL_OPTIONAL_VALUE

        constexpr MixInValue const & valueOr(MixInValue const & value) const & noexcept
        {
            using WrapperRefer = MixInWrapper const &;
            if (!static_cast<WrapperRefer>(*this).m_holder.m_is_exists)
                return value;
            return static_cast<WrapperRefer>(*this).m_holder.m_value;
        }

        template <typename _Type,
            typename = ::std::enable_if_t<::std::is_constructible<MixInValue, _Type &&>::value>>
        constexpr MixInValue valueOr(_Type && value) const & noexcept
        {
            using WrapperRefer = MixInWrapper const &;
            if (!static_cast<WrapperRefer>(*this).m_holder.m_is_exists)
                return MixInValue{::std::forward<_Type>(value)};
            return static_cast<WrapperRefer>(*this).m_holder.m_value;
        }

        template <typename _Type,
            typename = ::std::enable_if_t<::std::is_constructible<MixInValue, _Type &&>::value>>
        MixInValue valueOr(_Type && value) && noexcept
        {
            using WrapperRefer = MixInWrapper &&;
            if (!static_cast<WrapperRefer>(*this).m_holder.m_is_exists)
                return MixInValue{::std::forward<_Type>(value)};
            return ::std::move(static_cast<WrapperRefer>(*this).m_holder.m_value);
        }

        constexpr void swap(MixInWrapper & other) noexcept
        {
            auto _this = static_cast<MixInWrapper *>(this);

            if (_this->m_holder.m_is_exists && other.m_holder.m_is_exists)
                ::std::swap(_this->m_holder.m_value, other.m_holder.m_value);
            else if (_this->m_holder.m_is_exists)
            {
                other.m_holder.construct(::std::move(_this->m_holder.m_value));
                _this->m_holder.reset();
            }
            else if (other.m_holder.m_is_exists)
            {
                _this->m_holder.construct(::std::move(other.m_holder.m_value));
                other.m_holder.reset();
            }
        }

        constexpr void reset() noexcept { static_cast<MixInWrapper *>(this)->m_holder.reset(); }

        template <typename... _Arguments>
        constexpr ::std::enable_if_t<::std::is_constructible<_Value, _Arguments &&...>::value,
            MixInValue &>
            emplace(_Arguments &&... arguments)
        {
            auto _this = static_cast<MixInWrapper *>(this);
            _this->m_holder.reset();
            _this->m_holder.construct(::std::forward<_Arguments>(arguments)...);
            return _this->m_holder.m_value;
        }

        template <typename _Type, typename... _Arguments>
        constexpr ::std::enable_if_t<::std::is_constructible<_Value, _Arguments &&...>::value,
            MixInValue &>
            emplace(std::initializer_list<_Type> list, _Arguments &&... arguments)
        {
            auto _this = static_cast<MixInWrapper *>(this);
            _this->m_holder.reset();
            _this->m_holder.construct(list, ::std::forward<_Arguments>(arguments)...);
            return _this->m_holder.m_value;
        }

        constexpr explicit operator bool() const noexcept
        {
            using WrapperRefer = MixInWrapper const &;
            return static_cast<WrapperRefer>(*this).m_holder.m_is_exists;
        }

        constexpr bool hasValue() const noexcept
        {
            using WrapperRefer = MixInWrapper const &;
            return static_cast<WrapperRefer>(*this).m_holder.m_is_exists;
        }
    };
} // namespace ScL::Feature

#undef SCL_OPTIONAL_VALUE
#undef SCL_OPTIONAL_VALUE_METHOD

namespace ScL::Feature
{
    template <typename Self_, typename SelfHolder_>
    class ToolReflectionMixIn<Self_, SelfHolder_, Inplace::Optional>
    {
    public:
        template <typename... Args_>
        decltype(auto) value(Args_ &&... /*args*/) const
        {
            return 0;
        }

        template <typename... Args_>
        decltype(auto) valueOr(Args_ &&... /*args*/) const
        {
            return 0;
        }
    };
} // namespace ScL::Feature

#endif
