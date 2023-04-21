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
#else
#define DECLARE_LEAF_CLASS(CLASS_WITHOUT_Sg) \
    NEW_TERMINAL_MACRO(CLASS_WITHOUT_Sg, #CLASS_WITHOUT_Sg, #CLASS_WITHOUT_Sg "Tag"); \
    CLASS_WITHOUT_Sg.setCppCondition("!defined(DOCUMENTATION)");\
    CLASS_WITHOUT_Sg.setAutomaticGenerationOfConstructor(false);\
    CLASS_WITHOUT_Sg.setAutomaticGenerationOfDestructor(false)
#endif

#ifdef DOCUMENTATION
#define DECLARE_HEADERS(CLASS_WITHOUT_Sg) /*void*/
#else
#define DECLARE_HEADERS(CLASS_WITHOUT_Sg) \
    CLASS_WITHOUT_Sg.setPredeclarationString("Sg" #CLASS_WITHOUT_Sg "_HEADERS", \
                          ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR + "/src/ROSETTA/src/jovialNodes.C")
#endif

#ifdef DOCUMENTATION
#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) /*void*/
#else
#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) \
    CLASS_WITHOUT_Sg.setFunctionPrototype("Sg" #CLASS_WITHOUT_Sg "_OTHERS", \
                          ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR + "/src/ROSETTA/src/jovialNodes.C")
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
#endif // DOCUMENTATION

#ifndef DOCUMENTATION
void Grammar::setUpJovialNodes() {
#endif // !DOCUMENTATION

#ifndef DOCUMENTATION
} // Grammar::setUpBinaryInstruction
#endif // !DOCUMENTATION
