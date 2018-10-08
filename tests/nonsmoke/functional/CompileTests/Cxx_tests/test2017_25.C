
// #include <boost/mpl/aux_/config/gcc.hpp>

// This is example of how ROSE might take the wrong branch comparied to EDG (we would have to turn off __EDG_VERSION_.
// #if defined(__GNUC__) && !defined(__EDG_VERSION__)
// #   define BOOST_MPL_CFG_GCC ((__GNUC__ << 8) | __GNUC_MINOR__)
// #else
// #   define BOOST_MPL_CFG_GCC 0
// #endif

#   define BOOST_MPL_CFG_GCC 0

// if we don't have a user config, then use the default location:
#if !defined(BOOST_USER_CONFIG) && !defined(BOOST_NO_USER_CONFIG)
#  define BOOST_USER_CONFIG <boost/config/user.hpp>
#endif

// include it first:
#ifdef BOOST_USER_CONFIG
#  include BOOST_USER_CONFIG
#endif

// if we don't have a compiler config set, try and find one:
#if !defined(BOOST_COMPILER_CONFIG) && !defined(BOOST_NO_COMPILER_CONFIG) && !defined(BOOST_NO_CONFIG)
#  include <boost/config/select_compiler_config.hpp>
#endif
// if we have a compiler config, include it now:
#ifdef BOOST_COMPILER_CONFIG
#  include BOOST_COMPILER_CONFIG
#endif

// if we don't have a std library config set, try and find one:
#if !defined(BOOST_STDLIB_CONFIG) && !defined(BOOST_NO_STDLIB_CONFIG) && !defined(BOOST_NO_CONFIG) && defined(__cplusplus)
#  include <boost/config/select_stdlib_config.hpp>
#endif
// if we have a std library config, include it now:
#ifdef BOOST_STDLIB_CONFIG
#  include BOOST_STDLIB_CONFIG
#endif

// if we don't have a platform config set, try and find one:
#if !defined(BOOST_PLATFORM_CONFIG) && !defined(BOOST_NO_PLATFORM_CONFIG) && !defined(BOOST_NO_CONFIG)
#  include <boost/config/select_platform_config.hpp>
#endif
// if we have a platform config, include it now:
#ifdef BOOST_PLATFORM_CONFIG
#  include BOOST_PLATFORM_CONFIG
#endif

// get config suffix code:
#include <boost/config/suffix.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif





#ifndef __GNUC__
#define __GNUC___WORKAROUND_GUARD 1
#else
#define __GNUC___WORKAROUND_GUARD 0
#endif


#ifndef __GNUC_MINOR__
#define __GNUC_MINOR___WORKAROUND_GUARD 1
#else
#define __GNUC_MINOR___WORKAROUND_GUARD 0
#endif

#ifndef __GNUC_PATCHLEVEL__
#define __GNUC_PATCHLEVEL___WORKAROUND_GUARD 1
#else
#define __GNUC_PATCHLEVEL___WORKAROUND_GUARD 0
#endif




// Always define to zero, if it's used it'll be defined my MPL:
#define BOOST_MPL_CFG_GCC_WORKAROUND_GUARD 0

#  define BOOST_WORKAROUND(symbol, test)                \
         ((symbol ## _WORKAROUND_GUARD + 0 == 0) &&     \
         (symbol != 0) && (1 % (( (symbol test) ) + 1)))
//                              ^ ^           ^ ^
// The extra level of parenthesis nesting above, along with the
// BOOST_OPEN_PAREN indirection below, is required to satisfy the
// broken preprocessor in MWCW 8.3 and earlier.
//
// The basic mechanism works as follows:
//      (symbol test) + 1        =>   if (symbol test) then 2 else 1
//      1 % ((symbol test) + 1)  =>   if (symbol test) then 1 else 0
//
// The complication with % is for cooperation with BOOST_TESTED_AT().
// When "test" is BOOST_TESTED_AT(x) and
// BOOST_DETECT_OUTDATED_WORKAROUNDS is #defined,
//
//      symbol test              =>   if (symbol <= x) then 1 else -1
//      (symbol test) + 1        =>   if (symbol <= x) then 2 else 0
//      1 % ((symbol test) + 1)  =>   if (symbol <= x) then 1 else divide-by-zero
//

#   define BOOST_TESTED_AT(value) != ((value)-(value))







#   define BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE mpl_
#   define BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN namespace mpl_ {
#   define BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE }
#   define BOOST_MPL_AUX_ADL_BARRIER_DECL(type) \
    namespace boost { namespace mpl { \
    using ::BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::type; \
    } } \
/**/

namespace BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE { namespace aux {} }
namespace boost { namespace mpl { using namespace BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE; 
namespace aux { using namespace BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::aux; }
}}


BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_OPEN

template< bool C_ > struct bool_;

// shorcuts
typedef bool_<true> true_;
typedef bool_<false> false_;

BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE_CLOSE

BOOST_MPL_AUX_ADL_BARRIER_DECL(bool_)
BOOST_MPL_AUX_ADL_BARRIER_DECL(true_)
BOOST_MPL_AUX_ADL_BARRIER_DECL(false_)


namespace boost { namespace mpl { namespace aux {

template< typename T > struct type_wrapper
{
    typedef T type;
};

template< typename T > struct wrapped_type;
template< typename T > struct wrapped_type< type_wrapper<T> >
{
    typedef T type;
};

}}}


namespace boost { namespace mpl { namespace aux {

typedef char (&no_tag)[1];
typedef char (&yes_tag)[2];

template< bool C_ > struct yes_no_tag
{
    typedef no_tag type;
};

template<> struct yes_no_tag<true>
{
    typedef yes_tag type;
};

}}}


#if BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
#   define BOOST_MSVC_TYPENAME
#else
#   define BOOST_MSVC_TYPENAME typename
#endif


#   define BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(trait, name, default_) \
template< typename T, typename fallback_ = boost::mpl::bool_<default_> > \
struct trait \
{ \
    struct gcc_3_2_wknd \
    { \
        template< typename U > \
        static boost::mpl::aux::yes_tag test( \
              boost::mpl::aux::type_wrapper<U> const volatile* \
            , boost::mpl::aux::type_wrapper<BOOST_MSVC_TYPENAME U::name>* = 0 \
            ); \
    \
        static boost::mpl::aux::no_tag test(...); \
    }; \
    \
    typedef boost::mpl::aux::type_wrapper<T> t_; \
    BOOST_STATIC_CONSTANT(bool, value = \
          sizeof(gcc_3_2_wknd::test(static_cast<t_*>(0))) \
            == sizeof(boost::mpl::aux::yes_tag) \
        ); \
    typedef boost::mpl::bool_<value> type; \
}; \
/**/

#define BOOST_MPL_HAS_XXX_TRAIT_DEF(name) \
    BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(BOOST_PP_CAT(has_,name), name, false) \
/**/



#define BOOST_MPL_HAS_XXX_NO_WRAPPED_TYPES 0
#define BOOST_MPL_HAS_XXX_NO_EXPLICIT_TEST_FUNCTION 0
#define BOOST_MPL_HAS_XXX_NEEDS_TEMPLATE_SFINAE 0

#   define BOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args) \
      BOOST_PP_CAT(BOOST_PP_ARRAY_ELEM(0, args) , _introspect) \
    /**/

#   define BOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) \
      BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_ARRAY_ELEM(0, args) , _substitute), n) \
    /**/

#   define BOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args) \
      BOOST_PP_CAT(BOOST_PP_ARRAY_ELEM(0, args) , _test) \
    /**/

#   define BOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE(z, n, args) \
      template< \
          template< BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename V) > class V \
       > \
      struct BOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) { \
      }; \
    /**/

#   define BOOST_MPL_HAS_MEMBER_SUBSTITUTE(args, substitute_macro) \
      BOOST_PP_REPEAT( \
          BOOST_PP_ARRAY_ELEM(2, args) \
        , BOOST_MPL_HAS_MEMBER_MULTI_SUBSTITUTE \
        , args \
      ) \
    /**/

#     define BOOST_MPL_HAS_MEMBER_REJECT(args, member_macro) \
        template< typename V > \
        static boost::mpl::aux::no_tag \
        BOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)(...); \
      /**/

#     define BOOST_MPL_HAS_MEMBER_MULTI_ACCEPT(z, n, args) \
        template< typename V > \
        static boost::mpl::aux::yes_tag \
        BOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)( \
            boost::mpl::aux::type_wrapper< V > const volatile* \
          , BOOST_MPL_HAS_MEMBER_INTROSPECTION_SUBSTITUTE_NAME(args, n) < \
                V::template BOOST_PP_ARRAY_ELEM(1, args) \
            >* = 0 \
        ); \
      /**/
#     define BOOST_MPL_HAS_MEMBER_ACCEPT(args, member_macro) \
        BOOST_PP_REPEAT( \
            BOOST_PP_ARRAY_ELEM(2, args) \
          , BOOST_MPL_HAS_MEMBER_MULTI_ACCEPT \
          , args \
        ) \
      /**/

#     define BOOST_MPL_HAS_MEMBER_TEST(args) \
          sizeof(BOOST_MPL_HAS_MEMBER_INTROSPECTION_TEST_NAME(args)< U >(0)) \
              == sizeof(boost::mpl::aux::yes_tag) \
      /**/

#   define BOOST_MPL_HAS_MEMBER_INTROSPECT( \
               args, substitute_macro, member_macro \
           ) \
      template< typename U > \
      struct BOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args) { \
          BOOST_MPL_HAS_MEMBER_SUBSTITUTE(args, substitute_macro) \
          BOOST_MPL_HAS_MEMBER_REJECT(args, member_macro) \
          BOOST_MPL_HAS_MEMBER_ACCEPT(args, member_macro) \
          BOOST_STATIC_CONSTANT( \
              bool, value = BOOST_MPL_HAS_MEMBER_TEST(args) \
          ); \
          typedef boost::mpl::bool_< value > type; \
      }; \
    /**/

#   define BOOST_MPL_HAS_MEMBER_IMPLEMENTATION( \
               args, introspect_macro, substitute_macro, member_macro \
           ) \
      template< \
          typename T \
        , typename fallback_ \
              = boost::mpl::bool_< BOOST_PP_ARRAY_ELEM(3, args) > \
      > \
      class BOOST_PP_ARRAY_ELEM(0, args) { \
          introspect_macro(args, substitute_macro, member_macro) \
      public: \
          static const bool value \
              = BOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args)< T >::value; \
          typedef typename BOOST_MPL_HAS_MEMBER_INTROSPECTION_NAME(args)< \
              T \
          >::type type; \
      }; \
    /**/

#   define BOOST_MPL_HAS_MEMBER_WITH_FUNCTION_SFINAE( \
               args, substitute_macro, member_macro \
           ) \
      BOOST_MPL_HAS_MEMBER_IMPLEMENTATION( \
          args \
        , BOOST_MPL_HAS_MEMBER_INTROSPECT \
        , substitute_macro \
        , member_macro \
      ) \
    /**/


#     define BOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF(trait, name, default_) \
        BOOST_MPL_HAS_MEMBER_WITH_FUNCTION_SFINAE( \
            ( 4, ( trait, name, BOOST_MPL_LIMIT_METAFUNCTION_ARITY, default_ ) ) \
          , BOOST_MPL_HAS_MEMBER_TEMPLATE_SUBSTITUTE_PARAMETER \
          , BOOST_MPL_HAS_MEMBER_TEMPLATE_ACCESS \
        ) \
      /**/

#   define BOOST_MPL_HAS_XXX_TEMPLATE_DEF(name) \
      BOOST_MPL_HAS_XXX_TEMPLATE_NAMED_DEF( \
          BOOST_PP_CAT(has_, name), name, false \
      ) \
    /**/


namespace boost { namespace mpl { namespace aux {
   BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_apply, apply, false)
}}}

#define BOOST_MPL_PREPROCESSED_HEADER apply_wrap.hpp

#define BOOST_MPL_CFG_COMPILER_DIR gcc

#define BOOST_PP_STRINGIZE(text) BOOST_PP_STRINGIZE_I(text)

#define BOOST_PP_STRINGIZE_I(text) #text

#define AUX778076_PREPROCESSED_HEADER BOOST_MPL_CFG_COMPILER_DIR/BOOST_MPL_PREPROCESSED_HEADER /**/

#include BOOST_PP_STRINGIZE(boost/mpl/aux_/preprocessed/AUX778076_PREPROCESSED_HEADER)



template<class E, class F>
class vector_unary
   {
     typedef E expression_closure_type;

     public:
          void foobar (int i) const 
            {
              F::apply (e_ (i));
            }

     private:
          expression_closure_type e_;
   };



