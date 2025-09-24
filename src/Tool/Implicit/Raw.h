#pragma once
#ifndef SCL_WRAPPER_TOOL_IMPLICIT_RAW_H
#define SCL_WRAPPER_TOOL_IMPLICIT_RAW_H

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

#include <ScL/Feature/Access/HolderGuard.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Meta/Trait/Detection.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL::Feature::Implicit
{
    /*!
     * Инструмент для формирования значения в "куче" на основе raw указателя.
     * Реализует технику ленивых вычислений, когда копирование
     * экземпляра значения происходит только в момент доступа к неконстантному
     * экземпляру.
     */
    template <typename _Counter>
    struct CountedRaw
    {
        struct BaseCounted
        {
            using Counter = _Counter;
            using TypeId = std::size_t;

            Counter m_counter;

            BaseCounted()
                : m_counter()
            {}

            virtual ~BaseCounted() {}

            virtual TypeId typeId() const noexcept = 0;
            virtual BaseCounted * clone() const = 0;
        };

        template <typename _Value>
        struct Counted : public BaseCounted
        {
            using ThisType = Counted<_Value>;
            using ParentType = BaseCounted;

            using Value = _Value;
            using TypeId = typename ParentType::TypeId;

            Value m_value;

            template <typename... _Arguments>
            Counted(_Arguments &&... arguments)
                : BaseCounted{}
                , m_value(::std::forward<_Arguments>(arguments)...)
            {}

            virtual TypeId typeId() const noexcept override
            {
                static auto const type_id = typeid(Value).hash_code();
                return type_id;
            }

            virtual BaseCounted * clone() const override { return new ThisType(m_value); }
        };

        template <typename _Holder, bool abstract_case>
        struct ConstructHelper;

        template <typename _Holder>
        struct ConstructHelper<_Holder, true>
        {
            using Access = typename _Holder::Access;
            using CountedPointer = typename _Holder::CountedPointer;

            template <typename... _Arguments>
            static CountedPointer makePointer(_Arguments &&...)
            {
                return nullptr;
            }

            static Access access(CountedPointer) { return nullptr; }
        };

        template <typename _Holder>
        struct ConstructHelper<_Holder, false>
        {
            using Access = typename _Holder::Access;
            using CountedPointer = typename _Holder::CountedPointer;
            using CountedValue = typename _Holder::CountedValue;

            template <typename... _Arguments>
            static CountedPointer makePointer(_Arguments &&... arguments)
            {
                return new CountedValue(::std::forward<_Arguments>(arguments)...);
            }

            static Access access(CountedPointer pointer)
            {
                return ::std::addressof(static_cast<CountedValue *>(pointer)->m_value);
            }
        };

        template <typename _Value>
        struct Holder
        {
            using ThisType = Holder<_Value>;
            using Value = _Value;

            using CountedValue = Counted<Value>;
            using CountedPointer = BaseCounted *;
            using Access = Value *;
            using WritableGuard = ::ScL::Feature::HolderGuard<ThisType &>;

            struct Empty
            {};

            CountedPointer m_pointer;
            Access m_access; // to resolve multiple inheritance

            Holder(Empty)
                : m_pointer{}
                , m_access{}
            {}

            template <typename... _Arguments>
            Holder(_Arguments &&... arguments)
                : m_pointer{ConstructHelper<ThisType,
                      ::std::is_abstract<
                          Value>::value>::makePointer(::std::forward<_Arguments>(arguments)...)}
                , m_access{ConstructHelper<ThisType, ::std::is_abstract<Value>::value>::access(
                      m_pointer)}
            {
                increment();
            }

            Holder(ThisType && other)
                : m_pointer{::std::forward<CountedPointer>(other.m_pointer)}
                , m_access{::std::forward<Access>(other.m_access)}
            {
                other.m_pointer = nullptr;
                other.m_access = nullptr;
            }

            Holder(ThisType const && other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            Holder(ThisType volatile && other)
                : m_pointer{::std::forward<CountedPointer volatile>(other.m_pointer)}
                , m_access{::std::forward<Access volatile>(other.m_access)}
            {
                other.m_pointer = nullptr;
                other.m_access = nullptr;
            }

            Holder(ThisType const volatile && other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            Holder(ThisType & other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            Holder(ThisType const & other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            Holder(ThisType volatile & other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            Holder(ThisType const volatile & other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> && other)
                : m_pointer{::std::forward<typename Holder<_OtherValue>::CountedPointer>(
                      other.m_pointer)}
                , m_access{::std::forward<typename Holder<_OtherValue>::Access>(other.m_access)}
            {
                other.m_pointer = nullptr;
                other.m_access = nullptr;
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> const && other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> volatile && other)
                : m_pointer{::std::forward<typename Holder<_OtherValue>::CountedPointer volatile>(
                      other.m_pointer)}
                , m_access{
                      ::std::forward<typename Holder<_OtherValue>::Access volatile>(other.m_access)}
            {
                other.m_pointer = nullptr;
                other.m_access = nullptr;
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> const volatile && other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> & other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> const & other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> volatile & other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            template <typename _OtherValue>
            Holder(Holder<_OtherValue> const volatile & other)
                : m_pointer{other.m_pointer}
                , m_access{other.m_access}
            {
                increment();
            }

            ~Holder() { decrement(); }

            void increment()
            {
                if (m_pointer)
                    ++m_pointer->m_counter;
            }

            void decrement()
            {
                if (m_pointer && !(--m_pointer->m_counter))
                {
                    delete m_pointer;
                    m_pointer = nullptr;
                    m_access = nullptr;
                }
            }

            /*!
             * Assignment operation between compatible Holders. Specialization
             * of operation enabled if left is not constant reference and any
             * kind of right.
             */
            template <typename _LeftWrapperRefer,
                typename _RightWrapperRefer,
                typename = ::std::enable_if_t<
                    !::std::is_const< ::std::remove_reference_t<_LeftWrapperRefer> >::value
                    && ::ScL::Feature::IsThisCompatibleWithOther<
                        ::std::decay_t<_RightWrapperRefer>,
                        ::std::decay_t<_LeftWrapperRefer> >::value
                    && (::std::is_volatile< ::std::remove_reference_t<_LeftWrapperRefer> >::value
                        == ::std::is_volatile<
                            ::std::remove_reference_t<_RightWrapperRefer> >::value)> >
            static decltype(auto) operatorAssignment(
                _LeftWrapperRefer && left, _RightWrapperRefer && right)
            {
                auto && left_holder = ::ScL::Feature::Detail::wrapperHolder(
                    ::std::forward<_LeftWrapperRefer>(left));
                auto && right_holder = ::ScL::Feature::Detail::wrapperHolder(
                    ::std::forward<_RightWrapperRefer>(right));

                if (left_holder.m_pointer != right_holder.m_pointer)
                {
                    left_holder.decrement();
                    left_holder.m_pointer = right_holder.m_pointer;
                    left_holder.m_access = right_holder.m_access;
                    left_holder.increment();
                }

                return ::std::forward<_LeftWrapperRefer>(left);
            }

            /*!
             * Guard internal value of Holder for any not constant referencies.
             */
            template <typename _HolderRefer,
                typename = ::std::enable_if_t<
                    !::std::is_const< ::std::remove_reference_t<_HolderRefer> >::value> >
            static constexpr void guard(_HolderRefer && holder)
            {
                if (!!holder.m_pointer && holder.m_pointer->m_counter > 1)
                {
                    using Offset = ::std::intptr_t;
                    Offset offset = Offset(holder.m_access) - Offset(holder.m_pointer);
                    auto pointer = holder.m_pointer->clone();
                    holder.decrement();
                    holder.m_pointer = pointer;
                    holder.m_access = decltype(holder.m_access)(Offset(pointer) + offset);
                    holder.increment();
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
                return ::std::forward<ValueRefer>(*holder.m_access);
            }
        };

        template <typename _WrapperRefer>
        static bool isEmpty(_WrapperRefer && wrapper) noexcept
        {
            return !::ScL::Feature::Detail::wrapperHolder(::std::forward<_WrapperRefer>(wrapper))
                        .m_pointer;
        }

        template <typename _WrapperRefer>
        static typename ::std::decay_t<_WrapperRefer>::TypeId typeId(
            _WrapperRefer && wrapper) noexcept
        {
            if (isEmpty(::std::forward<_WrapperRefer>(wrapper)))
                return {};
            return ::ScL::Feature::Detail::wrapperHolder(::std::forward<_WrapperRefer>(wrapper))
                .m_pointer->typeId();
        }

        template <typename _TestWrapper, typename _WrapperRefer>
        static bool isA(_WrapperRefer && wrapper) noexcept
        {
            if (isEmpty(::std::forward<_WrapperRefer>(wrapper)))
                return false;

            using TestType = typename _TestWrapper::Holder::Value;
            static auto const test_type_id = typeid(TestType).hash_code();
            return test_type_id
                == ::ScL::Feature::Detail::wrapperHolder(::std::forward<_WrapperRefer>(wrapper))
                       .m_pointer->typeId();
        }

        template <typename _TestWrapper, typename _WrapperRefer>
        static bool isKindOf(_WrapperRefer && wrapper) noexcept
        {
            if (isEmpty(::std::forward<_WrapperRefer>(wrapper)))
                return false;

            using TestAccess = typename _TestWrapper::Holder::Access;
            return dynamic_cast<TestAccess>(
                ::ScL::Feature::Detail::wrapperHolder(::std::forward<_WrapperRefer>(wrapper))
                    .m_access);
        }

        template <typename _LeftWrapper, typename _RightWrapperRefer>
        static decltype(auto) staticCast(_RightWrapperRefer && right) noexcept
        {
            using LeftHolder = typename _LeftWrapper::Holder;
            using Empty = typename _LeftWrapper::Holder::Empty;

            LeftHolder left_holder{Empty{}};
            auto && right_holder = ::ScL::Feature::Detail::wrapperHolder(
                ::std::forward<_RightWrapperRefer>(right));

            if (left_holder.m_pointer != right_holder.m_pointer)
            {
                left_holder.decrement();
                left_holder.m_pointer = right_holder.m_pointer;
                left_holder.m_access = static_cast<decltype(left_holder.m_access)>(
                    right_holder.m_access);
                left_holder.increment();
            }

            return _LeftWrapper{left_holder};
        }

        template <typename _LeftWrapper, typename _RightWrapperRefer>
        static decltype(auto) dynamicCast(_RightWrapperRefer && right) noexcept
        {
            using LeftHolder = typename _LeftWrapper::Holder;
            using Empty = typename _LeftWrapper::Holder::Empty;

            LeftHolder left_holder{Empty{}};
            auto && right_holder = ::ScL::Feature::Detail::wrapperHolder(
                ::std::forward<_RightWrapperRefer>(right));

            if (left_holder.m_pointer != right_holder.m_pointer)
            {
                auto access = dynamic_cast<decltype(left_holder.m_access)>(right_holder.m_access);
                if (access)
                {
                    left_holder.decrement();
                    left_holder.m_pointer = right_holder.m_pointer;
                    left_holder.m_access = access;
                    left_holder.increment();
                }
            }

            return _LeftWrapper{left_holder};
        }
    };
} // namespace ScL::Feature::Implicit

namespace ScL::Feature::Implicit
{
    using Raw = ::ScL::Feature::Implicit::CountedRaw< ::std::atomic<int32_t> >;
} // namespace ScL::Feature::Implicit

#include <ScL/Feature/MixIn.h>

namespace ScL::Feature
{
    template <typename _Type>
    class ToolAdditionMixIn< ::ScL::Feature::Detail::Wrapper<_Type, ::ScL::Feature::Implicit::Raw> >
    {
        using Extended = ::ScL::Feature::Detail::Wrapper<_Type, ::ScL::Feature::Implicit::Raw>;

        auto & self() const { return *static_cast<Extended const *>(this); }

    public:
        using TypeId = ::std::size_t;

    public:
        bool isEmpty() const noexcept { return Extended::Tool::isEmpty(self()); }

        auto typeId() const noexcept { return Extended::Tool::typeId(self()); }

        template <typename _Other>
        bool isA() const noexcept
        {
            return Extended::Tool::template isA<_Other>(self());
        }

        template <typename _Other>
        bool isKindOf() const noexcept
        {
            return Extended::Tool::template isKindOf<_Other>(self());
        }

        template <typename _Other>
        _Other staticCast() const noexcept
        {
            return Extended::Tool::template staticCast<_Other>(self());
        }

        template <typename _Other>
        _Other dynamicCast() const noexcept
        {
            return Extended::Tool::template dynamicCast<_Other>(self());
        }
    };
} // namespace ScL::Feature
#endif
