// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "arrayPreprocessor.h"

// #include "scalarIndexingStatementSynthesizedAttribute.h"

// generate a display string of the information in the index operand
string
ScalarIndexingStatementQuerySynthesizedAttributeType::
displayString() const
   {
     string returnString = SynthesizedAttributeBaseClassType::displayString();
     returnString += string("[ Workspace: ") + workspace + "] ";

     return returnString;
   }

// ##########################################
//        Constructors and Operators
// ##########################################
ScalarIndexingStatementQuerySynthesizedAttributeType::
~ScalarIndexingStatementQuerySynthesizedAttributeType ()
   {
   }

ScalarIndexingStatementQuerySynthesizedAttributeType::
ScalarIndexingStatementQuerySynthesizedAttributeType ()
   : SynthesizedAttributeBaseClassType(NULL)
   {
   }

ScalarIndexingStatementQuerySynthesizedAttributeType::
ScalarIndexingStatementQuerySynthesizedAttributeType ( SgNode* astNode )
   : SynthesizedAttributeBaseClassType(astNode)
   {
   }

ScalarIndexingStatementQuerySynthesizedAttributeType::
ScalarIndexingStatementQuerySynthesizedAttributeType (
     const ScalarIndexingStatementQuerySynthesizedAttributeType & X )
   : SynthesizedAttributeBaseClassType(NULL)
   {
  // Deep copy semantics
  // ((ScalarIndexingStatementQuerySynthesizedAttributeType*) this)->operator= (X);
     operator= (X);
   }

#if 0
// I don't think this is used (and we don't want to need it)
ScalarIndexingStatementQuerySynthesizedAttributeType::
ScalarIndexingStatementQuerySynthesizedAttributeType (
     const vector<ScalarIndexingStatementQuerySynthesizedAttributeType> & synthesizedAttributeList )
   : SynthesizedAttributeBaseClassType(NULL)
   {
   }
#endif

// Assignment of the base class data
ScalarIndexingStatementQuerySynthesizedAttributeType &
ScalarIndexingStatementQuerySynthesizedAttributeType::
operator= ( const SynthesizedAttributeBaseClassType & X )
   {
     printf ("In ScalarIndexingStatementQuerySynthesizedAttributeType operator=(SynthesizedAttributeBaseClassType) (this = %p) \n",this);

     SynthesizedAttributeBaseClassType::operator= ( X );

     workspace = "";

     return *this;
   }

ScalarIndexingStatementQuerySynthesizedAttributeType &
ScalarIndexingStatementQuerySynthesizedAttributeType::
operator= ( const ScalarIndexingStatementQuerySynthesizedAttributeType & X )
   {
  // Call the assignment operator in the base class
     SynthesizedAttributeBaseClassType::operator=(X);

     workspace = X.workspace;

     return *this;
   }

ScalarIndexingStatementQuerySynthesizedAttributeType &
ScalarIndexingStatementQuerySynthesizedAttributeType::
operator+= ( const ScalarIndexingStatementQuerySynthesizedAttributeType & X )
   {
  // This function is called by the ScalarIndexingStatementQueryAssemblyFunction to handle the
  // assembly of systhesized attributes for the default case.  This function implements the
  // policy regarding the workspace strings.

     SynthesizedAttributeBaseClassType::operator+=(X);

  // Clearly the effect of this depends upon evaluation order!
     workspace += X.workspace;

     return *this;
   }

string
ScalarIndexingStatementQuerySynthesizedAttributeType::getWorkSpace () const
   {
  // access function for loopDependence variable
     return workspace;
   }

void
ScalarIndexingStatementQuerySynthesizedAttributeType::setWorkSpace ( string X )
   {
  // access function for loopDependence variable
     workspace = X;
   }










