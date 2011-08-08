#ifndef ROSE_ARRAY_PREPROCESSOR_H
#define ROSE_ARRAY_PREPROCESSOR_H

#include "rose.h"
// Support for use of query library
#include "roseQueryLib.h"

// Allows use of transformationOptionNames variable for mapping
// enum values to strings that describe each option
#include "transformationOptions.h"

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

// Main (top level) transformation mechanism (calling all others)
#include "programTransformation.h"

#define TRUE 1
#define FALSE 0

void ArrayPreprocessor(SgProject* project);
// endif for ROSE_ARRAY_PREPROCESSOR_H
#endif
