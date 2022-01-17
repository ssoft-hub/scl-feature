#pragma once
#ifndef SCL_FEATURED_TRAITS_H
#define SCL_FEATURED_TRAITS_H

#include <type_traits>

namespace ScL { namespace Feature { namespace Inplace { struct Default; }}}
namespace ScL { namespace Feature { namespace Detail { template < typename, typename > class Wrapper; }}}


namespace ScL { namespace Feature
{
    template < typename _Test > struct IsWrapper;
    template < typename _Test > /*inline*/ constexpr bool is_wrapper = IsWrapper< _Test >::value;
    template < typename _Test > inline constexpr bool isWrapper () { return IsWrapper< _Test >::value; }
        //!< Признак того, что экземпляр данного типа является Wrapper.

    template < typename _Test, typename _Other > struct IsCompatible;
    template < typename _Test, typename _Other > /*inline*/ constexpr bool is_compatible = IsCompatible< _Test, _Other >::value;
    template < typename _Test, typename _Other > inline constexpr bool isCompatible () { return IsCompatible< _Test, _Other >::value; }
        //!< Признак совместимости типов.

    template < typename _Test, typename _Other > struct IsThisPartOfOther;
    template < typename _Test, typename _Other > /*inline*/ constexpr bool is_this_part_of_other = IsThisPartOfOther< _Test, _Other >::value;
    template < typename _Test, typename _Other > inline constexpr bool isThisPartOfOther () { return IsThisPartOfOther< _Test, _Other >::value; }
        //!< Признак вложенности одного типа в другой.

    template < typename _Type, typename _Other > struct IsSimilar;
    template < typename _Type, typename _Other > /*inline*/ constexpr bool is_similar = IsSimilar< _Type, _Other >::value;
    template < typename _Type, typename _Other > inline constexpr bool isSimilar () { return IsSimilar< _Type, _Other >::value; }
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
    //! Типы являются совместимыми, если они одинаковые тип _Test является
    /// производным от другого _Other.
    template < typename _Test, typename _Other >
    struct IsCompatible
        : public ::std::integral_constant< bool, ::ScL::Feature::is_similar< _Test, _Other >
            && ( ::std::is_same< ::std::decay_t< _Test >, ::std::decay_t< _Other > >::value
               || ::std::is_base_of< ::std::decay_t< _Test >, ::std::decay_t< _Other > >::value ) >
    {};

    //! Типы Wrapper являются совместимыми, если в них используется идентичный
    /// инструмент _Tool, и вложенные типы также являются совместимыми.
    template < typename _Test, typename _Other, typename _Tool >
    struct IsCompatible< ::ScL::Feature::Detail::Wrapper< _Test, _Tool >, ::ScL::Feature::Detail::Wrapper< _Other, _Tool > >
       : public ::std::integral_constant< bool, ::ScL::Feature::is_compatible< _Test, _Other > >
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
        : public ::std::integral_constant< bool,
               ::ScL::Feature::is_compatible< ::ScL::Feature::Detail::Wrapper< _Test, _TestTool >, _Other >
            || ::ScL::Feature::is_this_part_of_other< ::ScL::Feature::Detail::Wrapper< _Test, _TestTool >, _Other > >
    {};
}}

namespace ScL { namespace Feature
{
    template < typename _Type, typename _Other >
    struct IsSimilar
        : public ::std::integral_constant< bool,
               ::std::is_rvalue_reference< _Type >::value == ::std::is_rvalue_reference< _Other >::value
            && ::std::is_lvalue_reference< _Type >::value == ::std::is_lvalue_reference< _Other >::value
            && ::std::is_const< ::std::remove_reference_t< _Type > >::value == ::std::is_const< ::std::remove_reference_t< _Other > >::value
            && ::std::is_volatile< ::std::remove_reference_t< _Type > >::value == ::std::is_volatile< ::std::remove_reference_t< _Other > >::value >
    {
    };
}}

#endif
