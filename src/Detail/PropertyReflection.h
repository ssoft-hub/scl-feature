#pragma once
#ifndef SCL_FEATURE_DETAIL_PROPERTY_REFLECTION_H
#define SCL_FEATURE_DETAIL_PROPERTY_REFLECTION_H

#include <ScL/Feature/Access/WrapperLock.h>
#include <ScL/Feature/Wrapper.h>
#include <ScL/Utility/SimilarRefer.h>

#include <utility>

namespace ScL::Feature::Tool
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
                using WrapperHolderRefer = ::ScL::SimilarRefer<WrapperHolder, HolderRefer>;
                using HolderInterface = ::ScL::Feature::Detail::HolderInterface;
                HolderInterface::guard<WrapperHolderRefer>(
                    ::std::forward<WrapperHolderRefer>(*holder.m_holder));
            }

            template <typename _HolderRefer>
            static constexpr void unguard(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using WrapperHolderRefer = ::ScL::SimilarRefer<WrapperHolder, HolderRefer>;
                using HolderInterface = ::ScL::Feature::Detail::HolderInterface;
                HolderInterface::unguard<WrapperHolderRefer>(
                    ::std::forward<WrapperHolderRefer>(*holder.m_holder));
            }

            template <typename _HolderRefer>
            static constexpr decltype(auto) baseValue(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using WrapperHolderRefer = ::ScL::SimilarRefer<WrapperHolder, HolderRefer>;
                using HolderInterface = ::ScL::Feature::Detail::HolderInterface;
                return HolderInterface::value<WrapperHolderRefer>(
                    ::std::forward<WrapperHolderRefer>(*holder.m_holder));
            }

            template <typename _HolderRefer>
            static constexpr decltype(auto) value(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using WrappedValueRefer = ::ScL::SimilarRefer<WrappedValue, HolderRefer>;
                using ValueRefer = ::ScL::SimilarRefer<Value, WrappedValueRefer>;
                return ::std::forward<ValueRefer>(
                    baseValue(::std::forward<_HolderRefer>(holder)).*holder.m_property);
            }
        };
    };
} // namespace ScL::Feature::Tool

#define SCL_DECLTYPE_PROPERTY_P(property)                                                     \
    ::ScL::Feature::Wrapper<decltype(::std::declval<typename SelfHolder_::Value>().property), \
        ::ScL::Feature::Tool::PropertyReflection<SelfHolder_>>

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
    static constexpr ::std::enable_if_t<::ScL::Feature::IsWrapper<___>::value,                     \
        decltype(&___::___private_##property##___)>                                                \
        ___addressof___##property##___()                                                           \
    {                                                                                              \
        return &___::___private_##property##___;                                                   \
    }                                                                                              \
                                                                                                   \
    template <typename ___>                                                                        \
    static constexpr ::std::enable_if_t<!::ScL::Feature::IsWrapper<___>::value,                    \
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
