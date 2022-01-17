#pragma once
#ifndef SCL_FEATURE_ACCESS_VALUE_POINTER_H
#define SCL_FEATURE_ACCESS_VALUE_POINTER_H

#include "ValueGuard.h"

namespace ScL { namespace Feature
{
    /*!
     * Указатель на экземпляр вложенного в Wrapper базового значения, к которому применены
     * все особенности, реализуемые посредством используемых Wrapper.
     */
    template < typename _WrapperRefer >
    class ValuePointer
    {
        template < typename, typename > friend class ::ScL::Feature::Detail::Wrapper;
        using ThisType = ValuePointer< _WrapperRefer >;

    public:
        using WrapperRefer = _WrapperRefer;
        using ValueGuard = ::ScL::Feature::ValueGuard< WrapperRefer >;
        using PointerAccess = typename ValueGuard::PointerAccess;
        using ValueAccess = typename ValueGuard::ValueAccess;

    private:
        ValueGuard m_value_guard;

    private:
        ValuePointer ( WrapperRefer refer )
            : m_value_guard( ::std::forward< WrapperRefer >( refer ) )
        {
        }

    public:
        ValuePointer ( ThisType && other )
            : m_value_guard( ::std::forward< ThisType && >( other ).m_value_guard )
        {}

        ValuePointer ( const ThisType & ) = delete;
        void operator = ( const ThisType & ) = delete;

        ValueAccess operator * () const
        {
            return m_value_guard.valueAccess();
        }

        PointerAccess operator -> () const
        {
            return m_value_guard.pointerAccess();
        }
    };
}}

#endif
