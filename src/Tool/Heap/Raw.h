#pragma once
#ifndef SCL_WRAPPER_TOOL_HEAP_RAW_H
#define SCL_WRAPPER_TOOL_HEAP_RAW_H

#include <ScL/Feature/Access/HolderGuard.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>
#include <utility>

namespace ScL::Feature::Heap
{

    /*!
     * Инструмент для формирования значения в "куче" на основе raw указателя.
     */
    struct Raw
    {
        template <typename _Value>
        struct Holder
        {
            using ThisType = Holder<_Value>;
            using Value = _Value;
            using Pointer = _Value *;

            Pointer m_pointer;

            template <typename... _Arguments>
            Holder(_Arguments &&... arguments)
                : m_pointer{new Value{::std::forward<_Arguments>(arguments)...}}
            {}

            Holder(ThisType && other)
                : m_pointer{::std::forward<Pointer>(other.m_pointer)}
            {
                other.m_pointer = nullptr;
            }

            Holder(ThisType const && other)
                : Holder(::std::forward<Value const>(*other.m_pointer))
            {}

            Holder(ThisType volatile && other)
                : m_pointer{::std::forward<Pointer volatile>(other.m_pointer)}
            {
                other.m_pointer = nullptr;
            }

            Holder(ThisType const volatile && other)
                : Holder{::std::forward<Value const volatile>(*other.m_pointer)}
            {}

            Holder(ThisType & other)
                : Holder{*other.m_pointer}
            {}

            Holder(ThisType const & other)
                : Holder{*other.m_pointer}
            {}

            Holder(ThisType volatile & other)
                : Holder{*other.m_pointer}
            {}

            Holder(ThisType const volatile & other)
                : Holder{*other.m_pointer}
            {}

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> && other)
                : m_pointer{::std::forward<typename Holder<_OtherValue>::Pointer>(other.m_pointer)}
            {
                other.m_pointer = nullptr;
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> const && other)
                : Holder{*other.m_pointer}
            {}

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> volatile && other)
                : m_pointer{::std::forward<typename Holder<_OtherValue>::Pointer volatile>(
                      other.m_pointer)}
            {
                other.m_pointer = nullptr;
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> const volatile && other)
                : Holder{*other.m_pointer}
            {}

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> & other)
                : Holder{*other.m_pointer}
            {}

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> const & other)
                : Holder{*other.m_pointer}
            {}

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> volatile & other)
                : Holder{*other.m_pointer}
            {}

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> const volatile & other)
                : Holder{*other.m_pointer}
            {}

            ~Holder() { delete m_pointer; }

            /*!
             * Assignment operation between compatible Holders. Specialization
             * of operation enabled if left is not constant any reference and
             * right is not constantrvalue reference.
             */
            template <typename _LeftWrapperRefer,
                typename _RightWrapperRefer,
                typename = ::std::enable_if_t<
                    !::std::is_const< ::std::remove_reference_t<_LeftWrapperRefer> >::value
                    && !::std::is_const< ::std::remove_reference_t<_RightWrapperRefer> >::value
                    && ::ScL::Feature::IsThisCompatibleWithOther<
                        ::std::decay_t<_RightWrapperRefer>,
                        ::std::decay_t<_LeftWrapperRefer> >::value
                    && (::std::is_volatile< ::std::remove_reference_t<_LeftWrapperRefer> >::value
                        == ::std::is_volatile<
                            ::std::remove_reference_t<_RightWrapperRefer> >::value)
                    && ::std::is_rvalue_reference<_RightWrapperRefer &&>::value> >
            static decltype(auto) operatorAssignment(
                _LeftWrapperRefer && left, _RightWrapperRefer && right)
            {
                ::std::swap(::ScL::Feature::Detail::wrapperHolder(left).m_pointer,
                    ::ScL::Feature::Detail::wrapperHolder(right).m_pointer);
                return ::std::forward<_LeftWrapperRefer>(left);
            }

            //! Access to internal value of Holder for any king of referencies.
            template <typename _HolderRefer>
            static constexpr decltype(auto) value(_HolderRefer && holder)
            {
                using HolderRefer = _HolderRefer &&;
                using ValueRefer = ::ScL::SimilarRefer<Value, HolderRefer>;
                return ::std::forward<ValueRefer>(*holder.m_pointer);
            }
        };
    };
} // namespace ScL::Feature::Heap

#endif
