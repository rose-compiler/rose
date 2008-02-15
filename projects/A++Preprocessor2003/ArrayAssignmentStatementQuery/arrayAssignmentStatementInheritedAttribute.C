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
// #include "arrayAssignmentStatementTransformation.h"

ArrayAssignmentStatementQueryInheritedAttributeType::
~ArrayAssignmentStatementQueryInheritedAttributeType()
   {
     skipSubstitutionOfSubscriptComputationMacro = FALSE;
     usingIndexObjectsInSubscriptComputation     = FALSE;

     arrayStatementDimensionDefined = FALSE;
     arrayStatementDimension        = -99;
     isIndexedArrayOperand          = FALSE;
   }

ArrayAssignmentStatementQueryInheritedAttributeType::
ArrayAssignmentStatementQueryInheritedAttributeType()
   : ArrayStatementQueryInheritedAttributeType (NULL)
   {
     skipSubstitutionOfSubscriptComputationMacro = FALSE;
     usingIndexObjectsInSubscriptComputation     = FALSE;

     arrayStatementDimensionDefined = TRUE;
     arrayStatementDimension        = 0;
     isIndexedArrayOperand          = FALSE;
   }

ArrayAssignmentStatementQueryInheritedAttributeType::
ArrayAssignmentStatementQueryInheritedAttributeType( SgNode* astNode )
   : ArrayStatementQueryInheritedAttributeType (astNode)
   {
     skipSubstitutionOfSubscriptComputationMacro = FALSE;
     usingIndexObjectsInSubscriptComputation     = FALSE;

     arrayStatementDimensionDefined = TRUE;
     arrayStatementDimension        = 0;
     isIndexedArrayOperand          = FALSE;
   }

ArrayAssignmentStatementQueryInheritedAttributeType::
ArrayAssignmentStatementQueryInheritedAttributeType (
   const ArrayAssignmentStatementQueryInheritedAttributeType & X )
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

#if 0
ArrayAssignmentStatementQueryInheritedAttributeType::
ArrayAssignmentStatementQueryInheritedAttributeType (
   const ArrayAssignmentStatementQueryInheritedAttributeType & X,
   SgNode* astNode )
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
#endif

ArrayAssignmentStatementQueryInheritedAttributeType::
ArrayAssignmentStatementQueryInheritedAttributeType (
   const ArrayStatementQueryInheritedAttributeType & X,
   SgNode* astNode )
   : ArrayStatementQueryInheritedAttributeType (X,astNode)
   {
     skipSubstitutionOfSubscriptComputationMacro = false;
     usingIndexObjectsInSubscriptComputation     = false;
     isIndexedArrayOperand                       = false;
   }

ArrayAssignmentStatementQueryInheritedAttributeType &
ArrayAssignmentStatementQueryInheritedAttributeType::operator= (
     const ArrayAssignmentStatementQueryInheritedAttributeType & X )
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
ArrayAssignmentStatementQueryInheritedAttributeType::display ( const char* label ) const
   {
     printf ("ArrayAssignmentStatementQueryInheritedAttributeType::display(%s) \n",label);

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
ArrayAssignmentStatementQueryInheritedAttributeType::getTransformationOptions () const
   {
  // Note: cast away const
     return ((ArrayAssignmentStatementQueryInheritedAttributeType*) this)->transformationOptions;
  // return transformationOptions;
   }

void
ArrayAssignmentStatementQueryInheritedAttributeType::setTransformationOptions ( const list<int> & X )
   {
     transformationOptions = X;
   }

bool
ArrayAssignmentStatementQueryInheritedAttributeType::getSkipSubstitutionOfSubscriptComputationMacro () const
   {
     return skipSubstitutionOfSubscriptComputationMacro;
   }

void
ArrayAssignmentStatementQueryInheritedAttributeType::setSkipSubstitutionOfSubscriptComputationMacro ( bool newValue )
   {
     skipSubstitutionOfSubscriptComputationMacro = newValue;
   }

bool
ArrayAssignmentStatementQueryInheritedAttributeType::getUsingIndexObjectsInSubscriptComputation () const
   {
     return usingIndexObjectsInSubscriptComputation;
   }

void
ArrayAssignmentStatementQueryInheritedAttributeType::setUsingIndexObjectsInSubscriptComputation ( bool newValue )
   {
     usingIndexObjectsInSubscriptComputation = newValue;
   }

bool
ArrayAssignmentStatementQueryInheritedAttributeType::getIsIndexedArrayOperand ()
   {
     return isIndexedArrayOperand;
   }

void
ArrayAssignmentStatementQueryInheritedAttributeType::setIsIndexedArrayOperand ( bool value )
   {
     isIndexedArrayOperand = value;
   }













