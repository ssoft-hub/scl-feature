#pragma once
#ifndef SCL_FEATURE_DETAIL_PROPERTY_REFLECTION_H
#define SCL_FEATURE_DETAIL_PROPERTY_REFLECTION_H

#include <scl/feature/access/wrapper_lock.h>
#include <scl/feature/wrapper.h>
#include <scl/utility/type_traits.h>

#include <utility>

namespace scl::feature::Tool
{
    template <typename WrapperHolder_>
    struct PropertyReflection
    {
        template <typename Value_>
        struct Holder
        {
            using ThisType = Holder<Value_>;
            using Value = Value_;

            using WrapperHolder = WrapperHolder_;
            using WrappedValue = typename WrapperHolder::Value;
            using Type = ::std::remove_reference_t<Value>;
            using WrappedType = ::std::remove_reference_t<WrappedValue>;
            using WrapperHolderPointer = WrapperHolder *;
            using Property = Type WrappedType::*;

            WrapperHolderPointer m_holder;
            Property m_property;

            constexpr Holder(WrapperHolderPointer holder, Property property)
                : m_holder{holder}
                , m_property{property}
            {}

            template <typename _HolderRefer>
            static constexpr void guard(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using WrapperHolderRefer = ::scl::SimilarRefer<WrapperHolder, HolderRefer>;
                using HolderInterface = ::scl::feature::detail::HolderInterface;
                HolderInterface::guard<WrapperHolderRefer>(
                    ::std::forward<WrapperHolderRefer>(*holder.m_holder));
            }

            template <typename _HolderRefer>
            static constexpr void unguard(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using WrapperHolderRefer = ::scl::SimilarRefer<WrapperHolder, HolderRefer>;
                using HolderInterface = ::scl::feature::detail::HolderInterface;
                HolderInterface::unguard<WrapperHolderRefer>(
                    ::std::forward<WrapperHolderRefer>(*holder.m_holder));
            }

            template <typename _HolderRefer>
            static constexpr decltype(auto) baseValue(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using WrapperHolderRefer = ::scl::SimilarRefer<WrapperHolder, HolderRefer>;
                using HolderInterface = ::scl::feature::detail::HolderInterface;
                return HolderInterface::value<WrapperHolderRefer>(
                    ::std::forward<WrapperHolderRefer>(*holder.m_holder));
            }

            template <typename _HolderRefer>
            static constexpr decltype(auto) value(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using WrappedValueRefer = ::scl::SimilarRefer<WrappedValue, HolderRefer>;
                using ValueRefer = ::scl::SimilarRefer<Value, WrappedValueRefer>;
                return ::std::forward<ValueRefer>(
                    baseValue(::std::forward<_HolderRefer>(holder)).*holder.m_property);
            }
        };
    };
} // namespace scl::feature::Tool

#define SCL_DECLTYPE_PROPERTY_P(property)                                                     \
    ::scl::feature::wrapper<decltype(::std::declval<typename SelfHolder_::Value>().property), \
        ::scl::feature::Tool::PropertyReflection<SelfHolder_>>

#define SCL_DECLTYPE_PROPERTY(property)                                                   \
    ::std::conditional_t<                                                                 \
        ::std::is_const_v<::std::remove_reference_t<typename SelfHolder_::Value>>,        \
        ::std::conditional_t<                                                             \
            ::std::is_volatile_v<::std::remove_reference_t<typename SelfHolder_::Value>>, \
            ::std::add_cv_t<SCL_DECLTYPE_PROPERTY_P(property)>,                           \
            ::std::add_const_t<SCL_DECLTYPE_PROPERTY_P(property)>>,                       \
        ::std::conditional_t<                                                             \
            ::std::is_volatile_v<::std::remove_reference_t<typename SelfHolder_::Value>>, \
            ::std::add_volatile_t<SCL_DECLTYPE_PROPERTY_P(property)>,                     \
            SCL_DECLTYPE_PROPERTY_P(property)>>

#define SCL_REFLECT_PROPERTY(property)                                                             \
public:                                                                                            \
    template <typename ___>                                                                        \
    static constexpr ::std::enable_if_t<::scl::feature::IsWrapper<___>::value,                     \
        decltype(&___::___private_##property##___)>                                                \
        ___addressof___##property##___()                                                           \
    {                                                                                              \
        return &___::___private_##property##___;                                                   \
    }                                                                                              \
                                                                                                   \
    template <typename ___>                                                                        \
    static constexpr ::std::enable_if_t<!::scl::feature::IsWrapper<___>::value,                    \
        decltype(&___::property)>                                                                  \
        ___addressof___##property##___()                                                           \
    {                                                                                              \
        return &___::property;                                                                     \
    }                                                                                              \
                                                                                                   \
    SCL_DECLTYPE_PROPERTY(property)                                                                \
    ___private_##property##___{std::addressof(static_cast<Self_ *>(this)->m_holder),               \
        ___addressof___##property##___<::std::remove_reference_t<typename SelfHolder_::Value>>()}; \
                                                                                                   \
    SCL_DECLTYPE_PROPERTY(property) & property{___private_##property##___};

/*
#define SCL_REFLECT_PROPERTY(property) \
    private: \
        SCL_DECLTYPE_PROPERTY(property) ___private_ ## property ## ___{
std::addressof(static_cast<Self_*>(this)->m_holder),  \
            &::std::remove_reference_t<typename SelfHolder_::Value>::property }; \
    public: \
        SCL_DECLTYPE_PROPERTY(property) & property{ ___private_ ## property ## ___ }; \
//*/

#endif
