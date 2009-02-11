#ifndef AST_REWRITE_AST_FRAGMENT_STRING_TEMPLATES_C
#define AST_REWRITE_AST_FRAGMENT_STRING_TEMPLATES_C

// #include "rose.h"

// ************************************************************
//               Source Code String Class
// ************************************************************

// AbstractInterfaceNodeCollection::TransformationStringType
// TransformationStringTemplatedType
// AbstractInterfaceNodeCollection::TransformationStringType
template <class T>
TransformationStringTemplatedType<T>::~TransformationStringTemplatedType()
   {
     associatedASTNode        = NULL;
     sourceCode               = "";
     relativeScope            = T::unknownScope;
     relativeLocation         = T::unknownPositionInScope;
//   sourceCodeClassification = unknownSourceCodeStringClassification;
     buildInNewScope          = false;
   }

template <class T>
TransformationStringTemplatedType<T>::
TransformationStringTemplatedType( const TransformationStringTemplatedType & X )
   {
     operator= (X);
   }

template <class T>
TransformationStringTemplatedType<T> &
TransformationStringTemplatedType<T>::operator= ( const TransformationStringTemplatedType & X )
   {
     associatedASTNode        = X.associatedASTNode;
     relativeScope            = X.relativeScope;
     relativeLocation         = X.relativeLocation;
  // sourceCodeClassification = X.sourceCodeClassification;
     sourceCode               = X.sourceCode;
     buildInNewScope          = X.buildInNewScope;

     return *this;
   }

template <class T>
bool
TransformationStringTemplatedType<T>::operator== ( const TransformationStringTemplatedType & X ) const
   {
  // boolean test function
     bool returnValue = false;

  // Any location in a specific scope should be considered the same since 
  // they would count as redundent when compiling the resulting intermediate file.
  // (relativeLocation         == X.relativeLocation)
     if ( (relativeScope            == X.relativeScope) &&
       // (sourceCodeClassification == X.sourceCodeClassification) &&
          (buildInNewScope          == X.buildInNewScope) &&
          (sourceCode               == X.sourceCode) )
        {
          returnValue = true;
        }

     return returnValue;
   }

template <class T>
bool
TransformationStringTemplatedType<T>::getBuildInNewScope() const
   {
  // Access function for internal variable
     return buildInNewScope;
   }

template <class T>
typename T::ScopeIdentifierEnum 
TransformationStringTemplatedType<T>::getRelativeScope() const
   {
  // Access function for internal variable
     return relativeScope;
   }

template <class T>
typename T::PlacementPositionEnum 
TransformationStringTemplatedType<T>::getRelativePosition() const
   {
  // Access function for internal variable
     return relativeLocation;
   }

template <class T>
std::string 
TransformationStringTemplatedType<T>::getSourceCodeString() const
   {
  // Access function for internal variable
     return sourceCode;
   }

template <class T>
std::string
TransformationStringTemplatedType<T>::getRelativeScopeString() const
   {
  // This function convert enum values into strings to simplify debugging
  // since the enums are defined in the AST_Rewrite class the supporting 
  // conversion functions are there as well.
     return T::getRelativeScopeString(relativeScope);
   }

template <class T>
std::string
TransformationStringTemplatedType<T>::getRelativeLocationString() const
   {
  // This function convert enum values into strings to simplify debugging
  // since the enums are defined in the AST_Rewrite class the supporting 
  // conversion functions are there as well.
     return T::getRelativeLocationString(relativeLocation);
   }

template <class T>
void
TransformationStringTemplatedType<T>::display ( std::string label ) const
   {
  // DQ (12/31/2005): This is OK since it is in a function (not in global scope).
     using namespace std;

  // Calling conversion functions to convert enum values into strings for debugging
     string relativeScopeString            = getRelativeScopeString();
     string relativeLocationString         = getRelativeLocationString();
  // string sourceCodeClassificationString = getSourceCodeClassificationString();
     string sourceCodeString               = sourceCode;

     string nodeTypeString = "NULL POINTER";
     if (associatedASTNode != NULL)
          nodeTypeString = getSgVariant(associatedASTNode->variant());

     printf ("TransformationStringTemplatedType<T>::display(%s) \n",label.c_str());
     printf ("     associatedASTNode        = %p (is a %s node) \n",associatedASTNode,nodeTypeString.c_str());
     printf ("     relativeScope            = %s \n",relativeScopeString.c_str());
     printf ("     relativeLocation         = %s \n",relativeLocationString.c_str());
  // printf ("     sourceCodeClassification = %s \n",sourceCodeClassificationString.c_str());
  // printf ("     sourceCode =============== \n%s\n",sourceCodeString.c_str());
     printf ("     sourceCode =============== \n");
     printf ("     ******************************* \n");
     printf ("     %s\n",sourceCodeString.c_str());
     printf ("     ******************************* \n");
     printf ("\n");
   }

template <class T>
TransformationStringTemplatedType<T>
TransformationStringTemplatedType<T>::operator+ ( const TransformationStringTemplatedType & X )
   {
     TransformationStringTemplatedType<T> returnValue (*this);
     returnValue.sourceCode += X.sourceCode;

     return returnValue;
   }

template <class T>
TransformationStringTemplatedType<T>
TransformationStringTemplatedType<T>::operator+= ( const TransformationStringTemplatedType & X )
   {
  // Don't modify the enum values just concatinate the string
  // relativeScope            = X.relativeScope;
  // relativeLocation         = X.relativeLocation;
  // sourceCodeClassification = X.sourceCodeClassification;
     sourceCode              += X.sourceCode;

     return *this;
   }

template <class T>
bool
TransformationStringTemplatedType<T>::matchingAttributes ( const TransformationStringTemplatedType & X ) const
   {
     bool returnValue = false;

  // Must match scope location and string classification
     if ( (relativeScope            == X.relativeScope) &&
          (relativeLocation         == X.relativeLocation) ) 
    // && (sourceCodeClassification == X.sourceCodeClassification) )
          returnValue = true;

     return returnValue;
   }

template <class T>
bool
TransformationStringTemplatedType<T>::isSameScope( SgNode* inputAstNode ) const
   {
  // This function checks to see if we are in the correct scope. If a match 
  // occures then it means that the inputAstNode is the specified scope for 
  // the string to be placed in the AST.  Then we have to consult the position 
  // in scope information to identify where in the current scope we have to 
  // place the string.  This operations is separated into it's own function 
  // because the matching process can be expected to become more complex 
  // later (or so I expect, we will see).

     bool returnValue = false;

  // printf ("AST_FragmentString::isSameScope not implemented (debugging only for now) \n");

  // simple test for now
     if (associatedASTNode == inputAstNode)
          returnValue = true;

     return returnValue;
   }

template <class T>
bool
TransformationStringTemplatedType<T>::isTargetedForHigherLevelScope( SgNode* inputAstNode ) const
   {
  // This function searches the parent list of AST nodes to check if a parent of inputAstNode
  // is associated with the current AST_FragmentString object.  The function is strict in the since that
  // if the inputAstNode != associatedASTNode we return false, only the parents of the input node are 
  // checked.

     bool returnValue = false;

     ROSE_ASSERT (inputAstNode != NULL);
     SgStatement* inputStatement     = isSgStatement(inputAstNode);
     SgStatement* inputAstNodeParent = (isSgStatement(inputStatement) == NULL) ? NULL : 
                                        isSgStatement(inputStatement->get_parent());

     while (inputAstNodeParent != NULL && inputAstNodeParent != associatedASTNode)
        {
       // Loop through the parents until we get to the root (parent of root node is NULL)
          inputAstNodeParent = isSgStatement(inputAstNodeParent->get_parent());
        }

  // make sure that we have a match
     if (inputAstNodeParent == associatedASTNode)
        {
          returnValue = true;
#if 0
          printf ("Found a matching parent node ... (exiting) \n");
          ROSE_ABORT();
#endif
        }

     return returnValue;
   }

// endif for AST_REWRITE_AST_FRAGMENT_STRING_TEMPLATES_C
#endif













