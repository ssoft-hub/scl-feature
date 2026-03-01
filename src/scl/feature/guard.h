#pragma once
#ifndef SCL_FEATURE_GUARDED_H
#define SCL_FEATURE_GUARDED_H

#include <scl/feature/tool/reference/raw.h>
#include <scl/feature/trait.h>
#include <scl/feature/wrapper.h>
#include <scl/utility/type_traits.h>

namespace scl::feature
{
    template <typename Type_>
    inline decltype(auto) guarded(Type_ && value) noexcept
    {
        if constexpr (::scl::feature::isWrapper<::std::decay_t<Type_>>())
        {
            using WrapperRefer = Type_ &&;
            using ValueRefer = ::scl::SimilarRefer<typename ::std::decay_t<WrapperRefer>::Value,
                WrapperRefer>;

            if constexpr (::scl::feature::isWrapper<::std::decay_t<ValueRefer>>())
            {
                auto invokable = [](ValueRefer && value) {
                    return guarded<ValueRefer>(::std::forward<ValueRefer>(value));
                };

                using Invokable = decltype(invokable);
                using Returned = ::std::invoke_result_t<Invokable, ValueRefer>;

                return ::scl::feature::detail::Operator::ResultSwitch<
                    ::scl::feature::detail::Operator::LeftWrapperCase,
                    ::scl::feature::detail::Operator::ResultSwitchCase<Returned,
                        ValueRefer>>::invoke(::std::forward<Invokable>(invokable),
                    ::std::forward<WrapperRefer>(value));
            }
            else
            {
                auto invokable = [](ValueRefer && value) -> ValueRefer && {
                    return ::std::forward<ValueRefer>(value);
                };

                using Invokable = decltype(invokable);
                using Returned = ::std::invoke_result_t<Invokable, ValueRefer>;

                return ::scl::feature::detail::Operator::ResultSwitch<
                    ::scl::feature::detail::Operator::LeftWrapperCase,
                    ::scl::feature::detail::Operator::ResultSwitchCase<Returned,
                        ValueRefer>>::invoke(::std::forward<Invokable>(invokable),
                    ::std::forward<WrapperRefer>(value));
            }
        }
        else
        {
            using Result = ::scl::feature::wrapper<::std::remove_reference_t<Type_>,
                scl::feature::Reference::Raw>;
            return Result{value};
        }
    }
} // namespace scl::feature

#endif
