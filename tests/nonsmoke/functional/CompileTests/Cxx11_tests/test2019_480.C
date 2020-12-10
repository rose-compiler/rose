
// DQ (1/24/2017): Added declaration of this type that comes from the rose_edg_required_macros_and_functions.h file.
// Note that this is only required within ROSE is using the GNU 5.1 compiler.  Not yet clear why that is.
// typedef unsigned int __mmask32;

template <typename S, typename T>
class ParseAndSetMember
{};

template <typename S, typename T>
ParseAndSetMember<S,T> createParseAndSetMember( T S::*mptr)
{}

// DQ (7/23/2020): GNU 10 STL requires that we include the STL string header file explicitly.
#include <string>

#include <map>
namespace LEOS {
    enum functionsType {
        FT_Bcv,  
        FT_Cs,
#if 0
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
#endif
        FT_Bltemp
    };
    typedef unsigned int L8UINT;
    typedef L8UINT functionType_t;
    const functionType_t FT_LEOS(FT_Bltemp + 1);
    typedef std::string  L8STRING;
    using std::map;
    typedef map<functionType_t, L8STRING> genericNamesMap_t;

#if 1
    //BELOW NEEDED FOR ERROR
    const genericNamesMap_t::value_type genericNamesData[] = {
            //IF STRINGS ARE NULL BELOW, FAILS DIFFERENTLY:
            genericNamesMap_t::value_type(FT_Bcv,    "curve name: Phase_Curve"),
#if 0
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
#endif
#if 0
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
#endif
    };
#endif

#if 0
    typedef map<functionType_t, L8STRING> genericTypesMap_t;
#endif

}
