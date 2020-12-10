// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "AstNodeClass.h"
#include "grammarString.h"
#include <sstream>

// This generates debugging code in the generation of the aterms from the AST.
#define GENERATE_ATERM_DEBUG_CODE 0
#define GENERATE_AST_DEBUG_CODE 1

// This generated comments in the generated code
#define GENERATE_ATERM_COMMENTS 0
#define GENERATE_AST_COMMENTS 0

using namespace std;
using namespace Rose;

void
Grammar::buildAtermGenerationSupportFunctions(AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile)
   {
     vector<GrammarString*> includeList = Grammar::classMemberIncludeList(node);

  // AstNodeClass::TypeEvaluation typeKind = AstNodeClass::evaluateType(std::string& varTypeString);

  // printf ("node = %s includeList = %zu \n",node.getName().c_str(),includeList.size());

  // bool outputAterm = false;
     bool outputAterm = isAstObject(node);

     bool firstAterm      = true;
     bool firstAnnotation = true;

     bool isContainer     = false;

     string atermPatternSubstring = "";
     string atermAnnotationString = "";

#if 0
     printf ("In Grammar::buildAtermSupportFunctions(): node.getName() = %s \n",node.getName().c_str());
#endif

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
  // Allow for special cases as needed to add a annotations for specific data members or computed values we want to save.
     if (node.getName() == "SgFunctionDeclaration")
        {
       // handledAsSpecialCase = true;

          string variableName = "return_type";
          string variableAccessFunctionName = "type()->get_return_type";
          atermAnnotationString += string("  // SPECIAL CASE: node : ") + node.getName() + "\n";
          atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + variableName + "\"),ATmake(\"<str>\", uniqueId(this->get_" + variableAccessFunctionName + "()).c_str())); \n";

          variableName = "current_scope";
          variableAccessFunctionName = "scope";
          atermAnnotationString += string("  // SPECIAL CASE: node : ") + node.getName() + "\n";
          atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + variableName + "\"),ATmake(\"<str>\", uniqueId(this->get_" + variableAccessFunctionName + "()).c_str())); \n";

          atermAnnotationString += "\n";
        }
#endif

#if 1
     if (handledAsSpecialCase == false)
        {
     for (vector<GrammarString*>::iterator i = includeList.begin(); i != includeList.end(); i++)
        {
          GrammarString* grammarString = *i;
          ROSE_ASSERT(grammarString != NULL);

       // We need a AstNodeClass so that we can call the evaluateType() function (this is a bit awkward).
       // AstNodeClass::TypeEvaluation typeKind = node.evaluateType(grammarString->getTypeNameString());
          string typenameString = grammarString->getTypeNameString();
          AstNodeClass::TypeEvaluation typeKind = node.evaluateType(typenameString);

       // outputFile << "// data member variable: " << grammarString->getVariableNameString() << "\n";
       // generateAtermSupport(grammarString,outputFile);
#if 0
          if (outputAterm == true)
             {
               outputFile << "// data member type: " << grammarString->getTypeNameString() << " data member variable: " << grammarString->getVariableNameString() << "typeKind = " << StringUtility::numberToString((int)typeKind) << "\n";
             }
#endif
#if GENERATE_ATERM_COMMENTS
          outputFile << "// data member type (generate ATERM): " << grammarString->getTypeNameString() << " data member variable: " << grammarString->getVariableNameString() 
                     << " typeKind = " << StringUtility::numberToString((int)typeKind) << " enum kind = " << node.typeEvaluationName(typeKind) << "\n";
#endif
       // printf ("   --- typenameString = %s typeKind = %d \n",typenameString.c_str(),typeKind);

          TraversalEnum toBeTraversed = grammarString->getToBeTraversed();

#if 1
          if (typeKind == AstNodeClass::SGCLASS_POINTER)
             {
            // This should generate a sequence of aterms.
               if (toBeTraversed == DEF_TRAVERSAL)
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
#if 0
                 // This should be added as an aterm annotation.
                     outputFile << "// node = " << node.getName() << " toBeTraversed == NO_TRAVERSAL: data member type: " 
                                << grammarString->getTypeNameString() << " data member variable: " 
                                << grammarString->getVariableNameString() << " typeKind = " 
                                << StringUtility::numberToString((int)typeKind) << "\n";
#endif
                 // atermAnnotationString += string("  // ") + typenameString + "\n";

                    bool atermAnnotationStringProcessed = false;
#if 1
                 // This is where we can support the source file position information.
                    if (typenameString == "Sg_File_Info*")
                       {
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),convertFileInfoToAterm(this->get_" + grammarString->getVariableNameString() + "())); \n";
                         atermAnnotationStringProcessed = true;
                       }
#endif
                 // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
                    if (atermAnnotationStringProcessed == false && grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
                       {
                      // Note that the type traversal will include the static members, so we need to include it in our pattern for the match to work.
                      // if (typenameString.substr(0,7) != "static " && typenameString.substr(0,11) != "$CLASSNAME*")
                         if (typenameString.substr(0,11) != "$CLASSNAME*")
                            {
                           // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this->get_" + grammarString->getVariableNameString() + "()).c_str())); \n";
                              bool isType = false;
#if 0
                              atermAnnotationString += "     // type is: " + grammarString->getTypeNameString() + "\n";
#endif
                           // isType = (grammarString->getTypeNameString() == "SgPointerType");
                              isType = (grammarString->getTypeNameString() == "SgType*");

                           // Where this is a type, it must be unwound to the base type.
                              if (isType == true)
                                 {
                                // <appl(<list>)>
                                // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<term>\", getTraversalChildrenAsAterm(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                                // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<appl(<term>)>\"," + grammarString->getTypeNameString() + ",getTraversalChildrenAsAterm(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                                // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<appl(<term>)>\"," + grammarString->getTypeNameString() + ",getTraversalChildrenAsAterm(this))); \n";
                                // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<appl(<term>)>\"," + grammarString->getVariableNameString() + ",getTraversalChildrenAsAterm(this))); \n";
                                // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),this->get_" + grammarString->getVariableNameString() + "()->generate_ATerm()); \n";

                                // We need to call the AtermSupport::convertNodeToAterm(SgNode* n) function so that both generate_ATerm() and generate_ATerm_Annotation() functions will be called.
                                   atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),convertNodeToAterm(this->get_" + grammarString->getVariableNameString() + "())); \n";
                                 }
                                else
                                 {
                                   atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this->get_" + grammarString->getVariableNameString() + "()).c_str())); \n";
                                 }
                            }
                           else
                            {
#if 0
                              printf ("NOTE: Skipping output of ATerm annotation for grammarString->getVariableNameString() = %s (ends with $CLASSNAME*) \n",grammarString->getVariableNameString().c_str());
#endif
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
          if (typeKind == AstNodeClass::SGCLASS_POINTER_VECTOR)
             {
            // This should generate a list of aterms.
               printf ("   --- typeKind == SGCLASS_POINTER_VECTOR typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
            // printf ("Found typeKind == SGCLASS_POINTER_VECTOR \n");
             }
#endif
          if (typeKind == AstNodeClass::SGCLASS_POINTER_LIST)
             {
               if (toBeTraversed == DEF_TRAVERSAL)
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
                 // atermPatternSubstring += "[<list>]";

                    isContainer = true;
                    firstAterm = false;
                  }
             }
#if 0
       // This is not used for containers of SGCLASS_POINTER.
          if (typeKind == AstNodeClass::SGCLASS_POINTER_VECTOR_NAMED_LIST)
             {
            // This should generate a list of aterms.
               printf ("   --- typeKind == SGCLASS_POINTER_VECTOR_NAMED_LIST typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
            // printf ("Found typeKind == SGCLASS_POINTER_VECTOR_NAMED_LIST \n");
             }
#endif

       // Ignore these initially.
          if (typeKind == AstNodeClass::BASIC_DATA_TYPE)
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
               if (grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
                  {
                 // string atermTypeName = "ATmake";
                 // These are the easy types to support using an ATerm Int
                 // if (typenameString == "bool" || typenameString == "int" || typenameString == "unsigned int")
                    bool isInteger = isIntegerKind(typenameString);
                    if (isInteger == true)
                       {
                      // atermTypeName = "ATmakeInt";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "())); \n";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\"),ATmakeInt(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<int>\",(int)(this->get_" + grammarString->getVariableNameString() + "()))); \n";
                       }
                      else
                       {
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this).c_str())); \n";
                      // atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this->get_" + grammarString->getVariableNameString() + "()).c_str())); \n";
                         bool processDataMember = true;
                         processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgFunctionModifier::opencl_work_group_size_t");
                         processDataMember = processDataMember && (grammarString->getTypeNameString() != "Rose::BinaryAnalysis::RegisterDescriptor");
                         processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgAsmNERelocEntry::iref_type");
                         processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgAsmNERelocEntry::iord_type");
                         processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgAsmNERelocEntry::iname_type");
                         processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgAsmNERelocEntry::osfixup_type");

                         if (processDataMember == true)
                            {
                              atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),ATmake(\"<str>\", uniqueId(this->get_" + grammarString->getVariableNameString() + "()).c_str())); \n";
                            }
                           else
                            {
                              atermAnnotationString += "     // skipping some data members of specific unsupported structure types \n";
                            }
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
          if (typeKind == AstNodeClass::SGNAME)
             {
            // This will generate an Aterm annotation to store the value of the associated basic data type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
            // printf ("Found typeKind == SGNAME \n");
#endif
            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
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
          if (typeKind == AstNodeClass::ENUM_TYPE)
             {
            // This will generate an Aterm annotation to store the value of the associated enum type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
               printf ("Found typeKind == ENUM_TYPE \n");
#endif
            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
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

       // outputFile << "  // atermPatternSubstring = " << atermPatternSubstring << "\n";
          string atermPatternString = node.getName() + "(" + atermPatternSubstring + ")";
#if 1
          outputFile << "  // expected read pattern: atermPatternString = " << atermPatternString << "\n";
#endif
#if GENERATE_ATERM_DEBUG_CODE
       // This generates debugging code in the generation of the aterms from the AST.
          outputFile << string("     printf (\"In ") + atermPatternString + "\\n\");\n";
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
       // outputFile << "     ATerm term = ATmake(\"" << atermPatternString << "\",getTraversalChildrenAsAterm(this));\n";
       // term = ATmake("<appl(<list>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));
       // outputFile << "     ATerm term = ATmake(\"<appl(<list>)>\", \"" << node.getName() << "\",getTraversalChildrenAsAterm(this));\n";
#if 1
          outputFile << "  // isContainer = " << ((isContainer == true) ? "true" : "false") << "\n";
#endif
          if (isContainer == false)
             {
#if 1
               outputFile << "     ATerm term = ATmake(\"<appl(<list>)>\", \"" << node.getName() << "\",getTraversalChildrenAsAterm(this));\n";
#else
               bool isType = false;
               atermAnnotationString += "     // node is: " + node.getName() + "\n";
            // isType = (grammarString->getTypeNameString() == "SgPointerType");
            // isType = (node.getName() == "SgTypeInt");

            // Where this is a type, it must be unwound to the base type.
               if (isType == true)
                  {
                    outputFile << "     ATerm term = ATmake(\"<appl()>\", \"" << node.getName() << "\");\n";
                  }
                 else
                  {
                    outputFile << "     ATerm term = ATmake(\"<appl(<list>)>\", \"" << node.getName() << "\",getTraversalChildrenAsAterm(this));\n";
                  }
#endif
             }
            else
             {
            // Case of when we have a container (since containers can have more that 255 elements which is a limit in the ATerm library).
            // term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(n));
#if 0
               outputFile << "     ATerm term = ATmake(\"<appl(<term>)>\", \"" << node.getName() << "\",getTraversalChildrenAsAterm(this));\n";
#else
               if (node.getName() == "SgVariableDeclaration")
                  {
                 // term = ATmake("VarDecl(<term>)", convertSgNodeRangeToAterm(variableDeclaration->get_variables().begin(),variableDeclaration->get_variables().end()));
                 // outputFile << "     ATerm term = ATmake(\"<appl(<term>, <list>)>\", \"" << node.getName() << "\",getTraversalChildrenAsAterm(this));\n";
                 // outputFile << "     ATerm term = ATmake(\"<appl(<term>)>\", \"" << node.getName() << "\",convertSgNodeRangeToAterm(variableDeclaration->get_variables().begin(),variableDeclaration->get_variables().end()));\n";

                 // outputFile << "     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(this);\n";
                 // outputFile << "     ROSE_ASSERT(variableDeclaration != NULL);\n";

                    outputFile << "     ATerm term = ATmake(\"<appl(<term>, <list>)>\", \"" << node.getName() << "\",convertNodeToAterm(this->get_baseTypeDefiningDeclaration()),convertSgNodeRangeToAterm(this->get_variables().begin(),this->get_variables().end()));\n";
                  }
                 else
                  {
                    outputFile << "     ATerm term = ATmake(\"<appl(<term>)>\", \"" << node.getName() << "\",getTraversalChildrenAsAterm(this));\n";
                  }
#endif
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
       // outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n     ATerm term = ATmake(\"NULL\");\n     return term;\n   }\n";
          outputFile << "ATerm " << node.getName() << "::generate_ATerm() \n   {\n     ATerm term = ATmake(\"" + node.getName() + "\");\n     return term;\n   }\n";
        }

  // Add vertical space to format the generated file better.
     outputFile << "\n";

  // We only want to annotate ATerms that we will actually generate.
  // if (availableAtermPatternString == true && availableAtermAnnotationString == true)
     if (availableAtermAnnotationString == true)
        {
       // Make this a virtual function (so we don't need to pass in a ROSE IR node).
       // outputFile << "void " << node.getName() << "::generate_ATerm_Annotation(ATerm & term, " << node.getName() << "* n) \n   {\n";
          outputFile << "void " << node.getName() << "::generate_ATerm_Annotation(ATerm & term)\n   {\n";

#if GENERATE_ATERM_DEBUG_CODE
       // This generates debugging code in the generation of the aterms from the AST.
          outputFile << string("     printf (\"In ") + node.getName() + "::generate_ATerm_Annotation(ATerm & term)\\n\");\n";
#endif
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
     vector<AstNodeClass*>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin(); treeNodeIterator != node.subclasses.end(); treeNodeIterator++ )
        {
          ROSE_ASSERT((*treeNodeIterator) != NULL);
          ROSE_ASSERT((*treeNodeIterator)->getBaseClass() != NULL);
          buildAtermGenerationSupportFunctions(**treeNodeIterator, outputFile);
        }
   }



void
Grammar::buildAtermConsumerSupportFunctions(AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile)
   {
  // This function generates the code to convert the ATERM --> AST.

     vector<GrammarString*> includeList = Grammar::classMemberIncludeList(node);

  // AstNodeClass::TypeEvaluation typeKind = AstNodeClass::evaluateType(std::string& varTypeString);
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
#if GENERATE_AST_COMMENTS
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
  // Special case handling in Grammar::buildAtermConsumerSupportFunctions().
  // This shows how to support adding specific data (as annotations) to the aterms and reading it from the aterm.
     if (node.getName() == "SgFunctionDeclaration")
        {
       // handledAsSpecialCase = true;

       // Handle the case of the return type (which is not a data member in SgFunctionDeclaration.
          string attributeName     = "return_type";
          string typenameBase      = "SgType";
          string typenameString    = "SgType*";
          string initializerString = "getAtermTypeNodeAttribute(term,\"" + attributeName + "\")";

          dataMemberString += string("       // SPECIAL CASE: ") + node.getName() + "\n";
          dataMemberString += "          " + typenameString + " local_" + attributeName + " = is" + typenameBase + "(" + initializerString + ");\n";
          dataMemberString += "          ROSE_ASSERT(local_" + attributeName + " != NULL);\n";

       // Another case: scope
          attributeName     = "current_scope";
          typenameBase      = "SgScopeStatement";
          typenameString    = "SgScopeStatement*";
          initializerString = "getAtermScopeNodeAttribute(term,\"" + attributeName + "\")";

          dataMemberString += string("       // SPECIAL CASE: ") + node.getName() + "\n";
          dataMemberString += "          " + typenameString + " local_" + attributeName + " = is" + typenameBase + "(" + initializerString + ");\n";
          dataMemberString += "          ROSE_ASSERT(local_" + attributeName + " != NULL);\n";
          dataMemberString += "\n";
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

#if GENERATE_AST_COMMENTS
     outputFile << "  // STARTING processing of loop over all data members for node = " + node.getName() + "\n";
#endif

     for (vector<GrammarString*>::iterator i = includeList.begin(); i != includeList.end(); i++)
        {
          GrammarString* grammarString = *i;
          ROSE_ASSERT(grammarString != NULL);

       // We need a AstNodeClass so that we can call the evaluateType() function (this is a bit awkward).
       // AstNodeClass::TypeEvaluation typeKind = node.evaluateType(grammarString->getTypeNameString());
          string typenameString = grammarString->getTypeNameString();
          AstNodeClass::TypeEvaluation typeKind = node.evaluateType(typenameString);

       // outputFile << "// data member variable: " << grammarString->getVariableNameString() << "\n";
       // generateAtermSupport(grammarString,outputFile);
#if 0
          if (outputAterm == true)
             {
               outputFile << "// data member type: " << grammarString->getTypeNameString() << " data member variable: " << grammarString->getVariableNameString() << " typeKind = " << StringUtility::numberToString((int)typeKind) << "\n";
             }
#endif
#if GENERATE_AST_COMMENTS
          outputFile << "// data member type (generate AST): " << grammarString->getTypeNameString() << " data member variable: " << grammarString->getVariableNameString() 
                     << " typeKind = " << StringUtility::numberToString((int)typeKind) << " enum kind = " << node.typeEvaluationName(typeKind) << "\n";
#endif
       // printf ("   --- typenameString = %s typeKind = %d \n",typenameString.c_str(),typeKind);

          TraversalEnum toBeTraversed = grammarString->getToBeTraversed();

          // MS 11/12/2015: uses new function
          // isInConstructorParameterList to allow to properly check
          // on boolean return value
          bool isInConstructorParameterList = grammarString->isInConstructorParameterList();

#if 0
          outputFile << "// firstConstructorParameter             = " << (firstConstructorParameter             ? "true" : "false") << "\n";
          outputFile << "// lastDataMemberWasConstructorParameter = " << (lastDataMemberWasConstructorParameter ? "true" : "false") << "\n";
          outputFile << "// isInConstructorParameterList          = " << (isInConstructorParameterList          ? "true" : "false") << "\n";
#endif

#if 1
          if (typeKind == AstNodeClass::SGCLASS_POINTER)
             {
            // This should generate a sequence of aterms.
               if (toBeTraversed == DEF_TRAVERSAL)
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

                    atermArgumentsSubstring += integer_counter_string;

                    size_t typenameStringSize = grammarString->getTypeNameString().size();
                    string typenameBase       = grammarString->getTypeNameString().substr(0,typenameStringSize-1);
                    
                    dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = is" + typenameBase + "(generate_AST(term" + integer_counter_string + ")); \n";

                 // Note that the returned value can be NULL (e.g. the initialized for translation of an aterm to a SgInitializedName).
                 // dataMemberString += "          ROSE_ASSERT(local_" + grammarString->getVariableNameString() + " != NULL); \n";

                 // We have to use this variable with a set access function for the data member after the IR node has been built.
                    dataMemberString_post += string("       // ##### traversed data member (SGCLASS_POINTER): put in use of local_") + grammarString->getVariableNameString() + "\n";
                    dataMemberString_post +=        "          local_returnNode->set_" + grammarString->getVariableNameString() + "(local_" + grammarString->getVariableNameString() + ");\n";

                    if (isInConstructorParameterList == true)
                       {
                      // This variable should be empty when we have a container.
                         constructorArgumentsString += "local_" + grammarString->getVariableNameString();
                      // atermArgumentsSubstring += integer_counter_string;

                         constructorParametersString += grammarString->getTypeNameString() + " " + "local_" + grammarString->getVariableNameString();
                         lastDataMemberWasConstructorParameter = true;

                         firstConstructorParameter = false;
                       }
                      else 
                       {
                      // lastDataMemberWasConstructorParameter = false;
#if 0
                      // dataMemberString += " // Is not a constructor parameter \n";
                         dataMemberString += string("// Is not a constructor parameter: data member type: ") + grammarString->getTypeNameString() 
                                          + " data member variable: " + grammarString->getVariableNameString() + " typeKind = " 
                                          + StringUtility::numberToString((int)typeKind) + "\n";
#endif
                       }
                         
                 // Increment the counter used to build the substring that will be a part of the function argument names for terms (e.g. term1, term2, term3, etc.).
                    integer_counter++;

                    firstAterm = false;
                  }
                 else
                  {
                 // This should be added as an aterm annotation.
#if 0
                     outputFile << "// node = " << node.getName() << " toBeTraversed == NO_TRAVERSAL: data member type: " 
                                << grammarString->getTypeNameString() << " data member variable: " 
                                << grammarString->getVariableNameString() << " typeKind = " 
                                << StringUtility::numberToString((int)typeKind) << "\n";
#endif
                 // atermAnnotationString += string("  // ") + typenameString + "\n";

                     buildDataMember(node,grammarString,firstAterm,firstConstructorParameter,lastDataMemberWasConstructorParameter,isInConstructorParameterList,constructorArgumentsString,atermArgumentsSubstring,atermPatternSubstring,dataMemberString,dataMemberString_post,integer_counter);

#if 0
                    bool atermAnnotationStringProcessed = false;

                 // This is where we can support the source file information.
                    if (typenameString == "Sg_File_Info*")
                       {
                         atermAnnotationString += "     term = ATsetAnnotation(term, ATmake(\"" + grammarString->getVariableNameString() + "\"),convertFileInfoToAterm(this->get_" + grammarString->getVariableNameString() + "())); \n";
                         atermAnnotationStringProcessed = true;
                       }

                 // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
                    if (atermAnnotationStringProcessed == false && grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
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

          if (typeKind == AstNodeClass::SGCLASS_POINTER_LIST)
             {
               if (toBeTraversed == DEF_TRAVERSAL)
                  {
                 // This should generate a list of aterms.
#if 0
                    printf ("   --- typeKind == SGCLASS_POINTER_LIST typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
#endif
                 // printf ("Found typeKind == SGCLASS_POINTER_LIST \n");

                 // This is the rule we will use to build Aterms for this case.
                 // term = ATmake("<appl(<term>)>",getShortVariantName((VariantT)(n->variantT())).c_str(),getTraversalChildrenAsAterm(this));

                 // DQ (10/9/2014): Bug: handling of atermArgumentsSubstring
                    buildDataMember(node,grammarString,firstAterm,firstConstructorParameter,lastDataMemberWasConstructorParameter,isInConstructorParameterList,constructorArgumentsString,atermArgumentsSubstring,atermPatternSubstring,dataMemberString,dataMemberString_post,integer_counter);
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
          if (typeKind == AstNodeClass::SGCLASS_POINTER_VECTOR_NAMED_LIST)
             {
            // This should generate a list of aterms.
               printf ("   --- typeKind == SGCLASS_POINTER_VECTOR_NAMED_LIST typenameString = %s typeKind = %d name = %s \n",typenameString.c_str(),typeKind,grammarString->getVariableNameString().c_str());
            // printf ("Found typeKind == SGCLASS_POINTER_VECTOR_NAMED_LIST \n");
             }
#endif

       // Ignore these initially.
          if (typeKind == AstNodeClass::BASIC_DATA_TYPE)
             {
            // This will generate an Aterm annotation to store the value of the associated basic data type.
#if 0
               printf ("   --- typenameString = %s typeKind = %d \n",typenameString.c_str(),typeKind);
               printf ("Found typeKind == BASIC_DATA_TYPE \n");
#endif
            // This is the code most similar that we want to generate.
            // term = ATsetAnnotation(term, ATmake("id"),ATmake("<str>", uniqueId(this).c_str()));

            // atermAnnotationString += string("  // ") + typenameString + "\n";

               buildDataMember(node,grammarString,firstAterm,firstConstructorParameter,lastDataMemberWasConstructorParameter,isInConstructorParameterList,
                  constructorArgumentsString,atermArgumentsSubstring,atermPatternSubstring,dataMemberString,dataMemberString_post,integer_counter);

            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
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
          if (typeKind == AstNodeClass::SGNAME)
             {
            // This will generate an Aterm annotation to store the value of the associated basic data type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
            // printf ("Found typeKind == SGNAME \n");
#endif

               buildDataMember(node,grammarString,firstAterm,firstConstructorParameter,lastDataMemberWasConstructorParameter,isInConstructorParameterList,
                  constructorArgumentsString,atermArgumentsSubstring,atermPatternSubstring,dataMemberString,dataMemberString_post,integer_counter);

            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
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
          if (typeKind == AstNodeClass::ENUM_TYPE)
             {
            // This will generate an Aterm annotation to store the value of the associated enum type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
               printf ("Found typeKind == ENUM_TYPE \n");
#endif
               buildDataMember(node,grammarString,firstAterm,firstConstructorParameter,lastDataMemberWasConstructorParameter,isInConstructorParameterList,constructorArgumentsString,atermArgumentsSubstring,atermPatternSubstring,dataMemberString,dataMemberString_post,integer_counter);

            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
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
          if (typeKind == AstNodeClass::STRING)
             {
            // This will generate an Aterm annotation to store the value of the associated enum type.
#if 0
               printf ("   --- typenameString = %s grammarString->getVariableNameString() = %s typeKind = %d \n",typenameString.c_str(),grammarString->getVariableNameString().c_str(),typeKind);
               printf ("Found typeKind == STRING \n");
#endif

               buildDataMember(node,grammarString,firstAterm,firstConstructorParameter,lastDataMemberWasConstructorParameter,isInConstructorParameterList,constructorArgumentsString,atermArgumentsSubstring,atermPatternSubstring,dataMemberString,dataMemberString_post,integer_counter);
             }
#if 0
          outputFile << "  // at end of loop over data member = " + grammarString->getVariableNameString() + " for node = " + node.getName() + "\n";
          outputFile << "  //    --- dataMemberString.size()      = " << StringUtility::numberToString(dataMemberString.size()) << "\n";
          outputFile << "  //    --- dataMemberString_post.size() = " << StringUtility::numberToString(dataMemberString_post.size()) << "\n";
#endif
        }

#if GENERATE_AST_COMMENTS
     outputFile << "  // Finished processing loop over all data members for node = " + node.getName() + "\n";
#endif

     bool availableAtermPatternString    = (outputAterm == true) && (firstAterm == false);
     bool availableAtermAnnotationString = (outputAterm == true) && (firstAnnotation == false);

#if 0
     outputFile << string("  // availableAtermPatternString    = ") + (availableAtermPatternString    ? "true" : "false") + "\n";
     outputFile << string("  // availableAtermAnnotationString = ") + (availableAtermAnnotationString ? "true" : "false") + "\n";
#endif

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

#if 0
     outputFile << string("  // availableAtermPatternString = ") + (availableAtermPatternString ? "true" : "false") + "\n";
#endif

  // if (availableAtermPatternString == true)
     if (availableAtermPatternString == true || availableAtermAnnotationString == true)
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
          outputFile << string("  // atermPatternString = ") + atermPatternString + "\n";

       // outputFile << "     if (ATmatch(term, \"" << atermPatternString << "\", " << atermArgumentsSubstring << "))\n        {\n";
          if (node.getName() == "SgVariableDeclaration")
             {
            // Note special case handling.

            // if (ATmatch(term, "SgVariableDeclaration(<term>, [<list>])", &term1, &term2))
            // outputFile << "     if (ATmatch(term, \"" << atermPatternString << "\", " << atermArgumentsSubstring << "))\n        {\n";
            // outputFile << "     if (ATmatch(term, \"" << "SgVariableDeclaration(<term>, <term>)\",&term1,&term2))\n        {\n";
            // outputFile << "     if (ATmatch(term, \"" << "SgVariableDeclaration(<term>, [<list>])\",&term1,&term2))\n        {\n";
               outputFile << "     if (ATmatch(term, \"" << "SgVariableDeclaration(<term>, <term>)\",&term1,&term2))\n        {\n";
             }
            else
             {
            // We need to handle the case where there are no child aterms (but there are aterm annotations).
            // outputFile << "     if (ATmatch(term, \"" << atermPatternString << "\", " << atermArgumentsSubstring << "))\n        {\n";
               if (atermArgumentsSubstring == "")
                  {
                 // I think we can enforce that this is true.
                    ROSE_ASSERT(availableAtermPatternString == false);

                    atermPatternString = node.getName();
                    outputFile << "     if (ATmatch(term, \"" << atermPatternString << "\"))\n        {\n";
                  }
                 else
                  {
                    outputFile << "     if (ATmatch(term, \"" << atermPatternString << "\", " << atermArgumentsSubstring << "))\n        {\n";
                  }
             }
          
#if GENERATE_AST_DEBUG_CODE
          outputFile << string("          printf (\"In ") + atermPatternString + "\\n\");\n";
#endif
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

          if (node.getBaseClass() != NULL)
             {
               outputFile << "       // node.getBaseClass()->getName() = " + node.getBaseClass()->getName() + "\n";
             }

          if (isContainer == false)
             {
               outputFile << "       // handle the case of: is NOT a container \n";
               outputFile << dataMemberString;
               outputFile << "\n";

            // This is the call to the function to build the IR node using the children already constructed (bottom up).
            // outputFile << string("          returnNode = new ") + node.getName() + "(" + constructorArgumentsString + "); \n";
            // outputFile << string("          returnNode = build_") + node.getName() + "_from_children(" + constructorArgumentsString + "); \n";
            // outputFile << string("          returnNode = ") + buildNodeFunctionName + "(" + constructorArgumentsString + "); \n";

            // outputFile << string("          ") + node.getName() + "* local_returnNode = " + buildNodeFunctionName + "(" + constructorArgumentsString + "); \n";
               if (node.getName() == "SgFunctionDeclaration")
                  {
                 // Add extra data to the function were we will connect this to the SageBuilder API.
                    string added_data_members = "local_return_type, local_parameterList, local_current_scope, ";
                    outputFile << string("          ") + node.getName() + "* local_returnNode = /* SPECIAL CASE */ " + buildNodeFunctionName + "(" + added_data_members + constructorArgumentsString + "); \n";
                  }
                 else
                  {
                    outputFile << string("          ") + node.getName() + "* local_returnNode = " + buildNodeFunctionName + "(" + constructorArgumentsString + "); \n";
                  }

               outputFile << "          ROSE_ASSERT(local_returnNode != NULL); \n";

#if 1
               outputFile << "\n";
            // outputFile << "   // Output of dataMemberString_post: \n";
               outputFile << dataMemberString_post;
            // outputFile << "\n";
               outputFile << "          returnNode = local_returnNode;\n";
#endif
             }
            else
             {
            // Note that scopes (and other IR nodes having containers of IR nodes have to be constructed top-down 
            // (so this complexity need to be encoded into this code to generate the code to construct the ROSE AST).
               outputFile << "       // handle the case of: IS a container \n";
            // outputFile << "\n";
               outputFile << dataMemberString;
               outputFile << "\n";
            // outputFile << string("          ") + node.getName() + "* local_returnNode = build_" + node.getName() + "_from_children(" + constructorArgumentsString + "); \n";
               outputFile << string("          ") + node.getName() + "* local_returnNode = " + buildNodeFunctionName + "(" + constructorArgumentsString + "); \n";
               outputFile << "          ROSE_ASSERT(local_returnNode != NULL); \n";
               outputFile << "\n";
            // outputFile << dataMemberString;
               outputFile << dataMemberString_post;
               outputFile << "\n";
               outputFile << "          returnNode = local_returnNode;\n";
             }
#endif
#if GENERATE_AST_DEBUG_CODE
          outputFile << string("          printf (\"Leaving ") + atermPatternString + "\\n\");\n";
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
  // if (availableAtermPatternString == true && availableAtermAnnotationString == true)
     if (availableAtermAnnotationString == true)
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
     vector<AstNodeClass*>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin(); treeNodeIterator != node.subclasses.end(); treeNodeIterator++ )
        {
          ROSE_ASSERT((*treeNodeIterator) != NULL);
          ROSE_ASSERT((*treeNodeIterator)->getBaseClass() != NULL);
          buildAtermConsumerSupportFunctions(**treeNodeIterator, outputFile);
        }
   }


void
Grammar::buildDataMember(AstNodeClass & node, GrammarString* grammarString, bool & firstAterm,
     bool & firstConstructorParameter, bool & lastDataMemberWasConstructorParameter, bool & isInConstructorParameterList, 
     string & constructorArgumentsString, string & atermArgumentsSubstring, string & atermPatternSubstring,
     string & dataMemberString, string & dataMemberString_post, int integer_counter)
   {
  // Refactored code to support building default values date members for marked as constructor initializers.

     string typenameString             = grammarString->getTypeNameString();
     AstNodeClass::TypeEvaluation typeKind = node.evaluateType(typenameString);

     TraversalEnum toBeTraversed = grammarString->getToBeTraversed();

     string atermArgumentsName     = "term";
     string integer_counter_string = StringUtility::numberToString(integer_counter);

     if (firstConstructorParameter == false && isInConstructorParameterList == true)
        {
          constructorArgumentsString  += ", ";
        }

     if (toBeTraversed == DEF_TRAVERSAL)
        {
#if 0
          dataMemberString += string("       // firstConstructorParameter    = ") + (firstConstructorParameter    ? "true" : "false") + "\n";
          dataMemberString += string("       // isInConstructorParameterList = ") + (isInConstructorParameterList ? "true" : "false") + "\n";
#endif
       // AstNodeClass::SGCLASS_POINTER_LIST types are never in the constructor parameter list.
       // if (firstConstructorParameter == false && isInConstructorParameterList == true)
       // if (firstConstructorParameter == false && ( (isInConstructorParameterList == true) || (typeKind == AstNodeClass::SGCLASS_POINTER_LIST) ) )
          if (firstAterm == false)
             {
               atermArgumentsSubstring  += ", ";
             }

       // atermPatternSubstring        += "<list>";
          atermArgumentsSubstring      += "&" + atermArgumentsName;
          atermArgumentsSubstring      += integer_counter_string;
        }
       else
        {
        }

  // The function we build to translate the child data members to a proper IR node required all of the constructor parameters (even if they are not traversed).
     if (isInConstructorParameterList == true)
        {
#if 0
          dataMemberString += "       // This needs to be supported because it is a constructor parameter. \n";
          dataMemberString += string("       // node = ") + node.getName() + " toBeTraversed == NO_TRAVERSAL: data member type: "
                           + grammarString->getTypeNameString() + " data member variable: " 
                           + grammarString->getVariableNameString() + " typeKind = " 
                           + StringUtility::numberToString((int)typeKind) + "\n";
#endif
       // string defaultInitializerString = grammarString->getDefaultInitializerString();
          string attributeName = grammarString->getVariableNameString();
       // string defaultInitializerString = string("= getAtermStringAttribute(term,\"") + attributeName + "\")";
       // string initializerString = string("= getAtermStringAttribute(term,\"") + attributeName + "\")";
          string initializerString;

          AstNodeClass::TypeEvaluation typeKind = node.evaluateType(typenameString);
          switch(typeKind)
             {
               case AstNodeClass::SGCLASS_POINTER:
                  {
                 // initializerString = string("= getAtermNodeNameAttribute(term,\"") + attributeName + "\")";
                 // initializerString = string("= getAtermStringAttribute(term,\"") + attributeName + "\")";
                 // initializerString = string("= ") + typenameString + "(getAtermStringAttribute(term,\"" + attributeName + "\"))";

                    size_t typenameStringSize = grammarString->getTypeNameString().size();
                    string typenameBase       = grammarString->getTypeNameString().substr(0,typenameStringSize-1);
                 // initializerString = string("= ") + "is" + typenameBase + "(getAtermNodeAttribute(term,\"" + attributeName + "\"))";
                    if (typenameBase == "Sg_File_Info")
                       {
                         initializerString = string("= getAtermFileInfo(term,\"") + attributeName + "\")";
                       }
                      else
                       {
                         if (grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
                            {
                           // initializerString = string("= ") + "is" + typenameBase + "(getAtermNodeAttribute(term,\"" + attributeName + "\"))";
                           // if (node.isDerivedFrom("SgType") == true)
                              if (grammarString->getTypeNameString() == "SgType*")
                                 {
                                   initializerString = string("= ") + "is" + typenameBase + "(getAtermTypeNodeAttribute(term,\"" + attributeName + "\"))";
                                 }
                                else
                                 {
                                   if (grammarString->getTypeNameString() == "SgScopeStatement*")
                                      {
                                        initializerString = string("= ") + "is" + typenameBase + "(getAtermScopeNodeAttribute(term,\"" + attributeName + "\"))";
                                      }
                                     else
                                      {
                                        initializerString = string("= ") + "is" + typenameBase + "(getAtermNodeAttribute(term,\"" + attributeName + "\"))";
                                      }
                                 }
                            }
                           else
                            {
                           // Since data member without access functions cannot be accessed to build the aterm annotations, we can't exoect to read them.
                           // We still have to provide them as inputs since they are constructor parameters, but likely they are relics from a time long ago.
                           // For example, the "expression_type" in many expressions is from when we used to store the extression in all SgExpression IR nodes.
                           // This was mostly removed for where it is not required since we design ROSE to be a mutable AST, and thus it is computed on expressions
                           // instead of explicitly stored (so it can always be correct during transformations).
                              dataMemberString += "       // Since a data member without access functions cannot be accessed to build the aterm annotations, we can't exoect to read them. \n";
                              initializerString = "= NULL"; 
                            }
                       }
                    break;
                  }

               case AstNodeClass::STRING:
               case AstNodeClass::SGNAME:
                    initializerString = string("= getAtermStringAttribute(term,\"") + attributeName + "\")";
                    break;

            // This case included bools and integer values.
               case AstNodeClass::BASIC_DATA_TYPE:
                  {
                    initializerString = string("= getAtermIntegerAttribute(term,\"") + attributeName + "\")";

                 // if (typenameString == "bool" || typenameString == "int" || typenameString == "unsigned int")
                    bool isInteger = isIntegerKind(typenameString);
                    if (isInteger == true)
                       {
                      // We want to support all kinds of integers here.
                         initializerString = string("= getAtermIntegerAttribute(term,\"") + attributeName + "\")";
                       }
                      else
                       {
                      // I think this is to support char*
                         initializerString = string("= getAtermStringAttribute(term,\"") + attributeName + "\")";
                       }
                    break;
                  }

               case AstNodeClass::ENUM_TYPE:
                 // initializerString = string("= getAtermEnumAttribute(term,\"") + attributeName + "\")";
                    initializerString = string("= ") + typenameString + "(getAtermIntegerAttribute(term,\"" + attributeName + "\"))";
                    break;

               default:
                  {
                    printf ("ERROR: PART 1: default reached in generation of code for ATerm support: typeKind = %d \n",typeKind);
                    ROSE_ASSERT(false);
                  }
             }

       // Implement initalization of enum using cast (alternatively the default value is available in ROSETTA).
       // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = \"\"; \n";
       // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " = " + grammarString->getTypeNameString() + "(0); \n";
       // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " " + defaultInitializerString + "; \n";
       // dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " " + initializerString + "; \n";

       // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
          if (grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
             {
               bool processDataMember = true;
               processDataMember = processDataMember && (grammarString->getTypeNameString() != "Rose::BinaryAnalysis::RegisterDescriptor");

               if (processDataMember == true)
                  {
                    dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " " + initializerString + "; \n";

                    dataMemberString_post += string("       // ##### isInConstructorParameterList == true : put in use of local_") + grammarString->getVariableNameString() + "\n";                    
                    dataMemberString_post +=        "          local_returnNode->set_" + grammarString->getVariableNameString() + "(local_" + attributeName + ");\n";
                  }
                 else
                  {
                    dataMemberString += string("       // ##### isInConstructorParameterList == true : skipping some unsupported structure type data members: local_") + grammarString->getVariableNameString() + "\n";

                 // Output the variable uninitalized (as a structure it is initialized using its default constructor.
                    dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + "; \n";
                  }
             }
            else
             {
               dataMemberString += "          " + grammarString->getTypeNameString() + " local_" + grammarString->getVariableNameString() + " " + initializerString + "; \n";

               dataMemberString_post += string("       // ##### isInConstructorParameterList == true : no access functions defined for local_") + grammarString->getVariableNameString() + "\n";
             }

          constructorArgumentsString += "local_" + grammarString->getVariableNameString();

          firstConstructorParameter = false;
          lastDataMemberWasConstructorParameter = true;
        }
       else 
        {
       // lastDataMemberWasConstructorParameter = false;

          if (typeKind == AstNodeClass::SGCLASS_POINTER_LIST)
             {
               if (firstAterm == false)
                  {
                    atermPatternSubstring += ", ";
                  }

            // atermPatternSubstring += "<list>";
               atermPatternSubstring += "[<list>]";

               string typenameBase = grammarString->getTypeNameString();

#if 0
               outputFile << "// node = " << node.getName() << " toBeTraversed == NO_TRAVERSAL: data member type: " 
                          << grammarString->getTypeNameString() << " data member variable: " 
                          << grammarString->getVariableNameString() << " typeKind = " 
                          << StringUtility::numberToString((int)typeKind) << "\n";
#endif
#if 0
               dataMemberString_post += string("       // node = ") + node.getName() + " toBeTraversed == NO_TRAVERSAL: data member type: "
                                      + grammarString->getTypeNameString() + " data member variable: " 
                                      + grammarString->getVariableNameString() + " typeKind = " 
                                      + StringUtility::numberToString((int)typeKind) + "\n";
#endif

               string containerElementTypeString        = grammarString->containerElementTypeString(node);
               string containerAppendFunctionNameString = grammarString->containerAppendFunctionNameString(node);
#if 0
               dataMemberString_post += string("       // containerElementTypeString        = ") + containerElementTypeString + "\n";
               dataMemberString_post += string("       // containerAppendFunctionNameString = ") + containerAppendFunctionNameString + "\n";
#endif
            // In the case of a list of ROSE IR nodes we need to build the elements of the list seperately in a loop.
               dataMemberString_post += "          vector<ATerm> terms = getAtermList(" + atermArgumentsName + integer_counter_string + ");\n";
               dataMemberString_post += "          printf (\"vector<ATerm> terms.size() = %zu \\n\",terms.size()); \n";
               dataMemberString_post += "          for (size_t i = 0; i < terms.size(); ++i)\n";
               dataMemberString_post += "             {\n";
#if 1
               dataMemberString_post += "#if 1 \n";
               dataMemberString_post += "               printf (\"In container IR node: loop: i = %zu \\n\",i);\n";
               dataMemberString_post += "#endif \n";
#endif
               dataMemberString_post += "               " + containerElementTypeString + "* child = is" + containerElementTypeString + "(generate_AST(terms[i]));\n";
               dataMemberString_post += "               ROSE_ASSERT(child != NULL);\n";
               dataMemberString_post += "               local_returnNode->" + containerAppendFunctionNameString + "(child);\n";
               dataMemberString_post += "             }\n";
            // dataMemberString_post += "\n";
             }
            else
             {
            // Get the annotations and call the access function on the ROSE IR node to set the data members.
            // dataMemberString_post += "       // Get the annotations and call the access function on the ROSE IR node to set the data members\n";
#if 1
               dataMemberString_post += "       // Get annotation and set data member: node = " + node.getName() + " toBeTraversed == NO_TRAVERSAL: data member type: "
                     + grammarString->getTypeNameString() + " data member variable: "
                     + grammarString->getVariableNameString() + " typeKind = "
                     + StringUtility::numberToString((int)typeKind) + "\n";
#endif
               string attributeName = grammarString->getVariableNameString();
               string initializerString;
               AstNodeClass::TypeEvaluation typeKind = node.evaluateType(typenameString);

               string typenameBase = typenameString;

               switch(typeKind)
                  {
                    case AstNodeClass::SGCLASS_POINTER:
                       {
                         size_t typenameStringSize = grammarString->getTypeNameString().size();

                      // Reset the typenameBase to exclude the '*'.
                         typenameBase      = grammarString->getTypeNameString().substr(0,typenameStringSize-1);
                      // initializerString = "getAtermNodeAttribute(term,\"" + attributeName + "\")";
                         if (typenameBase == "Sg_File_Info")
                            {
                              initializerString = "getAtermFileInfo(term,\"" + attributeName + "\")";
                            }
                           else
                            {
                           // initializerString = "getAtermNodeAttribute(term,\"" + attributeName + "\")";
                              if (grammarString->getTypeNameString() == "SgType*")
                                 {
                                   initializerString = "getAtermTypeNodeAttribute(term,\"" + attributeName + "\")";
                                 }
                                else
                                 {
                                   if (grammarString->getTypeNameString() == "SgScopeStatement*")
                                      {
                                        initializerString = "getAtermScopeNodeAttribute(term,\"" + attributeName + "\")";
                                      }
                                     else
                                      {
                                        initializerString = "getAtermNodeAttribute(term,\"" + attributeName + "\")";
                                      }
                                 }
                            }
                         break;
                       }

                    case AstNodeClass::STRING:
                    case AstNodeClass::SGNAME:
                         initializerString = string("getAtermStringAttribute(term,\"") + attributeName + "\")";
                         break;

                 // This case included bools and integer values.
                    case AstNodeClass::BASIC_DATA_TYPE:
                         initializerString = string("getAtermIntegerAttribute(term,\"") + attributeName + "\")";
                         break;

                    case AstNodeClass::ENUM_TYPE:
                         initializerString = string("getAtermIntegerAttribute(term,\"") + attributeName + "\")";
                         break;

                    default:
                       {
                         printf ("ERROR: PART 2: default reached in generation of code for ATerm support: typeKind = %d \n",typeKind);
                         ROSE_ASSERT(false);
                       }
                  }

            // Don't generate ATerm annotations for data members that don't have ROSE IR node access functions.
               if (grammarString->automaticGenerationOfDataAccessFunctions != NO_ACCESS_FUNCTIONS)
                  {
                 // Exclude the freepointer variable since it is part of the ROSE memory management and not intented to be set explicitly.
                 // It might be equivalent to filter on those data member that have access functions.  Also exclude some data members
                 // specific to unsupported types (these are mostly associated with the binary analysis, not a target for initial aterm work).

                    bool processDataMember = true;
                    processDataMember = processDataMember && (grammarString->getVariableNameString() != "freepointer");
                    processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgFunctionModifier::opencl_work_group_size_t");
                    processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgAsmNERelocEntry::iref_type");
                    processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgAsmNERelocEntry::iord_type");
                    processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgAsmNERelocEntry::iname_type");
                    processDataMember = processDataMember && (grammarString->getTypeNameString() != "SgAsmNERelocEntry::osfixup_type");

                    if (processDataMember == true)
                       {
                      // dataMemberString_post += "          local_returnNode->set_" + grammarString->getVariableNameString() + "(" + initializerString + ");\n";
                         if (typeKind == AstNodeClass::SGCLASS_POINTER)
                            {
                              dataMemberString_post += "          " + typenameString + " local_" + attributeName + " = is" + typenameBase + "(" + initializerString + ");\n";
                            }
                           else
                            {
                              dataMemberString_post += "          " + typenameString + " local_" + attributeName + " = (" + typenameBase + ") " + initializerString + ";\n";
                            }
                         dataMemberString_post += "          local_returnNode->set_" + grammarString->getVariableNameString() + "(local_" + attributeName + ");\n";

                         dataMemberString_post += "\n";
                       }
                      else
                       {
                         dataMemberString_post += "       // Skipping output for freepointer (data member specific to ROSE memory management)  and some specific structure types in IR nodes \n";
                       }
                  }
                 else
                  {
                    dataMemberString_post += "       // Skipping output for data members without generated access functions (e.g. freepointer, data member specific to ROSE memory management) \n";
                  }
             }
        }
   }


bool Grammar::isIntegerKind(const string & typenameString)
   {
      bool isInteger = false;
      if (typenameString == "bool"          || typenameString == "int"   || typenameString == "unsigned int" || 
          typenameString == "unsigned"      || typenameString == "short" || typenameString == "short int"    || 
          typenameString == "unsigned long" || typenameString == "long"  || typenameString == "size_t"       ||
          typenameString == "unsigned long int" ||  typenameString == "unsigned short" || typenameString == "long int"  || 
          typenameString == "long long int"  || typenameString == "long long unsigned int"  ||
          typenameString == "unsigned long long int"  ||
          typenameString == "uint64_t"      || typenameString == "rose_addr_t")
         {
           isInteger = true;
         }

   // Also save an char as an int as well (better ATerm type than using a string).
      if (typenameString == "char" || typenameString == "unsigned char"  )
         {
           isInteger = true;
         }

   // For the moment (to support initial debugging) save floating point values as an integer as well.
      if (typenameString == "float" || typenameString == "double" || typenameString == "long double"  )
         {
           isInteger = true;
         }

      return isInteger;
   }

void
Grammar::buildAtermSupportFunctions(AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile)
   {
  // This function calls the two separate function to write and read the aterm.

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
     outputFile << "     ATerm term19;\n";
     outputFile << "     ATerm term20;\n";
     outputFile << "     ATerm term21;\n";
     outputFile << "     ATerm term22;\n";
     outputFile << "     string unrecognizedAterm;\n";
  // outputFile << "\n";

     outputFile << "\n";
  // outputFile << "     if (ATmatch(term, \"NULL\") || ATmatch(term, "\\"NULL\\"")) \n";
     outputFile << "     if (ATmatch(term, \"NULL\")) \n";
     outputFile << "        { \n";
     outputFile << "#if 1 \n";
     outputFile << "          printf (\"Found NULL pointer! goto done \\n\");\n";
     outputFile << "#endif \n";
     outputFile << "          returnNode = NULL; \n";
     outputFile << "          goto done; \n";
     outputFile << "        } \n";
     outputFile << "\n";

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
AstNodeClass::buildAtermSupport ()
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

     for (AstNodeClass *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
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


