#pragma once
#ifndef SCL_WRAPPER_RESOLVER_H
#define SCL_WRAPPER_RESOLVER_H

#include <ScL/Feature/Access/WrapperGuard.h>
#include <ScL/Feature/Access/ValueGuard.h>
#include <ScL/Feature/Trait.h>
#include <ScL/Utility/SimilarRefer.h>

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _Wrapper, typename _OtherRefer >
    struct WrapperResolverHelper;
}}}

/*!
 * Вычислитель необходимого способа предоставления доступа к экземпляру значения
 * посредством метода resolve().
 *
 * Если типы _Wrapper и _Other между собой являются совместимыми,
 * или _Other совместим с любой внутренней частью _Wrapper,
 *
 * то операция выполняется между внутренними представлениями Wrapper.
 * Если данный тип совместим с внутренней частью другого, то другой
 * "разрешается" до типа совместимого с этим. Во всех других случаях
 * другой "разрешается" до базового внутреннего значения.
 */
namespace ScL { namespace Feature { namespace Detail
{
    template < typename _Wrapper, typename _OtherRefer >
    using WrapperResolver = typename ::ScL::Feature::Detail::WrapperResolverHelper< _Wrapper, _OtherRefer >::Type;
}}}

namespace ScL { namespace Feature { namespace Detail
{
    template < typename _Wrapper, typename _OtherRefer >
    class WrapperCompatibleResolver;

    template < typename _Wrapper, typename _OtherRefer >
    class WrapperOtherPathOfThisResolver;

    template < typename _Wrapper, typename _OtherRefer >
    class WrapperThisPathOfOtherResolver;

    template < typename _Wrapper, typename _OtherRefer >
    class WrapperValueResolver;
}}}

/*
 * NOTE: Можно было бы реализовать случай преобразования одного вида Holder
 * в другой с помощью WrapperConvertResolver. Однако это нарушит инкапсуляцию
 * Holder и позволит получить доступ к его внутреннему содержимому,
 * что в свою очередь может изменить ожидаемое поведение.
 */
namespace ScL { namespace Feature { namespace Detail
{
    template < typename _Wrapper, typename _OtherRefer >
    struct WrapperResolverHelper
    {
        using Wrapper = _Wrapper;
        using OtherRefer = _OtherRefer;
        using OtherWrapper = ::std::decay_t< _OtherRefer >;

        using Type = ::std::conditional_t<
            ::ScL::Feature::IsThisCompatibleWithOther< OtherWrapper, Wrapper >{},
            ::ScL::Feature::Detail::WrapperCompatibleResolver< Wrapper, OtherRefer >,
            ::std::conditional_t<
                ::ScL::Feature::IsThisPartOfOther< OtherWrapper, Wrapper >{},
                ::ScL::Feature::Detail::WrapperOtherPathOfThisResolver< Wrapper, OtherRefer >,
                ::std::conditional_t<
                    ::ScL::Feature::IsThisPartOfOther< Wrapper, OtherWrapper >{},
                    ::ScL::Feature::Detail::WrapperThisPathOfOtherResolver< Wrapper, OtherRefer >,
                    ::ScL::Feature::Detail::WrapperValueResolver< Wrapper, OtherRefer > > > >;
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    /*!
     * Используется, если _Other совместим с _Wrapper.
     */
    template < typename _Wrapper, typename _OtherRefer >
    class WrapperCompatibleResolver
    {
    public:
        using Wrapper = _Wrapper;
        using OtherRefer = _OtherRefer;
        using OtherWrapper = ::std::decay_t< OtherRefer >;
        using OtherHolder = typename OtherWrapper::Holder;
        using OtherHolderRefer = ::ScL::SimilarRefer< OtherHolder, OtherRefer >;
        using AccessRefer = OtherHolderRefer;

    private:
        OtherRefer m_other_refer;

    public:
        WrapperCompatibleResolver ( OtherRefer other )
            : m_other_refer( ::std::forward< OtherRefer >( other ) )
        {
        }

        AccessRefer resolve () const
        {
            return ::ScL::Feature::Detail::wrapperHolder< OtherRefer >( ::std::forward< OtherRefer >( m_other_refer ) );
        }
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    /*!
     * Используется, если _Other совместим c вложенной частью _Wrapper.
     */
    template < typename _Wrapper, typename _OtherRefer >
    class WrapperOtherPathOfThisResolver
    {
    public:
        using Wrapper = _Wrapper;
        using OtherRefer = _OtherRefer;
        using AccessRefer = OtherRefer;

    private:
        OtherRefer m_other_refer;

    public:
        WrapperOtherPathOfThisResolver ( OtherRefer other )
            : m_other_refer( ::std::forward< OtherRefer >( other ) )
        {
        }

        AccessRefer resolve () const
        {
            return ::std::forward< AccessRefer >( m_other_refer );
        }
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    /*!
     * Используется, если _Wrapper совместим с вложенной частью _Other.
     */
    template < typename _Wrapper, typename _OtherRefer >
    class WrapperThisPathOfOtherResolver
    {
    public:
        using Wrapper = _Wrapper;
        using OtherRefer = _OtherRefer;
        using OtherWrapperGuard = ::ScL::Feature::Detail::WrapperGuard< OtherRefer >;
        using OtherWrapper = ::std::decay_t< OtherRefer >;
        using OtherValue = typename OtherWrapper::Value;
        using OtherValueRefer = ::ScL::SimilarRefer< OtherValue, OtherRefer >;
        using NextResolver = ::ScL::Feature::Detail::WrapperResolver< Wrapper, OtherValueRefer >;
        using AccessRefer = typename NextResolver::AccessRefer;

        static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "The template parameter _Wrapper must to be a Wrapper type!" );
        static_assert( ::std::is_reference< OtherRefer >{}, "The template parameter _OtherRefer must to be a reference type." );
        static_assert( ::ScL::Feature::isWrapper< OtherWrapper >(), "The template parameter _OtherRefer must to be a Wrapper type reference!" );
        static_assert( ::ScL::Feature::isSimilar< OtherRefer, OtherValueRefer >(), "The OtherRefer and OtherValueRefer must to be similar types!" );

    private:
        OtherWrapperGuard m_wrapper_guard;
        NextResolver m_next_resolver;

    public:
        WrapperThisPathOfOtherResolver ( OtherRefer other )
            : m_wrapper_guard( ::std::forward< OtherRefer >( other ) )
            , m_next_resolver( ::std::forward< OtherValueRefer >( m_wrapper_guard.wrapperAccess() ) )
        {
        }

        AccessRefer resolve () const
        {
            return ::std::forward< AccessRefer >( m_next_resolver.resolve() );
        }
    };
}}}

namespace ScL { namespace Feature { namespace Detail
{
    /*!
     * Используется, если _Wrapper не совместим с _Other.
     */
    template < typename _Wrapper, typename _OtherRefer >
    class WrapperValueResolver
    {
    public:
        using Wrapper = _Wrapper;
        using OtherRefer = _OtherRefer;
        using OtherValueGuard = ::ScL::Feature::ValueGuard< OtherRefer >;
        using AccessRefer = typename OtherValueGuard::ValueRefer;

        static_assert( ::ScL::Feature::isWrapper< Wrapper >(), "The template parameter _Wrapper must to be a wrapper!" );
        static_assert( ::std::is_reference< OtherRefer >{}, "The template parameter _OtherRefer must to be a reference type." );

    private:
        OtherValueGuard m_value_guard;

    public:
        WrapperValueResolver ( OtherRefer other )
            : m_value_guard( ::std::forward< OtherRefer >( other ) )
        {
        }

        AccessRefer resolve () const
        {
            return ::std::forward< AccessRefer >( m_value_guard.valueAccess() );
        }
    };
}}}

#endif
