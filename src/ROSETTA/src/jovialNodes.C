//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         THIS FILE IS MACHINE GENERATED  --  DO NOT MODIFY
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This file was generated with ROSE's "rosebud" tool by reading node definitions written in a
// C++-like language and emitting this ROSETTA input.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DO NOT MODIFY THIS FILE MANUALLY!
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <featureTests.h>
#include "ROSETTA_macros.h"
#include "grammar.h"
#include "AstNodeClass.h"

//#undef DOCUMENTATION -- commented out so IDEs can't figure it out
#ifdef DOCUMENTATION
DOCUMENTATION_should_never_be_defined;
#endif

#ifdef DOCUMENTATION
#define DECLARE_LEAF_CLASS(CLASS_WITHOUT_Sg) /*void*/
#define DECLARE_LEAF_CLASS2(CLASS_WITHOUT_Sg, TAG) /*void*/
#else
#define DECLARE_LEAF_CLASS(CLASS_WITHOUT_Sg) \
    NEW_TERMINAL_MACRO(CLASS_WITHOUT_Sg, #CLASS_WITHOUT_Sg, #CLASS_WITHOUT_Sg "Tag"); \
    CLASS_WITHOUT_Sg.setCppCondition("!defined(DOCUMENTATION)");\
    CLASS_WITHOUT_Sg.setAutomaticGenerationOfConstructor(false);\
    CLASS_WITHOUT_Sg.setAutomaticGenerationOfDestructor(false)
#define DECLARE_LEAF_CLASS2(CLASS_WITHOUT_Sg, TAG) \
    NEW_TERMINAL_MACRO(CLASS_WITHOUT_Sg, #CLASS_WITHOUT_Sg, #TAG); \
    CLASS_WITHOUT_Sg.setCppCondition("!defined(DOCUMENTATION)");\
    CLASS_WITHOUT_Sg.setAutomaticGenerationOfConstructor(false);\
    CLASS_WITHOUT_Sg.setAutomaticGenerationOfDestructor(false)
#endif

#ifdef DOCUMENTATION
#define DECLARE_HEADERS(CLASS_WITHOUT_Sg) /*void*/
#else
#define DECLARE_HEADERS(CLASS_WITHOUT_Sg) \
    CLASS_WITHOUT_Sg.setPredeclarationString("Sg" #CLASS_WITHOUT_Sg "_HEADERS", \
                          ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR + "//src/ROSETTA/src/jovialNodes.C")
#endif

#ifdef DOCUMENTATION
#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) /*void*/
#else
#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) \
    CLASS_WITHOUT_Sg.setFunctionPrototype("Sg" #CLASS_WITHOUT_Sg "_OTHERS", \
                          ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR + "//src/ROSETTA/src/jovialNodes.C")
#endif

#ifdef DOCUMENTATION
#define IS_SERIALIZABLE() /*void*/
#else
#define IS_SERIALIZABLE(CLASS_WITHOUT_Sg) \
    CLASS_WITHOUT_Sg.isBoostSerializable(true)
#endif


// Since ROSETTA builds classes from the leaves up to the base, and C++ builds classes from the
// base down to the leaves, we need to make sure that doxygen sees the base classes before the derived classes. So
// just list all the non-leaf classes here.
#ifdef DOCUMENTATION
class SgValueExp;
#endif // DOCUMENTATION

#ifndef DOCUMENTATION
void Grammar::setUpJovialNodes() {
#endif // !DOCUMENTATION


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgJovialBitVal           -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_LEAF_CLASS(JovialBitVal);

#ifdef DOCUMENTATION
/** An expression class for Jovial bit values.
 *
 *  A Jovial bit formula (expression) consists of bit operands and a bit operator.
 *  The bit operators are NOT, AND, OR, XOR, and EQV.
 */
class SgJovialBitVal: public SgValueExp {
#endif // DOCUMENTATION

#ifndef DOCUMENTATION
    JovialBitVal.setDataPrototype(
        "std::string", "valueString", "",
        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif // !DOCUMENTATION

#ifndef DOCUMENTATION
    JovialBitVal.setFunctionSource("SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code");
    JovialBitVal.setFunctionSource("SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code");
    JovialBitVal.editSubstitute("GENERIC_TYPE", "SgJovialBitType");
#endif // !DOCUMENTATION

    DECLARE_OTHERS(JovialBitVal);
#if defined(SgJovialBitVal_OTHERS) || defined(DOCUMENTATION)

public:
    /** Property: A copy (as coded) of the bit value string. 
     * 
     * @{ */
    std::string const& get_valueString() const;
    void set_valueString(std::string const&);
    /** @} */
    // FIXME[Robb Matzke 2023-04-21]: These pragmas are compatibility of last resort. Please remove when possible because in my
    // experience most of this stuff doesn't need to be generated--it can be written directly in C++ using virtual functions,
    // template meta programming, and introspection rather than text substitution.

public:
    /** Destructor. */
    virtual ~SgJovialBitVal();

public:
    /** Default constructor. */
    SgJovialBitVal();

public:
    /** Constructor. */
    explicit SgJovialBitVal(std::string const& valueString);

protected:
    /** Initialize all properties that have explicit initial values.
     *
     *  This function is mostly for use in user-defined constructors where the user desires to initialize
     *  all the properties but does not know the names of the data members that store the property values.
     *  This function initializes the properties that have explicit initializations within this class, but
     *  does not recursively initialize base classes. */
    void initializeProperties();
#endif // SgJovialBitVal_OTHERS
#ifdef DOCUMENTATION
};
#endif // DOCUMENTATION

#ifndef DOCUMENTATION
} // Grammar::setUpJovialNodes
#endif // !DOCUMENTATION
