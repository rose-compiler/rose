// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "grammarString.h"
#include <sstream>

using namespace std;

void
Grammar::buildAtermGenerationSupportFunctions(Terminal & node, StringUtility::FileWithLineNumbers & outputFile)
   {
     vector<GrammarString*> includeList = Grammar::classMemberIncludeList(node);

  // Terminal::TypeEvaluation typeKind = Terminal::evaluateType(std::string& varTypeString);

  // printf ("node = %s includeList = %zu \n",node.getName().c_str(),includeList.size());

  // bool outputAterm = false;
     bool outputAterm = isAstObject(node);

     bool firstAterm      = true;
     bool firstAnnotation = true;

     bool isContainer     = false;

     string atermPatternSubstring = "";
     string atermAnnotationString = "";

#if 0
  // This is a way to reduce the size of the denerated file so that the code generation can be debugged.
     printf ("In Grammar::buildAtermSupportFunctions(): node.getName() = %s \n",node.getName().c_str());
  // outputFile << "// In Grammar::buildAtermSupportFunctions(): data member type: " << grammarString->getTypeNameString() << " data member variable: " << grammarString->getVariableNameString() << "\n";
     if (node.getName() == "SgSupport" || node.getName() == "SgScopeStatement" || node.getName() == "SgExpression" || node.getName() == "SgSymbol" || node.getName() == "SgType" || node.getName() == "SgAsmNode")
        {
          printf ("Exiting early to debug subset of ATerm functionality \n");
          return;
        }
#endif

     bool handledAsSpecialCase = false;

#if 0
  // Allow for special cases as needed.
     if (node.getName() == "SgVariableDeclaration")
        {
          handledAsSpecialCase = true;

          outputFile << "// SPECIAL CASE: node : " << node.getName() << "\n";

       // We need to build each of the required functions (at least as defaults).
       // outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n     ATerm term = ATmake(\"[]\");\n     return term;\n   }\n";
          outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n     ATerm term = ATmake(\"NULL\");\n     return term;\n   }\n";
          outputFile << "void "  << node.getName() << "::generate_ATerm_Annotation(ATerm & term)\n   {\n   }\n";
        }
#endif

#if 0
  // Allow for special cases as needed.
     if (node.getName() == "Sg_File_Info")
        {
          handledAsSpecialCase = true;

          outputFile << "// SPECIAL CASE: node : " << node.getName() << "\n";

       // We need to build each of the required functions (at least as defaults).
       // outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n     ATerm term = ATmake(\"[]\");\n     return term;\n   }\n";
       // outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n     ATerm term = ATmake(\"NULL\");\n     return term;\n   }\n";
          outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n     ATerm term = convertFileInfoToAterm(this);\n     return term;\n   }\n";
          outputFile << "void "  << node.getName() << "::generate_ATerm_Annotation(ATerm & term)\n   {\n  /* no annotations supported */\n   }\n";
        }
#endif

#if 1
     if (handledAsSpecialCase == false)
        {
     for (vector<GrammarString*>::iterator i = includeList.begin(); i != includeList.end(); i++)
        {
          GrammarString* grammarString = *i;
          ROSE_ASSERT(grammarString != NULL);

       // We need a Terminal so that we can call the evaluateType() function (this is a bit awkward).
       // Terminal::TypeEvaluation typeKind = node.evaluateType(grammarString->getTypeNameString());
          string typenameString = grammarString->getTypeNameString();
          Terminal::TypeEvaluation typeKind = node.evaluateType(typenameString);

       // outputFile << "// data member variable: " << grammarString->getVariableNameString() << "\n";
       // generateAtermSupport(grammarString,outputFile);
#if 0
          if (outputAterm == true)
             {
               outputFile << "// data member type: " << grammarString->getTypeNameString() << " data member variable: " << grammarString->getVariableNameString() << "typeKind = " << StringUtility::numberToString((int)typeKind) << "\n";
             }
#endif
       // printf ("   --- typenameString = %s typeKind = %d \n",typenameString.c_str(),typeKind);

          bool toBeTraversed = grammarString->getToBeTraversed().getValue();
#if 1
          if (typeKind == Terminal::SGCLASS_POINTER)
             {
            // This should generate a sequence of aterms.
               if (toBeTraversed == true)
                  {
#if 0
                    printf ("   --- typeKind == SGCLASS_POINTER typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
#endif
                    if (firstAterm == false)
                       {
                         atermPatternSubstring += ", ";
                       }
                    atermPatternSubstring += "<term>";

                    firstAterm = false;
                  }
                 else
                  {
#if 1
                 // This should be added as an aterm annotation.
                     outputFile << "// node = " << node.getName() << " toBeTraversed == false: data member type: " 
                                << grammarString->getTypeNameString() << " data member variable: " 
                                << grammarString->getVariableNameString() << " typeKind = " 
                                << StringUtility::numberToString((int)typeKind) << "\n";
#endif
                 // atermAnnotationString += string("  // ") + typenameString + "\n";

                    bool atermAnnotationStringProcessed = false;

                 // This is where we can support the source file information.
                    if (typenameString == "Sg_File_Info*")
                       {
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),convertFileInfoToAterm(this->get_" + grammarString->getVariableNameString() + "())); \n";
                         atermAnnotationStringProcessed = true;
                       }

                 // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
                    if (atermAnnotationStringProcessed == false && grammarString->automaticGenerationOfDataAccessFunctions.getValue() != TAG_NO_ACCESS_FUNCTIONS)
                       {
                         if (typenameString.substr(0,7) != "static " && typenameString.substr(0,11) != "$CLASSNAME*")
                            {
                              atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this->get_" + grammarString->getVariableNameString() + "()).c_str())); \n";
                            }

                         firstAnnotation = false;
                       }
                      else
                       {
#if 0
                         printf ("NOTE: Skipping output of ATerm annotation for grammarString->getVariableNameString() = %s (no access functions available) \n",grammarString->getVariableNameString().c_str());
#endif
                       }
                  }

            // printf ("Found typeKind == SGCLASS_POINTER \n");

            // This is the rule we will use to build Aterms for this case.
            // term = ATmake("<appl(<list>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(this));

             }
#endif
#if 0
       // This is not used for containers of SGCLASS_POINTER.
          if (typeKind == Terminal::SGCLASS_POINTER_VECTOR)
             {
            // This should generate a list of aterms.
               printf ("   --- typeKind == SGCLASS_POINTER_VECTOR typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
            // printf ("Found typeKind == SGCLASS_POINTER_VECTOR \n");
             }
#endif
          if (typeKind == Terminal::SGCLASS_POINTER_LIST)
             {
               if (toBeTraversed == true)
                  {
                 // This should generate a list of aterms.
#if 0
                    printf ("   --- typeKind == SGCLASS_POINTER_LIST typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
#endif
                 // printf ("Found typeKind == SGCLASS_POINTER_LIST \n");

                 // This is the rule we will use to build Aterms for this case.
                 // term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(this));

                    if (firstAterm == false)
                       {
                         atermPatternSubstring += ", ";
                       }
                    atermPatternSubstring += "<list>";

                    isContainer = true;
                    firstAterm = false;
                  }
             }
#if 0
       // This is not used for containers of SGCLASS_POINTER.
          if (typeKind == Terminal::SGCLASS_POINTER_VECTOR_NAMED_LIST)
             {
            // This should generate a list of aterms.
               printf ("   --- typeKind == SGCLASS_POINTER_VECTOR_NAMED_LIST typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
            // printf ("Found typeKind == SGCLASS_POINTER_VECTOR_NAMED_LIST \n");
             }
#endif

       // Ignore these initially.
          if (typeKind == Terminal::BASIC_DATA_TYPE)
             {
            // This will generate an Aterm annotation to store the value of the associated basic data type.
#if 0
               printf ("   --- typenameString = %s typeKind = %d \n",typenameString.c_str(),typeKind);
               printf ("Found typeKind == BASIC_DATA_TYPE \n");
#endif
            // This is the code most similar that we want to generate.
            // term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(this).c_str()));

            // atermAnnotationString += string("  // ") + typenameString + "\n";

            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions.getValue() != TAG_NO_ACCESS_FUNCTIONS)
                  {
                 // string atermTypeName = "ATmake";
                 // These are the easy types to support using an ATerm Int
                    if (typenameString == "bool" || typenameString == "int")
                       {
                      // atermTypeName = "ATmakeInt";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "())); \n";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\",(int)(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                       }
                      else
                       {
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this).c_str())); \n";
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this->get_" + grammarString->getVariableNameString() + "()).c_str())); \n";
                       }

                    firstAnnotation = false;
                  }
                 else
                  {
#if 0
                    printf ("NOTE: Skipping output of ATerm annotation for grammarString->getVariableNameString() = %s (no access functions available) \n",grammarString->getVariableNameString().c_str());
#endif
                  }
             }

       // This is where names are stored and we need them for numerous sources of declarations (stored as a string annotation).
          if (typeKind == Terminal::SGNAME)
             {
            // This will generate an Aterm annotation to store the value of the associated basic data type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
            // printf ("Found typeKind == SGNAME \n");
#endif
            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions.getValue() != TAG_NO_ACCESS_FUNCTIONS)
                  {
                 // string atermTypeName = "ATmake";
                 // These are the easy types to support using an ATerm Int
#if 0
                    if (typenameString == "bool" || typenameString == "int")
                       {
                      // atermTypeName = "ATmakeInt";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "())); \n";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\",(this->get_" + grammarString->getVariableNameString() + "().str()))); \n";
                       }
                      else
                       {
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this).c_str())); \n";
                       }
#else
                    atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\",(this->get_" + grammarString->getVariableNameString() + "().str()))); \n";
#endif

                    firstAnnotation = false;
                  }
                 else
                  {
#if 0
                    printf ("NOTE: Skipping output of ATerm annotation for grammarString->getVariableNameString() = %s (no access functions available) \n",grammarString->getVariableNameString().c_str());
#endif
                  }
             }

       // DQ (make the annotations more complete by adding the enum types.
          if (typeKind == Terminal::ENUM_TYPE)
             {
            // This will generate an Aterm annotation to store the value of the associated enum type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
               printf ("Found typeKind == ENUM_TYPE \n");
#endif
            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions.getValue() != TAG_NO_ACCESS_FUNCTIONS)
                  {
                    atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\",(this->get_" + grammarString->getVariableNameString() + "()))); \n";

                    firstAnnotation = false;
                  }
                 else
                  {
#if 0
                    printf ("NOTE: Skipping output of ATerm annotation for grammarString->getVariableNameString() = %s (no access functions available) \n",grammarString->getVariableNameString().c_str());
#endif
                  }
             }
        }

     bool availableAtermPatternString    = (outputAterm == true) && (firstAterm == false);
     bool availableAtermAnnotationString = (outputAterm == true) && (firstAnnotation == false);

     if (availableAtermPatternString == true && availableAtermAnnotationString == true)
        {
          if (isAstObject(node))
             {
               outputFile << "// " <<  node.getName() << " IS an AST object \n";
             }
            else
             {
               outputFile << "// " <<  node.getName() << " is NOT an AST object \n";
             }
        }

     if (availableAtermPatternString == true)
        {
       // Make this a virtual function (so we don't need to pass in a ROSE IR node).
       // outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n";
       // outputFile << "ATerm " << node.getName() << "::generate_ATerm(" << node.getName() << "* n) \n   {\n";
          outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n";

       // outputFile << "atermPatternSubstring = " << atermPatternSubstring << "\n";
          string atermPatternString = node.getName() + "(" + atermPatternSubstring + ")";
          outputFile << "  // expected read pattern: atermPatternString = " << atermPatternString << "\n";

#if 0
       // I don't think we need this complexity.
          if (isContainer == true)
             {
               outputFile << "     ATerm term = ATmake(" << atermPatternString << ",getTraversalChildrenAsAterm(this));\n";
             }
            else
             {
               outputFile << "     ATerm term = ATmake(" << atermPatternString << ",getTraversalChildrenAsAterm(this));\n";
             }
#else
       // I think the explicit specification of "<term>, <term>, ..." is inconsistant with using the getTraversalChildrenAsAterm(this) 
       // function and that we should be using another form of construction for aterms (similar to the explicit code that has been 
       // demonstrated previously).

       // This is the simplest form.
       // outputFile << "     ATerm term = ATmake(\"" << atermPatternString << "\",getTraversalChildrenAsAterm(this));\n";
       // term = ATmake("<appl(<list>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));
       // outputFile << "     ATerm term = ATmake(\"<appl(<list>)>\", \"" << node.getName() << "\",getTraversalChildrenAsAterm(this));\n";
#if 0
          outputFile << "  // isContainer = " << ((isContainer == true) ? "true" : "false") << "\n";
#endif
          if (isContainer == false)
             {
               outputFile << "     ATerm term = ATmake(\"<appl(<list>)>\", \"" << node.getName() << "\",getTraversalChildrenAsAterm(this));\n";
             }
            else
             {
            // Case of when we have a container (since containers can have more that 255 elements which is a limit in the ATerm library).
            // term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));
               outputFile << "     ATerm term = ATmake(\"<appl(<term>)>\", \"" << node.getName() << "\",getTraversalChildrenAsAterm(this));\n";
             }
#endif
       // outputFile << "  // end of function: " << node.getName() << "::generate_ATerm() \n";
          outputFile << "     return term; \n   } \n\n";
        }
       else
        {
       // Output an empty function because we have had to unconditially add virtual function to 
       // every ROSE IR node and we require a defining function declaration.
       // outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n     ATerm term = ATmake(\"[]\");\n     return term;\n   }\n";
          outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n     ATerm term = ATmake(\"NULL\");\n     return term;\n   }\n";
        }

  // Add vertical space to format the generated file better.
     outputFile << "\n";

  // We only want to annotate ATerms that we will actually generate.
     if (availableAtermPatternString == true && availableAtermAnnotationString == true)
        {
       // Make this a virtual function (so we don't need to pass in a ROSE IR node).
       // outputFile << "void " << node.getName() << "::generate_ATerm_Annotation(ATerm & term, " << node.getName() << "* n) \n   {\n";
          outputFile << "void " << node.getName() << "::generate_ATerm_Annotation(ATerm & term)\n   {\n";

       // outputFile << "  // atermAnnotationString = " << atermAnnotationString << "\n";
          outputFile << atermAnnotationString << "\n";

       // outputFile << "  // end of function: " << node.getName() << "::generate_ATerm_attributes() \n";
          outputFile << "   } \n\n";
        }
       else
        {
       // Output an empty function because we have had to unconditially add virtual function to 
       // every ROSE IR node and we require a defining function declaration.
          outputFile << "void " << node.getName() << "::generate_ATerm_Annotation(ATerm & term)\n   {\n   }\n";
        }
#endif

       // End of block for handling when(handledAsSpecialCase == false).
        }
     
  // Add vertical space to format the generated file better.
     outputFile << "\n\n";

  // Traverse all nodes of the grammar recursively and build the tree traversal function for each of them.
     vector<Terminal*>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin(); treeNodeIterator != node.subclasses.end(); treeNodeIterator++ )
        {
          ROSE_ASSERT((*treeNodeIterator) != NULL);
          ROSE_ASSERT((*treeNodeIterator)->getBaseClass() != NULL);
          buildAtermGenerationSupportFunctions(**treeNodeIterator, outputFile);
        }
   }


#if 0
// It is not efficient to refactor the code into this level of function on the GrammarString objects.
void
Grammar::generateAtermSupport(GrammarString* gs, StringUtility::FileWithLineNumbers & outputFile)
   {
     outputFile << "// data member type: " << gs->getTypeNameString() << " data member variable: " << gs->getVariableNameString() << "\n";

  // Terminal::TypeEvaluation typeKind = Terminal::evaluateType(gs->getTypeNameString());

  // outputFile << "// data member prototype: " << gs->getDataPrototypeString() << "\n";
  // outputFile << "// function name: " << gs->getFunctionNameString() << "\n\n";
   }
#endif


void
Grammar::buildAtermConsumerSupportFunctions(Terminal & node, StringUtility::FileWithLineNumbers & outputFile)
   {
  // This function generates the code to convert the ATERM --> AST.

     vector<GrammarString*> includeList = Grammar::classMemberIncludeList(node);

  // Terminal::TypeEvaluation typeKind = Terminal::evaluateType(std::string& varTypeString);
  // printf ("node = %s includeList = %zu \n",node.getName().c_str(),includeList.size());

  // bool outputAterm = false;
     bool outputAterm = isAstObject(node);

     bool firstAterm      = true;
     bool firstAnnotation = true;

     bool firstConstructorParameter = true;

     bool isContainer     = false;

     string atermPatternSubstring   = "";
     string atermArgumentsSubstring = "";
     string atermAnnotationString   = "";

  // Setup to build the children required for the constructor (builder) function.
     string dataMemberString = "";

  // Post constructor (builder) initialization (mostly for lists at present, likely for attributes later).
     string dataMemberString_post = "";

     string constructorArgumentsString = "";
     string constructorParametersString = "";

     string buildNodeFunctionName = node.getName() + "::build_node_from_nonlist_children";

     bool complete = false;
     bool withTypes         = true;
     bool withInitializers  = false;
     ConstructParamEnum cur = CONSTRUCTOR_PARAMETER;

     string defaultConstructorParametersString = buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);
#if 0
     outputFile << "  // node = " << node.getName() << " defaultConstructorParameterString = " << defaultConstructorParametersString << "\n";
#endif
#if 0
  // This is a way to reduce the size of the denerated file so that the code generation can be debugged.
     printf ("In Grammar::buildAtermConsumerSupportFunctions(): node.getName() = %s \n",node.getName().c_str());
  // outputFile << "// In Grammar::buildAtermSupportFunctions(): data member type: " << grammarString->getTypeNameString() << " data member variable: " << grammarString->getVariableNameString() << "\n";
     if (node.getName() == "SgSupport" || node.getName() == "SgScopeStatement" || node.getName() == "SgExpression" || 
         node.getName() == "SgSymbol" || node.getName() == "SgType" || node.getName() == "SgAsmNode" || 
         node.getName() == "SgUntypedNode" || node.getName() == "SgC_PreprocessorDirectiveStatement")
        {
          printf ("Exiting early to debug subset of ATerm to AST functionality \n");
          return;
        }
#endif

     bool handledAsSpecialCase = false;

#if 0
  // Allow for special cases as needed.
     if (node.getName() == "SgVariableDeclaration")
        {
          handledAsSpecialCase = true;

          outputFile << "// SPECIAL CASE: node : " << node.getName() << "\n";
        }
#endif

#if 1
     if (handledAsSpecialCase == false)
        {
       // The counter used to build the substring that will be a part of the function argument names for terms (e.g. term1, term2, term3, etc.).
          int integer_counter = 1;

  // We need to keep track of some history to get the use of "," correct.
  // This is only relevant for those parameters that will be output.
     bool lastDataMemberWasConstructorParameter = false;

     for (vector<GrammarString*>::iterator i = includeList.begin(); i != includeList.end(); i++)
        {
          GrammarString* grammarString = *i;
          ROSE_ASSERT(grammarString != NULL);

       // We need a Terminal so that we can call the evaluateType() function (this is a bit awkward).
       // Terminal::TypeEvaluation typeKind = node.evaluateType(grammarString->getTypeNameString());
          string typenameString = grammarString->getTypeNameString();
          Terminal::TypeEvaluation typeKind = node.evaluateType(typenameString);

       // outputFile << "// data member variable: " << grammarString->getVariableNameString() << "\n";
       // generateAtermSupport(grammarString,outputFile);
#if 0
          if (outputAterm == true)
             {
               outputFile << "// data member type: " << grammarString->getTypeNameString() << " data member variable: " << grammarString->getVariableNameString() << " typeKind = " << StringUtility::numberToString((int)typeKind) << "\n";
             }
#endif
#if 0
          outputFile << "// data member type: " << grammarString->getTypeNameString() << " data member variable: " << grammarString->getVariableNameString() << " typeKind = " << StringUtility::numberToString((int)typeKind) << "\n";
#endif
       // printf ("   --- typenameString = %s typeKind = %d \n",typenameString.c_str(),typeKind);

          bool toBeTraversed = grammarString->getToBeTraversed().getValue();

          bool isInConstructorParameterList = grammarString->getIsInConstructorParameterList().getValue();

#if 0
          outputFile << "// firstConstructorParameter             = " << (firstConstructorParameter             ? "true" : "false") << "\n";
          outputFile << "// lastDataMemberWasConstructorParameter = " << (lastDataMemberWasConstructorParameter ? "true" : "false") << "\n";
          outputFile << "// isInConstructorParameterList          = " << (isInConstructorParameterList          ? "true" : "false") << "\n";
#endif

#if 1
          if (typeKind == Terminal::SGCLASS_POINTER)
             {
            // This should generate a sequence of aterms.
               if (toBeTraversed == true)
                  {
#if 0
                    printf ("   --- typeKind == SGCLASS_POINTER typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
#endif
                    if (firstAterm == false)
                       {
                         atermPatternSubstring   += ", ";
                         atermArgumentsSubstring += ", ";
                       }
                    
                 // if (isInConstructorParameterList == true)
                 // if (firstConstructorParameter == false && lastDataMemberWasConstructorParameter && isInConstructorParameterList == true)
                 // if (firstConstructorParameter == false && isInConstructorParameterList == true)
                    if (firstConstructorParameter == false && lastDataMemberWasConstructorParameter == true && isInConstructorParameterList == true)
                       {
                         constructorArgumentsString  += ", ";
                         constructorParametersString += ", ";
                       }

                    atermPatternSubstring   += "<term>";
                    atermArgumentsSubstring += "&term";

                 // We need to build the code to build the data members (from aterms).
                 // This could should look like:
                 //      SgForInitStatement* forInit = isSgForInitStatement(generate_AST(temp1)); \n
                 //      ROSE_ASSERT(forInit != NULL); \n
                 //      result = new SgForStatement(forInit, test, increment, body); \n\n

                 // dataMemberString += "          SgForInitStatement* forInit = isSgForInitStatement(generate_AST(temp1)); \n";
                 // dataMemberString += "          ROSE_ASSERT(forInit != NULL); \n";

                    string integer_counter_string = StringUtility::numberToString(integer_counter);

                    size_t typenameStringSize = grammarString->getTypeNameString().size();
                    string typenameBase       = grammarString->getTypeNameString().substr(0,typenameStringSize-1);
                    
                    dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = is" + typenameBase + "(generate_AST(term" + integer_counter_string + ")); \n";
                    dataMemberString += "          ROSE_ASSERT(local_" + grammarString->getVariableNameString() + " != NULL); \n";

                    if (isInConstructorParameterList == true)
                       {
                      // This variable should be empty when we have a container.
                         constructorArgumentsString += "local_" + grammarString->getVariableNameString();
                         atermArgumentsSubstring += integer_counter_string;

                         constructorParametersString += grammarString->getTypeNameString() + " " + "local_" + grammarString->getVariableNameString();
                         lastDataMemberWasConstructorParameter = true;

                         firstConstructorParameter = false;
                       }
                      else 
                       {
                      // lastDataMemberWasConstructorParameter = false;
                       }
                         
                 // Increment the counter used to build the substring that will be a part of the function argument names for terms (e.g. term1, term2, term3, etc.).
                    integer_counter++;

                    firstAterm = false;
                  }
                 else
                  {
                 // This should be added as an aterm annotation.
#if 0
                     outputFile << "// node = " << node.getName() << " toBeTraversed == false: data member type: " 
                                << grammarString->getTypeNameString() << " data member variable: " 
                                << grammarString->getVariableNameString() << " typeKind = " 
                                << StringUtility::numberToString((int)typeKind) << "\n";
#endif
                 // atermAnnotationString += string("  // ") + typenameString + "\n";

                    if (firstAterm == false)
                       {
                      // atermPatternSubstring   += ", ";
                      // atermArgumentsSubstring += ", ";
                       }
                    
                      // if (isInConstructorParameterList == true)
                      // if (lastDataMemberWasConstructorParameter && isInConstructorParameterList == true)
                    if (firstConstructorParameter == false && isInConstructorParameterList == true)
                       {
                         constructorArgumentsString  += ", ";
                      // constructorParametersString += ", ";
                       }

                 // The function we build to translate the child data members to a proper IR node required all of the constructor parameters (even if they are not traversed).
                    if (isInConstructorParameterList == true)
                       {
                         dataMemberString += "       // This needs to be supported because it is a constructor parameter. \n";
                         dataMemberString += string("       // node = ") + node.getName() + " toBeTraversed == false: data member type: "
                                          + grammarString->getTypeNameString() + " data member variable: " 
                                          + grammarString->getVariableNameString() + " typeKind = " 
                                          + StringUtility::numberToString((int)typeKind) + "\n";
                         dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = NULL; \n";

                         constructorArgumentsString += "local_" + grammarString->getVariableNameString();

                         firstConstructorParameter = false;
                         lastDataMemberWasConstructorParameter = true;
                       }
                      else 
                       {
                      // lastDataMemberWasConstructorParameter = false;
                       }

#if 0
                    bool atermAnnotationStringProcessed = false;

                 // This is where we can support the source file information.
                    if (typenameString == "Sg_File_Info*")
                       {
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),convertFileInfoToAterm(this->get_" + grammarString->getVariableNameString() + "())); \n";
                         atermAnnotationStringProcessed = true;
                       }

                 // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
                    if (atermAnnotationStringProcessed == false && grammarString->automaticGenerationOfDataAccessFunctions.getValue() != TAG_NO_ACCESS_FUNCTIONS)
                       {
                         if (typenameString.substr(0,7) != "static " && typenameString.substr(0,11) != "$CLASSNAME*")
                            {
                              atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this->get_" + grammarString->getVariableNameString() + "()).c_str())); \n";
                            }

                         firstAnnotation = false;
                       }
                      else
                       {
#if 0
                         printf ("NOTE: Skipping output of ATerm annotation for grammarString->getVariableNameString() = %s (no access functions available) \n",grammarString->getVariableNameString().c_str());
#endif
                       }
#endif
                  }

            // printf ("Found typeKind == SGCLASS_POINTER \n");

            // This is the rule we will use to build Aterms for this case.
            // term = ATmake("<appl(<list>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(this));

             }
#endif

          if (typeKind == Terminal::SGCLASS_POINTER_LIST)
             {
               if (toBeTraversed == true)
                  {
                 // This should generate a list of aterms.
#if 0
                    printf ("   --- typeKind == SGCLASS_POINTER_LIST typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
#endif
                 // printf ("Found typeKind == SGCLASS_POINTER_LIST \n");

                 // This is the rule we will use to build Aterms for this case.
                 // term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(this));

                    if (firstAterm == false)
                       {
                         atermPatternSubstring   += ", ";
                         atermArgumentsSubstring += ", ";

                      // constructorArgumentsString  += ", ";
                      // constructorParametersString += ", ";
                       }

                    atermPatternSubstring += "<list>";
                    string atermArgumentsName = "term";
                    atermArgumentsSubstring += "&" + atermArgumentsName;
                    string integer_counter_string = StringUtility::numberToString(integer_counter);
                    atermArgumentsSubstring += integer_counter_string;

                    string typenameBase = grammarString->getTypeNameString();

                 // In the case of a list of ROSE IR nodes we need to build the elements of the list seperately in a loop.
                 // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = is" + typenameBase + "(generate_AST(term" + integer_counter_string + ")); \n";
                 // dataMemberString += "          ROSE_ASSERT(local_" + grammarString->getVariableNameString() + " != NULL); \n";
                 // We need code such as:
                 //      vector<ATerm> terms = getAtermList(temp1);
                 //      for (size_t i = 0; i < terms.size(); ++i)
                 //         {
                 //           SgDeclarationStatement* child = isSgDeclarationStatement(convertAtermToNode(terms[i]));
                 //           ROSE_ASSERT (child);
                 //           class_definition->append_member(child);
                 //         }
                 //      result = class_definition;
                 // dataMemberString_post += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = NULL";
                    dataMemberString_post += "          vector<ATerm> terms = getAtermList(" + atermArgumentsName + integer_counter_string + ");\n";
                    dataMemberString_post += "          for (size_t i = 0; i < terms.size(); ++i)\n";
                    dataMemberString_post += "             {\n";
                    dataMemberString_post += "               SgDeclarationStatement* child = isSgDeclarationStatement(generate_AST(terms[i]));\n";
                    dataMemberString_post += "               ROSE_ASSERT(child != NULL);\n";
                 // dataMemberString_post += "               local_returnNode->append(child);\n";
                    dataMemberString_post += "            // local_returnNode->append(child); (need an append member function) \n";
                    dataMemberString_post += "             }\n";
                 //           SgDeclarationStatement* child = isSgDeclarationStatement(convertAtermToNode(terms[i]));
                 //           ROSE_ASSERT (child);
                 //           class_definition->append_member(child);
                 //         }

                 // constructorArgumentsString += "local_" + grammarString->getVariableNameString();

                 // constructorParametersString += grammarString->getTypeNameString() + " " + "local_" + grammarString->getVariableNameString();

                 // Increment the counter used to build the substring that will be a part of the function argument names for terms (e.g. term1, term2, term3, etc.).
                    integer_counter++;

                    isContainer = true;
                    firstAterm  = false;
                  }
             }
#if 0
       // This is not used for containers of SGCLASS_POINTER.
          if (typeKind == Terminal::SGCLASS_POINTER_VECTOR_NAMED_LIST)
             {
            // This should generate a list of aterms.
               printf ("   --- typeKind == SGCLASS_POINTER_VECTOR_NAMED_LIST typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
            // printf ("Found typeKind == SGCLASS_POINTER_VECTOR_NAMED_LIST \n");
             }
#endif

       // Ignore these initially.
          if (typeKind == Terminal::BASIC_DATA_TYPE)
             {
            // This will generate an Aterm annotation to store the value of the associated basic data type.
#if 0
               printf ("   --- typenameString = %s typeKind = %d \n",typenameString.c_str(),typeKind);
               printf ("Found typeKind == BASIC_DATA_TYPE \n");
#endif
            // This is the code most similar that we want to generate.
            // term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(this).c_str()));

            // atermAnnotationString += string("  // ") + typenameString + "\n";

               if (firstConstructorParameter == false && isInConstructorParameterList == true)
                  {
                    constructorArgumentsString  += ", ";
                 // constructorParametersString += ", ";
                  }

            // The function we build to translate the child data members to a proper IR node required all of the constructor parameters (even if they are not traversed).
               if (isInConstructorParameterList == true)
                  {
                    dataMemberString += "       // This needs to be supported because it is a constructor parameter. \n";
                    dataMemberString += string("       // node = ") + node.getName() + " toBeTraversed == false: data member type: "
                                     + grammarString->getTypeNameString() + " data member variable: " 
                                     + grammarString->getVariableNameString() + " typeKind = " 
                                     + StringUtility::numberToString((int)typeKind) + "\n";
                    dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = NULL; \n";

                    constructorArgumentsString += "local_" + grammarString->getVariableNameString();

                    firstConstructorParameter = false;
                    lastDataMemberWasConstructorParameter = true;
                  }
                 else 
                  {
                 // lastDataMemberWasConstructorParameter = false;
                  }

            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions.getValue() != TAG_NO_ACCESS_FUNCTIONS)
                  {
                 // string atermTypeName = "ATmake";
                 // These are the easy types to support using an ATerm Int
                    if (typenameString == "bool" || typenameString == "int")
                       {
                      // atermTypeName = "ATmakeInt";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "())); \n";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\",(int)(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                       }
                      else
                       {
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this).c_str())); \n";
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this->get_" + grammarString->getVariableNameString() + "()).c_str())); \n";
                       }

                    firstAnnotation = false;
                  }
                 else
                  {
#if 0
                    printf ("NOTE: Skipping output of ATerm annotation for grammarString->getVariableNameString() = %s (no access functions available) \n",grammarString->getVariableNameString().c_str());
#endif
                  }
             }

       // This is where names are stored and we need them for numerous sources of declarations (stored as a string annotation).
          if (typeKind == Terminal::SGNAME)
             {
            // This will generate an Aterm annotation to store the value of the associated basic data type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
            // printf ("Found typeKind == SGNAME \n");
#endif
               if (firstConstructorParameter == false && isInConstructorParameterList == true)
                  {
                    constructorArgumentsString  += ", ";
                 // constructorParametersString += ", ";
                  }

            // The function we build to translate the child data members to a proper IR node required all of the constructor parameters (even if they are not traversed).
               if (isInConstructorParameterList == true)
                  {
                    dataMemberString += "       // This needs to be supported because it is a constructor parameter. \n";
                    dataMemberString += string("       // node = ") + node.getName() + " toBeTraversed == false: data member type: "
                                     + grammarString->getTypeNameString() + " data member variable: " 
                                     + grammarString->getVariableNameString() + " typeKind = " 
                                     + StringUtility::numberToString((int)typeKind) + "\n";
                    dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = \"\"; \n";

                    constructorArgumentsString += "local_" + grammarString->getVariableNameString();

                    firstConstructorParameter = false;
                    lastDataMemberWasConstructorParameter = true;
                  }
                 else 
                  {
                 // lastDataMemberWasConstructorParameter = false;
                  }

            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions.getValue() != TAG_NO_ACCESS_FUNCTIONS)
                  {
                 // string atermTypeName = "ATmake";
                 // These are the easy types to support using an ATerm Int
#if 0
                    if (typenameString == "bool" || typenameString == "int")
                       {
                      // atermTypeName = "ATmakeInt";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "())); \n";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\",(this->get_" + grammarString->getVariableNameString() + "().str()))); \n";
                       }
                      else
                       {
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this).c_str())); \n";
                       }
#else
                    atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\",(this->get_" + grammarString->getVariableNameString() + "().str()))); \n";
#endif

                    firstAnnotation = false;
                  }
                 else
                  {
#if 0
                    printf ("NOTE: Skipping output of ATerm annotation for grammarString->getVariableNameString() = %s (no access functions available) \n",grammarString->getVariableNameString().c_str());
#endif
                  }
             }

       // DQ: make the annotations more complete by adding the enum types.
          if (typeKind == Terminal::ENUM_TYPE)
             {
            // This will generate an Aterm annotation to store the value of the associated enum type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
               printf ("Found typeKind == ENUM_TYPE \n");
#endif
               if (firstConstructorParameter == false && isInConstructorParameterList == true)
                  {
                    constructorArgumentsString  += ", ";
                 // constructorParametersString += ", ";
                  }

            // The function we build to translate the child data members to a proper IR node required all of the constructor parameters (even if they are not traversed).
               if (isInConstructorParameterList == true)
                  {
                    dataMemberString += "       // This needs to be supported because it is a constructor parameter. \n";
                    dataMemberString += string("       // node = ") + node.getName() + " toBeTraversed == false: data member type: "
                                     + grammarString->getTypeNameString() + " data member variable: " 
                                     + grammarString->getVariableNameString() + " typeKind = " 
                                     + StringUtility::numberToString((int)typeKind) + "\n";

                 // Implement initalization of enum using cast (alternatively the default value is available in ROSETTA).
                 // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = 0; \n";
                    dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = " + grammarString->getTypeNameString() + "(0); \n";

                    constructorArgumentsString += "local_" + grammarString->getVariableNameString();

                    firstConstructorParameter = false;
                    lastDataMemberWasConstructorParameter = true;
                  }
                 else 
                  {
                 // lastDataMemberWasConstructorParameter = false;
                  }

            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions.getValue() != TAG_NO_ACCESS_FUNCTIONS)
                  {
                    atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\",(this->get_" + grammarString->getVariableNameString() + "()))); \n";

                    firstAnnotation = false;
                  }
                 else
                  {
#if 0
                    printf ("NOTE: Skipping output of ATerm annotation for grammarString->getVariableNameString() = %s (no access functions available) \n",grammarString->getVariableNameString().c_str());
#endif
                  }
             }

       // DQ: make the annotations more complete by adding the enum types.
          if (typeKind == Terminal::STRING)
             {
            // This will generate an Aterm annotation to store the value of the associated enum type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
               printf ("Found typeKind == STRING \n");
#endif
#if 1
            // buildDataMember(Terminal & node, GrammarString* grammarString, bool & firstConstructorParameter, bool & lastDataMemberWasConstructorParameter, bool & isInConstructorParameterList, string & dataMemberString );
               buildDataMember(node,grammarString,firstConstructorParameter,lastDataMemberWasConstructorParameter,isInConstructorParameterList,constructorArgumentsString,dataMemberString);
#else
               if (firstConstructorParameter == false && isInConstructorParameterList == true)
                  {
                    constructorArgumentsString  += ", ";
                 // constructorParametersString += ", ";
                  }

            // The function we build to translate the child data members to a proper IR node required all of the constructor parameters (even if they are not traversed).
               if (isInConstructorParameterList == true)
                  {
                    dataMemberString += "       // This needs to be supported because it is a constructor parameter. \n";
                    dataMemberString += string("       // node = ") + node.getName() + " toBeTraversed == false: data member type: "
                                     + grammarString->getTypeNameString() + " data member variable: " 
                                     + grammarString->getVariableNameString() + " typeKind = " 
                                     + StringUtility::numberToString((int)typeKind) + "\n";

                 // Implement initalization of enum using cast (alternatively the default value is available in ROSETTA).
                    dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = \"\"; \n";
                 // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = " + grammarString->getTypeNameString() + "(0); \n";

                    constructorArgumentsString += "local_" + grammarString->getVariableNameString();

                    firstConstructorParameter = false;
                    lastDataMemberWasConstructorParameter = true;
                  }
                 else 
                  {
                 // lastDataMemberWasConstructorParameter = false;
                  }
#endif
             }
        }

     bool availableAtermPatternString    = (outputAterm == true) && (firstAterm == false);
     bool availableAtermAnnotationString = (outputAterm == true) && (firstAnnotation == false);

     if (availableAtermPatternString == true && availableAtermAnnotationString == true)
        {
#if 0
          if (isAstObject(node))
             {
               outputFile << "// " <<  node.getName() << " IS an AST object \n";
             }
            else
             {
               outputFile << "// " <<  node.getName() << " is NOT an AST object \n";
             }
#endif
        }

     if (availableAtermPatternString == true)
        {
       // Make this a virtual function (so we don't need to pass in a ROSE IR node).
       // outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n";
       // outputFile << "ATerm " << node.getName() << "::generate_ATerm(" << node.getName() << "* n) \n   {\n";
       // outputFile << "SgNode* " << node.getName() << "::generate_AST(ATerm & term) \n   {\n";

       // outputFile << "atermPatternSubstring = " << atermPatternSubstring << "\n";
          string atermPatternString = node.getName() + "(" + atermPatternSubstring + ")";
#if 0
          outputFile << "  // expected read pattern: atermPatternString      = " << atermPatternString << "\n";
          outputFile << "  // expected read pattern: atermArgumentsSubstring = " << atermArgumentsSubstring << "\n";
#endif
#if 0
       // I don't think we need this complexity.
          if (isContainer == true)
             {
               outputFile << "     ATerm term = ATmake(" << atermPatternString << ",getTraversalChildrenAsAterm(this));\n";
             }
            else
             {
               outputFile << "     ATerm term = ATmake(" << atermPatternString << ",getTraversalChildrenAsAterm(this));\n";
             }
#else
       // I think the explicit specification of "<term>, <term>, ..." is inconsistant with using the getTraversalChildrenAsAterm(this) 
       // function and that we should be using another form of construction for aterms (similar to the explicit code that has been 
       // demonstrated previously).

       // This is the simplest form.
       // if (ATmatch(term, "Class(<str>, <term>)", &str, &temp1))

#if 0
          outputFile << "  // isContainer = " << ((isContainer == true) ? "true" : "false") << "\n";
#endif

       // outputFile << "  // Function prototype for building IR node (encapsulates required un-automatable details). \n";
       // outputFile << string("     ") + node.getName() + "* build_" + node.getName() + "_from_children(" + constructorParametersString + "); \n";
       // outputFile << string("     ") + node.getName() + "* " + buildNodeFunctionName + "(" + constructorParametersString + "); \n";
       // outputFile << string("     ") + node.getName() + "* " + buildNodeFunctionName + "(" + defaultConstructorParametersString + "); \n";

          outputFile << "\n";
          outputFile << "  // Call to Aterm API function to match aterm against predefined pattern. \n";
          outputFile << "     if (ATmatch(term, \"" << atermPatternString << "\", " << atermArgumentsSubstring << "))\n        {\n";

#if 0
       // Allow for special cases as needed.
          if (node.getName() == "SgForStatement")
             {
            // handledAsSpecialCase = true;

               outputFile << "       // SPECIAL CASE: node : " << node.getName() << "\n";
               outputFile << "\
#if 1 \n\
          printf (\"Identified SgForStatement: For(<term>, <term>, <term>, <term>, <term>) \"); \n\
#endif \n\
          SgForInitStatement* forInit = isSgForInitStatement(generate_AST(term1)); \n\
          ROSE_ASSERT(forInit != NULL); \n\
          SgStatement* test = isSgStatement(generate_AST(term2)); \n\
          ROSE_ASSERT(test != NULL); \n\
          SgExpression* increment = isSgExpression(generate_AST(term3)); \n\
          ROSE_ASSERT(increment != NULL); \n\
          SgStatement* body = isSgStatement(generate_AST(term4)); \n\
          ROSE_ASSERT(body != NULL); \n\
\n\
          result = new SgForStatement(forInit, test, increment, body); \n\n";
             }
#endif

          if (isContainer == false)
             {
               outputFile << "       // handle the case of: is NOT a container \n";
               outputFile << dataMemberString;
               outputFile << "\n";

            // This is the call to the function to build the IR node using the children already constructed (bottom up).
            // outputFile << string("          returnNode = new ") + node.getName() + "(" + constructorArgumentsString + "); \n";
            // outputFile << string("          returnNode = build_") + node.getName() + "_from_children(" + constructorArgumentsString + "); \n";
               outputFile << string("          returnNode = ") + buildNodeFunctionName + "(" + constructorArgumentsString + "); \n";
             }
            else
             {
            // Note that scopes (and other IR nodes having containers of IR nodes have to be constructed top-down 
            // (so this complexity need to be encoded into this code to generate the code to construct the ROSE AST).
               outputFile << "       // handle the case of: IS a container \n";
               outputFile << "\n";
               outputFile << dataMemberString;
               outputFile << "\n";
            // outputFile << string("          ") + node.getName() + "* local_returnNode = build_" + node.getName() + "_from_children(" + constructorArgumentsString + "); \n";
               outputFile << string("          ") + node.getName() + "* local_returnNode = " + buildNodeFunctionName + "(" + constructorArgumentsString + "); \n";
               outputFile << "\n";
            // outputFile << dataMemberString;
               outputFile << dataMemberString_post;
               outputFile << "\n";
               outputFile << "          returnNode = local_returnNode;\n";
             }
#endif
          outputFile << "\n";
          outputFile << "       // Skip to the end of the function \n";
          outputFile << "          goto done;\n";

       // Add vertical space to format the generated file better.
          outputFile << "        }\n\n";
        }
       else
        {
       // We don't need an empty function because we are not supporting a virtual function implementation 
       // (because we don't know what ROSE IR node will be read).  Alternatively we could call a long list 
       // of functions looking for which one will return a valid ROSE IR node.
        }

  // Add vertical space to format the generated file better.
  // outputFile << "\n";

#if 0
  // We only want to annotate ATerms that we will actually generate.
     if (availableAtermPatternString == true && availableAtermAnnotationString == true)
        {
       // Make this a virtual function (so we don't need to pass in a ROSE IR node).
       // outputFile << "void " << node.getName() << "::generate_ATerm_Annotation(ATerm & term, " << node.getName() << "* n) \n   {\n";
          outputFile << "void " << node.getName() << "::read_ATerm_Annotation(ATerm & term)\n   {\n";

       // outputFile << "  // atermAnnotationString = " << atermAnnotationString << "\n";
          outputFile << atermAnnotationString << "\n";

       // outputFile << "  // end of function: " << node.getName() << "::generate_ATerm_attributes() \n";
          outputFile << "   } \n\n";
        }
       else
        {
       // Output an empty function because we have had to unconditially add virtual function to 
       // every ROSE IR node and we require a defining function declaration.
          outputFile << "void " << node.getName() << "::read_ATerm_Annotation(ATerm & term)\n   {\n   }\n";
        }
#endif
#endif

       // End of block for handling when(handledAsSpecialCase == false).
        }
     
  // Add vertical space to format the generated file better.
  // outputFile << "\n\n";

  // Traverse all nodes of the grammar recursively and build the tree traversal function for each of them.
     vector<Terminal*>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin(); treeNodeIterator != node.subclasses.end(); treeNodeIterator++ )
        {
          ROSE_ASSERT((*treeNodeIterator) != NULL);
          ROSE_ASSERT((*treeNodeIterator)->getBaseClass() != NULL);
          buildAtermConsumerSupportFunctions(**treeNodeIterator, outputFile);
        }
   }


void
Grammar::buildDataMember(Terminal & node, GrammarString* grammarString, bool & firstConstructorParameter, bool & lastDataMemberWasConstructorParameter, bool & isInConstructorParameterList, string & constructorArgumentsString, string & dataMemberString )
   {
  // Refactored code to support building default values date members for marked as constructor initializers.

     string typenameString = grammarString->getTypeNameString();
     Terminal::TypeEvaluation typeKind = node.evaluateType(typenameString);

     if (firstConstructorParameter == false && isInConstructorParameterList == true)
        {
          constructorArgumentsString  += ", ";
       // constructorParametersString += ", ";
        }

  // The function we build to translate the child data members to a proper IR node required all of the constructor parameters (even if they are not traversed).
     if (isInConstructorParameterList == true)
        {
          dataMemberString += "       // This needs to be supported because it is a constructor parameter. \n";
          dataMemberString += string("       // node = ") + node.getName() + " toBeTraversed == false: data member type: "
                           + grammarString->getTypeNameString() + " data member variable: " 
                           + grammarString->getVariableNameString() + " typeKind = " 
                           + StringUtility::numberToString((int)typeKind) + "\n";

       // string defaultInitializerString = grammarString->getDefaultInitializerString();
          string attributeName = grammarString->getVariableNameString();
          string defaultInitializerString = string("= getAtermStringAttribute(term,\"") + attributeName + "\")";

       // Implement initalization of enum using cast (alternatively the default value is available in ROSETTA).
       // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = \"\"; \n";
       // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = " + grammarString->getTypeNameString() + "(0); \n";
       // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " " + defaultInitializerString + "; \n";
          dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " " + defaultInitializerString + "; \n";

          constructorArgumentsString += "local_" + grammarString->getVariableNameString();

          firstConstructorParameter = false;
          lastDataMemberWasConstructorParameter = true;

       // We want to build code that looks like this:
       //      ATerm idannot = ATgetAnnotation(term, ATmake("id"));
       //      if (idannot)
       //         {
       // #if 1
       //           printf ("Found an annotation \n");
       // #endif
       //           char* unique_id = NULL;
       //           if (ATmatch(idannot, "<str>", &unique_id))
       //              {
       //                printf ("unique_id = %s \n",unique_id);
       //              }
       //         }

#if 0
          dataMemberString += "     ATerm idannot = ATgetAnnotation(term, ATmake(" + grammarString->getVariableNameString() + "));\n";
          dataMemberString += "     if (idannot)\n";
          dataMemberString += "        {\n";
          dataMemberString += "#if 1\n";
          dataMemberString += "          printf (\"Found an annotation \\n\");\n";
          dataMemberString += "#endif\n";
          dataMemberString += "        \n";
          dataMemberString += "          char* unique_id = NULL;\n";
          dataMemberString += "          if (ATmatch(idannot, \"<str>\", &unique_id))\n";
          dataMemberString += "             {\n";
          dataMemberString += "               printf (\"unique_id = %s \\n\",unique_id);\n";
          dataMemberString += "             }\n";
          dataMemberString += "        \n";
          dataMemberString += "        }\n";
#endif


        }
       else 
        {
       // lastDataMemberWasConstructorParameter = false;
        }
   }


void
Grammar::buildAtermSupportFunctions(Terminal & node, StringUtility::FileWithLineNumbers & outputFile)
   {
  // This function calls the two seperate function to write and read the aterm.

     outputFile << "\n";
     outputFile << "// Conditional compilation of ATerm support in ROSE (see ROSE configure options).\n";
     outputFile << "#ifdef ROSE_USE_ROSE_ATERM_SUPPORT\n\n";

#if 1
  // Call the function to build the support to generate ATerms from the AST.
     buildAtermGenerationSupportFunctions(node, outputFile);
#endif

  // For the case of reading the Aterms to build the AST, we have to build a single function and put all of the cases into this function.
  // outputFile << "SgNode* " << node.getName() << "::generate_AST(ATerm & term) \n   {\n";
  // outputFile << "SgNode* SgNode::generate_AST(ATerm & term)\n   {\n     SgNode* returnNode = NULL;\n";
     outputFile << "SgNode* AtermSupport::generate_AST(ATerm & term)\n   {\n";
     outputFile << "     SgNode* returnNode = NULL;\n\n";
     outputFile << "     ATerm term1;\n";
     outputFile << "     ATerm term2;\n";
     outputFile << "     ATerm term3;\n";
     outputFile << "     ATerm term4;\n";
     outputFile << "     ATerm term5;\n";
     outputFile << "     ATerm term6;\n";
     outputFile << "     ATerm term7;\n";
     outputFile << "     ATerm term8;\n";
     outputFile << "     ATerm term9;\n";
     outputFile << "     ATerm term10;\n";
     outputFile << "     ATerm term11;\n";
     outputFile << "     ATerm term12;\n";
     outputFile << "     ATerm term13;\n";
     outputFile << "     ATerm term14;\n";
     outputFile << "     ATerm term15;\n";
     outputFile << "     ATerm term16;\n";
     outputFile << "     ATerm term17;\n";
     outputFile << "     ATerm term18;\n";
     outputFile << "     string unrecognizedAterm;\n";
  // outputFile << "\n";

#if 1
  // Call the function to build the support to read the ATerms and generate the AST.
     buildAtermConsumerSupportFunctions(node, outputFile);
#endif

     outputFile << "  // It is an error if the ATerm has not been translated at this point! \n";
  // outputFile << "     printf (\"ERROR: aterm not translated: aterm = %s \n\", );\n";
     outputFile << "     unrecognizedAterm = ATwriteToString(term);\n";
     outputFile << "     printf (\"\\n\\nERROR: Unknown Aterm \\n\");\n";
     outputFile << "     printf (\"ERROR: unrecognizedAterm = %s \\n\",unrecognizedAterm.c_str());\n";
     outputFile << "     printf (\"DIAGNOSTIC: aterm_type_name = %s \\n\",aterm_type_name(term).c_str());\n";
     outputFile << "     ROSE_ASSERT (false);\n";
     outputFile << "\n";

     outputFile << "done:\n";
     outputFile << "\n";
     outputFile << "  /* wrap up */ \n";
     outputFile << "\n";
     outputFile << "     return returnNode; \n   } \n\n";

     outputFile << "// endif for ROSE_USE_ROSE_ATERM_SUPPORT\n";
     outputFile << "#endif\n";

#if 0
     printf ("Exiting as a test in Grammar::buildAtermSupportFunctions() \n");
     ROSE_ASSERT(false);
#endif
   }

#if 0
string
Terminal::buildAtermSupport ()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     string s ;
  // Maybe, this could be handled nicer by using a macro implementation ... however, I put it here!
     s += "     " + classNameString + "* source = (" + classNameString +"*) (pointer); \n" ;
     s += "#if FILE_IO_EXTRA_CHECK \n" ;
     s += "     assert ( source != NULL ) ; \n";
     s += "     assert ( source->p_freepointer != NULL) ; \n";
     s += "#endif \n" ;

     for (Terminal *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               GrammarString *data = *stringListIterator;
               string varNameString = string(data->getVariableNameString());
               string varTypeString = string(data->getTypeNameString());
               string varStorageNameString = "storageOf_" + varNameString;
               string sg_string  = varTypeString;
               if (varNameString != "freepointer"  &&  varTypeString.substr(0,7) !=  "static " )
                  {
                    switch (evaluateType(varTypeString) )
                       {
                         case SGCLASS_POINTER:
                              s += "     " + varStorageNameString + " = AST_FILE_IO::getGlobalIndexFromSgClassPointer ( source->p_" + varNameString + " );\n" ;
                              break;

                         case ASTATTRIBUTEMECHANISM:
                         case ATTACHEDPREPROCESSINGINFOTYPE:
                         case BIT_VECTOR:
                         case CHAR_POINTER:
                         case CONST_CHAR_POINTER:
                         case ROSEATTRUBUTESLISTCONTAINER:
                         case STL_CONTAINER:
                         case STL_SET:
                      // DQ (10/4/2006): Added case of STL_MAP
                         case STL_MAP:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STRING:
                              s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                              break;
                         case MODIFIERCLASS:
                         case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                         case SGNAME:
                              s += "     " + varStorageNameString + ".pickOutIRNodeData( &(source->p_" + varNameString + ") );\n" ;
                              break;
                         case BASIC_DATA_TYPE:
                         case ENUM_TYPE:
                              s += "     " + varStorageNameString +" =  source->p_" + varNameString + " ;\n";
                              break;
                         case OSTREAM:
                         case SKIP_TYPE:
                         case TO_HANDLE:
                              break;

                         default:
                              std::cout << " There is a class not handled in buildAtermSupport.C, Line " << __LINE__ << endl ;
                              std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                              assert (!"Stop immediately, since variable to build support is not found ... " ) ;
                              break;
                       }
                  }
             }
        }
     return s;
   }
#endif


