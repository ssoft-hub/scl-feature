#pragma once
#ifndef SCL_FEATURED_TRAITS_H
#define SCL_FEATURED_TRAITS_H

#include <scl/utility/type_traits.h>

#include <type_traits>

namespace scl
{
    template <typename _Type, typename _Refer>
    // using SimilarRefer = typename ::scl::detail::SimilarReferHelper<_Type, _Refer>::Type;
    using SimilarRefer = typename ::scl::forward_like_t<_Refer, _Type>;
} // namespace scl

namespace scl::feature::Inplace
{
    struct Default;
} // namespace scl::feature::Inplace

namespace scl::feature::detail
{
    template <typename, typename>
    class wrapper;
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    //! По умолчанию, типы не являются wrapper.
    template <typename>
    struct IsWrapper : ::std::false_type
    {};

    template <typename _Test, typename _Tool>
    struct IsWrapper< ::scl::feature::detail::wrapper<_Test, _Tool> > : ::std::true_type
    {};
} // namespace scl::feature::detail

namespace scl::feature
{
    //! Признак того, что тип является wrapper.
    template <typename _Test>
    using IsWrapper = ::scl::feature::detail::IsWrapper< ::std::remove_cv_t<_Test> >;

    template <typename _Test>
    inline constexpr bool isWrapper()
    {
        return ::scl::feature::IsWrapper<_Test>::value;
    }

    template <typename _Test>
    using is_wrapper = ::scl::feature::IsWrapper<_Test>;
    template <typename _Test>
    using is_wrapper_t = typename ::scl::feature::IsWrapper<_Test>::type;
    template <typename _Test>
    inline constexpr auto is_wrapper_v = ::scl::feature::IsWrapper<_Test>::value;
} // namespace scl::feature

namespace scl::feature::detail
{
    //! Типы являются совместимыми, если они одинаковые
    /// или тип _Test является производным от типа _Other.
    template <typename _Test, typename _Other>
    struct IsThisCompatibleWithOther
        : ::std::integral_constant<bool,
              ::std::is_same< ::std::remove_cv_t<_Test>, ::std::remove_cv_t<_Other> >::value
                  || ::std::is_base_of< ::std::remove_cv_t<_Other>,
                      ::std::remove_cv_t<_Test> >::value>
    {};

    //! Типы wrapper являются совместимыми, если в них используется идентичный
    /// инструмент _Tool, и вложенные типы также являются совместимыми.
    template <typename _Test, typename _Other, typename _Tool>
    struct IsThisCompatibleWithOther< ::scl::feature::detail::wrapper<_Test, _Tool>,
        ::scl::feature::detail::wrapper<_Other, _Tool> >
        : ::scl::feature::detail::IsThisCompatibleWithOther< ::std::remove_cv_t<_Test>,
              ::std::remove_cv_t<_Other> >
    {};
} // namespace scl::feature::detail

namespace scl::feature
{
    //! Признак совместимости типов.
    template <typename _Test, typename _Other>
    using IsThisCompatibleWithOther = ::scl::feature::detail::IsThisCompatibleWithOther<
        ::std::remove_cv_t<_Test>,
        ::std::remove_cv_t<_Other> >;

    template <typename _Test, typename _Other>
    inline constexpr bool isThisCompatibleWithOther()
    {
        return ::scl::feature::IsThisCompatibleWithOther<_Test, _Other>::value;
    }

    template <typename _Test, typename _Other>
    using is_this_compatible_with_other = ::scl::feature::IsThisCompatibleWithOther<_Test, _Other>;
    template <typename _Test, typename _Other>
    using is_this_compatible_with_other_t = typename ::scl::feature::IsThisCompatibleWithOther<
        _Test,
        _Other>::type;
    template <typename _Test, typename _Other>
    inline constexpr auto
        is_this_compatible_with_other_v = ::scl::feature::IsThisCompatibleWithOther<_Test,
            _Other>::value;
} // namespace scl::feature

namespace scl::feature::detail
{
    //! Типы, не являющиеся wrapper, не имеют вложенных частей
    template <typename _Test, typename _Other>
    struct IsThisCompatibleWithPartOfOther : ::std::false_type
    {};

    //! Тип является частью wrapper, если он совместим
    ///  с любой его вложенной частью
    template <typename _Test, typename _Other, typename _OtherTool>
    struct IsThisCompatibleWithPartOfOther<_Test,
        ::scl::feature::detail::wrapper<_Other, _OtherTool> >
        : ::std::integral_constant<bool,
              ::scl::feature::IsThisCompatibleWithOther< ::std::remove_cv_t<_Test>,
                  ::std::remove_cv_t<_Other> >::value>
    {};

    //! Один тип wrapper является частью другого wrapper,
    /// если он совместим с любой вложенной частью другого.
    template <typename _Test, typename _TestTool, typename _Other, typename _OtherTool>
    struct IsThisCompatibleWithPartOfOther< ::scl::feature::detail::wrapper<_Test, _TestTool>,
        ::scl::feature::detail::wrapper<_Other, _OtherTool> >
        : ::std::integral_constant<bool,
              ::scl::feature::IsThisCompatibleWithOther<
                  ::scl::feature::detail::wrapper<_Test, _TestTool>,
                  ::std::remove_cv_t<_Other> >::value
                  || ::scl::feature::detail::IsThisCompatibleWithPartOfOther<
                      ::scl::feature::detail::wrapper<_Test, _TestTool>,
                      ::std::remove_cv_t<_Other> >::value>
    {};
} // namespace scl::feature::detail

namespace scl::feature
{
    //! Признак совместимости типа с вложенным типом другого.
    template <typename _Test, typename _Other>
    using IsThisCompatibleWithPartOfOther = ::scl::feature::detail::IsThisCompatibleWithPartOfOther<
        ::std::remove_cv_t<_Test>,
        ::std::remove_cv_t<_Other> >;

    template <typename _Test, typename _Other>
    inline constexpr bool isThisCompatibleWithPartOfOther()
    {
        return ::scl::feature::IsThisCompatibleWithPartOfOther<_Test, _Other>::value;
    }

    template <typename _Test, typename _Other>
    using is_this_compatible_with_part_of_other = ::scl::feature::IsThisCompatibleWithPartOfOther<
        _Test,
        _Other>;
    template <typename _Test, typename _Other>
    using is_this_compatible_with_part_of_other_t =
        typename ::scl::feature::IsThisCompatibleWithPartOfOther<_Test, _Other>::type;
    template <typename _Test, typename _Other>
    inline constexpr auto
        is_this_compatible_with_part_of_other_v = ::scl::feature::IsThisCompatibleWithPartOfOther<
            _Test,
            _Other>::value;
} // namespace scl::feature

namespace scl::feature::detail
{
    //! Типы, не являющиеся wrapper, не могут быть вложенными
    template <typename _Test, typename _Other>
    struct IsPartOfThisCompatibleWithOther : ::std::false_type
    {};

    template <typename _Test, typename _TestTool, typename _Other>
    struct IsPartOfThisCompatibleWithOther< ::scl::feature::detail::wrapper<_Test, _TestTool>,
        _Other>
        : ::std::integral_constant<bool,
              ::scl::feature::IsThisCompatibleWithOther< ::std::remove_cv_t<_Test>,
                  ::std::remove_cv_t<_Other> >::value>
    {};

    //!< Один тип wrapper является частью другого, если он совместим с любой
    /// вложенной частью другого.
    template <typename _Test, typename _TestTool, typename _Other, typename _OtherTool>
    struct IsPartOfThisCompatibleWithOther< ::scl::feature::detail::wrapper<_Test, _TestTool>,
        ::scl::feature::detail::wrapper<_Other, _OtherTool> >
        : ::std::integral_constant<bool,
              ::scl::feature::IsThisCompatibleWithOther< ::std::remove_cv_t<_Test>,
                  ::scl::feature::detail::wrapper<_Other, _OtherTool> >::value
                  || ::scl::feature::detail::IsPartOfThisCompatibleWithOther<
                      ::std::remove_cv_t<_Test>,
                      ::scl::feature::detail::wrapper<_Other, _OtherTool> >::value>
    {};
} // namespace scl::feature::detail

namespace scl::feature
{
    //! Признак совместимости вложенного типа с другим.
    template <typename _Test, typename _Other>
    using IsPartOfThisCompatibleWithOther = ::scl::feature::detail::IsPartOfThisCompatibleWithOther<
        ::std::remove_cv_t<_Test>,
        ::std::remove_cv_t<_Other> >;

    template <typename _Test, typename _Other>
    inline constexpr bool isPartOfThisCompatibleWithOther()
    {
        return ::scl::feature::IsPartOfThisCompatibleWithOther<_Test, _Other>::value;
    }

    template <typename _Test, typename _Other>
    using is_part_of_this_compatible_with_other = ::scl::feature::IsPartOfThisCompatibleWithOther<
        _Test,
        _Other>;
    template <typename _Test, typename _Other>
    using is_part_of_this_compatible_with_other_t =
        typename ::scl::feature::IsPartOfThisCompatibleWithOther<_Test, _Other>::type;
    template <typename _Test, typename _Other>
    inline constexpr auto
        is_part_of_this_compatible_with_other_v = ::scl::feature::IsPartOfThisCompatibleWithOther<
            _Test,
            _Other>::value;
} // namespace scl::feature

namespace scl::feature
{
    //! Признак подобия типов. Типы считаются подобными, если у них одинаковые
    /// признаки const/volatile и rvalue/lvalue.
    template <typename _Test, typename _Other>
    struct IsSimilar
        : ::std::integral_constant<bool,
              ::std::is_rvalue_reference<_Test>::value
                  == ::std::is_rvalue_reference<_Other>::value && ::std::is_lvalue_reference<
                      _Test>::value
                  == ::std::is_lvalue_reference<_Other>::value && ::std::is_const<
                      ::std::remove_reference_t<_Test> >::value
                  == ::std::is_const< ::std::remove_reference_t<
                      _Other> >::value && ::std::is_volatile< ::std::remove_reference_t<_Test> >::value
                  == ::std::is_volatile< ::std::remove_reference_t<_Other> >::value>
    {};

    template <typename _Test, typename _Other>
    inline constexpr bool isSimilar()
    {
        return ::scl::feature::IsSimilar<_Test, _Other>::value;
    }

    template <typename _Test, typename _Other>
    using is_similar = ::scl::feature::IsSimilar<_Test, _Other>;
    template <typename _Test, typename _Other>
    using is_similar_t = typename ::scl::feature::IsSimilar<_Test, _Other>::type;
    template <typename _Test, typename _Other>
    inline constexpr auto is_similar_v = ::scl::feature::IsSimilar<_Test, _Other>::value;
} // namespace scl::feature

namespace scl::feature
{
    //! Признак совместимости типов по спецификатору. Типы считаются совместимыми,
    /// если можно _Other поставить слева, а _Test справа при преобразовании
    /// спецификаторов const/volatile.
    template <typename _Test, typename _Other>
    struct IsThisSpecifierCompatibleWithOther
        : ::std::integral_constant<bool,
              (!::std::is_const<_Other>::value
                  || (::std::is_const<_Other>::value && ::std::is_const<_Test>::value))
                  && (!::std::is_volatile<_Other>::value
                      || (::std::is_volatile<_Other>::value && ::std::is_volatile<_Test>::value))>
    {};

    template <typename _Test, typename _Other>
    inline constexpr bool isThisSpecifierCompatibleWithOther()
    {
        return ::scl::feature::IsThisSpecifierCompatibleWithOther<_Test, _Other>::value;
    }

    template <typename _Test, typename _Other>
    using is_this_specifier_compatible_with_other =
        ::scl::feature::IsThisSpecifierCompatibleWithOther<_Test, _Other>;
    template <typename _Test, typename _Other>
    using is_this_specifier_compatible_with_other_t =
        typename ::scl::feature::IsThisSpecifierCompatibleWithOther<_Test, _Other>::type;
    template <typename _Test, typename _Other>
    inline constexpr auto is_this_specifier_compatible_with_other_v =
        ::scl::feature::IsThisSpecifierCompatibleWithOther<_Test, _Other>::value;
} // namespace scl::feature

#endif
