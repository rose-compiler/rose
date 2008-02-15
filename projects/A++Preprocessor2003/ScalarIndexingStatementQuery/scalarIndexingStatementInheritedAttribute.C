// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "arrayPreprocessor.h"

// include file for transformation specification support
// #include "specification.h"
// #include "globalTraverse.h"
// #include "query.h"
// string class used if compiler does not contain a C++ string class
// #include <roseString.h>

// #include "nameQuery.h"

// #include "arrayStatementTransformation.h"
// #include "arrayTransformationSupport.h"
// #include "indexOffsetQuery.h"
// #include "scalarIndexingStatementTransformation.h"

ScalarIndexingStatementQueryInheritedAttributeType::
~ScalarIndexingStatementQueryInheritedAttributeType()
   {
     skipSubstitutionOfSubscriptComputationMacro = FALSE;
     usingIndexObjectsInSubscriptComputation     = FALSE;

     arrayStatementDimensionDefined = FALSE;
     arrayStatementDimension        = -99;
     isIndexedArrayOperand          = FALSE;
   }

ScalarIndexingStatementQueryInheritedAttributeType::
ScalarIndexingStatementQueryInheritedAttributeType()
   : ArrayStatementQueryInheritedAttributeType (NULL)
   {
     skipSubstitutionOfSubscriptComputationMacro = FALSE;
     usingIndexObjectsInSubscriptComputation     = FALSE;

     arrayStatementDimensionDefined = TRUE;
     arrayStatementDimension        = 0;
     isIndexedArrayOperand          = FALSE;
   }

ScalarIndexingStatementQueryInheritedAttributeType::
ScalarIndexingStatementQueryInheritedAttributeType ( SgNode* astNode )
   : ArrayStatementQueryInheritedAttributeType (astNode)
   {
     skipSubstitutionOfSubscriptComputationMacro = FALSE;
     usingIndexObjectsInSubscriptComputation     = FALSE;

     arrayStatementDimensionDefined = TRUE;
     arrayStatementDimension        = 0;
     isIndexedArrayOperand          = FALSE;
   }

ScalarIndexingStatementQueryInheritedAttributeType::
ScalarIndexingStatementQueryInheritedAttributeType
   ( const ArrayStatementQueryInheritedAttributeType & X, SgNode* astNode )
   : ArrayStatementQueryInheritedAttributeType (X,astNode)
   {
  // This function is required in initial construction of the 
  // inherited attribute within the transformation function.
     transformationOptions                       = X.transformationOptions;
  // skipSubstitutionOfSubscriptComputationMacro = X.skipSubstitutionOfSubscriptComputationMacro;
  // usingIndexObjectsInSubscriptComputation     = X.usingIndexObjectsInSubscriptComputation;

     arrayDimensions                = X.arrayDimensions;
     arrayStatementDimensionDefined = X.arrayStatementDimensionDefined;
     arrayStatementDimension        = X.arrayStatementDimension;
  // isIndexedArrayOperand          = X.isIndexedArrayOperand;
   }

ScalarIndexingStatementQueryInheritedAttributeType::
ScalarIndexingStatementQueryInheritedAttributeType
   ( const ScalarIndexingStatementQueryInheritedAttributeType & X )
   : ArrayStatementQueryInheritedAttributeType (X)
   {
     transformationOptions                       = X.transformationOptions;
     skipSubstitutionOfSubscriptComputationMacro = X.skipSubstitutionOfSubscriptComputationMacro;
     usingIndexObjectsInSubscriptComputation     = X.usingIndexObjectsInSubscriptComputation;

     arrayDimensions                = X.arrayDimensions;
     arrayStatementDimensionDefined = X.arrayStatementDimensionDefined;
     arrayStatementDimension        = X.arrayStatementDimension;
     isIndexedArrayOperand          = X.isIndexedArrayOperand;
   }

ScalarIndexingStatementQueryInheritedAttributeType::
ScalarIndexingStatementQueryInheritedAttributeType
   ( const ScalarIndexingStatementQueryInheritedAttributeType & X, SgNode* astNode )
   : ArrayStatementQueryInheritedAttributeType (X,astNode)
   {
     transformationOptions                       = X.transformationOptions;
     skipSubstitutionOfSubscriptComputationMacro = X.skipSubstitutionOfSubscriptComputationMacro;
     usingIndexObjectsInSubscriptComputation     = X.usingIndexObjectsInSubscriptComputation;

     arrayDimensions                = X.arrayDimensions;
     arrayStatementDimensionDefined = X.arrayStatementDimensionDefined;
     arrayStatementDimension        = X.arrayStatementDimension;
     isIndexedArrayOperand          = X.isIndexedArrayOperand;
   }

ScalarIndexingStatementQueryInheritedAttributeType &
ScalarIndexingStatementQueryInheritedAttributeType::operator= ( 
     const ScalarIndexingStatementQueryInheritedAttributeType & X )
   {
     ArrayStatementQueryInheritedAttributeType::operator=(X);

     transformationOptions                       = X.transformationOptions;
     skipSubstitutionOfSubscriptComputationMacro = X.skipSubstitutionOfSubscriptComputationMacro;
     usingIndexObjectsInSubscriptComputation     = X.usingIndexObjectsInSubscriptComputation;

     arrayDimensions                = X.arrayDimensions;
     arrayStatementDimensionDefined = X.arrayStatementDimensionDefined;
     arrayStatementDimension        = X.arrayStatementDimension;
     isIndexedArrayOperand          = X.isIndexedArrayOperand;

     return *this;
   }

void
ScalarIndexingStatementQueryInheritedAttributeType::display ( const char* label ) const
   {
     printf ("ScalarIndexingStatementQueryInheritedAttributeType::display(%s) \n",label);

     list<int>::const_iterator i;
     printf ("arrayDimensions                             = ");
     for (i = arrayDimensions.begin(); i != arrayDimensions.end(); i++)
          printf ("%d ",*i);
     printf ("\n");

     printf ("transformationOptions                       = ");
     for (i = transformationOptions.begin(); i != transformationOptions.end(); i++)
          printf ("%d ",*i);
     printf ("\n");

     printf ("arrayStatementDimension                     = %d \n",arrayStatementDimension);
     printf ("arrayStatementDimensionDefined              = %s \n",
          arrayStatementDimensionDefined ? "TRUE" : "FALSE");
     printf ("usingIndexObjectsInSubscriptComputation     = %s \n",
          usingIndexObjectsInSubscriptComputation ? "TRUE" : "FALSE");
     printf ("skipSubstitutionOfSubscriptComputationMacro = %s \n",
          skipSubstitutionOfSubscriptComputationMacro ? "TRUE" : "FALSE");
     printf ("isIndexedArrayOperand = %s \n",
          isIndexedArrayOperand ? "TRUE" : "FALSE");
   }

list<int> &
ScalarIndexingStatementQueryInheritedAttributeType::getTransformationOptions () const
   {
  // Note: cast away const
     return ((ScalarIndexingStatementQueryInheritedAttributeType*) this)->transformationOptions;
  // return transformationOptions;
   }

void
ScalarIndexingStatementQueryInheritedAttributeType::setTransformationOptions ( const list<int> & X )
   {
     transformationOptions = X;
   }

bool
ScalarIndexingStatementQueryInheritedAttributeType::getSkipSubstitutionOfSubscriptComputationMacro () const
   {
     return skipSubstitutionOfSubscriptComputationMacro;
   }

void
ScalarIndexingStatementQueryInheritedAttributeType::setSkipSubstitutionOfSubscriptComputationMacro ( bool newValue )
   {
     skipSubstitutionOfSubscriptComputationMacro = newValue;
   }

bool
ScalarIndexingStatementQueryInheritedAttributeType::getUsingIndexObjectsInSubscriptComputation () const
   {
     return usingIndexObjectsInSubscriptComputation;
   }

void
ScalarIndexingStatementQueryInheritedAttributeType::setUsingIndexObjectsInSubscriptComputation ( bool newValue )
   {
     usingIndexObjectsInSubscriptComputation = newValue;
   }

bool
ScalarIndexingStatementQueryInheritedAttributeType::getIsIndexedArrayOperand ()
   {
     return isIndexedArrayOperand;
   }

void
ScalarIndexingStatementQueryInheritedAttributeType::setIsIndexedArrayOperand ( bool value )
   {
     isIndexedArrayOperand = value;
   }













