// ################################################################
// #                           Header Files                       #
// ################################################################

#include "grammarString.h"
#include "terminal.h"
#include "string_functions.h"
#include <string>

using namespace std;

// ################################################################
// #            GrammarString Member Functions                    #
// ################################################################

const string&
GrammarString::getTypeNameString() const
   {
     return typeNameString;
   }

const string&
GrammarString::getVariableNameString() const
   {
     return variableNameString;
   }

const string&
GrammarString::getDefaultInitializerString() const
   {
     return defaultInitializerString;
   }

ConstructParamEnum
GrammarString::getIsInConstructorParameterList() const
   {
     return isInConstructorParameterList;
   }

TraversalFlag
GrammarString::getToBeTraversed() const
   {
     return toBeTraversed;
   }

string
GrammarString::getFunctionPrototypeString () const
   {
  // return the prebuild string (from which the keys are computed!)
  // This function returns the "functionNameString" which is used to
  // hold source code and header file prototypes.  Other functions return
  // more specialized strings for constructor parameter lists etc.
     return functionNameString;
   }

string
GrammarString::getRawString () const
   {
  // return the prebuild string (from which the keys are computed!)
  // This function returns the "functionNameString" which is used to
  // hold source code and header file prototypes.  Other functions return
  // more specialized strings for constructor parameter lists etc.
     return functionNameString;
   }

string
listIteratorInitialization ( string typeName, string iteratorName, string listName, string accessOperator )
   {
     string returnString  = "     " + typeName + "::const_iterator " + iteratorName + " = " + listName + accessOperator + "begin(); \n";
     return returnString;
   }


string
forLoopOpening ( string iteratorName, string listName, string accessOperator )
   {
     string returnString = "     for ( /* empty by design */; " + iteratorName
                         + " != " + listName + accessOperator + "end(); ++" 
                         + iteratorName + ") \n        { \n";
     return returnString;
   }

string
forLoopBody ( string typeName, string variableName, string iteratorName )
   {
     string returnString = "          " + typeName + " " + variableName + " = *" + iteratorName + "; \n";
     return returnString;
   }

#if 0
string
forLoopBodyForCopy ( string typeName, string variableName )
   {
     string returnString = "          " + typeName + " " + variableName + " = NULL; \n";
     return returnString;
   }
#endif

string
conditionalToSetParent ( string variableName)
   {
#if 0
     string returnString = "          if (" + variableName + " != NULL && " + variableName + "->get_parent() == NULL ) \n" 
                         + "             { \n"
                         + "               " + variableName + "->set_parent(result); \n"
                         + "             } \n";
#else
  // DQ (8/29/2006): Skip setting the parents of types since they are shared and it is enforced that they have NULL valued parent pointers.
     string returnString = "          if ( (" + variableName + " != NULL) && (" + variableName + "->get_parent() == NULL) && (isSgType(" + variableName + ") == NULL) ) \n" 
                         + "             { \n"
                         + "               " + variableName + "->set_parent(result); \n"
                         + "             } \n";
#endif

     return returnString;
   }

string
conditionalToCopyVariable ( string typeName, string variableNameSource, string variableNameCopy, string iteratorName )
   {
  // string returnString = "          " + typeName + " " + variableNameCopy + " = NULL; \n"
  // PC (8/3/2006): Flexibility improvement to copy mechanism
     string returnString = "          if (" + variableNameSource + " != NULL) \n" 
                         + "             { \n"
                         + "               " + variableNameCopy + " = static_cast<" + typeName + ">(help.copyAst(" + iteratorName + ")); \n"
                         + "             } \n"
                         + "            else \n"
                         + "             { \n"
                         + "               " + variableNameCopy + " = NULL; \n"
                         + "             } \n";
     return returnString;
   }


string
forLoopClosing()
   {
     string returnString = "        } \n";
     return returnString;
   }

string
variableInitialization ( string copyOfVariableName, string sourceVariableName )
   {
     string returnString = "     " + copyOfVariableName + " = " + sourceVariableName + "; \n";
     return returnString;
   }

string
variableDeclaration ( string typeName, string variableName )
   {
     string returnString = "     " + typeName + " " + variableName + " = NULL; \n";
     return returnString;
   }

string
stringCopyConditional ( string typeName, string variableName, string copyVariableName )
   {
  // string returnString = "     " + typeName + " " + copyVariableName + " = NULL; \n"
     string returnString = "     if (" + variableName + " != NULL) \n"
                         + "          " + copyVariableName + " = strdup(" + variableName + "); \n";
                      // + "     result->" + variableName + " = " + copyVariableName + "; \n";
     return returnString;
   }

string
conditionalToBuildNewVariable ( string typeName, string variableNameSource, string newVariableName )
   {
     string rhs;
  // Handle special case of Sg_File_Info, where we want to build the source file position 
  // information to be marked as a transformation (using the static member fuction:
  // "Sg_File_Info::generateDefaultFileInfoForTransformationNode()").
     if (typeName == "$GRAMMAR_PREFIX__File_Info")
        {
       // DQ (10/21/2005): The copy should be a semantic preserving as possible 
       // (so don't make copies as transformations and call the copy constructor).
       // rhs = "          " + newVariableName + " = Sg_File_Info::generateDefaultFileInfoForTransformationNode(); \n";
          rhs = "          " + newVariableName + " = new Sg_File_Info(*" + variableNameSource + "); \n";
        }
       else
        {
          rhs = "          " + newVariableName + " = new " + typeName + "( *" + variableNameSource + "); \n";
        }
     string returnString = "     if ( " + variableNameSource + " != NULL ) \n"
                         + "        { \n"
                         + rhs
                         + "        } \n" 
                         + "       else \n"
                         + "        { \n"
                         + "          " + newVariableName + " = NULL; \n"
                         + "        } \n";

  // printf ("returnString = %s \n",returnString.c_str());
  // ROSE_ASSERT(typeName != "Sg_File_Info");

     return returnString;
   }

string
GrammarString::buildCopyMemberFunctionSetParentSource ( string copyString )
   {
  // DQ (9/25/2005): This function builds code to reset parent pointers in the copy function

     string returnString;

     string variableName = getVariableNameString();
     string typeName     = getTypeNameString();

     ROSE_ASSERT (typeName.empty() == false);
     ROSE_ASSERT (variableName.empty() == false);

  // printf ("In GrammarString::buildCopyMemberFunctionSetParentSource(): type = %s variable = %s \n",typeName.c_str(),variableName.c_str());

  // Check if the type name is "char*"
     bool typeIsCharString = typeName.find("char*") != string::npos && typeName.find("char**") == string::npos;
     
  // if ( strstr(typeName.c_str(),"char*") != NULL && strstr(typeName.c_str(),"char**") == NULL)
     if ( typeIsCharString )
        {
       // Nothing to do since strings don't have parents 
          returnString = "  // case: typeName == char* or char** for " + variableName + "\n";
          returnString += "";
          return returnString;
        }

  // check if the member is accessed in tree traversal
     if ( toBeTraversed == DEF_TRAVERSAL || toBeCopied == CLONE_TREE)
        {
       // Control variables for code generation
          bool typeIsPointerToListOfPointers    = typeName.find("PtrListPtr") != string::npos;
          bool typeIsPointerToListOfNonpointers = (typeIsPointerToListOfPointers == false) && typeName.find("ListPtr") != string::npos;
          bool typeIsPointerToList              = typeIsPointerToListOfPointers || typeIsPointerToListOfNonpointers;

       // By "simple list" we mean NOT a pointer to a list (just a list, e.g. STL list)
          bool typeIsSimpleListOfPointers       = (typeIsPointerToListOfPointers == false) && typeName.find("PtrList") != string::npos;
          bool typeIsList                       = typeIsPointerToList || typeIsSimpleListOfPointers; 
          bool typeIsSgNode                     = typeName.find('*') != string::npos;

#if 0
          printf ("typeIsPointerToListOfPointers    = %s \n",typeIsPointerToListOfPointers ? "true" : "false");
          printf ("typeIsPointerToListOfNonpointers = %s \n",typeIsPointerToListOfNonpointers ? "true" : "false");
          printf ("typeIsPointerToList              = %s \n",typeIsPointerToList ? "true" : "false");
          printf ("typeIsSimpleListOfPointers       = %s \n",typeIsSimpleListOfPointers ? "true" : "false");
          printf ("typeIsList                       = %s \n",typeIsList ? "true" : "false");
          printf ("typeIsSgNode                     = %s \n",typeIsSgNode ? "true" : "false");
#endif

       // One of these should be true!
          ROSE_ASSERT(typeIsList == true  || typeIsSgNode == true);
          ROSE_ASSERT(typeIsList == false || typeIsSgNode == false);

       // Support for adding commented to generated source code
          string commentString;

          if (typeIsList == true)
             {
            // Comment to add to generated source code
               commentString += "  // case: listType for " + variableName + "\n";

            // name constant for all cases below (in this scope)
               string listElementName = "list_element";

            // names that are set differently for different cases 
               string iteratorBaseType;
               string needPointer;
               string listElementType;
               string copyOfList;
               string iteratorName;

            // Access member functions using "->" or "." (set to some string 
            // that will cause an error if used, instead of empty string).
               string accessOperator = "error string for access operator";

               if (typeIsPointerToList == true)
                  {
                    commentString += "  // case: listType (typeIsPointerToList == true) for " + variableName + "\n";
                    if (typeIsPointerToListOfPointers == true)
                       {
                         commentString += "  // case: listType (typeIsPointerToList == true && typeIsPointerToListOfPointers == true) for " + variableName + "\n";
                         needPointer = "*";
                         accessOperator = "->";
                       }
                      else
                       {
                         commentString += "  // case: listType (typeIsPointerToList == true && typeIsPointerToListOfPointers == false) for " + variableName + "\n";
                         ROSE_ASSERT(typeIsPointerToListOfNonpointers == true);
                         accessOperator = ".";
                       }

                 // iteratorBaseType = string("NeedBaseType_of_") + typeName;
                    int positionOfListPtrSubstring = typeName.find("ListPtr");
                    int positionOfPtrSubstring     = typeName.find("Ptr",positionOfListPtrSubstring);
                    iteratorBaseType = typeName.substr(0,positionOfPtrSubstring);

                    copyOfList = variableName + "_copy";
                    iteratorName = copyOfList + "_iterator";
                  }
                 else
                  {
                    commentString += "  // case: listType (typeIsPointerToList == false) for " + variableName + "\n";

                    ROSE_ASSERT(typeIsSimpleListOfPointers == true);
                    iteratorBaseType = typeName;
                    needPointer = "*";
                    accessOperator = ".";

                 // Need to generate different code, for example:
                 //      SgStatementPtrList::const_iterator cpinit_stmt = get_init_stmt().begin(); 
                 // instead of:
                 //      SgStatementPtrList::const_iterator init_stmt_copy_iterator = init_stmt_copy.begin();

                    copyOfList = string("result->get_") + variableName + "()";
                    iteratorName = variableName + "_iterator";
                  }

            // Need to get the prefix substring to strings like "SgFilePtrList" (i.e. "SgFile")
               int positionOfPtrListSubstring = iteratorBaseType.find("PtrList");
               int positionOfListSubstring    = iteratorBaseType.find("Ptr",positionOfPtrListSubstring);
               listElementType = typeName.substr(0,positionOfListSubstring) + needPointer;

            // Declare the loop index iterator
               returnString  += commentString + listIteratorInitialization(iteratorBaseType,iteratorName,copyOfList,accessOperator);

            // Open up the loop over the list elements
               returnString  += forLoopOpening(iteratorName,copyOfList,accessOperator);

            // Declare the a loop variable (reference to current element of list)
               returnString  += forLoopBody(listElementType,listElementName,iteratorName);

            // insert the conditional test (also used below)
               returnString  += conditionalToSetParent(listElementName);

            // close off the loop
               returnString  += forLoopClosing();
             }
            else
             {
               ROSE_ASSERT(typeIsSgNode == true);

               commentString += "  // case: not a listType for (using conditionalToSetParent)" + variableName + "\n";

               string copyOfVariable = variableName + "_copy";
            // insert the conditional test (also used above)
               returnString  += commentString + conditionalToSetParent(copyOfVariable);
             }
        }

     return returnString;
   }

// DQ (9/26/2005): This is the new source code generator for the copy mechanism.  
// the previous version was coplex and didn't generate the correct code to support
// the copy of a SgFile within the pointer to the list of SgFile in SgProject.
// I will see if I can fix this :-).

// Note that the input parameter is never used!
string
GrammarString::buildCopyMemberFunctionSource ( bool buildConstructorArgument )
   {
  // DQ (9/25/2005): This function builds code to copy the data members (within the copy function)

  // Return value for this function
     string returnString;

  // Support for adding commented to generated source code
     string commentString;

     string variableName = getVariableNameString();
     string typeName     = getTypeNameString();

     ROSE_ASSERT (typeName.empty() == false);
     ROSE_ASSERT (variableName.empty() == false);

  // printf ("In GrammarString::buildCopyMemberFunctionSetParentSource(): type = %s variable = %s \n",typeName.c_str(),variableName.c_str());

  // Check if the type name is "char*"
     bool typeIsCharString = typeName.find("char*") != string::npos && typeName.find("char**") == string::npos;
     
  // if ( strstr(typeName.c_str(),"char*") != NULL && strstr(typeName.c_str(),"char**") == NULL)
     if ( typeIsCharString )
        {
       // Always copy C style strings
          string copyOfVariableName = variableName + "_copy";
          string sourceVariableName = "p_" + variableName;
          commentString = "  // case: typeName == char* or char** for " + variableName + "\n";
       // Declare the copy of the variable
       // returnString += "     " + typeName + " " + variableName + "_copy; \n";
          returnString += "     " + typeName + " " + copyOfVariableName + "; \n";
          returnString += commentString + stringCopyConditional(typeName,sourceVariableName,copyOfVariableName);

       // string copyOfVariableName = "result->p_" + variableName;
       // printf ("\n\n*****************************************************\n");
       // printf ("Case of typeIsCharString: buildConstructorArgument = %s \n",buildConstructorArgument ? "true" : "false");
       // printf ("Case of typeIsCharString (before variableInitialization): returnString = %s \n",returnString.c_str());

          if (buildConstructorArgument == false)
             {
            // For constructor arguments we can't reference the "result" pointer in the generated 
            // code because it will be set with the call to the constructor.  So don't output this 
            // generated code when generating code to handle constructor arguments.
               returnString += variableInitialization(copyOfVariableName,sourceVariableName);

            // DQ (3/23/2006): Set the internal value by calling the access function to set it.
            // Need to add (for example): "result->set_value(value_copy);"
               returnString += "     result->" + sourceVariableName + " = " + copyOfVariableName + ";\n";
             }

       // printf ("Case of typeIsCharString: returnString = %s \n",returnString.c_str());

          return returnString;
        }

   // The rule is that if it is not a char* or char** then if it ia a pointer type it is a pointer to a Sage IR node
      bool typeIsSgNode = typeName.find('*') != string::npos;

  // check if the member is accessed in tree traversal
     if ( toBeTraversed == DEF_TRAVERSAL || toBeCopied == CLONE_TREE)
        {
       // Control variables for code generation
          bool typeIsPointerToListOfPointers    = typeName.find("PtrListPtr") != string::npos;
          bool typeIsPointerToListOfNonpointers = (typeIsPointerToListOfPointers == false) && typeName.find("ListPtr") != string::npos;
          bool typeIsPointerToList              = typeIsPointerToListOfPointers || typeIsPointerToListOfNonpointers;

       // By "simple list" we mean NOT a pointer to a list (just a list, e.g. STL list)
          bool typeIsSimpleListOfPointers       = (typeIsPointerToListOfPointers == false) && typeName.find("PtrList") != string::npos;
          bool typeIsList                       = typeIsPointerToList || typeIsSimpleListOfPointers;


#if 0
          printf ("typeIsPointerToListOfPointers    = %s \n",typeIsPointerToListOfPointers ? "true" : "false");
          printf ("typeIsPointerToListOfNonpointers = %s \n",typeIsPointerToListOfNonpointers ? "true" : "false");
          printf ("typeIsPointerToList              = %s \n",typeIsPointerToList ? "true" : "false");
          printf ("typeIsSimpleListOfPointers       = %s \n",typeIsSimpleListOfPointers ? "true" : "false");
          printf ("typeIsList                       = %s \n",typeIsList ? "true" : "false");
          printf ("typeIsSgNode                     = %s \n",typeIsSgNode ? "true" : "false");
#endif

       // One of these should be true!
          ROSE_ASSERT(typeIsList == true  || typeIsSgNode == true);
          ROSE_ASSERT(typeIsList == false || typeIsSgNode == false);

          string listElementType = "default-error-type";

       // Declare the copy of the variable
          returnString += "     " + typeName + " " + variableName + "_copy; \n";

          if (typeIsList == true)
             {
            // Comment to add to generated source code
               commentString += "  // case: listType for " + variableName + "\n";

            // name constant for all cases below (in this scope)
               string listElementName       = "source_list_element";
               string copyOfListElementName = "copy_list_element";

            // names that are set differently for different cases 
               string iteratorBaseType;
               string needPointer;
               string originalList;
               string iteratorName;

            // Access member functions using "->" or "." (set to some string 
            // that will cause an error if used, instead of empty string).
               string accessOperator = "error string for access operator";

               if (typeIsPointerToList == true)
                  {
                    commentString += "  // case: listType (typeIsPointerToList == true) for " + variableName + "\n";
                    if (typeIsPointerToListOfPointers == true)
                       {
                         commentString += "  // case: listType (typeIsPointerToList == true && typeIsPointerToListOfPointers == true) for " + variableName + "\n";
                         needPointer = "*";
                         accessOperator = "->";
                       }
                      else
                       {
                         commentString += "  // case: listType (typeIsPointerToList == true && typeIsPointerToListOfPointers == false) for " + variableName + "\n";
                         ROSE_ASSERT(typeIsPointerToListOfNonpointers == true);
                         accessOperator = ".";
                       }

                 // iteratorBaseType = string("NeedBaseType_of_") + typeName;
                    int positionOfListPtrSubstring = typeName.find("ListPtr");
                    int positionOfPtrSubstring     = typeName.find("Ptr",positionOfListPtrSubstring);
                    iteratorBaseType = typeName.substr(0,positionOfPtrSubstring);

                 // copyOfList = variableName + "_source";
                    originalList = string("get_") + variableName + "()";
                    iteratorName = variableName + "_iterator";

                 // Initialize the pointer to the list (of pointers)
                    returnString += "     " + variableName + "_copy" + " = new " + iteratorBaseType + "; // initialize the pointer to the list \n";
                  }
                 else
                  {
                    commentString += "  // case: listType (typeIsPointerToList == false) for " + variableName + "\n";

                    ROSE_ASSERT(typeIsSimpleListOfPointers == true);
                    iteratorBaseType = typeName;
                    needPointer = "*";
                    accessOperator = ".";

                 // Need to generate different code, for example:
                 //      SgStatementPtrList::const_iterator cpinit_stmt = get_init_stmt().begin(); 
                 // instead of:
                 //      SgStatementPtrList::const_iterator init_stmt_copy_iterator = init_stmt_copy.begin();

                    originalList = string("get_") + variableName + "()";
                    iteratorName = string("source_") + variableName + "_iterator";
                  }

            // Need to get the prefix substring to strings like "SgFilePtrList" (i.e. "SgFile")
               int positionOfPtrListSubstring = iteratorBaseType.find("PtrList");
               int positionOfListSubstring    = iteratorBaseType.find("Ptr",positionOfPtrListSubstring);
               listElementType = typeName.substr(0,positionOfListSubstring) + needPointer;

            // Declare the loop index iterator
               returnString += commentString + listIteratorInitialization(iteratorBaseType,iteratorName,originalList,accessOperator);

            // Open up the loop over the list elements
               returnString += forLoopOpening(iteratorName,originalList,accessOperator);

            // Declare the a loop variable (reference to current element of list)
               returnString += forLoopBody(listElementType,listElementName,iteratorName);

               returnString += "     " + variableDeclaration(listElementType,copyOfListElementName);

            // insert the conditional test (also used below)
               string dereferencedIteratorName = string("*") + iteratorName;
               returnString += conditionalToCopyVariable(listElementType,listElementName,copyOfListElementName,dereferencedIteratorName);

               returnString += "          " + variableName + "_copy" + accessOperator + "push_back(" + copyOfListElementName + "); \n";

            // close off the loop
               returnString += forLoopClosing();
#if 0
            // DQ (10/22/2005): Copy the "variableName + _copy" back to the "result->p_ + variableName + _copy"
            // returnString += "  /* copy " + variableName + "_copy" + " to the result */ \n";
               returnString += "     result->p_" + variableName + " = " + variableName + "_copy;" + " // list case \n";
#endif
             }
            else
             {
               ROSE_ASSERT(typeIsSgNode == true);

               commentString += "  // case: not a listType for (using conditionalToCopyVariable)" + variableName + "\n";

               string variableType = typeName;
               string copyOfVariableName = variableName + "_copy";
               string sourceVariableName = string("get_") + variableName + "()";
            // insert the conditional test (also used above)
               returnString += commentString + conditionalToCopyVariable(variableType,sourceVariableName,copyOfVariableName,sourceVariableName);
#if 0
            // DQ (10/22/2005): Copy the "variableName + _copy" back to the "result->p_ + variableName + _copy"
            // returnString += "  /* copy " + variableName + "_copy" + " to the result */ \n";
               returnString += "     result->p_" + variableName + " = " + variableName + "_copy;" + " // non list case \n";
#endif

               if (buildConstructorArgument == false)
                  {
                 // DQ (3/10/2007): SgFunctionDeclaration has a parameter list that is maintained internally so we want to avoid overwitting it.
                    returnString += "  /* check for a valid pointer and delete if present */ \n";
                    returnString += "     if (result->p_" + variableName + " != NULL) delete result->p_" + variableName + "; \n";
                  }
             }
#if 1
          if (buildConstructorArgument == false)
             {
            // DQ (10/22/2005): Copy the "variableName + _copy" back to the "result->p_ + variableName + _copy"
            // returnString += "  /* copy " + variableName + "_copy" + " to the result */ \n";
               returnString += "     result->p_" + variableName + " = " + variableName + "_copy;" + " \n";
             }
#endif
        }
       else
        {
          commentString += "  // case: (toBeTraversed == false) && (toBeCopied != CLONE_TREE) for " + variableName + "\n";
          returnString += commentString;

       // Declare the copy of the variable
       // returnString += "     " + typeName + " " + variableName + "_copy; \n";

#if 0
          string copyOfVariableName;
          string sourceVariableName;

       // Not all data members have access functions (though this might be fixed at some point)
          if ( automaticGenerationOfDataAccessFunctions == BUILD_ACCESS_FUNCTIONS ||
               automaticGenerationOfDataAccessFunctions == BUILD_FLAG_ACCESS_FUNCTIONS ||
               automaticGenerationOfDataAccessFunctions == BUILD_LIST_ACCESS_FUNCTIONS )
             {
               copyOfVariableName = variableName + "_copy";
               sourceVariableName = string("get_") + variableName + "()";

            // Declare the copy of the variable
               returnString += "     " + typeName + " " + copyOfVariableName + "; \n";
             }
            else
             {
               copyOfVariableName = "result->p_" + variableName;
               sourceVariableName = "p_" + variableName;

            // Declare the copy of the variable
            // returnString += "     " + typeName + " " + copyOfVariableName + "; \n";
             }
#else
       // Declare the copy of the variable
       // returnString       += "     " + typeName + " " + variableName + "_copy = p_" + variableName + "; // needs initialization? \n";
          string variableType = typeName;
          string sourceVariableName = variableName + "_copy";
#endif
       // returnString += variableInitialization(copyOfVariableName,sourceVariableName);

          if (toBeCopied == COPY_DATA)
             {
            // Amongst all other data members, this case also handles all SgSymbol* objects.

               commentString = "  // case: toBeCopied == COPY_DATA for " + variableName + "\n";
               returnString += commentString;
            // Declare the copy of the variable (requires initialization)
               returnString       += "     " + typeName + " " + variableName + "_copy = p_" + variableName + "; \n";
               string copyOfVariableName = "result->p_" + variableName;
               if (buildConstructorArgument == false)
                  {
                 // For constructor arguments we can't reference the "result" pointer in the generated 
                 // code because it will be set with the call to the constructor.  So don't output this 
                 // generated code when generating code to handle constructor arguments.
                    returnString += variableInitialization(copyOfVariableName,sourceVariableName);
                  }
             }
            else
             {
               if (toBeCopied == CLONE_PTR)
                  {
                    commentString = "  // case: toBeCopied == CLONE_PTR for " + variableName + "\n";
                    returnString += commentString;

                 // Declare the copy of the variable (does not require initialization)
                    returnString       += "     " + typeName + " " + variableName + "_copy = NULL; \n";
                    string copyOfVariableName = "p_" + variableName;
                    ROSE_ASSERT(typeIsSgNode == true);
                    unsigned long int positionOfStarSubstring = typeName.find("*");
                    ROSE_ASSERT(positionOfStarSubstring != string::npos);
                    string variableBaseType = variableType.substr(0,positionOfStarSubstring);
                    returnString += conditionalToBuildNewVariable(variableBaseType,copyOfVariableName,sourceVariableName);
                    if (buildConstructorArgument == false)
                       {
                      // DQ (3/10/2007): SgFunctionDeclaration has a parameter list that is maintings internally so we want to avoid overwitting it.
                         returnString += "  /* check for a valid pointer and delete if present */ \n";
                         returnString += "     if (result->p_" + variableName + " != NULL) delete result->p_" + variableName + "; \n";

                         returnString += "  /* add assignment to result here */ \n";
                      // DQ (10/22/2005): Copy the "variableName + _copy" back to the "result->p_ + variableName + _copy"
                         returnString += "     result->p_" + variableName + " = " + variableName + "_copy;" + " \n";
                       }
                  }
             }
        }

  // ROSE_ASSERT(false);

     return returnString;
   }


string
GrammarString::getDataPrototypeString () const
   {
  // This function returns the data prototype (without the initializer, e.g. the " = 0" part)
  // The string returned by this functions includes the ";" and the newline
     string returnString = typeNameString + " p_" + variableNameString + ";\n          ";
     return returnString;
   }


string
GrammarString::getDataAccessFunctionPrototypeString () const
   {
     string typeNameStringTmp = typeNameString;

  // DQ (12/20/2005): strip the "static " substring from the typeName
  // so that we generate non-static member access functions and non-static 
  // parameter variable types (which are not legal C++).
     string::size_type positionOfSubstring = typeNameStringTmp.find("static ");
     if (positionOfSubstring != string::npos)
        {
          typeNameStringTmp.erase(positionOfSubstring, 7 /* strlen("static ") */ );
        }

     string variableNameStringTmp = string(variableNameString);

     string returnString;
     switch (automaticGenerationOfDataAccessFunctions.getValue()) {
     case TAG_NO_ACCESS_FUNCTIONS:
           break;
     case TAG_BUILD_ACCESS_FUNCTIONS:
     case TAG_BUILD_FLAG_ACCESS_FUNCTIONS:
          returnString = "     public: \n         " + typeNameStringTmp + " get_" +
                         variableNameStringTmp + "() const;\n         void set_"  
                         + variableNameStringTmp + "(" + typeNameStringTmp + " " + 
                         variableNameStringTmp + ");\n";
          break;
     case TAG_BUILD_LIST_ACCESS_FUNCTIONS:
          returnString = "     public: \n         const " + typeNameStringTmp + 
                         "& " + " get_" + variableNameStringTmp + "() const;\n         " + 
                         typeNameStringTmp + "& " + "get_" + variableNameStringTmp
                         + "(); \n";
          break;
     default:
          assert(false);
     }
     return returnString;
   }

string
GrammarString::getFunctionNameString ( Terminal & node )
   {

  // printf ("Inside of GrammarString::getFunctionNameString(node) \n");

     string memberFunctionString = functionNameString;

#if 0
     printf ("pureVirtualFunction = %s \n",(pureVirtualFunction == true) ? "true" : "false");
     printf ("node.isRootNode() = %s \n",(node.isRootNode() == true) ? "true" : "false");
     printf ("node.isLeafNode() = %s \n",(node.isLeafNode() == true) ? "true" : "false");
#endif

     if (pureVirtualFunction == true)
        {
       // Now we have to edit the string
       // ROSE_ASSERT (pureVirtualFunction == false);

          string className = node.getName();

          string derivedClassString;

       // printf ("EDIT className (%s) durring copy \n",className);
          string parentClassName    = "NO PARENT FOUND";
          if (node.getBaseClass() == NULL)
             {
               parentClassName    = node.getBaseClass()->getName();

            // Later this has to be automatically derived
               derivedClassString = ": " + parentClassName + "(exp)";

            // printf ("Exiting when node.parentTreeNode != NULL (parentClassName %s) ... \n",parentClassName);
            // ROSE_ABORT();
             }

          string pureVirtualMarkerString = "";
          if (!node.subclasses.empty())
             {
               pureVirtualMarkerString = " = 0";
             }

       // printf ("Exiting when node.parentTreeNode != NULL (parentClassName %s) ... \n",parentClassName);
       // ROSE_ABORT();

       // printf ("EDIT parentClassName (%s) durring copy \n",parentClassName);

          memberFunctionString = copyEdit (memberFunctionString,"$PURE_VIRTUAL_MARKER",pureVirtualMarkerString);
          memberFunctionString = copyEdit (memberFunctionString,"$CLASSNAME",className);
       // memberFunctionString = copyEdit (memberFunctionString,"$BASECLASS_CONSTRUCTOR_CALL",derivedClassString);
        }

     return memberFunctionString;
   }

string
GrammarString::getConstructorPrototypeParameterString()
   {
  // Not clear yet if we need to know the node!
  // This function assembles the parameter in a form in which it can be used
  // within the constructor prototype code declaration.

  // Verify that this is a GrammarString object representing a 
  // data variable (with type, variable name, and an initializer)

#if 1   // BP : 10/25/2001, rewritten to make only one alloc instead of allocs and deletes
     // and also fixes a memory leak
     string startString =  getConstructorSourceParameterString();
     string endString   = getDefaultInitializerString();
     string returnString = startString + " " + endString;
#else
     char* returnString = getConstructorSourceParameterString();
     returnString = stringConcatenate (returnString," ");
     returnString = stringConcatenate (returnString,getDefaultInitializerString());
#endif

     return returnString;
   }

string
GrammarString::getConstructorSourceParameterString()
   {
  // Not clear yet if we need to know the node!
  // This function assembles the parameter in a form in which it can be used
  // within the constructor source code definition.

     string returnString = getTypeNameString() + " " + getVariableNameString();

     return returnString;
   }

string
GrammarString::getBaseClassConstructorSourceParameterString()
   {
     return getVariableNameString();
   }

GrammarString::~GrammarString()
   {
   }

GrammarString::GrammarString()
   : pureVirtualFunction(0), functionNameString(""), 
     typeNameString(""), variableNameString(""), 
     defaultInitializerString(""), isInConstructorParameterList(CONSTRUCTOR_PARAMETER), 
     toBeCopied(COPY_DATA), toBeTraversed(true), key(0),
     automaticGenerationOfDataAccessFunctions(BUILD_ACCESS_FUNCTIONS),
     toBeDeleted(false)
   {
   }

// DQ & AJ (12/3/2004): Added support for deleation of data members
GrammarString::GrammarString( 
   const string& inputTypeNameString, const string& inputVariableNameString, const string& inputDefaultInitializerString,
   const ConstructParamEnum& isConstructorParameter, 
   const BuildAccessEnum& inputAutomaticGenerationOfDataAccessFunctions,
   const TraversalFlag& toBeTraversedDuringTreeTraversal, 
   const DeleteFlag& delete_flag,
   const CopyConfigEnum& _toBeCopied)
  // DQ (12/7/2003): Reordered parameters
   : pureVirtualFunction(0), functionNameString(""), 
     typeNameString(inputTypeNameString), variableNameString(inputVariableNameString), 
     defaultInitializerString(inputDefaultInitializerString), isInConstructorParameterList(isConstructorParameter), 
     toBeCopied(_toBeCopied), toBeTraversed(true), key(0),
     automaticGenerationOfDataAccessFunctions(BUILD_ACCESS_FUNCTIONS),
     toBeDeleted(delete_flag)
{
  string tempString = defaultInitializerString;
// printf ("GrammarString constructor: tempString.length() = %d tempString = %s \n",
//      tempString.length(),tempString.c_str());

  // setup the main function string from the type and variable name (not indented properly)
  functionNameString = inputTypeNameString + " " + inputVariableNameString + " " + inputDefaultInitializerString + ";";
  
  // Compute the key once as the object is constructed (this is used to test equality between strings)
  key = computeKey();
  automaticGenerationOfDataAccessFunctions = 
    inputAutomaticGenerationOfDataAccessFunctions;
  
  // Set the flag which determines if the data member occurs in the parameter
  // lists of the constructors of the class (and all its derived classes)
  // setIsInConstructorParameterList(isConstructorParameter);

  // Set the flag which determines if the data member is to be traversed
  // in the course of a tree traversal
  setToBeTraversed(toBeTraversedDuringTreeTraversal);
}

GrammarString::GrammarString( const string& inputFunctionNameString )
  // DQ (12/7/2003): Reordered parameters
   : pureVirtualFunction(0), functionNameString(inputFunctionNameString), 
     typeNameString(""), variableNameString(""), 
     defaultInitializerString(""), isInConstructorParameterList(CONSTRUCTOR_PARAMETER), 
     toBeCopied(COPY_DATA), toBeTraversed(true), key(0),
     automaticGenerationOfDataAccessFunctions(BUILD_ACCESS_FUNCTIONS),
     toBeDeleted(false)
   {
  // Compute the key once as the object is constructed (this is used to test equality between strings)
     key = computeKey();
   }

GrammarString::GrammarString( const GrammarString & X )
  // DQ (12/7/2003): Reordered parameters
   : pureVirtualFunction(0), functionNameString(""), 
     typeNameString(""), variableNameString(""), 
     defaultInitializerString(""), isInConstructorParameterList(CONSTRUCTOR_PARAMETER), 
     toBeCopied(X.toBeCopied), toBeTraversed(true), key(0),
     automaticGenerationOfDataAccessFunctions(BUILD_ACCESS_FUNCTIONS),
     toBeDeleted(false)
   {
  // printf ("Calling the GrammarString copy CONSTRUCTOR! \n");

  // It is a common technique to implement the copy constructor using the operator=
  // so that we can consolidate detail on the implementation and provide a consistent 
  // semantics.
     *this = X;
   }

GrammarString & 
GrammarString::operator= ( const GrammarString & X )
   {
     functionNameString = X.functionNameString;

     typeNameString           = X.typeNameString;
     variableNameString       = X.variableNameString;
     defaultInitializerString = X.defaultInitializerString;

  // printf ("Exiting in GrammarString::operator= \n");
  // ROSE_ABORT();

     key                                      = X.key;
     pureVirtualFunction                      = X.pureVirtualFunction;
     automaticGenerationOfDataAccessFunctions = X.automaticGenerationOfDataAccessFunctions;
     isInConstructorParameterList             = X.isInConstructorParameterList;
     toBeTraversed                            = X.toBeTraversed;
     toBeCopied                               = X.toBeCopied;
     toBeDeleted                              = X.toBeDeleted;

     return *this;
   }

void 
GrammarString::setVirtual ( const bool & X )
   {
     pureVirtualFunction = X;
   }

bool 
operator!= ( const GrammarString & X, const GrammarString & Y )
   {
  // The not equals logical operator is implemented using the equals logical operator
     return !(X == Y);
   }

bool 
operator== ( const GrammarString & X, const GrammarString & Y )
   {
  // Implementation of operator== (checks only if the strings in X and Y are identical)
  // It first tests to see if they are the same length
  // This function does not test based upon the "automaticGenerationOfDataAccessFunctions" variable

     bool returnValue = false;
     int lengthX = X.getLength();
     int lengthY = Y.getLength();
     if (lengthX == lengthY)
        {
          int keyX = X.getKey();
          int keyY = Y.getKey();

          if (keyX == keyY)
             {
               bool tempResult = true;
               int i = 0;

               ROSE_ASSERT (lengthX > 0);
               while ( (tempResult == true) && (i < lengthX) )
                    {
                      if (tempResult == true)
                           tempResult = X.functionNameString[i] == Y.functionNameString[i];
                      i++;
                    }

#if 0
               if (tempResult == true)
                    printf ("X.functionNameString = %s \n",X.functionNameString);
#endif

               returnValue = tempResult;
             }
        }

  // For now this should always evaluate to be false (later this will not be so)
  // ROSE_ASSERT (returnValue == false);

     return returnValue;
   }

// DQ & AJ (12/3/2004): Added support for deleation of data members
DeleteFlag
GrammarString::getToBeDeleted() const
   {
     return toBeDeleted;
   }

int 
GrammarString::getKey() const
   {
  // This function returns the key that should already be computed
     ROSE_ASSERT (key > 0);
     return key;
   }

int GrammarString::getLength() const
   {
  // This function sums the ascii values of the characters in the character string
     int stringLength = (int)functionNameString.size();
     return stringLength;
   }

int GrammarString::computeKey()
   {
  // This function sums the ascii values of the characters in the character string
     int returnKey = 0;
     int stringLength = getLength();
     int i = 0;
     for (i=0; i < stringLength; i++)
          returnKey += functionNameString[i];

     ROSE_ASSERT (returnKey > 0);

     return returnKey;
   }

void
GrammarString::setAutomaticGenerationOfDataAccessFunctions ( const BuildAccessEnum& X )
   {
     automaticGenerationOfDataAccessFunctions = X;
   }

void
GrammarString::setIsInConstructorParameterList(ConstructParamEnum X)
{
  isInConstructorParameterList= X;
}

void
GrammarString::setToBeTraversed(const TraversalFlag& X)
{
  toBeTraversed= X;
}

BuildAccessEnum GrammarString::generateDataAccessFunctions() const
   {
     return automaticGenerationOfDataAccessFunctions;
   }


void
GrammarString::consistencyCheck() const
   {
  // Error checking (not sure what is a good test here!)
     ROSE_ASSERT (key > 0);
   }

void
GrammarString::display( const string& label ) const
   {
     //     printf ("In GrammarString::display ( %s ) \n",label);
     // BP : 10/10/2001, changed printf to cout
     cout << "In GrammarString::display ( " << label << endl;
     printf ("functionNameString = %s \n",functionNameString.c_str());
     printf ("typeNameString = %s \n",typeNameString.c_str());
     printf ("variableNameString = %s \n",variableNameString.c_str());
     printf ("defaultInitializerString = %s \n",defaultInitializerString.c_str());
     printf ("key = %d \n",key);
   }

// BP : 10/25/2001, a non recursive version that
// allocs memory only once
string GrammarString::copyEdit ( const string& inputString, 
                                 const string& oldToken, 
                                 const string& newToken )
{
  return StringUtility::copyEdit(inputString, oldToken, newToken);
}

bool
GrammarString::isContainedIn ( const string& longString, const string& shortString )
   {
  // This function checks to see if the shortString is contained within the longString

     return (longString.find(shortString) != string::npos);
   }


string
GrammarString::buildDestructorSource()
   {
  // DQ (5/22/2006): This function builds code for the destructor data members (within the destructor)

  // Return value for this function
     string returnString;

  // Support for adding commented to generated source code
     string commentString;

     string variableName = getVariableNameString();
     string typeName     = getTypeNameString();

     string initializerString  = getDefaultInitializerString();

     ROSE_ASSERT (typeName.empty()     == false);
     ROSE_ASSERT (variableName.empty() == false);

  // printf ("In GrammarString::buildDestructorSource(): type = %s variable = %s \n",typeName.c_str(),variableName.c_str());

  // Check if the type name is "char*"
     bool typeIsCharString = typeName.find("char*") != string::npos && typeName.find("char**") == string::npos;
     
     if ( typeIsCharString )
        {
       // Always copy C style strings
          string sourceVariableName = "p_" + variableName;
          commentString = "  // case: typeName == char* or char** for " + variableName + "\n";
       // returnString += "     delete [] " + sourceVariableName + "; \n";
          returnString += "     " + sourceVariableName + " = NULL; \n";

       // DQ (9/5/2006): Get the order right, so that comment appears before the code fragment
       // returnString += commentString;
          returnString = commentString + returnString;

          return returnString;
        }

  // The rule is that if it is not a char* or char** then if it is a pointer type it is a pointer to a Sage IR node
  // bool typeIsSgNode = strstr(typeName.c_str(), "*");

  // Set all all data members to default values
     if ( true )
        {
       // Control variables for code generation
          bool typeIsPointerToListOfPointers    = typeName.find("PtrListPtr") != string::npos;
          bool typeIsPointerToListOfNonpointers = (typeIsPointerToListOfPointers == false) && typeName.find("ListPtr") != string::npos;
          bool typeIsPointerToList              = typeIsPointerToListOfPointers || typeIsPointerToListOfNonpointers;

       // By "simple list" we mean NOT a pointer to a list (just a list, e.g. STL list)
          bool typeIsSimpleListOfPointers       = (typeIsPointerToListOfPointers == false) && typeName.find("PtrList") != string::npos;

       // DQ (5/22/2006): Make sure this is not triggered from "List" substring of ROSEAttributesListContainerPtr
          int typeSize = typeName.size();
          bool typeIsSimpleListOfNonpointers    = (typeIsSimpleListOfPointers == false) && ( typeSize > 4 && typeName.substr( typeSize-4 ) == "List" );

       // bool typeIsList                       = typeIsPointerToList || typeIsSimpleListOfPointers;
          bool typeIsList                       = typeIsPointerToList || typeIsSimpleListOfPointers || typeIsSimpleListOfNonpointers;

#if 0
          printf ("typeIsPointerToListOfPointers    = %s \n",typeIsPointerToListOfPointers    ? "true" : "false");
          printf ("typeIsPointerToListOfNonpointers = %s \n",typeIsPointerToListOfNonpointers ? "true" : "false");
          printf ("typeIsPointerToList              = %s \n",typeIsPointerToList              ? "true" : "false");
          printf ("typeIsSimpleListOfPointers       = %s \n",typeIsSimpleListOfPointers       ? "true" : "false");
          printf ("typeIsSimpleListOfNonpointers    = %s \n",typeIsSimpleListOfNonpointers    ? "true" : "false");
          printf ("typeIsList                       = %s \n",typeIsList                       ? "true" : "false");
          printf ("typeIsSgNode                     = %s \n",typeIsSgNode                     ? "true" : "false");
#endif

       // One of these should be true!
       // ROSE_ASSERT(typeIsList == true  || typeIsSgNode == true);
       // ROSE_ASSERT(typeIsList == false || typeIsSgNode == false);

          string listElementType = "default-error-type";

          if (typeIsList == true)
             {
            // Comment to add to generated source code
               commentString += "  // case: listType for " + variableName + "\n";

            // name constant for all cases below (in this scope)
               string listElementName       = "source_list_element";
               string copyOfListElementName = "copy_list_element";

            // names that are set differently for different cases 
               string iteratorBaseType;
               string needPointer;
               string originalList;
               string iteratorName;

            // Access member functions using "->" or "." (set to some string 
            // that will cause an error if used, instead of empty string).
               string accessOperator = "error string for access operator";

               if (typeIsPointerToList == true)
                  {
                    commentString += "  // case: listType (typeIsPointerToList == true) for " + variableName + "\n";

                    returnString += "     p_" + variableName + " = NULL;\n";
                    returnString = commentString + returnString;
                  }
                 else
                  {
                    commentString += "  // case: listType (typeIsPointerToList == false) for " + variableName + "\n";
                    ROSE_ASSERT(typeIsSimpleListOfPointers == true || typeIsSimpleListOfNonpointers == true);
                    returnString += "     p_" + variableName + ".erase(p_" + variableName + ".begin(),p_" + variableName + ".end()); \n";
                    returnString = commentString + returnString;
                  }
             }
            else
             {
            // ROSE_ASSERT(typeIsSgNode == true);
               commentString += "  // case: not a listType for " + variableName + "\n";

               returnString += "     p_" + variableName + " " + initializerString +";" + " // non list case \n";
               returnString = commentString + returnString;
             }
        }

     return returnString;
   }

