#pragma once
#ifndef SCL_FEATURE_CASTER_H
#define SCL_FEATURE_CASTER_H

#include <scl/feature/access/value_lock.h>
#include <scl/feature/trait.h>
#include <scl/feature/wrapper.h>
#include <scl/utility/type_traits.h>
#include <utility>

#define ConceptCaster typename
#define ConceptReference typename
#define ConceptNonReference typename
#define ConceptWrapper typename
#define ConceptNonWrapper typename

namespace scl::feature::detail
{

    template <ConceptCaster Caster_, ConceptReference Ref_>
    struct CastMixIn
    {
        operator Ref_() && { return static_cast<Caster_ &&>(*this).template reference<Ref_>(); }
    };

#define SCL_FEATURE_CAST_MIXIN(ref)                                                                \
    template <typename Caster_, typename Type_, typename Tool_>                                    \
    struct CastMixIn<Caster_, wrapper<Type_, Tool_> ref> : public CastMixIn<Caster_, Type_ ref>    \
    {                                                                                              \
        operator wrapper<Type_, Tool_> ref() &&                                                    \
        {                                                                                          \
            return static_cast<Caster_ &&>(*this).template reference<wrapper<Type_, Tool_> ref>(); \
        }                                                                                          \
    };

    SCL_FEATURE_CAST_MIXIN(&)
    SCL_FEATURE_CAST_MIXIN(const &)
    SCL_FEATURE_CAST_MIXIN(volatile &)
    SCL_FEATURE_CAST_MIXIN(const volatile &)
    SCL_FEATURE_CAST_MIXIN(&&)
    SCL_FEATURE_CAST_MIXIN(const &&)
    SCL_FEATURE_CAST_MIXIN(volatile &&)
    SCL_FEATURE_CAST_MIXIN(const volatile &&)
} // namespace scl::feature::detail

namespace scl::feature::detail
{

    template <ConceptReference Ref_>
    struct Caster : public ::scl::feature::detail::CastMixIn<Caster<Ref_>, Ref_>
    {
        static_assert(::std::is_reference_v<Ref_>, "Ref_ must be a reference type.");

        using Refer = Ref_;
        using Locker = ::scl::feature::ValueLock<Refer>;

        Locker m_locker;

        Caster(Refer refer)
            : m_locker{::std::forward<Refer>(refer)}
        {}

        template <ConceptReference Type_>
        Type_ reference() &&
        {
            m_locker.template lockFor<Type_>();
            return m_locker.template valueAccessFor<Type_>();
        }
    };
} // namespace scl::feature::detail

namespace scl::feature
{
    template <ConceptNonWrapper Type_>
    inline auto cast(Type_ && value)
        -> ::std::enable_if_t<!::scl::feature::isWrapper<::std::remove_reference_t<Type_>>(),
            Type_ &&>
    {
        return ::std::forward<Type_ &&>(value);
    }

    template <ConceptWrapper Wrapper_>
    inline auto cast(Wrapper_ && wrapper)
        -> ::std::enable_if_t<::scl::feature::isWrapper<::std::remove_reference_t<Wrapper_>>(),
            ::scl::feature::detail::Caster<Wrapper_ &&>>
    {
        return ::scl::feature::detail::Caster<Wrapper_ &&>(::std::forward<Wrapper_ &&>(wrapper));
    }
} // namespace scl::feature

#endif
