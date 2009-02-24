#ifndef ROSE_ARRAY_PREPROCESSOR_H
#define ROSE_ARRAY_PREPROCESSOR_H



// Support for use of query library
#include "roseQueryLib.h"

// Allows use of transformationOptionNames variable for mapping
// enum values to strings that describe each option
#include "transformationOptions.h"

// Rewrite mechanism for use by all transformations
#include "rewrite.h"

// Define the inherited and synthesized attributes using the ROSE rewrite mechanism
typedef HighLevelRewrite::SynthesizedAttribute SynthesizedAttributeBaseClassType;
// typedef HighLevelRewrite::InheritedAttribute   InheritedAttributeBaseClassType;

#include "arrayInheritedAttributeBaseClass.h"

// This is a ROSE header file with general support for transformations
#include "transformationSupport.h"
#include "arrayTransformationSupport.h"

// Array Preprocessor specific mechanism for holding data specific 
// to operands within expressions.
#include "operandDataBase.h"

// Main header files for the nested transformations used by the top level of the array
// preprocessor.
#include "indexOffsetQuery.h"
#include "arrayAssignmentStatementTransformation.h"
#include "scalarIndexingStatementTransformation.h"

// #include "scalarIndexingArrayStatementTransformation.h"

// Main (top level) transformation mechanism (calling all others)
#include "programTransformation.h"

// endif for ROSE_ARRAY_PREPROCESSOR_H
#endif
