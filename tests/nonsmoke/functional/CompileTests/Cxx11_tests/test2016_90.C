
// DQ (1/24/2017): Added declaration of this type that comes from the rose_edg_required_macros_and_functions.h file.
// Note that this is only required within ROSE is using the GNU 5.1 compiler.  Not yet clear why that is.
// typedef unsigned int __mmask32;

template <typename S, typename T>
class ParseAndSetMember
{};

template <typename S, typename T>
ParseAndSetMember<S,T> createParseAndSetMember( T S::*mptr)
{}

#include <map>
namespace LEOS {
    enum functionsType {
        FT_Bcv,  
        FT_Cs,
        FT_Cs2p,
        FT_Df,
        FT_E2p,
        FT_Ecp,
        FT_Ecp1,
        FT_Ecp2,
        FT_Ee,
        FT_Ec,
        FT_Ei,
        FT_Et,
        FT_Gm,
        FT_Ke,
        FT_Ki,
        FT_Kp,
        FT_Kp1,
        FT_Kp2,
        FT_Kr,
        FT_Kr1,
        FT_Kr2,
        FT_Mpf,
        FT_P2p,
        FT_Pc,
        FT_Pe,
        FT_Pi,
        FT_Pt,
        FT_Re,
        FT_S2p,
        FT_Se,
        FT_Si,
        FT_St,
        FT_Tcx,
        FT_Tm,
        FT_Tml,
        FT_Tms,
        FT_Vs,
        FT_Ys,
        FT_Zeff,
        FT_Zeff1,
        FT_Zeff2,
        FT_Ce,
        FT_Eci,
        FT_Elcx,
        FT_Eml,
        FT_Ems,
        FT_Evcx,
        FT_Fci,
        FT_Fe,
        FT_Fi,
        FT_Flcx,
        FT_Fml,
        FT_Fms,
        FT_Ft,
        FT_Fvcx,
        FT_Gms,
        FT_Kbound,
        FT_Kec,
        FT_Keo,
        FT_Kt,
        FT_Pci,
        FT_Pcx,
        FT_Pml,
        FT_Pms,
        FT_Rhlcx,
        FT_Rhml,
        FT_Rhms,
        FT_Rhvcx,
        FT_Sci,
        FT_Tec,
        FT_Tlv,
        FT_Zeffop,
        FT_Zeffco,
        FT_Blrho,
        FT_Bltemp
    };
    typedef unsigned int L8UINT;
    typedef L8UINT functionType_t;
    const functionType_t FT_LEOS(FT_Bltemp + 1);
    typedef std::string  L8STRING;
    using std::map;
    typedef map<functionType_t, L8STRING> genericNamesMap_t;
    //BELOW NEEDED FOR ERROR
    const genericNamesMap_t::value_type genericNamesData[] = {
            //IF STRINGS ARE NULL BELOW, FAILS DIFFERENTLY:
            genericNamesMap_t::value_type(FT_Bcv,    "curve name: Phase_Curve"),
            genericNamesMap_t::value_type(FT_Cs,     "Sound_Speed"),
            genericNamesMap_t::value_type(FT_Cs2p,   "Sound_Speed"),
            genericNamesMap_t::value_type(FT_Df,     "Mass_Diffusion"),
            genericNamesMap_t::value_type(FT_E2p,    "Energy"),
            genericNamesMap_t::value_type(FT_Ecp,    "Electron_Chemical_Potential"),
            genericNamesMap_t::value_type(FT_Ecp1,   "Electron_Chemical_Potential"),
            genericNamesMap_t::value_type(FT_Ecp2,   "Electron_Chemical_Potential"),
            genericNamesMap_t::value_type(FT_Ee,     "Energy"),
            genericNamesMap_t::value_type(FT_Ec,     "Energy"),
            genericNamesMap_t::value_type(FT_Ei,     "Energy"),
            genericNamesMap_t::value_type(FT_Et,     "Energy"),
            genericNamesMap_t::value_type(FT_Gm,     "Shear_Modulus"),
            genericNamesMap_t::value_type(FT_Ke,     "Thermal_Conductivity"),
            genericNamesMap_t::value_type(FT_Ki,     "Thermal_Conductivity"),
            genericNamesMap_t::value_type(FT_Kp,     "Opacity"),
            genericNamesMap_t::value_type(FT_Kp1,    "Opacity"),
            genericNamesMap_t::value_type(FT_Kp2,    "Opacity"),
            genericNamesMap_t::value_type(FT_Kr,     "Opacity"),
            genericNamesMap_t::value_type(FT_Kr1,    "Opacity"),
            genericNamesMap_t::value_type(FT_Kr2,    "Opacity"),
            genericNamesMap_t::value_type(FT_Mpf,    "Phase_Fractions"),
            genericNamesMap_t::value_type(FT_P2p,    "Pressure"),
            genericNamesMap_t::value_type(FT_Pc,     "Pressure"),
            genericNamesMap_t::value_type(FT_Pe,     "Pressure"),
            genericNamesMap_t::value_type(FT_Pi,     "Pressure"),
            genericNamesMap_t::value_type(FT_Pt,     "Pressure"),
            genericNamesMap_t::value_type(FT_Re,     "Electrical_Resistivity"),
            genericNamesMap_t::value_type(FT_S2p,    "Entropy"),
            genericNamesMap_t::value_type(FT_Se,     "Entropy"),
            genericNamesMap_t::value_type(FT_Si,     "Entropy"),
            genericNamesMap_t::value_type(FT_St,     "Entropy"),
            genericNamesMap_t::value_type(FT_Tcx,    "Temperature"),
            genericNamesMap_t::value_type(FT_Tm,     "Temperature"),
            genericNamesMap_t::value_type(FT_Tml,    "Temperature"),
            genericNamesMap_t::value_type(FT_Tms,    "Temperature"),
            genericNamesMap_t::value_type(FT_Vs,     "Viscosity"),
            genericNamesMap_t::value_type(FT_Ys,     "Yield_Strength"),
            genericNamesMap_t::value_type(FT_Zeff,   "Z_Effective"),
            genericNamesMap_t::value_type(FT_Zeff1,  "Z_Effective"),
            genericNamesMap_t::value_type(FT_Zeff2,  "Z_Effective"),
            genericNamesMap_t::value_type(FT_Ce,     "Electrical_Conductivity"),
            genericNamesMap_t::value_type(FT_Eci,    "Energy"),
            genericNamesMap_t::value_type(FT_Elcx,   "Energy"),
            genericNamesMap_t::value_type(FT_Eml,    "Energy"),
            genericNamesMap_t::value_type(FT_Ems,    "Energy"),
            genericNamesMap_t::value_type(FT_Evcx,   "Energy"),
            genericNamesMap_t::value_type(FT_Fci,    "Energy"),
            genericNamesMap_t::value_type(FT_Fe,     "Energy"),
            genericNamesMap_t::value_type(FT_Fi,     "Energy"),
            genericNamesMap_t::value_type(FT_Flcx,   "Energy"),
            genericNamesMap_t::value_type(FT_Fml,    "Energy"),
            genericNamesMap_t::value_type(FT_Fms,    "Energy"),
            genericNamesMap_t::value_type(FT_Ft,     "Energy"),
            genericNamesMap_t::value_type(FT_Fvcx,   "Energy"),
            genericNamesMap_t::value_type(FT_Gms,    "Shear_Modulus"),
            genericNamesMap_t::value_type(FT_Kbound, "Opacity"),
            genericNamesMap_t::value_type(FT_Kec,    "Opacity"),
            genericNamesMap_t::value_type(FT_Keo,    "Opacity"),
            genericNamesMap_t::value_type(FT_Kt,     "Thermal_Conductivity"),
            genericNamesMap_t::value_type(FT_Pci,    "Pressure"),
            genericNamesMap_t::value_type(FT_Pcx,    "Pressure"),
            genericNamesMap_t::value_type(FT_Pml,    "Pressure"),
            genericNamesMap_t::value_type(FT_Pms,    "Pressure"),
            genericNamesMap_t::value_type(FT_Rhlcx,  "Density"),
            genericNamesMap_t::value_type(FT_Rhml,   "Density"),
            genericNamesMap_t::value_type(FT_Rhms,   "Density"),
            genericNamesMap_t::value_type(FT_Rhvcx,  "Density"),
            genericNamesMap_t::value_type(FT_Sci,    "Entropy"),
            genericNamesMap_t::value_type(FT_Tec,    "Thermoelectric_Coefficient"),
            genericNamesMap_t::value_type(FT_Tlv,    "Temperature"),
            genericNamesMap_t::value_type(FT_Zeffco, "Z_Effective"),
            genericNamesMap_t::value_type(FT_Zeffop, "Z_Effective"),
            genericNamesMap_t::value_type(FT_Blrho,  "Density"),
            genericNamesMap_t::value_type(FT_Bltemp, "Temperature"),
            genericNamesMap_t::value_type(FT_LEOS,   "Unknown_Function_Name"),
    };
    typedef map<functionType_t, L8STRING> genericTypesMap_t;
    //BELOW NEEDED FOR ERROR
    const genericTypesMap_t::value_type genericTypesData[] = {
            genericTypesMap_t::value_type(FT_Bcv,    "Parametric"),
            genericTypesMap_t::value_type(FT_Cs,     "Default"),
            genericTypesMap_t::value_type(FT_Cs2p,   "Two-phase"),
            genericTypesMap_t::value_type(FT_Df,     "Default"),
            genericTypesMap_t::value_type(FT_E2p,    "Two-phase"),
            genericTypesMap_t::value_type(FT_Ecp,    "Default"),
            genericTypesMap_t::value_type(FT_Ecp1,   "Type1"),
            genericTypesMap_t::value_type(FT_Ecp2,   "Type2"),
            genericTypesMap_t::value_type(FT_Ee,     "Electron-thermal"),
            genericTypesMap_t::value_type(FT_Ec,     "Cold"),
            genericTypesMap_t::value_type(FT_Ei,     "Ion-thermal"),
            genericTypesMap_t::value_type(FT_Et,     "Total"),
            genericTypesMap_t::value_type(FT_Gm,     "Default"),
            genericTypesMap_t::value_type(FT_Ke,     "Electron"),
            genericTypesMap_t::value_type(FT_Ki,     "Ion"),
            genericTypesMap_t::value_type(FT_Kp,     "Plank"),
            genericTypesMap_t::value_type(FT_Kp1,    "Plank_Type1"),
            genericTypesMap_t::value_type(FT_Kp2,    "Plank_Type1"),
            genericTypesMap_t::value_type(FT_Kr,     "Rosseland"),
            genericTypesMap_t::value_type(FT_Kr1,    "Rosseland_Type1"),
            genericTypesMap_t::value_type(FT_Kr2,    "Rosseland_Type2"),
            genericTypesMap_t::value_type(FT_Mpf,    "Default"),
            genericTypesMap_t::value_type(FT_P2p,    "Two-phase"),
            genericTypesMap_t::value_type(FT_Pc,     "Cold"),
            genericTypesMap_t::value_type(FT_Pe,     "Electron-thermal"),
            genericTypesMap_t::value_type(FT_Pi,     "Ion-thermal"),
            genericTypesMap_t::value_type(FT_Pt,     "Total"),
            genericTypesMap_t::value_type(FT_Re,     "Default"),
            genericTypesMap_t::value_type(FT_S2p,    "Two-phase"),
            genericTypesMap_t::value_type(FT_Se,     "Electron-thermal"),
            genericTypesMap_t::value_type(FT_Si,     "Ion-thermal"),
            genericTypesMap_t::value_type(FT_St,     "Total"),
            genericTypesMap_t::value_type(FT_Tcx,    "Liquid_vapor_coexistence"),
            genericTypesMap_t::value_type(FT_Tm,     "Melt"),
            genericTypesMap_t::value_type(FT_Tml,    "Liquidus_melt"),
            genericTypesMap_t::value_type(FT_Tms,    "Solidus_melt"),
            genericTypesMap_t::value_type(FT_Vs,     "Default"),
            genericTypesMap_t::value_type(FT_Ys,     "Default"),
            genericTypesMap_t::value_type(FT_Zeff,   "Default"),
            genericTypesMap_t::value_type(FT_Zeff1,  "Type1"),
            genericTypesMap_t::value_type(FT_Zeff2,  "Type2"),
            genericTypesMap_t::value_type(FT_Ce,     "Default"),
            genericTypesMap_t::value_type(FT_Eci,    "Cold_plus_ion"),
            genericTypesMap_t::value_type(FT_Elcx,   "Liquid_coexistence"),
            genericTypesMap_t::value_type(FT_Eml,    "Liquidus"),
            genericTypesMap_t::value_type(FT_Ems,    "Solidus"),
            genericTypesMap_t::value_type(FT_Evcx,   "Vapor_coexistence"),
            genericTypesMap_t::value_type(FT_Fci,    "Cold_plus_ion"),
            genericTypesMap_t::value_type(FT_Fe,     "Helmholtz_electron"),
            genericTypesMap_t::value_type(FT_Fi,     "Helmholtz_ion"),
            genericTypesMap_t::value_type(FT_Flcx,   "Liquid_coexistence_Helmholtz"),
            genericTypesMap_t::value_type(FT_Fml,    "Liquidus_Helmholtz"),
            genericTypesMap_t::value_type(FT_Fms,    "Solidus_Helmholtz"),
            genericTypesMap_t::value_type(FT_Ft,     "Helmholtz_total"),
            genericTypesMap_t::value_type(FT_Fvcx,   "Vapor_coexistence_Helmholtz"),
            genericTypesMap_t::value_type(FT_Gms,    "Sesame"),
            genericTypesMap_t::value_type(FT_Kbound, "Extrapolated_above_boundary"),
            genericTypesMap_t::value_type(FT_Kec,    "Electron_Conductive_Opactity_Model"),
            genericTypesMap_t::value_type(FT_Keo,    "Electron_Conductive_Conductivity_Model"),
            genericTypesMap_t::value_type(FT_Kt,     "Default"),
            genericTypesMap_t::value_type(FT_Pci,    "Codl_plus_ion"),
            genericTypesMap_t::value_type(FT_Pcx,    "Vaporization"),
            genericTypesMap_t::value_type(FT_Pml,    "Liquidus"),
            genericTypesMap_t::value_type(FT_Pms,    "Solidus"),
            genericTypesMap_t::value_type(FT_Rhlcx,  "Liquid_coexistence"),
            genericTypesMap_t::value_type(FT_Rhml,   "Liquidus"),
            genericTypesMap_t::value_type(FT_Rhms,   "Solidus"),
            genericTypesMap_t::value_type(FT_Rhvcx,  "Vapor_coexistence"),
            genericTypesMap_t::value_type(FT_Sci,    "Cold_plus_ion"),
            genericTypesMap_t::value_type(FT_Tec,    "Default"),
            genericTypesMap_t::value_type(FT_Tlv,    "Vaporization"),
            genericTypesMap_t::value_type(FT_Zeffco, "Conductivity_Model"),
            genericTypesMap_t::value_type(FT_Zeffop, "Opacity_Model"),
            genericTypesMap_t::value_type(FT_Blrho,  "Default"),
            genericTypesMap_t::value_type(FT_Bltemp, "Default"),
            genericTypesMap_t::value_type(FT_LEOS,   "Unknown_Function_Type"),
    };
}

 // BEGIN reduced boost
//#  include <boost/function/function_base.hpp>
// Boost.Function library

//  Copyright Douglas Gregor 2001-2006
//  Copyright Emil Dotchevski 2007
//  Use, modification and distribution is subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

//#ifndef BOOST_FUNCTION_BASE_HEADER
//#define BOOST_FUNCTION_BASE_HEADER

#include <stdexcept>
#include <string>
#include <memory>
#include <new>
#include <boost/config.hpp>
#include <boost/detail/sp_typeinfo.hpp>
#include <boost/assert.hpp>
#include <boost/integer.hpp>
#include <boost/type_traits/has_trivial_copy.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_volatile.hpp>
#include <boost/type_traits/composite_traits.hpp>
#include <boost/type_traits/ice.hpp>
#include <boost/ref.hpp>
#include <boost/mpl/if.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/type_traits/alignment_of.hpp>
//#ifndef BOOST_NO_SFINAE
#  include "boost/utility/enable_if.hpp"
//#else
//#  include "boost/mpl/bool.hpp"
//#endif
#include <boost/function_equal.hpp>
#include <boost/function/function_fwd.hpp>

//#if defined(BOOST_MSVC)
//#   pragma warning( push )
//#   pragma warning( disable : 4793 ) // complaint about native code generation
//#   pragma warning( disable : 4127 ) // "conditional expression is constant"
//#endif

// Define BOOST_FUNCTION_STD_NS to the namespace that contains type_info.
//#ifdef BOOST_NO_STD_TYPEINFO
//// Embedded VC++ does not have type_info in namespace std
//#  define BOOST_FUNCTION_STD_NS
//#else
#  define BOOST_FUNCTION_STD_NS std
//#endif

// Borrowed from Boost.Python library: determines the cases where we
// need to use std::type_info::name to compare instead of operator==.
//#if defined( BOOST_NO_TYPEID )
//#  define BOOST_FUNCTION_COMPARE_TYPE_ID(X,Y) ((X)==(Y))
#if (defined(__GNUC__) && __GNUC__ >= 3) \
//#elif (defined(__GNUC__) && __GNUC__ >= 3) \
 || defined(_AIX) \
 || (   defined(__sgi) && defined(__host_mips))
#  include <cstring>
#  define BOOST_FUNCTION_COMPARE_TYPE_ID(X,Y) \
     (std::strcmp((X).name(),(Y).name()) == 0)
//# else
//#  define BOOST_FUNCTION_COMPARE_TYPE_ID(X,Y) ((X)==(Y))
#endif

//#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300 || defined(__ICL) && __ICL <= 600 || defined(__MWERKS__) && __MWERKS__ < 0x2406 && !defined(BOOST_STRICT_CONFIG)
//#  define BOOST_FUNCTION_TARGET_FIX(x) x
//#else
#  define BOOST_FUNCTION_TARGET_FIX(x)
//#endif // not MSVC

//#if !BOOST_WORKAROUND(__BORLANDC__, < 0x5A0)
#  define BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor,Type)              \
      typename ::boost::enable_if_c<(::boost::type_traits::ice_not<          \
                            (::boost::is_integral<Functor>::value)>::value), \
                           Type>::type
//#else
//// BCC doesn't recognize this depends on a template argument and complains
//// about the use of 'typename'
//#  define BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor,Type)     \
//      ::boost::enable_if_c<(::boost::type_traits::ice_not<          \
//                   (::boost::is_integral<Functor>::value)>::value), \
//                       Type>::type
//#endif

namespace boost {
    namespace detail {
        namespace function {
            class X;

            /**
             * A buffer used to store small function objects in
             * boost::function. It is a union containing function pointers,
             * object pointers, and a structure that resembles a bound
             * member function pointer.
             */
            union function_buffer
            {
                // For pointers to function objects
                mutable void* obj_ptr;

                // For pointers to std::type_info objects
                struct type_t {
                    // (get_functor_type_tag, check_functor_type_tag).
                    const detail::sp_typeinfo* type;

                    // Whether the type is const-qualified.
                    bool const_qualified;
                    // Whether the type is volatile-qualified.
                    bool volatile_qualified;
                } type;

                // For function pointers of all kinds
                mutable void (*func_ptr)();

                // For bound member pointers
                struct bound_memfunc_ptr_t {
                    void (X::*memfunc_ptr)(int);
                    void* obj_ptr;
                } bound_memfunc_ptr;

                // For references to function objects. We explicitly keep
                // track of the cv-qualifiers on the object referenced.
                struct obj_ref_t {
                    mutable void* obj_ptr;
                    bool is_const_qualified;
                    bool is_volatile_qualified;
                } obj_ref;

                // To relax aliasing constraints
                mutable char data;
            };

            /**
             * The unusable class is a placeholder for unused function arguments
             * It is also completely unusable except that it constructable from
             * anything. This helps compilers without partial specialization to
             * handle Boost.Function objects returning void.
             */
            struct unusable
            {
                unusable() {}
                template<typename T> unusable(const T&) {}
            };

            /* Determine the return type. This supports compilers that do not support
             * void returns or partial specialization by silently changing the return
             * type to "unusable".
             */
            template<typename T> struct function_return_type { typedef T type; };

            template<>
            struct function_return_type<void>
            {
                typedef unusable type;
            };

            // The operation type to perform on the given functor/function pointer
            enum functor_manager_operation_type {
                clone_functor_tag,
                move_functor_tag,
                destroy_functor_tag,
                check_functor_type_tag,
                get_functor_type_tag
            };

            // Tags used to decide between different types of functions
            struct function_ptr_tag {};
            struct function_obj_tag {};
            struct member_ptr_tag {};
            struct function_obj_ref_tag {};

            template<typename F>
            class get_function_tag
            {
                typedef typename mpl::if_c<(is_pointer<F>::value),
                        function_ptr_tag,
                        function_obj_tag>::type ptr_or_obj_tag;

                typedef typename mpl::if_c<(is_member_pointer<F>::value),
                        member_ptr_tag,
                        ptr_or_obj_tag>::type ptr_or_obj_or_mem_tag;

                typedef typename mpl::if_c<(is_reference_wrapper<F>::value),
                        function_obj_ref_tag,
                        ptr_or_obj_or_mem_tag>::type or_ref_tag;

            public:
                typedef or_ref_tag type;
            };

            // The trivial manager does nothing but return the same pointer (if we
            // are cloning) or return the null pointer (if we are deleting).
            template<typename F>
            struct reference_manager
            {
                static inline void
                manage(const function_buffer& in_buffer, function_buffer& out_buffer,
                       functor_manager_operation_type op)
                {
                    switch (op) {
                        case clone_functor_tag:
                            out_buffer.obj_ref = in_buffer.obj_ref;
                            return;

                        case move_functor_tag:
                            out_buffer.obj_ref = in_buffer.obj_ref;
                            in_buffer.obj_ref.obj_ptr = 0;
                            return;

                        case destroy_functor_tag:
                            out_buffer.obj_ref.obj_ptr = 0;
                            return;

                        case check_functor_type_tag:
                        {
                            const detail::sp_typeinfo& check_type
                                    = *out_buffer.type.type;

                            // Check whether we have the same type. We can add
                            // cv-qualifiers, but we can't take them away.
                            if (BOOST_FUNCTION_COMPARE_TYPE_ID(check_type, BOOST_SP_TYPEID(F))
                                && (!in_buffer.obj_ref.is_const_qualified
                                    || out_buffer.type.const_qualified)
                                && (!in_buffer.obj_ref.is_volatile_qualified
                                    || out_buffer.type.volatile_qualified))
                                out_buffer.obj_ptr = in_buffer.obj_ref.obj_ptr;
                            else
                                out_buffer.obj_ptr = 0;
                        }
                            return;

                        case get_functor_type_tag:
                            out_buffer.type.type = &BOOST_SP_TYPEID(F);
                            out_buffer.type.const_qualified = in_buffer.obj_ref.is_const_qualified;
                            out_buffer.type.volatile_qualified = in_buffer.obj_ref.is_volatile_qualified;
                            return;
                    }
                }
            };

            /**
             * Determine if boost::function can use the small-object
             * optimization with the function object type F.
             */
            template<typename F>
            struct function_allows_small_object_optimization
            {
                BOOST_STATIC_CONSTANT
                (bool,
                 value = ((sizeof(F) <= sizeof(function_buffer) &&
                           (alignment_of<function_buffer>::value
                            % alignment_of<F>::value == 0))));
            };

            template <typename F,typename A>
            struct functor_wrapper: public F, public A
            {
                functor_wrapper( F f, A a ):
                        F(f),
                        A(a)
                {
                }

                functor_wrapper(const functor_wrapper& f) :
                        F(static_cast<const F&>(f)),
                        A(static_cast<const A&>(f))
                {
                }
            };

            /**
             * The functor_manager class contains a static function "manage" which
             * can clone or destroy the given function/function object pointer.
             */
            template<typename Functor>
            struct functor_manager_common
            {
                typedef Functor functor_type;

                // Function pointers
                static inline void
                manage_ptr(const function_buffer& in_buffer, function_buffer& out_buffer,
                           functor_manager_operation_type op)
                {
                    if (op == clone_functor_tag)
                        out_buffer.func_ptr = in_buffer.func_ptr;
                    else if (op == move_functor_tag) {
                        out_buffer.func_ptr = in_buffer.func_ptr;
                        in_buffer.func_ptr = 0;
                    } else if (op == destroy_functor_tag)
                        out_buffer.func_ptr = 0;
                    else if (op == check_functor_type_tag) {
                        const detail::sp_typeinfo& check_type
                                = *out_buffer.type.type;
                        if (BOOST_FUNCTION_COMPARE_TYPE_ID(check_type, BOOST_SP_TYPEID(Functor)))
                            out_buffer.obj_ptr = &in_buffer.func_ptr;
                        else
                            out_buffer.obj_ptr = 0;
                    } else /* op == get_functor_type_tag */ {
                        out_buffer.type.type = &BOOST_SP_TYPEID(Functor);
                        out_buffer.type.const_qualified = false;
                        out_buffer.type.volatile_qualified = false;
                    }
                }

                // Function objects that fit in the small-object buffer.
                static inline void
                manage_small(const function_buffer& in_buffer, function_buffer& out_buffer,
                             functor_manager_operation_type op)
                {
                    if (op == clone_functor_tag || op == move_functor_tag) {
                        const functor_type* in_functor =
                                reinterpret_cast<const functor_type*>(&in_buffer.data);
                        new (reinterpret_cast<void*>(&out_buffer.data)) functor_type(*in_functor);

                        if (op == move_functor_tag) {
                            functor_type* f = reinterpret_cast<functor_type*>(&in_buffer.data);
                            (void)f; // suppress warning about the value of f not being used (MSVC)
                            f->~Functor();
                        }
                    } else if (op == destroy_functor_tag) {
                        // Some compilers (Borland, vc6, ...) are unhappy with ~functor_type.
                        functor_type* f = reinterpret_cast<functor_type*>(&out_buffer.data);
                        (void)f; // suppress warning about the value of f not being used (MSVC)
                        f->~Functor();
                    } else if (op == check_functor_type_tag) {
                        const detail::sp_typeinfo& check_type
                                = *out_buffer.type.type;
                        if (BOOST_FUNCTION_COMPARE_TYPE_ID(check_type, BOOST_SP_TYPEID(Functor)))
                            out_buffer.obj_ptr = &in_buffer.data;
                        else
                            out_buffer.obj_ptr = 0;
                    } else /* op == get_functor_type_tag */ {
                        out_buffer.type.type = &BOOST_SP_TYPEID(Functor);
                        out_buffer.type.const_qualified = false;
                        out_buffer.type.volatile_qualified = false;
                    }
                }
            };

            template<typename Functor>
            struct functor_manager
            {
            private:
                typedef Functor functor_type;

                // Function pointers
                static inline void
                manager(const function_buffer& in_buffer, function_buffer& out_buffer,
                        functor_manager_operation_type op, function_ptr_tag)
                {
                    functor_manager_common<Functor>::manage_ptr(in_buffer,out_buffer,op);
                }

                // Function objects that fit in the small-object buffer.
                static inline void
                manager(const function_buffer& in_buffer, function_buffer& out_buffer,
                        functor_manager_operation_type op, mpl::true_)
                {
                    functor_manager_common<Functor>::manage_small(in_buffer,out_buffer,op);
                }

                // Function objects that require heap allocation
                static inline void
                manager(const function_buffer& in_buffer, function_buffer& out_buffer,
                        functor_manager_operation_type op, mpl::false_)
                {
                    if (op == clone_functor_tag) {
                        // Clone the functor
                        // GCC 2.95.3 gets the CV qualifiers wrong here, so we
                        // can't do the static_cast that we should do.
                        // jewillco: Changing this to static_cast because GCC 2.95.3 is
                        // obsolete.
                        const functor_type* f =
                                static_cast<const functor_type*>(in_buffer.obj_ptr);
                        functor_type* new_f = new functor_type(*f);
                        out_buffer.obj_ptr = new_f;
                    } else if (op == move_functor_tag) {
                        out_buffer.obj_ptr = in_buffer.obj_ptr;
                        in_buffer.obj_ptr = 0;
                    } else if (op == destroy_functor_tag) {
                        /* Cast from the void pointer to the functor pointer type */
                        functor_type* f =
                                static_cast<functor_type*>(out_buffer.obj_ptr);
                        delete f;
                        out_buffer.obj_ptr = 0;
                    } else if (op == check_functor_type_tag) {
                        const detail::sp_typeinfo& check_type
                                = *out_buffer.type.type;
                        if (BOOST_FUNCTION_COMPARE_TYPE_ID(check_type, BOOST_SP_TYPEID(Functor)))
                            out_buffer.obj_ptr = in_buffer.obj_ptr;
                        else
                            out_buffer.obj_ptr = 0;
                    } else /* op == get_functor_type_tag */ {
                        out_buffer.type.type = &BOOST_SP_TYPEID(Functor);
                        out_buffer.type.const_qualified = false;
                        out_buffer.type.volatile_qualified = false;
                    }
                }

                // For function objects, we determine whether the function
                // object can use the small-object optimization buffer or
                // whether we need to allocate it on the heap.
                static inline void
                manager(const function_buffer& in_buffer, function_buffer& out_buffer,
                        functor_manager_operation_type op, function_obj_tag)
                {
                    manager(in_buffer, out_buffer, op,
                            mpl::bool_<(function_allows_small_object_optimization<functor_type>::value)>());
                }

                // For member pointers, we use the small-object optimization buffer.
                static inline void
                manager(const function_buffer& in_buffer, function_buffer& out_buffer,
                        functor_manager_operation_type op, member_ptr_tag)
                {
                    manager(in_buffer, out_buffer, op, mpl::true_());
                }

            public:
                /* Dispatch to an appropriate manager based on whether we have a
                   function pointer or a function object pointer. */
                static inline void
                manage(const function_buffer& in_buffer, function_buffer& out_buffer,
                       functor_manager_operation_type op)
                {
                    typedef typename get_function_tag<functor_type>::type tag_type;
                    switch (op) {
                        case get_functor_type_tag:
                            out_buffer.type.type = &BOOST_SP_TYPEID(functor_type);
                            out_buffer.type.const_qualified = false;
                            out_buffer.type.volatile_qualified = false;
                            return;

                        default:
                            manager(in_buffer, out_buffer, op, tag_type());
                            return;
                    }
                }
            };

            template<typename Functor, typename Allocator>
            struct functor_manager_a
            {
            private:
                typedef Functor functor_type;

                // Function pointers
                static inline void
                manager(const function_buffer& in_buffer, function_buffer& out_buffer,
                        functor_manager_operation_type op, function_ptr_tag)
                {
                    functor_manager_common<Functor>::manage_ptr(in_buffer,out_buffer,op);
                }

                // Function objects that fit in the small-object buffer.
                static inline void
                manager(const function_buffer& in_buffer, function_buffer& out_buffer,
                        functor_manager_operation_type op, mpl::true_)
                {
                    functor_manager_common<Functor>::manage_small(in_buffer,out_buffer,op);
                }

                // Function objects that require heap allocation
                static inline void
                manager(const function_buffer& in_buffer, function_buffer& out_buffer,
                        functor_manager_operation_type op, mpl::false_)
                {
                    typedef functor_wrapper<Functor,Allocator> functor_wrapper_type;
                    typedef typename Allocator::template rebind<functor_wrapper_type>::other
                            wrapper_allocator_type;
                    typedef typename wrapper_allocator_type::pointer wrapper_allocator_pointer_type;

                    if (op == clone_functor_tag) {
                        // Clone the functor
                        // GCC 2.95.3 gets the CV qualifiers wrong here, so we
                        // can't do the static_cast that we should do.
                        const functor_wrapper_type* f =
                                static_cast<const functor_wrapper_type*>(in_buffer.obj_ptr);
                        wrapper_allocator_type wrapper_allocator(static_cast<Allocator const &>(*f));
                        wrapper_allocator_pointer_type copy = wrapper_allocator.allocate(1);
                        wrapper_allocator.construct(copy, *f);

                        // Get back to the original pointer type
                        functor_wrapper_type* new_f = static_cast<functor_wrapper_type*>(copy);
                        out_buffer.obj_ptr = new_f;
                    } else if (op == move_functor_tag) {
                        out_buffer.obj_ptr = in_buffer.obj_ptr;
                        in_buffer.obj_ptr = 0;
                    } else if (op == destroy_functor_tag) {
                        /* Cast from the void pointer to the functor_wrapper_type */
                        functor_wrapper_type* victim =
                                static_cast<functor_wrapper_type*>(in_buffer.obj_ptr);
                        wrapper_allocator_type wrapper_allocator(static_cast<Allocator const &>(*victim));
                        wrapper_allocator.destroy(victim);
                        wrapper_allocator.deallocate(victim,1);
                        out_buffer.obj_ptr = 0;
                    } else if (op == check_functor_type_tag) {
                        const detail::sp_typeinfo& check_type
                                = *out_buffer.type.type;
                        if (BOOST_FUNCTION_COMPARE_TYPE_ID(check_type, BOOST_SP_TYPEID(Functor)))
                            out_buffer.obj_ptr = in_buffer.obj_ptr;
                        else
                            out_buffer.obj_ptr = 0;
                    } else /* op == get_functor_type_tag */ {
                        out_buffer.type.type = &BOOST_SP_TYPEID(Functor);
                        out_buffer.type.const_qualified = false;
                        out_buffer.type.volatile_qualified = false;
                    }
                }

                // For function objects, we determine whether the function
                // object can use the small-object optimization buffer or
                // whether we need to allocate it on the heap.
                static inline void
                manager(const function_buffer& in_buffer, function_buffer& out_buffer,
                        functor_manager_operation_type op, function_obj_tag)
                {
                    manager(in_buffer, out_buffer, op,
                            mpl::bool_<(function_allows_small_object_optimization<functor_type>::value)>());
                }

            public:
                /* Dispatch to an appropriate manager based on whether we have a
                   function pointer or a function object pointer. */
                static inline void
                manage(const function_buffer& in_buffer, function_buffer& out_buffer,
                       functor_manager_operation_type op)
                {
                    typedef typename get_function_tag<functor_type>::type tag_type;
                    switch (op) {
                        case get_functor_type_tag:
                            out_buffer.type.type = &BOOST_SP_TYPEID(functor_type);
                            out_buffer.type.const_qualified = false;
                            out_buffer.type.volatile_qualified = false;
                            return;

                        default:
                            manager(in_buffer, out_buffer, op, tag_type());
                            return;
                    }
                }
            };

            // A type that is only used for comparisons against zero
            struct useless_clear_type {};

//#ifdef BOOST_NO_SFINAE
//            // These routines perform comparisons between a Boost.Function
//      // object and an arbitrary function object (when the last
//      // parameter is mpl::bool_<false>) or against zero (when the
//      // last parameter is mpl::bool_<true>). They are only necessary
//      // for compilers that don't support SFINAE.
//      template<typename Function, typename Functor>
//        bool
//        compare_equal(const Function& f, const Functor&, int, mpl::bool_<true>)
//        { return f.empty(); }
//
//      template<typename Function, typename Functor>
//        bool
//        compare_not_equal(const Function& f, const Functor&, int,
//                          mpl::bool_<true>)
//        { return !f.empty(); }
//
//      template<typename Function, typename Functor>
//        bool
//        compare_equal(const Function& f, const Functor& g, long,
//                      mpl::bool_<false>)
//        {
//          if (const Functor* fp = f.template target<Functor>())
//            return function_equal(*fp, g);
//          else return false;
//        }
//
//      template<typename Function, typename Functor>
//        bool
//        compare_equal(const Function& f, const reference_wrapper<Functor>& g,
//                      int, mpl::bool_<false>)
//        {
//          if (const Functor* fp = f.template target<Functor>())
//            return fp == g.get_pointer();
//          else return false;
//        }
//
//      template<typename Function, typename Functor>
//        bool
//        compare_not_equal(const Function& f, const Functor& g, long,
//                          mpl::bool_<false>)
//        {
//          if (const Functor* fp = f.template target<Functor>())
//            return !function_equal(*fp, g);
//          else return true;
//        }
//
//      template<typename Function, typename Functor>
//        bool
//        compare_not_equal(const Function& f,
//                          const reference_wrapper<Functor>& g, int,
//                          mpl::bool_<false>)
//        {
//          if (const Functor* fp = f.template target<Functor>())
//            return fp != g.get_pointer();
//          else return true;
//        }
//#endif // BOOST_NO_SFINAE

            /**
             * Stores the "manager" portion of the vtable for a
             * boost::function object.
             */
            struct vtable_base
            {
                void (*manager)(const function_buffer& in_buffer,
                                function_buffer& out_buffer,
                                functor_manager_operation_type op);
            };
        } // end namespace function
    } // end namespace detail

/**
 * The function_base class contains the basic elements needed for the
 * function1, function2, function3, etc. classes. It is common to all
 * functions (and as such can be used to tell if we have one of the
 * functionN objects).
 */
    class function_base
    {
    public:
        function_base() : vtable(0) { }

        /** Determine if the function is empty (i.e., has no target). */
        bool empty() const { return !vtable; }

        /** Retrieve the type of the stored function object, or BOOST_SP_TYPEID(void)
            if this is empty. */
        const detail::sp_typeinfo& target_type() const
        {
            if (!vtable) return BOOST_SP_TYPEID(void);

            detail::function::function_buffer type;
            get_vtable()->manager(functor, type, detail::function::get_functor_type_tag);
            return *type.type.type;
        }

        template<typename Functor>
        Functor* target()
        {
            if (!vtable) return 0;

            detail::function::function_buffer type_result;
            type_result.type.type = &BOOST_SP_TYPEID(Functor);
            type_result.type.const_qualified = is_const<Functor>::value;
            type_result.type.volatile_qualified = is_volatile<Functor>::value;
            get_vtable()->manager(functor, type_result,
                                  detail::function::check_functor_type_tag);
            return static_cast<Functor*>(type_result.obj_ptr);
        }

        template<typename Functor>
//#if defined(BOOST_MSVC) && BOOST_WORKAROUND(BOOST_MSVC, < 1300)
//        const Functor* target( Functor * = 0 ) const
//#else
        const Functor* target() const
//#endif
        {
            if (!vtable) return 0;

            detail::function::function_buffer type_result;
            type_result.type.type = &BOOST_SP_TYPEID(Functor);
            type_result.type.const_qualified = true;
            type_result.type.volatile_qualified = is_volatile<Functor>::value;
            get_vtable()->manager(functor, type_result,
                                  detail::function::check_functor_type_tag);
            // GCC 2.95.3 gets the CV qualifiers wrong here, so we
            // can't do the static_cast that we should do.
            return static_cast<const Functor*>(type_result.obj_ptr);
        }

        template<typename F>
        bool contains(const F& f) const
        {
//#if defined(BOOST_MSVC) && BOOST_WORKAROUND(BOOST_MSVC, < 1300)
//            if (const F* fp = this->target( (F*)0 ))
//#else
            if (const F* fp = this->template target<F>())
//#endif
            {
                return function_equal(*fp, f);
            } else {
                return false;
            }
        }

//#if defined(__GNUC__) && __GNUC__ == 3 && __GNUC_MINOR__ <= 3
//        // GCC 3.3 and newer cannot copy with the global operator==, due to
//  // problems with instantiation of function return types before it
//  // has been verified that the argument types match up.
//  template<typename Functor>
//    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
//    operator==(Functor g) const
//    {
//      if (const Functor* fp = target<Functor>())
//        return function_equal(*fp, g);
//      else return false;
//    }
//
//  template<typename Functor>
//    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
//    operator!=(Functor g) const
//    {
//      if (const Functor* fp = target<Functor>())
//        return !function_equal(*fp, g);
//      else return true;
//    }
//#endif

    public: // should be protected, but GCC 2.95.3 will fail to allow access
        detail::function::vtable_base* get_vtable() const {
         // Original code: return reinterpret_cast<detail::function::vtable_base*>(reinterpret_cast<std::size_t>(vtable) & ~static_cast<std::size_t>(0x01));
            return reinterpret_cast<detail::function::vtable_base*>(reinterpret_cast<std::size_t>(vtable) & ~static_cast<std::size_t>(0x01));
        }

        bool has_trivial_copy_and_destroy() const {
            return reinterpret_cast<std::size_t>(vtable) & 0x01;
        }

        detail::function::vtable_base* vtable;
        mutable detail::function::function_buffer functor;
    };

/**
 * The bad_function_call exception class is thrown when a boost::function
 * object is invoked
 */
    class bad_function_call : public std::runtime_error
    {
    public:
        bad_function_call() : std::runtime_error("call to empty boost::function") {}
    };

#ifndef BOOST_NO_SFINAE
    inline bool operator==(const function_base& f,
                           detail::function::useless_clear_type*)
    {
        return f.empty();
    }

    inline bool operator!=(const function_base& f,
                           detail::function::useless_clear_type*)
    {
        return !f.empty();
    }

    inline bool operator==(detail::function::useless_clear_type*,
                           const function_base& f)
    {
        return f.empty();
    }

    inline bool operator!=(detail::function::useless_clear_type*,
                           const function_base& f)
    {
        return !f.empty();
    }
#endif

#ifdef BOOST_NO_SFINAE
    // Comparisons between boost::function objects and arbitrary function objects
template<typename Functor>
  inline bool operator==(const function_base& f, Functor g)
  {
    typedef mpl::bool_<(is_integral<Functor>::value)> integral;
    return detail::function::compare_equal(f, g, 0, integral());
  }

template<typename Functor>
  inline bool operator==(Functor g, const function_base& f)
  {
    typedef mpl::bool_<(is_integral<Functor>::value)> integral;
    return detail::function::compare_equal(f, g, 0, integral());
  }

template<typename Functor>
  inline bool operator!=(const function_base& f, Functor g)
  {
    typedef mpl::bool_<(is_integral<Functor>::value)> integral;
    return detail::function::compare_not_equal(f, g, 0, integral());
  }

template<typename Functor>
  inline bool operator!=(Functor g, const function_base& f)
  {
    typedef mpl::bool_<(is_integral<Functor>::value)> integral;
    return detail::function::compare_not_equal(f, g, 0, integral());
  }
#else

#  if !(defined(__GNUC__) && __GNUC__ == 3 && __GNUC_MINOR__ <= 3)
// Comparisons between boost::function objects and arbitrary function
// objects. GCC 3.3 and before has an obnoxious bug that prevents this
// from working.
    template<typename Functor>
    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
    operator==(const function_base& f, Functor g)
    {
        if (const Functor* fp = f.template target<Functor>())
            return function_equal(*fp, g);
        else return false;
    }

    template<typename Functor>
    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
    operator==(Functor g, const function_base& f)
    {
        if (const Functor* fp = f.template target<Functor>())
            return function_equal(g, *fp);
        else return false;
    }

    template<typename Functor>
    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
    operator!=(const function_base& f, Functor g)
    {
        if (const Functor* fp = f.template target<Functor>())
            return !function_equal(*fp, g);
        else return true;
    }

    template<typename Functor>
    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
    operator!=(Functor g, const function_base& f)
    {
        if (const Functor* fp = f.template target<Functor>())
            return !function_equal(g, *fp);
        else return true;
    }
#  endif

    template<typename Functor>
    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
    operator==(const function_base& f, reference_wrapper<Functor> g)
    {
        if (const Functor* fp = f.template target<Functor>())
            return fp == g.get_pointer();
        else return false;
    }

    template<typename Functor>
    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
    operator==(reference_wrapper<Functor> g, const function_base& f)
    {
        if (const Functor* fp = f.template target<Functor>())
            return g.get_pointer() == fp;
        else return false;
    }

    template<typename Functor>
    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
    operator!=(const function_base& f, reference_wrapper<Functor> g)
    {
        if (const Functor* fp = f.template target<Functor>())
            return fp != g.get_pointer();
        else return true;
    }

    template<typename Functor>
    BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL(Functor, bool)
    operator!=(reference_wrapper<Functor> g, const function_base& f)
    {
        if (const Functor* fp = f.template target<Functor>())
            return g.get_pointer() != fp;
        else return true;
    }

#endif // Compiler supporting SFINAE

    namespace detail {
        namespace function {
            inline bool has_empty_target(const function_base* f)
            {
                return f->empty();
            }

#if BOOST_WORKAROUND(BOOST_MSVC, <= 1310)
            inline bool has_empty_target(const void*)
    {
      return false;
    }
#else
            inline bool has_empty_target(...)
            {
                return false;
            }
#endif
        } // end namespace function
    } // end namespace detail
} // end namespace boost

#undef BOOST_FUNCTION_ENABLE_IF_NOT_INTEGRAL
#undef BOOST_FUNCTION_COMPARE_TYPE_ID

//#if defined(BOOST_MSVC)
//#   pragma warning( pop )
//#endif

//#endif // BOOST_FUNCTION_BASE_HEADER

namespace boost {

    template<typename R, typename T0>
    class function1 {
    };

    template<typename R, typename T0>
    void operator==(const function1<R, T0> &,
                    const function1<R, T0> &);

    template <bool B, class T = void>
    struct local_enable_if_c {
        typedef T type;
    };

    template<typename R, typename T0>
    class function<R(T0)>
            : public function1<R, T0> {
    public:
        template<typename Functor>
        function(Functor f, typename local_enable_if_c<(true), int>::type = 0) {
        }
    };
}
 // END reduced boost

typedef struct Database_s {
    double etamin;
} Database_t;
// This is not enough to fail:
#include <vector>
// Need this:
// #include "boost/shared_ptr.hpp"
struct struct1 {
    struct1(
            int int1,
            std::vector<std::string> const &vector2) {}
};
typedef std::vector<struct1> vector1_type;
typedef boost::function<int(Database_t *)> DBParseFunction;
typedef std::map<std::string, DBParseFunction> map1_type;

#include "boost/assign/list_of.hpp"
const vector1_type vector1 = boost::assign::list_of<struct1>
        (1, boost::assign::list_of("p"))
        (1, boost::assign::list_of("lmn")("o"))
        (1, boost::assign::list_of("ghij")("k"));

void func1() {
    map1_type map1 = boost::assign::map_list_of
            ("abcdef", createParseAndSetMember(&Database_t::etamin));
}

 // Code above is reproducer for ROSE IdentityTranslator error that looks like this::
 // ...
 // TypidentityTranslator: /tmp/too1/spack-stage/spack-stage-h9i5VC/rose/src/frontend/SageIII/sageInterface/sageInterface.C:20774: bool SageInterface::isEquivalentType(const SgType*, const SgType*): Assertion `false' failed.
 // ...
 // ERROR: In SageInterface::isEquivalentType(): recursive limit exceeded for : counter = 301

