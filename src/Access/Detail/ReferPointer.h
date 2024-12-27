#pragma once

#include <memory>
#include <utility>

namespace ScL { namespace Feature { namespace Detail
{
    /*!
     * Указатель на ссылку экземпляра значения, который ведет себя как "сырой" указатель
     * (raw pointer), независимо от того переопределен ли оператор & у типа значения или нет.
     * По умолчанию значение указателя нулевое. В случае перемещения указателя,
     * производится операция swap.
     */
    template < typename _Refer >
    class ReferPointer
    {
        using ThisType = ReferPointer< _Refer >;

    public:
        using Refer = _Refer;
        using Value = ::std::remove_reference_t< Refer >;
        using RawPointer = ::std::add_pointer_t< Value >;

        static_assert( ::std::is_reference< Refer >::value, "The template parameter _Refer must to be a reference type." );

    private:
        Refer m_refer;

    private:
        ReferPointer ( ThisType && other ) = delete;
        ReferPointer ( const ThisType & other ) = delete;

    public:
        constexpr ReferPointer ( Refer refer )
            : m_refer{ refer }
        {
        }

        constexpr Refer operator * () const
        {
            return ::std::forward< Refer >( m_refer );
        }

        constexpr RawPointer operator -> () const
        {
            return ::std::addressof( m_refer );
        }
    };
}}}
