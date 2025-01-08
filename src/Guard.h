#pragma once
#ifndef SCL_FEATURE_GUARDED_H
#define SCL_FEATURE_GUARDED_H

#include <ScL/Feature/Trait.h>
#include <ScL/Feature/Wrapper.h>
#include <ScL/Utility/SimilarRefer.h>
#include <ScL/Feature/Tool/Reference/Raw.h>

namespace ScL::Feature
{
    template <typename Type_>
    inline decltype(auto) guarded(Type_ && value) noexcept
    {
        if constexpr ( ::ScL::Feature::isWrapper< ::std::decay_t<Type_> >() )
        {
            using WrapperRefer = Type_ &&;
            using ValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< WrapperRefer >::Value, WrapperRefer >;

            if constexpr ( ::ScL::Feature::isWrapper< ::std::decay_t<ValueRefer> >() )
            {
                auto invokable = [](ValueRefer && value)
                {
                    return guarded<ValueRefer>( ::std::forward<ValueRefer>(value) );
                };

                using Invokable = decltype(invokable);
                using Returned = ::std::invoke_result_t< Invokable, ValueRefer >;

                return ::ScL::Feature::Detail::Operator::ResultSwitch<
                    ::ScL::Feature::Detail::Operator::LeftWrapperCase,
                    ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, ValueRefer > >
                        ::invoke( ::std::forward< Invokable >( invokable ), static_cast< WrapperRefer >( value) );
            }
            else
            {
                auto invokable = [](ValueRefer && value) -> ValueRefer &&
                {
                    return ::std::forward<ValueRefer>(value);
                };

                using Invokable = decltype(invokable);
                using Returned = ::std::invoke_result_t< Invokable, ValueRefer >;

                return ::ScL::Feature::Detail::Operator::ResultSwitch<
                    ::ScL::Feature::Detail::Operator::LeftWrapperCase,
                    ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, ValueRefer > >
                        ::invoke( ::std::forward< Invokable >( invokable ), static_cast< WrapperRefer >( value) );
            }
        }
        else
        {
            using Result = ::ScL::Feature::Wrapper<::std::remove_reference_t<Type_>, ScL::Feature::Reference::Raw>;
            return Result{value};
        }
    }
}


#endif
