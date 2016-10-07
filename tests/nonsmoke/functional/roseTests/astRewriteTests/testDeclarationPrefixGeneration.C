// This program tests the generaion of the prefix and suffix associated
// with the construction of an AST node from any string.  The rewrite mechanism
// uses strings to simplify the expression or transformations, but the strings
// could reference any variable in the nested set of scopes (from the current location
// where the string is introduced to the global scope).  A prefix string must
// be generated to support the compilation of the string representing the 
// transformation so that all variables will be available and the string can
// be gaurenteed to compile (absent syntax errors in the user's string representing 
// the transformation).  The prefix string must also contain all required openingbraces
// (since new versionable of only local scope might be defined and would otherwise
// result in multiple definitions).  The suffix string is required because of the 
// opening braces within the prefix string.  The suffix string contains only
// closing braces (and sometimes "};" to close off a class definition).

// #include <string>
// #include <iomanip>
#include "rose.h"

// #include "AstTests.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

class PrefixInheritedAttribute
   {
     public:
   };

class PrefixSynthesizedAttribute
   {
     public:         
   };

class PrefixSuffixGenerationTraversal : 
   public AstReversePrefixInhSynProcessing<PrefixInheritedAttribute,PrefixSynthesizedAttribute>
   {
     public:
       // build the stack of lists using data from each scope on the way back so that
       // we can later optimize the process by depositing the list at each node on the 
       // way down to the leaves of the AST. Later we will have a stack< list<SgStatement*> > 
       // data structure as an attribute.  Then we only have to traverse up the tree to 
       // the root until we find a previously deposited attribute and then push one stack 
       // onto the other.

          class DeclarationOrCommentListElement
             {
               public:
                 // We need to collect declarations AND comments (which might appear
                 // between non-declarations in non-global scopes) not attached to 
                 // declarations.
                    SgDeclarationStatement* declaration;
                    AttachedPreprocessingInfoType* comments;

                    DeclarationOrCommentListElement ( SgStatement* astNode )
                       {
                         declaration = isSgDeclarationStatement(astNode);
                         comments    = astNode->getAttachedPreprocessingInfo();
                       }

                 // Calls unparseToString using the correct unparse_info so that functions 
                 // bodies and variable initializers are excluded. Also strips all comments.
                    string generateDeclarationString ( SgDeclarationStatement* declaration ) const;

                    string unparseToString( int & openingIfCounter, 
                                            int & closingEndifCounter , 
                                            bool generateIncludeDirectives, 
                                            bool skipTrailingDirectives );
                   
                    void display( string s );
             };

          typedef list < DeclarationOrCommentListElement > ListOfStatementsType;
          typedef stack< ListOfStatementsType > StackOfListsType;


          ListOfStatementsType currentScope;
          StackOfListsType stackOfScopes;

          bool generateIncludeDirectives;

          PrefixSuffixGenerationTraversal ( bool generateIncludeDirectives = true );

          string generatePrefixString() const;
          string generateSuffixString() const;

          void display( string s );

       // We define the functions required to overwrite the virtual functions in the base class
       //                          evaluateInheritedAttributeReverseTraverseSourceSequence
          PrefixInheritedAttribute evaluateInheritedAttribute (
               SgNode* astNode,
               PrefixInheritedAttribute inputInheritedAttribute );

          PrefixSynthesizedAttribute evaluateSynthesizedAttribute (
               SgNode* astNode,
               PrefixInheritedAttribute inputInheritedAttribute,
               SynthesizedAttributesList inputSynthesizedAttributeList );
   };


PrefixSuffixGenerationTraversal::PrefixSuffixGenerationTraversal ( bool inputGenerateIncludeDirectives )
   : generateIncludeDirectives(inputGenerateIncludeDirectives)
   {
   }

PrefixInheritedAttribute
PrefixSuffixGenerationTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     PrefixInheritedAttribute inputInheritedAttribute )
   {
#if 0
     printf ("!!! In evaluateInheritedAttribute: astNode = %s \n",astNode->sage_class_name());
#endif

     if ( isSgScopeStatement(astNode) != NULL)
        {
//        printf ("Found a new scope! currentScope.size() = %zu \n",currentScope.size());
          stackOfScopes.push(currentScope);

       // empty the current scope stack
          while (currentScope.empty() == false)
               currentScope.pop_front();
        }

     SgStatement* currentStatement = isSgStatement(astNode);
     if ( currentStatement != NULL )
        {
       // printf ("Found a statement! \n");
       // string declarationFilename   = rose::getFileName(currentStatement);
          string declarationFilename   = currentStatement->get_file_info()->get_filename();
          string targetFilename        = rose::getFileNameByTraversalBackToFileNode(currentStatement);

       // printf ("targetFilename = %s declarationFilename = %s counter = %d (*i)->sage_class_name() = %s \n",
       //      targetFilename.c_str(),declarationFilename.c_str(),counter,(*i)->sage_class_name());

          if ( generateIncludeDirectives == false || declarationFilename == targetFilename )
             {
//             printf ("Found a declaration statement! currentScope.size() = %zu \n",currentScope.size());
               currentScope.push_front(currentStatement);
             }
        }

//   printf ("Leaving evaluateInheritedAttribute() \n");

     return inputInheritedAttribute;
   }

PrefixSynthesizedAttribute
PrefixSuffixGenerationTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     PrefixInheritedAttribute inputInheritedAttribute,
     SynthesizedAttributesList inputSynthesizedAttributeList )
   {
#if 0
     printf ("@@@ In evaluateSynthesizedAttribute: astNode = %s \n",astNode->sage_class_name());
     printf ("     inputSynthesizedAttributeList.size() = %zu \n",inputSynthesizedAttributeList.size());
#endif

     return PrefixSynthesizedAttribute();
   }

void
generatePrefixAndSuffixStrings( 
     SgNode* astNode, 
     string & prefixString, 
     string & suffixString, 
     bool generateIncludeDirectives )
   {
  // test definitions
     printf ("\n\n*************************************************************\n");
     printf ("In generatePrefixAndSuffixStrings: astNode is a %s \n",astNode->sage_class_name());

     if (isSgStatement(astNode) != NULL)
        {
          PrefixInheritedAttribute inheritedAttribute;
          PrefixSuffixGenerationTraversal traversal (generateIncludeDirectives);

          traversal.traverse(astNode,inheritedAttribute);

          prefixString = traversal.generatePrefixString();
          suffixString = traversal.generateSuffixString();

          printf ("Leaving generatePrefixAndSuffixStrings: astNode is a %s \n",astNode->sage_class_name());
          printf ("************************************************************* \n\n");
        }
       else
        {
          printf ("generatePrefixAndSuffixStrings() only works on SgStatements \n");
        }
     
   }

void
PrefixSuffixGenerationTraversal::DeclarationOrCommentListElement::display( string s )
   {
     printf ("Inside of PrefixSynthesizedAttribute::DeclarationOrCommentListElement::display(%s) \n",s.c_str());
   }

void
PrefixSuffixGenerationTraversal::display( string s )
   {
     printf ("Inside of PrefixSynthesizedAttribute::display(%s) \n",s.c_str());
     StackOfListsType tempStack = stackOfScopes;
     while (tempStack.empty() == false)
        {
          ListOfStatementsType topOfStack = tempStack.top();
          ListOfStatementsType::iterator j;
          for (j = topOfStack.begin(); j != topOfStack.end(); j++)
               (*j).display(s);
          tempStack.pop();
        }
   }

string
PrefixSuffixGenerationTraversal::DeclarationOrCommentListElement::
unparseToString( 
     int & openingIfCounter, 
     int & closingEndifCounter , 
     bool generateIncludeDirectives, 
     bool skipTrailingDirectives )
   {
     string returnString;

  // If we only have a comment, then the declaration pointer is NULL!
     if (declaration != NULL)
          returnString = generateDeclarationString(declaration);

  // Not prepend or append comments
     if (generateIncludeDirectives == true && comments != NULL)
        {
          string commentPrefix;
          string commentSuffix;
          AttachedPreprocessingInfoType::iterator j;
          for (j = comments->begin(); j != comments->end(); j++)
             {
               ROSE_ASSERT ( (*j) != NULL );
#if 0
               printf ("Attached Comment (relativePosition=%s): %s",
                    ((*j)->relativePosition == PreprocessingInfo::before) ? "before" : "after",(*j)->getString());
#endif
            // We have to keep track of any open #if #endif pairings and make sure 
            // that they match of the the remaining #if gts closed off with an #endif
            // But we only want ot count directives that are NOT skipped
               if ((*j)->getRelativePosition() == PreprocessingInfo::before || (skipTrailingDirectives == false))
                  {
                    switch ( (*j)->getTypeOfDirective() )
                       {
                         case PreprocessingInfo::CpreprocessorIfDeclaration:
                         case PreprocessingInfo::CpreprocessorIfdefDeclaration:
                         case PreprocessingInfo::CpreprocessorIfndefDeclaration:
                         case PreprocessingInfo::CpreprocessorElseDeclaration:
                         case PreprocessingInfo::CpreprocessorElifDeclaration:
                              openingIfCounter++;
                              break;
                         case PreprocessingInfo::CpreprocessorEndifDeclaration:
                              closingEndifCounter++;
                              break;
                          default:
                              break;
                       }
                  }

            // Only use the CPP directive in certain cases (ignore comments, etc.)
               if ( (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorDefineDeclaration  ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorUndefDeclaration   ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIfdefDeclaration   ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIfndefDeclaration  ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIfDeclaration      ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorDeadIfDeclaration  ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorElseDeclaration    ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorElifDeclaration    ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorEndifDeclaration   ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::ClinkageSpecificationStart      ||
                    (*j)->getTypeOfDirective() == PreprocessingInfo::ClinkageSpecificationEnd )
                  {
                    if ((*j)->getRelativePosition() == PreprocessingInfo::before)
                       {
                         commentPrefix += (*j)->getString();
                       }
                      else
                       {
                         if (skipTrailingDirectives == false)
                              commentSuffix += (*j)->getString();
                       }
                  }
             }

          returnString = commentPrefix + returnString + commentSuffix;
        }

     return returnString;
   }

string
PrefixSuffixGenerationTraversal::DeclarationOrCommentListElement::
generateDeclarationString ( SgDeclarationStatement* declaration ) const
   {
  // This function generates a string for a declaration. The string is required for 
  // the intermediate file to make sure that all transformation code will compile 
  // (since it could depend on declarations defined within the code).

  // Details:
  //   1) Only record declarations found within the source file (exclude all header files 
  //      since they will be seen when the same header files are included).
  //   2) Resort the variable declarations to remove redundent entries.
  //        WRONG: variable declarations could have dependences upon class declarations!
  //   3) Don't sort all declarations since some could have dependences.
  //        a) class declarations
  //        b) typedefs
  //        c) function declarations
  //        d) template declarations
  //        e) variable definition???

      ROSE_ASSERT (this != NULL);
      ROSE_ASSERT ( declaration != NULL );
      string declarationString;

   // Build a SgUnparse_Info object to represent formatting options for
   // this statement (use the default values).
      SgUnparse_Info info;

   // exclude comments
      info.set_SkipComments();

   // exclude all CPP directives (since they have already been evaluated by the front-end)
      info.set_SkipCPPDirectives();

      switch ( declaration->variantT() )
        {
       // Enum declarations should not skip their definition since 
       // this is where the constants are declared.
          case V_SgEnumDeclaration:

          case V_SgVariableDeclaration:
          case V_SgTemplateDeclaration:
          case V_SgTypedefDeclaration:
            // Need to figure out if a forward declaration would work or be 
            // more conservative and always output the complete class definition.

            // turn off output of initializer values
               info.set_SkipInitializer();
            // output the declaration as a string
               declarationString = globalUnparseToString(declaration,&info);
               break;

          case V_SgClassDeclaration:
            // Need to figure out if a forward declaration would work or be 
            // more conservative and always output the complete class definition.

            // turn off the generation of the function definitions only 
            // (we still want the restof the class definition since these 
            // define all member data and member functions).
            // info.set_SkipClassDefinition();
               info.set_SkipFunctionDefinition();
               info.set_AddSemiColonAfterDeclaration();

            // output the declaration as a string
               declarationString = globalUnparseToString(declaration,&info);
               break;

       // For functions just output the declaration and skip the definition
       // (This also avoids the generation of redundent definitions since the 
       // function we are in when we generate all declarations would be included).
          case V_SgMemberFunctionDeclaration:
          case V_SgFunctionDeclaration:
             {
            // turn off the generation of the definition
               info.set_SkipFunctionDefinition();
               info.set_AddSemiColonAfterDeclaration();

            // output the declaration as a string
               declarationString = globalUnparseToString(declaration,&info);
               break;
             }

          case V_SgFunctionParameterList:
             {
            // Handle generation of declaration strings this case differnetly from unparser
            // since want to generate declaration strings and not function parameter lists
            // (function parameter lists would be delimited by "," while declarations would
            // be delimited by ";").
               SgFunctionParameterList* parameterListDeclaration = dynamic_cast<SgFunctionParameterList*>(declaration);
               ROSE_ASSERT (parameterListDeclaration != NULL);
               SgInitializedNamePtrList & argList = parameterListDeclaration->get_args();
               SgInitializedNamePtrList::iterator i;
               for (i = argList.begin(); i != argList.end(); i++)
                  {
                    printf ("START: Calling unparseToString on type! \n");
                    ROSE_ASSERT((*i) != NULL);
                    ROSE_ASSERT((*i)->get_type() != NULL);
                    string typeNameString = (*i)->get_type()->unparseToString();
                    printf ("DONE: Calling unparseToString on type! \n");

                    string variableName;
                    if ( (*i)->get_name().getString() != "")
                       {
                         variableName   = (*i)->get_name().getString();
                         declarationString += typeNameString + " " + variableName + "; ";
                       }
                      else
                       {
                      // Don't need the tailing ";" if there is no variable name (I think)
                         declarationString += typeNameString + " ";
                       }
                  }
               break;
             }

       // ignore this case ... not really a declaration
          case V_SgCtorInitializerList:
            // printf ("Ignore the SgCtorInitializerList (constructor initializer list) \n");
               break;

          case V_SgVariableDefinition:
               printf ("ERROR: SgVariableDefinition nodes not used in AST \n");
               ROSE_ABORT();
               break;

       // default case should always be an error
          default:
               printf ("Default reached in AST_Rewrite::AccumulatedDeclarationsAttribute::generateDeclarationString() \n");
               printf ("     declaration->sage_class_name() = %s \n",declaration->sage_class_name());
               ROSE_ABORT();
               break;
        }

  // Add a space to make it easier to read (not required)
     declarationString += " ";

  // printf ("For this scope: declarationString = %s \n",declarationString.c_str());

     return declarationString;
   }



string
PrefixSuffixGenerationTraversal::generatePrefixString() const
   {
     string prefixString;

     StackOfListsType tempStack = stackOfScopes;
     int stackSize = tempStack.size();
     int stackCounter = 0;

  // Keep track of the number of #if and #endif so that they match up 
  // (force them to match up by adding a trailing #endif if required).
     int openingIfCounter    = 0;
     int closingEndifCounter = 0;

     int lastScopeIndex = tempStack.size()-1;
     for (int j=0; j < stackSize; j++)
        {
          ListOfStatementsType tempList = tempStack.top();
          int declarationCounter   = 0;
          int lastDeclarationIndex = tempList.size()-1;
          for (ListOfStatementsType::iterator i = tempList.begin(); i != tempList.end(); i++)
             {
#if 1
            // output the file, line, and column of the current declaration
            // printf ("filename = %s line# = %d column# = %d \n",(*i)->get_file_info()->get_filename(),(*i)->get_file_info()->get_line(),(*i)->get_file_info()->get_col());
            // printf ("(*i)->unparseToString() = %s \n",(*i)->unparseToString().c_str());
#endif

            // Skip all trailing directives on the last element of each list (except maybe the last one)
               bool skipTrailingDirectives = ( (stackCounter < lastScopeIndex) && (declarationCounter == lastDeclarationIndex) );

//             printf ("skipTrailingDirectives = %s \n",skipTrailingDirectives ? "true" : "false");
               string declarationString = (*i).unparseToString(openingIfCounter,closingEndifCounter,generateIncludeDirectives,skipTrailingDirectives);

            // printf ("stackCounter = %d declarationCounter = %d declarationString = %s \n",stackCounter,declarationCounter,declarationString.c_str());
               prefixString += declarationString + " \n";
               declarationCounter++;
             }

          if (stackCounter < stackSize-1)
               prefixString +=  "\n{\n";

          stackCounter++;
          tempStack.pop();
        }

//   printf ("openingIfCounter = %d closingEndifCounter = %d \n",openingIfCounter,closingEndifCounter);

  // Add a trailing set of #endif directives if the #if and #endif directive counters don't match
     if (openingIfCounter != closingEndifCounter)
        {
//        printf ("Adding #endif to correct mismatched #if #endif pairings \n");
          int numberOfClosingEndifDirectives = openingIfCounter - closingEndifCounter;
//        printf ("numberOfClosingEndifDirectives = %d \n",numberOfClosingEndifDirectives);
          for (int i = 0; i < numberOfClosingEndifDirectives; i++)
             {
               if (i==0)
                    prefixString += "\n#endif\n";
                 else
                    prefixString += "#endif\n";
             }
        }

     return prefixString;
   }

string
PrefixSuffixGenerationTraversal::generateSuffixString() const
   {
     string suffixString;

     for (unsigned int i = 1; i < stackOfScopes.size(); i++)
        {
          suffixString += "   }  ";
        }

     return suffixString;
   }

class TreeVisitorTraversal
   : public SgSimpleProcessing
   {
  // Simple traversal class to imimic the use of the analysis in other traversals of the AST

     public:
       // This value is a temporary data member to allow us to output the number of 
       // nodes traversed so that we can relate this number to the numbers printed 
       // in the AST graphs output via DOT.
          int traversalNodeCounter;

          TreeVisitorTraversal () : traversalNodeCounter(0) {};

       // Functions required by the traversal mechanism
          void visit ( SgNode* astNode )
             {
               string prefixString;
               string suffixString;

            // printf ("astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());

               bool generateIncludeDirectives = false;
               generatePrefixAndSuffixStrings(astNode,prefixString,suffixString,generateIncludeDirectives);

            // printOutGlobalScope(astNode,prefixString,suffixString);
            // printOutGlobalIncludes(astNode,prefixString,suffixString);

               if (prefixString.length() > 0)
                  {
                    printf ("prefixString = \n%s \n",prefixString.c_str());
                    printf ("suffixString = \n%s \n",suffixString.c_str());
                  }
             }
   };


int
main ( int argc, char* argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     SgProject* project = frontend(argc,argv);

     AstTests::runAllTests(const_cast<SgProject*>(project));

     TreeVisitorTraversal visitorTraversal;
//   visitorTraversal.traverseInputFiles(project,preorder);
     visitorTraversal.traverse(project,preorder);

  // printf ("Generate the pdf output of the SAGE III AST \n");
  // generatePDF ( project );

     return backend(project);
  // return backend(frontend(argc,argv));
   }





