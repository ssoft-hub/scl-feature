#pragma once
#ifndef SCL_WRAPPER_RESOLVER_H
#define SCL_WRAPPER_RESOLVER_H

#include <scl/feature/access/value_guard.h>
#include <scl/feature/access/wrapper_guard.h>
#include <scl/feature/trait.h>
#include <scl/utility/type_traits.h>

namespace scl::feature::detail
{
    template <typename _LeftWrapper, typename _RightRefer>
    struct WrapperResolverHelper;
} // namespace scl::feature::detail

/*!
 * Вычислитель необходимого способа предоставления доступа к экземпляру значения
 * посредством метода resolve().
 *
 * Если типы _LeftWrapper и _RightRefer между собой являются совместимыми,
 * или _RightRefer совместим с любой внутренней частью _LeftWrapper,
 *
 * то операция выполняется между внутренними представлениями wrapper.
 * Если данный тип совместим с внутренней частью другого, то другой
 * "разрешается" до типа совместимого с этим. Во всех других случаях
 * другой "разрешается" до базового внутреннего значения.
 */
namespace scl::feature::detail
{
    template <typename _LeftWrapper, typename _RightRefer>
    using WrapperResolver = typename ::scl::feature::detail::WrapperResolverHelper<_LeftWrapper,
        _RightRefer>::Type;
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    template <typename _LeftWrapper, typename _RightRefer>
    class RightCompatibleWithLeftWrapperResolver;

    template <typename _LeftWrapper, typename _RightRefer>
    class RightCompatibleWithPartOfLeftWrapperResolver;

    template <typename _LeftWrapper, typename _RightRefer>
    class PartOfRightCompatibleWithLeftWrapperResolver;

    template <typename _LeftWrapper, typename _RightRefer>
    class ValueWrapperResolver;
} // namespace scl::feature::detail

/*
 * NOTE: Можно было бы реализовать случай преобразования одного вида Holder
 * в другой с помощью WrapperConvertResolver. Однако это нарушит инкапсуляцию
 * Holder и позволит получить доступ к его внутреннему содержимому,
 * что в свою очередь может изменить ожидаемое поведение.
 */
namespace scl::feature::detail
{
    template <typename _LeftWrapper, typename _RightRefer>
    struct WrapperResolverHelper
    {
        using LeftWrapper = _LeftWrapper;
        using RightRefer = _RightRefer;
        using RightWrapper = ::std::remove_reference_t<_RightRefer>;

        using Type = ::std::conditional_t<
            ::scl::feature::IsThisCompatibleWithOther<RightWrapper, LeftWrapper>::value,
            ::scl::feature::detail::RightCompatibleWithLeftWrapperResolver<LeftWrapper, RightRefer>,
            ::std::conditional_t<
                ::scl::feature::IsThisCompatibleWithPartOfOther<RightWrapper, LeftWrapper>::value,
                ::scl::feature::detail::RightCompatibleWithPartOfLeftWrapperResolver<LeftWrapper,
                    RightRefer>,
                ::std::conditional_t< ::scl::feature::IsPartOfThisCompatibleWithOther<RightWrapper,
                                          LeftWrapper>::value,
                    ::scl::feature::detail::PartOfRightCompatibleWithLeftWrapperResolver<LeftWrapper,
                        RightRefer>,
                    ::scl::feature::detail::ValueWrapperResolver<LeftWrapper, RightRefer> > > >;
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    /*!
     * Используется, если _Other совместим с _LeftWrapper.
     */
    template <typename _LeftWrapper, typename _RightRefer>
    class RightCompatibleWithLeftWrapperResolver
    {
    public:
        using LeftWrapper = _LeftWrapper;
        using RightRefer = _RightRefer;
        using RightWrapper = ::std::remove_reference_t<RightRefer>;
        using RightHolder = typename RightWrapper::Holder;
        using RightHolderRefer = ::scl::SimilarRefer<RightHolder, RightRefer>;
        using AccessRefer = RightHolderRefer;

    private:
        RightRefer m_right_refer;

    public:
        RightCompatibleWithLeftWrapperResolver(RightRefer Right)
            : m_right_refer(::std::forward<RightRefer>(Right))
        {}

        AccessRefer resolve() const
        {
            return ::scl::feature::detail::wrapperHolder<RightRefer>(
                ::std::forward<RightRefer>(m_right_refer));
        }
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    /*!
     * Используется, если _Right совместим c вложенной частью _LeftWrapper.
     */
    template <typename _LeftWrapper, typename _RightRefer>
    class RightCompatibleWithPartOfLeftWrapperResolver
    {
    public:
        using LeftWrapper = _LeftWrapper;
        using RightRefer = _RightRefer;
        using AccessRefer = RightRefer;

    private:
        RightRefer m_right_refer;

    public:
        RightCompatibleWithPartOfLeftWrapperResolver(RightRefer right)
            : m_right_refer(::std::forward<RightRefer>(right))
        {}

        AccessRefer resolve() const { return ::std::forward<AccessRefer>(m_right_refer); }
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    /*!
     * Используется, если _LeftWrapper совместим с вложенной частью _Right.
     */
    template <typename _LeftWrapper, typename _RightRefer>
    class PartOfRightCompatibleWithLeftWrapperResolver
    {
    public:
        using LeftWrapper = _LeftWrapper;
        using RightRefer = _RightRefer;
        using RightWrapperGuard = ::scl::feature::WrapperGuard<RightRefer>;
        using RightWrapper = ::std::remove_reference_t<RightRefer>;
        using RightValue = typename RightWrapper::Value;
        using RightValueRefer = ::scl::SimilarRefer<RightValue, RightRefer>;
        using NextResolver = ::scl::feature::detail::WrapperResolver<LeftWrapper, RightValueRefer>;
        using AccessRefer = typename NextResolver::AccessRefer;

        static_assert(::scl::feature::isWrapper<LeftWrapper>(),
            "The template parameter _LeftWrapper must to be a wrapper type!");
        static_assert(::std::is_reference<RightRefer>::value,
            "The template parameter _RightRefer must to be a reference type.");
        static_assert(::scl::feature::isWrapper<RightWrapper>(),
            "The template parameter _RightRefer must to be a wrapper type reference!");
        // static_assert( ::scl::feature::isSimilar< RightRefer, RightValueRefer >(), "The
        // RightRefer and RightValueRefer must to be similar types!" );

    private:
        RightWrapperGuard m_wrapper_guard;
        NextResolver m_next_resolver;

    public:
        PartOfRightCompatibleWithLeftWrapperResolver(RightRefer right)
            : m_wrapper_guard(::std::forward<RightRefer>(right))
            , m_next_resolver(::std::forward<RightValueRefer>(m_wrapper_guard.wrapperAccess()))
        {}

        AccessRefer resolve() const
        {
            return ::std::forward<AccessRefer>(m_next_resolver.resolve());
        }
    };
} // namespace scl::feature::detail

namespace scl::feature::detail
{
    /*!
     * Используется, если _LeftWrapper не совместим с _Right.
     */
    template <typename _LeftWrapper, typename _RightRefer>
    class ValueWrapperResolver
    {
    public:
        using LeftWrapper = _LeftWrapper;
        using RightRefer = _RightRefer;
        using RightValueGuard = ::scl::feature::ValueGuard<RightRefer>;
        using AccessRefer = typename RightValueGuard::ValueRefer;

        static_assert(::scl::feature::isWrapper<LeftWrapper>(),
            "The template parameter _LeftWrapper must to be a wrapper!");
        static_assert(::std::is_reference<RightRefer>::value,
            "The template parameter _RightRefer must to be a reference type.");
        static_assert(::std::is_reference<AccessRefer>::value,
            "The AccessRefer must to be a reference type.");

    private:
        RightValueGuard m_value_guard;

    public:
        ValueWrapperResolver(RightRefer right)
            : m_value_guard(::std::forward<RightRefer>(right))
        {}

        AccessRefer resolve() const
        {
            return ::std::forward<AccessRefer>(m_value_guard.valueAccess());
        }
    };
} // namespace scl::feature::detail

#endif
