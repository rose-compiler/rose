#ifndef __GRAMMARSTRING_H__
#define __GRAMMARSTRING_H__

#include <string>
#include "ROSETTA_macros.h"
#include <assert.h>
#include <stdio.h>
#include <string>
#include "string_functions.h"

// BP : 11/30/01
// #ifndef STL_LIST_IS_BROKEN
// #include STL_LIST_HEADER_FILE
// #endif

// BP : 11/30/01
// #ifndef NAMESPACE_IS_BROKEN
// using namespace std;
// #endif

// BP : 11/30/01
// #ifndef IOSTREAM_IS_BROKEN
// #include IOSTREAM_HEADER_FILE
// #endif

#include <iostream>

// using namespace std;

// class GrammarTreeNode;   // forward declaration
class Terminal;

class GrammarString
{
 public:
  // Virtual Functions have special editied properties
  bool pureVirtualFunction;

  // char string associated with grammatical element
  std::string functionNameString;

  // part of support for data variables  (this provides GrammarString 
  // with optional capabilities specific to the requirements of 
  // (the typename and variable name can be accessed directly without parsing)
  std::string typeNameString;
  std::string variableNameString;
  std::string defaultInitializerString;
  ConstructParamEnum isInConstructorParameterList;

  CopyConfigEnum toBeCopied; // used to guide cloning of AST nodes

  // We introduce a new data member which determines if a data member to
  // be defined is to be traversed in the course of a tree traversal
  TraversalFlag toBeTraversed;

  // The sum of the ascii characters in functionNameString
  // (provides fast string comparision features)
  int key;

  BuildAccessEnum automaticGenerationOfDataAccessFunctions;

// DQ & AJ (12/3/2004): Added support for deleation of data members
  DeleteFlag toBeDeleted;

  // functions
  virtual ~GrammarString();
  GrammarString();
  GrammarString( const std::string& inputFunctionNameString );
  // GrammarString( char* inputTypeNameString, char* inputVariableNameString );
  GrammarString( const std::string& inputTypeNameString,
                 const std::string& inputVariableNameString, 
		 const std::string& defaultInitializer, 
		 const ConstructParamEnum& isConstructorParameter,
		 const BuildAccessEnum& buildAccessFunctions,
		 const TraversalFlag& toBeTraversedDuringTreeTraversal,
                 const DeleteFlag& delete_flag,
                 const CopyConfigEnum& toBeCopied);
  GrammarString( const GrammarString & X );
  GrammarString & operator= ( const GrammarString & X );

  void setVirtual ( const bool & X );
  virtual std::string getFunctionNameString ( Terminal & node );

  std::string getConstructorPrototypeParameterString();
  std::string getConstructorSourceParameterString();
  std::string getBaseClassConstructorSourceParameterString();

  // virtual char* getFunctionNameString ();
  std::string getRawString () const;
  std::string getFunctionPrototypeString () const;
  std::string getDataPrototypeString () const;
  std::string getDataAccessFunctionPrototypeString () const;

  int computeKey();
  int getLength() const;
  int getKey() const;

// DQ & AJ (12/3/2004): Added support for deleation of data members
  DeleteFlag getToBeDeleted() const;

  friend bool operator!= ( const GrammarString & X, const GrammarString & Y );
  friend bool operator== ( const GrammarString & X, const GrammarString & Y );
  // char* getFunctionNameStringTestAgainstExclusions ( GrammarTreeNode & node );
  std::string getFunctionNameStringTestAgainstExclusions 
    ( Terminal & node,
      std::vector<GrammarString *> &,
      std::vector<GrammarString *> & excludeList );

  // Access functions
  const std::string& getTypeNameString() const;
  const std::string& getVariableNameString() const;
  const std::string& getDefaultInitializerString() const;

  void setIsInConstructorParameterList(ConstructParamEnum X);
  ConstructParamEnum getIsInConstructorParameterList() const;

  void setToBeTraversed(const TraversalFlag& X);
  TraversalFlag getToBeTraversed() const;

  void setToBeCopied(const CopyConfigEnum& X) { toBeCopied = X; }
  CopyConfigEnum getToBeCopied() const { return toBeCopied; }

  void setAutomaticGenerationOfDataAccessFunctions ( const BuildAccessEnum& X );
  BuildAccessEnum generateDataAccessFunctions() const;

  // DQ (9/24/2005): Build the copy mechanism source code
  // string buildCopyMemberFunctionSource ( string copyString);
  std::string buildCopyMemberFunctionSource ( bool buildConstructorArgument );

  // DQ (9/24/2005): Build the copy mechanism's source code to set the parents of child nodes
  std::string buildCopyMemberFunctionSetParentSource ( std::string copyString );

  // DQ (5/22/2006): Build the source code for each variable in the destructor.
  std::string buildDestructorSource();

  void consistencyCheck() const;
  void display ( const std::string& label = "" ) const;

  // BP : 10/10/2001, five functions moved from Grammar class
  static std::string copyEdit ( const std::string& inputString, const std::string& oldToken, const std::string& newToken );
  static inline StringUtility::FileWithLineNumbers copyEdit ( const StringUtility::FileWithLineNumbers& inputString, const std::string& oldToken, const std::string& newToken ) {return StringUtility::copyEdit(inputString, oldToken, newToken);}
  static inline StringUtility::FileWithLineNumbers copyEdit ( const StringUtility::FileWithLineNumbers& inputString, const std::string& oldToken, const StringUtility::FileWithLineNumbers& newToken ) {return StringUtility::copyEdit(inputString, oldToken, newToken);}
  static bool isContainedIn ( const std::string& longString, const std::string& shortString );
  // A smarter string concatination function (corrects for space in the target automatically)
  static std::string stringConcatenate ( const std::string& target, const std::string& endingString );

};

#endif
