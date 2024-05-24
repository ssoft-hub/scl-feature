#include <ScL/Feature.h>
#include <iostream>
#include <type_traits>

#if defined( __GNUC__ )
#   include <cxxabi.h>
#endif

using namespace ::ScL::Feature;

template < typename _Type >
void printTypeOf ()
{
#if defined( __GNUC__ )
    int status = 0;
    char * realname = abi::__cxa_demangle( typeid( _Type ).name(), nullptr, nullptr, &status );
#elif defined( _MSC_VER )
    const char * realname = typeid( _Type ).name();
#endif

    ::std::cout
        << realname;

    if ( ::std::is_const< _Type >::value )
        ::std::cout
            << " const";

    if ( ::std::is_lvalue_reference< _Type >::value )
        ::std::cout
            << " &";

    if ( ::std::is_rvalue_reference< _Type >::value )
        ::std::cout
            << " &&";

    ::std::cout
        << ::std::endl << ::std::flush;

#if defined( __GNUC__ )
    if ( status )
        free( realname );
#endif
}

class Stream
{
public:
//    template < typename _Type >
//    Stream & operator << ( const _Type & ) { return *this; }

    Stream & operator << ( int ) { return *this; }
};

#define SCL_BINARY_OPERATOR_IMPLEMENTAION( symbol, Invokable ) \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Binary \
            { \
                SCL_DOES_BINARY_OPERATOR_EXIST_TRAIT( SCL_SINGLE_ARG( symbol ), Invokable ) \
                SCL_DOES_METHOD_EXIST_TRAIT( operator ## Invokable ) \
                SCL_DOES_METHOD_EXIST_TRAIT( operator ## Invokable ## Left ) \
                SCL_DOES_METHOD_EXIST_TRAIT( operator ## Invokable ## Right ) \
                SCL_DOES_BINARY_OPERATOR_EXIST_TEST_TRAIT( Invokable ) \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Global \
            { \
                struct Invokable ## Case; \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Binary \
            { \
                template < typename > \
                struct Invokable ## Operator; \
                 \
                template <> \
                struct Invokable ## Operator< ::ScL::Feature::Detail::Operator::Global::DefaultCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftRefer = _Left &&; \
                        using RightRefer = _Right &&; \
                        return ::std::forward< LeftRefer >( left ) symbol ::std::forward< RightRefer >( right ); \
                    } \
                }; \
                 \
                struct Invokable \
                { \
                    template < typename _Left, typename _Right > \
                    decltype(auto) operator () ( _Left && left, _Right && right ) \
                    { \
                        using LeftRefer = _Left &&; \
                        using RightRefer = _Right &&; \
                        using Invokable ## OperatorSwitchCase = ::std::conditional_t< \
                            ::ScL::Feature::isWrapper< ::std::decay_t< RightRefer > >(), \
                                ::ScL::Feature::Detail::Operator::Global::Invokable ## Case, \
                                ::ScL::Feature::Detail::Operator::Global::DefaultCase >; \
                        return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Operator< Invokable ## OperatorSwitchCase >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                    } \
                }; \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Binary \
            { \
                template < typename, typename > \
                struct Invokable ## Switch; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftWrapperRefer = _Left &&; \
                        using LeftHolder = typename ::std::decay_t< LeftWrapperRefer >::Holder; \
                        using RightRefer = _Right &&; \
                        LeftHolder::operator ## Invokable ## Left( ::ScL::Feature::Detail::wrapperHolder( ::std::forward< LeftWrapperRefer >( left ) ), ::std::forward< RightRefer >( right ) ); \
                        return ::std::forward< LeftWrapperRefer >( left ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftWrapperRefer = _Left &&; \
                        using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >; \
                        using RightRefer = _Right &&; \
                        using Returned = ::std::result_of_t< ::ScL::Feature::Detail::Operator::Binary::Invokable( LeftValueRefer, RightRefer ) >; \
                        return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::LeftWrapperCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, LeftValueRefer > > \
                            ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftRefer = _Left &&; \
                        using RightWrapperRefer = _Right &&; \
                        using RightHolder = typename ::std::decay_t< RightWrapperRefer >::Holder; \
                        RightHolder::operator ## Invokable ## Right( ::std::forward< LeftRefer >( left ), ::ScL::Feature::Detail::wrapperHolder( ::std::forward< RightWrapperRefer >( right ) ) ); \
                        return ::std::forward< RightWrapperRefer >( right ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftRefer = _Left &&; \
                        using RightWrapperRefer = _Right &&; \
                        using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >; \
                        using Returned = ::std::result_of_t< ::ScL::Feature::Detail::Operator::Binary::Invokable( LeftRefer, RightValueRefer ) >; \
                        return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::RightWrapperCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, RightValueRefer > > \
                            ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::BothWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftWrapperRefer = _Left &&; \
                        using LeftHolder = typename ::std::decay_t< LeftWrapperRefer >::Holder; \
                        using RightWrapperRefer = _Right &&; \
                        LeftHolder::operator ## Invokable ( ::ScL::Feature::Detail::wrapperHolder( ::std::forward< LeftWrapperRefer >( left ) ), ::ScL::Feature::Detail::wrapperHolder( ::std::forward< RightWrapperRefer >( right ) ) ); \
                        return ::std::forward< RightWrapperRefer >( right ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::BothWrapperCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftWrapperRefer = _Left &&; \
                        using RightWrapperRefer = _Right &&; \
                        return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Switch< ::ScL::Feature::Detail::Operator::ExposingSwitchCase< LeftWrapperRefer, RightWrapperRefer >, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
                            ::invoke( ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::BothExposingCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftWrapperRefer = _Left &&; \
                        using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >; \
                        using RightWrapperRefer = _Right &&; \
                        using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >; \
         \
                        using Returned = ::std::result_of_t< ::ScL::Feature::Detail::Operator::Binary::Invokable( LeftValueRefer, RightValueRefer ) >; \
                        return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::BothExposingCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, LeftValueRefer > > \
                            ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::LeftExposingCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftWrapperRefer = _Left &&; \
                        using LeftValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< LeftWrapperRefer >::Value, LeftWrapperRefer >; \
                        using RightWrapperRefer = _Right &&; \
         \
                        using Returned = ::std::result_of_t< ::ScL::Feature::Detail::Operator::Binary::Invokable( LeftValueRefer, RightWrapperRefer ) >; \
                        return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::LeftExposingCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, LeftValueRefer > > \
                            ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## Switch< ::ScL::Feature::Detail::Operator::RightExposingCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftWrapperRefer = _Left &&; \
                        using RightWrapperRefer = _Right &&; \
                        using RightValueRefer = ::ScL::SimilarRefer< typename ::std::decay_t< RightWrapperRefer >::Value, RightWrapperRefer >; \
         \
                        using Returned = ::std::result_of_t< ::ScL::Feature::Detail::Operator::Binary::Invokable( LeftWrapperRefer, RightValueRefer ) >; \
                        return ::ScL::Feature::Detail::Operator::ResultSwitch< ::ScL::Feature::Detail::Operator::RightExposingCase, ::ScL::Feature::Detail::Operator::ResultSwitchCase< Returned, LeftWrapperRefer > > \
                            ::invoke( ::ScL::Feature::Detail::Operator::Binary::Invokable(), ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                    } \
                }; \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Binary \
            { \
                template < typename > \
                struct Invokable ## WrapperSwitch; \
         \
                template <> \
                struct Invokable ## WrapperSwitch < ::ScL::Feature::Detail::Operator::LeftWrapperCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static constexpr decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftWrapperRefer = _Left &&; \
                        using LeftWrapper = ::std::decay_t< LeftWrapperRefer >; \
                        using LeftHolder = typename LeftWrapper::Holder; \
                        using LeftHolderRefer = ::ScL::SimilarRefer< LeftHolder, LeftWrapperRefer >; \
                        using RightRefer = _Right &&; \
         \
                        constexpr bool holder_has_method_for_operator = ::ScL::Feature::Detail::Operator::Binary::does_operator ## Invokable ## Left_method_exist< LeftHolder, void( LeftHolderRefer, RightRefer ) >; \
                        using OperatorSwitchCase = ::std::conditional_t< holder_has_method_for_operator, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase >; \
                        return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Switch< LeftWrapperCase, OperatorSwitchCase >::invoke( ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## WrapperSwitch < ::ScL::Feature::Detail::Operator::RightWrapperCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static constexpr decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using RightWrapperRefer = _Right &&; \
                        using RightWrapper = ::std::decay_t< RightWrapperRefer >; \
                        using RightHolder = typename RightWrapper::Holder; \
                        using RightHolderRefer = ::ScL::SimilarRefer< RightHolder, RightWrapperRefer >; \
                        using LeftRefer = _Left &&; \
         \
                        constexpr bool holder_has_method_for_operator = ::ScL::Feature::Detail::Operator::Binary::does_operator ## Invokable ## Right_method_exist< RightHolder, void( LeftRefer, RightHolderRefer ) >; \
                        using OperatorSwitchCase = ::std::conditional_t< holder_has_method_for_operator, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase >; \
                        return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Switch< RightWrapperCase, OperatorSwitchCase >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                    } \
                }; \
         \
                template <> \
                struct Invokable ## WrapperSwitch < ::ScL::Feature::Detail::Operator::BothWrapperCase > \
                { \
                    template < typename _Left, typename _Right > \
                    static constexpr decltype(auto) invoke ( _Left && left, _Right && right ) \
                    { \
                        using LeftWrapperRefer = _Left &&; \
                        using LeftWrapper = ::std::decay_t< LeftWrapperRefer >; \
                        using LeftHolder = typename LeftWrapper::Holder; \
                        using LeftHolderRefer = ::ScL::SimilarRefer< LeftHolder, LeftWrapperRefer >; \
                        using RightWrapperRefer = _Right &&; \
                        using RightWrapper = ::std::decay_t< RightWrapperRefer >; \
                        using RightHolder = typename RightWrapper::Holder; \
                        using RightHolderRefer = ::ScL::SimilarRefer< RightHolder, RightWrapperRefer >; \
         \
                        constexpr bool is_left_compatible_to_right = ::ScL::Feature::isCompatible< LeftHolder, RightHolder >(); \
                        constexpr bool holder_has_method_for_operator = ::ScL::Feature::Detail::Operator::Binary::does_operator ## Invokable ## _method_exist< LeftHolder, void( LeftHolderRefer, RightHolderRefer ) >; \
                        using OperatorSwitchCase = ::std::conditional_t< is_left_compatible_to_right && holder_has_method_for_operator, ::ScL::Feature::Detail::Operator::Binary::HolderHasOperatorCase, ::ScL::Feature::Detail::Operator::Binary::HolderHasNoOperatorCase >; \
                        return ::ScL::Feature::Detail::Operator::Binary::Invokable ## Switch< BothWrapperCase, OperatorSwitchCase >::invoke( ::std::forward< LeftWrapperRefer >( left ), ::std::forward< RightWrapperRefer >( right ) ); \
                    } \
                }; \
            } \
        } \
    }}} \
     \
    namespace ScL { namespace Feature { namespace Detail \
    { \
        namespace Operator \
        { \
            namespace Binary \
            { \
                template < typename _LeftRefer, typename _RightRefer > \
                struct Invokable ## Helper \
                { \
                    static_assert( ::std::is_reference< _LeftRefer >::value, "The template parameter _LeftRefer must to be a reference type." ); \
                    static_assert( ::std::is_reference< _RightRefer >::value, "The template parameter _RightRefer must to be a reference type." ); \
         \
                    using LeftRefer = _LeftRefer; \
                    using RightRefer = _RightRefer; \
         \
                    static constexpr decltype(auto) invoke( LeftRefer left, RightRefer right ) \
                    { \
                        return ::ScL::Feature::Detail::Operator::Binary::Invokable ## WrapperSwitch< ::ScL::Feature::Detail::Operator::WrapperSwitchCase< LeftRefer, RightRefer > > \
                            ::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) ); \
                    } \
                }; \
            } \
        } \
    }}} \

//SCL_GLOBAL_BINARY_OPERATOR( <<, ShiftLeft )
//SCL_GLOBAL_BINARY_OPERATOR_SPECIALIZATION( ShiftLeft )
namespace ScL { namespace Feature { namespace Detail
{
    namespace Operator
    {
        namespace Binary
        {
            template <>
            struct ShiftLeftOperator< ::ScL::Feature::Detail::Operator::Global::ShiftLeftCase >
            {
                template < typename _Left, typename _Right >
                static decltype(auto) invoke ( _Left && left, _Right && right )
                {
                    using LeftRefer = _Left &&;
                    using RightRefer = _Right &&;
                    return ::ScL::Feature::Detail::Operator::Binary::ShiftLeftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) );
                }
            };

        }
    }
}}}
//SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( << ), &&, ShiftLeft )
template < typename _Left, typename _RightValue, typename _RightTool,
    typename = ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< _Left > >()
        && ::ScL::Feature::Detail::Operator::Binary::does_ShiftLeft_operator_exist_test< _Left &&, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > && > > >
constexpr decltype(auto) operator << ( _Left && left, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > && right )
{
    using LeftRefer = _Left &&;
    using RightRefer = ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > &&;
    return ::ScL::Feature::Detail::Operator::Binary::ShiftLeftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) );
}
//SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( << ), const &&, ShiftLeft )
template < typename _Left, typename _RightValue, typename _RightTool,
    typename = ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< _Left > >()
        && ::ScL::Feature::Detail::Operator::Binary::does_ShiftLeft_operator_exist_test< _Left &&, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const && > > >
constexpr decltype(auto) operator << ( _Left && left, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const && right )
{
    using LeftRefer = _Left &&;
    using RightRefer = ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const &&;
    return ::ScL::Feature::Detail::Operator::Binary::ShiftLeftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) );
}
//SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( << ), volatile &&, ShiftLeft )
template < typename _Left, typename _RightValue, typename _RightTool,
    typename = ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< _Left > >()
        && ::ScL::Feature::Detail::Operator::Binary::does_ShiftLeft_operator_exist_test< _Left &&, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > volatile && > > >
constexpr decltype(auto) operator << ( _Left && left, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > volatile && right )
{
    using LeftRefer = _Left &&;
    using RightRefer = ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > volatile &&;
    return ::ScL::Feature::Detail::Operator::Binary::ShiftLeftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) );
}
//SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( << ), const volatile &&, ShiftLeft )
template < typename _Left, typename _RightValue, typename _RightTool,
    typename = ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< _Left > >()
        && ::ScL::Feature::Detail::Operator::Binary::does_ShiftLeft_operator_exist_test< _Left &&, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const volatile && > > >
constexpr decltype(auto) operator << ( _Left && left, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const volatile && right )
{
    using LeftRefer = _Left &&;
    using RightRefer = ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const volatile &&;
    return ::ScL::Feature::Detail::Operator::Binary::ShiftLeftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) );
}
//SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( << ), &, ShiftLeft )
template < typename _Left, typename _RightValue, typename _RightTool,
    typename = ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< _Left > >()
        && ::ScL::Feature::Detail::Operator::Binary::does_ShiftLeft_operator_exist_test< _Left &&, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > & > > >
constexpr decltype(auto) operator << ( _Left && left, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > & right )
{
    using LeftRefer = _Left &&;
    using RightRefer = ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > &;
    return ::ScL::Feature::Detail::Operator::Binary::ShiftLeftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) );
}
//SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( << ), const &, ShiftLeft )
template < typename _Left, typename _RightValue, typename _RightTool,
    typename = ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< _Left > >()
        && ::ScL::Feature::Detail::Operator::Binary::does_ShiftLeft_operator_exist_test< _Left &&, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const & > > >
constexpr decltype(auto) operator << ( _Left && left, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const & right )
{
    using LeftRefer = _Left &&;
    using RightRefer = ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const &;
    return ::ScL::Feature::Detail::Operator::Binary::ShiftLeftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) );
}
//SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( << ), volatile &, ShiftLeft )
template < typename _Left, typename _RightValue, typename _RightTool,
    typename = ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< _Left > >()
        && ::ScL::Feature::Detail::Operator::Binary::does_ShiftLeft_operator_exist_test< _Left &&, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > volatile & > > >
constexpr decltype(auto) operator << ( _Left && left, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > volatile & right )
{
    using LeftRefer = _Left &&;
    using RightRefer = ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > volatile &;
    return ::ScL::Feature::Detail::Operator::Binary::ShiftLeftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) );
}
//SCL_GLOBAL_BINARY_OPERATOR_PROTOTYPE( SCL_SINGLE_ARG( << ), const volatile &, ShiftLeft )
template < typename _Left, typename _RightValue, typename _RightTool,
    typename = ::std::enable_if_t< !::ScL::Feature::isWrapper< ::std::decay_t< _Left > >()
        && ::ScL::Feature::Detail::Operator::Binary::does_ShiftLeft_operator_exist_test< _Left &&, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const volatile & > > >
constexpr decltype(auto) operator << ( _Left && left, ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const volatile & right )
{
    using LeftRefer = _Left &&;
    using RightRefer = ::ScL::Feature::Detail::Wrapper< _RightValue, _RightTool > const volatile &;
    return ::ScL::Feature::Detail::Operator::Binary::ShiftLeftHelper< LeftRefer, RightRefer >::invoke( ::std::forward< LeftRefer >( left ), ::std::forward< RightRefer >( right ) );
}


int main ( int /*argc*/, char ** /*argv*/ )
{
    using Int = Wrapper< int >;
    using WStream = Stream;

    WStream s;
    Int i;

    s << i;

    return 0;
}
