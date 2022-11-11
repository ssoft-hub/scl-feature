#pragma once
#ifndef SCL_FEATURED_TRAITS_H
#define SCL_FEATURED_TRAITS_H

#include <type_traits>

namespace ScL { namespace Feature { namespace Inplace { struct Default; }}}
namespace ScL { namespace Feature { namespace Detail { template < typename, typename > class Wrapper; }}}


namespace ScL { namespace Feature
{
    template < typename _Test > struct IsWrapper;
    template < typename _Test > using is_wrapper = IsWrapper< _Test >; // snake style
    template < typename _Test > inline constexpr bool isWrapper () { return IsWrapper< _Test >{}; }
        //!< Признак того, что экземпляр данного типа является Wrapper.

    template < typename _Test, typename _Other > struct IsThisCompatibleWithOther;
    template < typename _Test, typename _Other > using is_this_compatible_with_other = IsThisCompatibleWithOther< _Test, _Other >;
    template < typename _Test, typename _Other > inline constexpr bool isThisCompatibleWithOther () { return IsThisCompatibleWithOther< _Test, _Other >{}; }
        //!< Признак совместимости типов.

    template < typename _Test, typename _Other > struct IsThisPartOfOther;
    template < typename _Test, typename _Other > using is_this_part_of_other = IsThisPartOfOther< _Test, _Other >;
    template < typename _Test, typename _Other > inline constexpr bool isThisPartOfOther () { return IsThisPartOfOther< _Test, _Other >{}; }
        //!< Признак вложенности одного типа в другой.

    template < typename _Type, typename _Other > struct IsSimilar;
    template < typename _Test, typename _Other > using is_similar = IsSimilar< _Test, _Other >;
    template < typename _Type, typename _Other > inline constexpr bool isSimilar () { return IsSimilar< _Type, _Other >{}; }
        //!< Признак подобия типов. Типы считаются подобными, если у них одинаковые
        /// признаки const/volatile и rvalue/lvalue.
}}

namespace ScL { namespace Feature
{
    template < typename > struct IsWrapper : public ::std::false_type {};
        //!< По умолчанию, типы не являются Wrapper.

    template < typename _Test, typename _Tool >
    struct IsWrapper< ::ScL::Feature::Detail::Wrapper< _Test, _Tool > > : public ::std::true_type {};
}}

namespace ScL { namespace Feature
{
    //! Типы являются совместимыми, если они одинаковые или тип _Test является
    /// производным от типа _Other.
    template < typename _Test, typename _Other >
    struct IsThisCompatibleWithOther
        : public ::std::bool_constant< ::ScL::Feature::IsSimilar< _Test, _Other >{}
            && ( ::std::is_same< ::std::decay_t< _Test >, ::std::decay_t< _Other > >{}
               || ::std::is_base_of< ::std::decay_t< _Other >, ::std::decay_t< _Test > >{} ) >
    {};

    //! Типы Wrapper являются совместимыми, если в них используется идентичный
    /// инструмент _Tool, и вложенные типы также являются совместимыми.
    template < typename _Test, typename _Other, typename _Tool >
    struct IsThisCompatibleWithOther< ::ScL::Feature::Detail::Wrapper< _Test, _Tool >, ::ScL::Feature::Detail::Wrapper< _Other, _Tool > >
       : public ::std::bool_constant< ::ScL::Feature::IsThisCompatibleWithOther< _Test, _Other >{} >
    {};
}}

namespace ScL { namespace Feature
{
    //! Типы, не являющиеся Wrapper, не могут быть вложенными.
    template < typename _Test, typename _Other >
    struct IsThisPartOfOther : public ::std::false_type {};

    //!< Один тип Wrapper является частью другого, если он совместим с любой
    /// вложенной частью другого.
    template < typename _Test, typename _TestTool, typename _Other, typename _OtherTool >
    struct IsThisPartOfOther< ::ScL::Feature::Detail::Wrapper< _Test, _TestTool >, ::ScL::Feature::Detail::Wrapper< _Other, _OtherTool > >
        : public ::std::bool_constant<
               ::ScL::Feature::IsThisCompatibleWithOther< ::ScL::Feature::Detail::Wrapper< _Test, _TestTool >, _Other >{}
            || ::ScL::Feature::IsThisPartOfOther< ::ScL::Feature::Detail::Wrapper< _Test, _TestTool >, _Other >{} >
    {};
}}

namespace ScL { namespace Feature
{
    template < typename _Type, typename _Other >
    struct IsSimilar
        : public ::std::bool_constant<
               ::std::is_rvalue_reference< _Type >{} == ::std::is_rvalue_reference< _Other >{}
            && ::std::is_lvalue_reference< _Type >{} == ::std::is_lvalue_reference< _Other >{}
            && ::std::is_const< ::std::remove_reference_t< _Type > >{} == ::std::is_const< ::std::remove_reference_t< _Other > >{}
            && ::std::is_volatile< ::std::remove_reference_t< _Type > >{} == ::std::is_volatile< ::std::remove_reference_t< _Other > >{} >
    {
    };
}}

#endif
