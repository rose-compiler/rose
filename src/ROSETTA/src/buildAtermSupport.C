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
Grammar::buildAtermSupportFunctions(Terminal & node, StringUtility::FileWithLineNumbers & outputFile)
   {
  // string successorContainerName="traversalSuccessorContainer";

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
                 // This should be added as an aterm annotation.
                     outputFile << "// node = " << node.getName() << " toBeTraversed == false: data member type: " 
                                << grammarString->getTypeNameString() << " data member variable: " 
                                << grammarString->getVariableNameString() << " typeKind = " 
                                << StringUtility::numberToString((int)typeKind) << "\n";

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
          buildAtermSupportFunctions(**treeNodeIterator, outputFile);
        }
   }


void
Grammar::generateAtermSupport(GrammarString* gs, StringUtility::FileWithLineNumbers & outputFile)
   {
     outputFile << "// data member type: " << gs->getTypeNameString() << " data member variable: " << gs->getVariableNameString() << "\n";

  // Terminal::TypeEvaluation typeKind = Terminal::evaluateType(gs->getTypeNameString());

  // outputFile << "// data member prototype: " << gs->getDataPrototypeString() << "\n";
  // outputFile << "// function name: " << gs->getFunctionNameString() << "\n\n";
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


