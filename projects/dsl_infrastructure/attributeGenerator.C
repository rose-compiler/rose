// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.

// 1) We need a mechanism to specify the DSL parts.
//    A pragma could be used for that (in the header files).  We can ignore this initally.
// 2) We need to generate code to be used with the DSL comiler.
//    Some of this code needs to be generated and some will be templates fromn header files.


// Code is generated in generated_dsl_attributes.h and generated_dsl_attributes.C



#include "rose.h"

#include "attributeGenerator.h"

// This might be helpful, but not clear yet.
// #include "dslSupport.h"
// using namespace DSL_Support;

using namespace std;


// Inherited Attribute
AttributeGenerator_InheritedAttribute::AttributeGenerator_InheritedAttribute()
   {
   }

AttributeGenerator_InheritedAttribute::AttributeGenerator_InheritedAttribute(const AttributeGenerator_InheritedAttribute & X)
   {
  // Note that DSL nodes are only identified amonsts children of a specific parent and not anywhere in the AST.
  // But since the traveral function semantics use copy constructors to copy attributes, we still have to copy the DSLnodes set.
  // DSLnodes = X.DSLnodes;
   }


// Synthesized Attribute
AttributeGenerator_SynthesizedAttribute::AttributeGenerator_SynthesizedAttribute()
   {
   }

AttributeGenerator_SynthesizedAttribute::AttributeGenerator_SynthesizedAttribute(SgNode* ast)
   {
   }

AttributeGenerator_SynthesizedAttribute::AttributeGenerator_SynthesizedAttribute(const AttributeGenerator_SynthesizedAttribute & X)
   {
   }


// Attribute Generator Traversal
// AttributeGeneratorTraversal::AttributeGeneratorTraversal()
AttributeGeneratorTraversal::AttributeGeneratorTraversal( SgProject* project )
   {
  // DQ (3/4/2016): Turning on internal debugging.
     internal_debugging = false;

  // SgSourceFile* buildSourceFile(string,SgProject = NULL)
     generatedHeaderFile    = NULL;
     generatedSourceFile    = NULL;
     requiredSourceCodeFile = NULL;

     dsl_type_names_variable            = NULL;
     dsl_function_names_variable        = NULL;
     dsl_member_function_names_variable = NULL;
     dsl_attribute_map_variable         = NULL;

  // This has to be a *.C file since we are not supporting writing header files just yet (rename afterward).
     generatedHeaderFile = SageBuilder::buildSourceFile("generated_dsl_attributes_header.C");
     generatedSourceFile = SageBuilder::buildSourceFile("generated_dsl_attributes.C");

     global_scope_header = generatedHeaderFile->get_globalScope();
     ROSE_ASSERT(global_scope_header != NULL);
     global_scope_source = generatedSourceFile->get_globalScope();
     ROSE_ASSERT(global_scope_source != NULL);

     global_scope_requiredSourceCode = NULL;
     ROSE_ASSERT(global_scope_requiredSourceCode == NULL);

#if 0
  // Read in the file containing code that we want to use (e.g. templates).
  // build the file name of the required with the path (use the source directory as the path)
  // string requiredCodeFileNameWithPath = ROSE_AUTOMAKE_TOP_SRCDIR + "/projects/ShiftCalculus4/" + "requiredCode.C";
  // string requiredCodeFileNameWithPath = ROSE_AUTOMAKE_TOP_SRCDIR + "/projects/dsl_infrastructure/" + "array_header.C";
  // string requiredCodeFileNameWithPath = ROSE_AUTOMAKE_TOP_SRCDIR + "/projects/dsl_infrastructure/" + "generated_dsl_attributes_wrapper.C";
     string requiredCodeFileNameWithPath = ROSE_AUTOMAKE_TOP_SRCDIR + "/projects/dsl_infrastructure/" + "generated_dsl_attributes_wrapper.C";

     printf ("Using requiredCodeFileNameWithPath = %s \n",requiredCodeFileNameWithPath.c_str());

  // requiredSourceCodeFile = SageBuilder::buildSourceFile(requiredCodeFileNameWithPath);
     ROSE_ASSERT(requiredSourceCodeFile == NULL);

     bool frontendConstantFolding = false;
     std::vector<string> argList;

  // DQ (2/8/2016): This does not appear to help set the isCxx11_only flag in the SgSourceFile.
  // argList.push_back("-std=c++11");
     generatedSourceFile->set_Cxx11_only(true);

  // argList.push_back("-rose:verbose 2");
     argList.push_back("dummy_name_placeholder");
     argList.push_back("-rose:verbose 2");

  // Turn on the C++11 support.
     argList.push_back("-std=c++11");

  // This is not specified explicitly in the generated_dsl_attributes_wrapper.C file.
  // argList.push_back("-DSKIP_ROSE_HEADERS");
     argList.push_back("-DSKIP_ROSE_HEADERS");

     argList.push_back("-c");
     argList.push_back(requiredCodeFileNameWithPath);

     std::string commandline = StringUtility::listToString(argList);
     printf ("Compiling initial input file: %s \n",commandline.c_str());

  // SgProject* requiredSourceCodeProject = frontend(argc,argv,frontendConstantFolding);
     SgProject* requiredSourceCodeProject = frontend(argList,frontendConstantFolding);
     ROSE_ASSERT(requiredSourceCodeProject != NULL);
#else
  // DQ (3/6/2016): Use the input project instead of another project read seperately.
     SgProject* requiredSourceCodeProject = project;
     ROSE_ASSERT(requiredSourceCodeProject != NULL);
#endif

#if 0
     printf ("In AttributeGeneratorTraversal constructor: Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     ROSE_ASSERT(requiredSourceCodeProject->get_fileList_ptr() != NULL);
     ROSE_ASSERT(requiredSourceCodeProject->get_fileList_ptr()->get_listOfFiles().empty() == false);
  // requiredSourceCodeFile = requiredSourceCodeProject->get_fileList_ptr()[0]->get_globalscope();
  // requiredSourceCodeFile = requiredSourceCodeProject->get_file(0)->get_globalscope();
  // requiredSourceCodeFile = isSgSourceFile(requiredSourceCodeProject->[0]);
     requiredSourceCodeFile = isSgSourceFile(requiredSourceCodeProject->operator[](0));
     ROSE_ASSERT(requiredSourceCodeFile != NULL);

#if 0
  // DQ (3/6/2016): Clear the mangled name cache so that we can generate better mnagled names.
#if 0
     if (requiredSourceCodeFile->get_globalMangledNameMap().size() != 0)
        {
          requiredSourceCodeFile->clearGlobalMangledNameMap();
        }
#else
     requiredSourceCodeFile->clearGlobalMangledNameMap();
#endif
#endif

     global_scope_requiredSourceCode = requiredSourceCodeFile->get_globalScope();
     ROSE_ASSERT(global_scope_requiredSourceCode != NULL);

  // At this point we should have all of the dls attribute variables set to NULL.
     ROSE_ASSERT(dsl_type_names_variable            == NULL);
     ROSE_ASSERT(dsl_function_names_variable        == NULL);
     ROSE_ASSERT(dsl_member_function_names_variable == NULL);
     ROSE_ASSERT(dsl_attribute_map_variable         == NULL);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
AttributeGeneratorTraversal::unparseGeneratedCode()
   {
     printf ("In unparseGeneratedCode(): unparse the header file: generatedHeaderFile name = %s \n",generatedHeaderFile->getFileName().c_str());
     generatedHeaderFile->unparse();

     printf ("In unparseGeneratedCode(): unparse the source file: generatedHeaderFile name = %s \n",generatedSourceFile->getFileName().c_str());
     generatedSourceFile->unparse();

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void
AttributeGeneratorTraversal::processvariable(SgInitializedName* initializedName)
   {
     SgType* type = initializedName->get_type();
     ROSE_ASSERT(type != NULL);

     printf ("Found SgInitializedName: name = %s \n",initializedName->get_name().str());
     printf ("   --- type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
     printf ("   --- initializer = %p \n",initializedName->get_initializer());

  // Find the variable declaration
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initializedName->get_parent());
     ROSE_ASSERT(variableDeclaration != NULL);

  // Mark as a transformation and specify to be output in the source code.
     variableDeclaration->setTransformation();
     variableDeclaration->setOutputInCodeGeneration();

  // Don't use any associated comments in the original use of the variable declaration.
     variableDeclaration->set_attachedPreprocessingInfoPtr(NULL);

  // Mark as a transformation and specify to be output in the source code.
     initializedName->setTransformation();
     initializedName->setOutputInCodeGeneration();
#if 0
     variableDeclaration->get_file_info()->display("variableDeclaration");
#endif
     ROSE_ASSERT(global_scope_header != NULL);
     ROSE_ASSERT(global_scope_source != NULL);

     if (initializedName->get_initializer() == NULL)
        {
       // This is the variable declaration prototype.
          SageInterface::appendStatement(variableDeclaration,global_scope_header);
        }
       else
        {
       // This is the defining decalration with the initializer that we want to modify.
          SageInterface::appendStatement(variableDeclaration,global_scope_source);
        }
   }


AttributeGenerator_InheritedAttribute
AttributeGeneratorTraversal::evaluateInheritedAttribute   (SgNode* astNode, AttributeGenerator_InheritedAttribute inheritedAttribute )
   {
     ROSE_ASSERT(astNode != NULL);

     AttributeGenerator_InheritedAttribute return_inheritedAttribute;

#if 0
     printf ("In evaluateInheritedAttribute(): astNode = %p = %s: DSLnodes.size() = %zu \n",astNode,astNode->class_name().c_str(),DSLnodes.size());
#endif

#if 0
     if (internal_debugging == true)
        {
          printf ("In evaluateInheritedAttribute(): astNode = %p = %s: DSLnodes.size() = %zu \n",astNode,astNode->class_name().c_str(),DSLnodes.size());
        }
#endif

     switch (astNode->variantT())
        {
          case V_SgGlobal:
             {
            // Turn of the debug mode at the to of each global scope (required because we proces an AST with two source files).
               internal_debugging = false;
               break;
             }

          case V_SgPragmaDeclaration:
             {
            // Statements after a pragma will be detected and be used as DSL keywords.

               SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(astNode);
               ROSE_ASSERT(pragmaDeclaration != NULL);

               std::string pragmaString = pragmaDeclaration->get_pragma()->get_pragma();
#if 0
               printf ("Detected marking pragma for DSL keyword: pragmaDeclaration = %p = %s = %s \n",pragmaDeclaration,pragmaDeclaration->class_name().c_str(),pragmaString.c_str());
#endif
            // We only want to support pragmas that are specific to our DSL keyword markers.
            // DSLnodes.insert(DSLKeywordStatement);
               if (pragmaString == "DSL keyword")
                  {
#if 1
                    printf ("Detected marking pragma for DSL keyword: pragmaDeclaration = %p = %s = %s \n",pragmaDeclaration,pragmaDeclaration->class_name().c_str(),pragmaString.c_str());
#endif
                 // At this point we have see the extern declarations of these variables, but the 
                 // defining declarations (with initializers) appear later in the input source file.
                    ROSE_ASSERT(dsl_type_names_variable            != NULL);
                    ROSE_ASSERT(dsl_type_names_variable->get_initializer() == NULL);
                    ROSE_ASSERT(dsl_function_names_variable        != NULL);
                    ROSE_ASSERT(dsl_member_function_names_variable != NULL);
                    ROSE_ASSERT(dsl_attribute_map_variable         != NULL);

                 // There should always be a next statement after a pragma declaration (at least for where we use our DSL pragmas).
                    SgStatement* DSLKeywordStatement = SageInterface::getNextStatement(pragmaDeclaration);
                    if (DSLKeywordStatement == NULL)
                       {
                         printf ("Error: There should always be a next statement after a pragma declaration \n");
                       }
                    ROSE_ASSERT(DSLKeywordStatement != NULL);
#if 1
                    printf ("DSLKeywordStatement = %p = %s \n",DSLKeywordStatement,DSLKeywordStatement->class_name().c_str());
                    SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(DSLKeywordStatement);
                    if (declarationStatement != NULL)
                       {
                         printf ("   --- nondefining declaration = %p \n",declarationStatement->get_firstNondefiningDeclaration());
                         printf ("   --- defining declaration    = %p \n",declarationStatement->get_definingDeclaration());

                         if (declarationStatement->get_definingDeclaration() == NULL)
                            {
                              printf ("Error: no associated defining declaration found for statement marked as DSL keyword \n");
                            }
                         ROSE_ASSERT(declarationStatement->get_definingDeclaration() != NULL);

                         if (declarationStatement != declarationStatement->get_definingDeclaration())
                            {
                              printf ("Marking the defining declaration instead of the non-defining declaration with the DSL Keyword \n");
                              DSLKeywordStatement = declarationStatement->get_definingDeclaration();
                            }
                       }
#endif
                    DSLnodes.insert(DSLKeywordStatement);
#if 1
                 // Turn on internal debugging.
                    internal_debugging = true;
#endif
                  }

               break;
             }

          case V_SgClassDeclaration:
          case V_SgTemplateClassDeclaration:
             {
            // The case of a class declaration need not generate new code since the DSL_Attribute template can take the class type as a parameter.
            // But the DSL comiler has to know what classes to use as a basis for DSL keywords, so it might be simpler to just generate code for these cases.

               SgClassDeclaration* classDeclaration = isSgClassDeclaration(astNode);
               ROSE_ASSERT(classDeclaration != NULL);

               if (DSLnodes.find(classDeclaration) != DSLnodes.end())
                  {
                    printf ("Build DSL attribute for class = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
                    printf ("   --- nondefining declaration = %p \n",classDeclaration->get_firstNondefiningDeclaration());
                    printf ("   --- defining declaration    = %p \n",classDeclaration->get_definingDeclaration());

                    SgType* type = classDeclaration->get_type();
                    ROSE_ASSERT(type != NULL);

                 // Maybe we should be saving the SgClassDeclaration to be semetric with the case of SgFunctionDeclaration below?
                 // dsl_type_list.push_back(type);
#if 1
                    SgNode* ast_fragment = buildAttribute(type);
                    ROSE_ASSERT(ast_fragment == NULL);
                 // ROSE_ASSERT(ast_fragment != NULL);
#endif
                  }

               break;
             }

          case V_SgFunctionDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgTemplateFunctionDeclaration:
          case V_SgTemplateMemberFunctionDeclaration:
             {
            // This case is more complex since functions have arguments (using std::function support in C++11).

               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
               ROSE_ASSERT(functionDeclaration != NULL);
#if 1
               if (internal_debugging == true)
                  {
                    printf ("In evaluateInheritedAttribute(): case V_SgFunctionDeclaration: functionDeclaration = %s \n",functionDeclaration->get_name().str());
                  }
#endif
            // Check if this is a member function of a defined DSL specific class.
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
               SgClassDeclaration* classDeclaration = NULL;
               if (memberFunctionDeclaration != NULL)
                  {
                    SgScopeStatement* scope = memberFunctionDeclaration->get_scope();
                    ROSE_ASSERT(scope != NULL);
#if 0
                    printf ("memberFunctionDeclaration->get_scope() = %p = %s \n",scope,scope->class_name().c_str());
#endif
                    SgClassDefinition* classDefinition = isSgClassDefinition(memberFunctionDeclaration->get_scope());
                    if (memberFunctionDeclaration != NULL)
                       {
                         ROSE_ASSERT(classDefinition != NULL);
                         classDeclaration = classDefinition->get_declaration();
                         ROSE_ASSERT(classDeclaration != NULL);
                         classDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                         ROSE_ASSERT(classDeclaration != NULL);
#if 1
                         if (internal_debugging == true)
                            {
                              printf ("Member function defining class declaration = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
                            }
#endif
                       }
                  }

            // if (DSLnodes.find(functionDeclaration) != DSLnodes.end())
               if (classDeclaration != NULL && DSLnodes.find(classDeclaration) != DSLnodes.end())
                  {
                    printf ("Build DSL attribute for member function: class = %p = %s = %s function = %p = %s = %s \n",
                         classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str(),
                         functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());

                 // dsl_function_list.push_back(functionDeclaration);
#if 1
                    SgNode* ast_fragment = buildAttribute(functionDeclaration);
                    ROSE_ASSERT(ast_fragment == NULL);
                 // ROSE_ASSERT(ast_fragment != NULL);
#endif
                  }

               break;
             }

       // Find out variable declarations that we want to reproduce in the generated files (header and *.C source file).
          case V_SgInitializedName:
             {
            // Check is this is part of a variable declaration and with the correct name.
               SgInitializedName* initializedName = isSgInitializedName(astNode);
               ROSE_ASSERT(initializedName != NULL);

               SgType* type = initializedName->get_type();
               ROSE_ASSERT(type != NULL);
#if 0
               printf ("Found SgInitializedName: name = %s \n",initializedName->get_name().str());
               printf ("   --- type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
               if (initializedName->get_name() == "dsl_type_names")
                  {
                 // The last variable processed will be the one with the valid initializer (but it is not elegant to rely on this detail).
                    dsl_type_names_variable = initializedName;
#if 1
                    processvariable(initializedName);
#endif
                  }

               if (initializedName->get_name() == "dsl_function_names")
                  {
                 // The last variable processed will be the one with the valid initializer (but it is not elegant to rely on this detail).
                    dsl_function_names_variable = initializedName;
#if 1
                    processvariable(initializedName);
#endif
                  }

               if (initializedName->get_name() == "dsl_member_function_names")
                  {
                 // The last variable processed will be the one with the valid initializer (but it is not elegant to rely on this detail).
                    dsl_member_function_names_variable = initializedName;
#if 1
                    processvariable(initializedName);
#endif
                  }

               if (initializedName->get_name() == "dsl_attribute_map")
                  {
                 // The last variable processed will be the one with the valid initializer (but it is not elegant to rely on this detail).
                    dsl_attribute_map_variable = initializedName;
#if 1
                    processvariable(initializedName);
#endif
                  }

               break;
             }

          default:
             {
            // Cases not handled
#if 0
               printf ("AttributeGeneratorTraversal::evaluateInheritedAttribute: case not handled: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
             }
        }

     return return_inheritedAttribute;
   }


AttributeGenerator_SynthesizedAttribute
AttributeGeneratorTraversal::evaluateSynthesizedAttribute (SgNode* astNode, AttributeGenerator_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // We might not need the synthesized attribute evaluation (we will see).
  // AttributeGenerator_SynthesizedAttribute return_SynthesizedAttribute;

     SgNode* generated_ast = NULL;

     SgGlobal* inputFileGlobalScope = isSgGlobal(astNode);
     if (inputFileGlobalScope != NULL)
        {
       // Use this location in the traversal (after traversal of the global scope of the input file) to add #include directives, comments, etc.

          ROSE_ASSERT(global_scope_source != NULL);
          if (global_scope_source->get_declarations().empty() == false)
             {
            // Nothing to do.
#if 1
               modify_dsl_variable_initializers();
#else
               printf ("Skipping call to modify_dsl_variable_initializers() \n");
#endif
             }
            else
             {
            // Skip adding #include and comments to an empty file (thoough it should work).
               printf ("Generating an empty file: no extra processing required \n");
             }
        }

     switch (astNode->variantT())
        {
          case V_SgClassDeclaration:
          case V_SgTemplateClassDeclaration:
             {
            // The case of a class declaration need not generate new code since the DSL_Attribute template can take the class type as a parameter.
            // But the DSL comiler has to know what classes to use as a basis for DSL keywords, so it might be simpler to just generate code for these cases.

            // SgClassDeclaration* classDeclaration = isSgClassDeclaration(astNode);
               SgClassDeclaration* classDeclaration = isSgTemplateInstantiationDecl(astNode) == NULL ? isSgClassDeclaration(astNode) : NULL;
               if (isSgTemplateClassDeclaration(classDeclaration) != NULL)
                  {
                    classDeclaration = NULL;
                  }
            // ROSE_ASSERT(classDeclaration != NULL);
#if 0
               if (classDeclaration != NULL)
                  {
                    printf ("In evaluateSynthesizedAttribute(): finished processing of class = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
                    printf ("   --- nondefining declaration = %p \n",classDeclaration->get_firstNondefiningDeclaration());
                    printf ("   --- defining declaration    = %p \n",classDeclaration->get_definingDeclaration());
                  }
#endif
               if (DSLnodes.find(classDeclaration) != DSLnodes.end())
                  {
                    printf ("In evaluateSynthesizedAttribute() class = %p = %s = %s (turn internal debugging off) \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());

                    internal_debugging = false;
                  }
             }

          default:
             {
            // Cases not handled
#if 0
               printf ("AttributeGeneratorTraversal::evaluateSynthesizedAttribute: case not handled: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
             }
        }

  // return return_SynthesizedAttribute(generated_ast);
     return AttributeGenerator_SynthesizedAttribute (generated_ast);
   }

// Generate unique name for use as a class name for the generated attribute classes.
// std::string AttributeGeneratorTraversal::generateUniqueNameForUseAsIdentifier ( SgDeclarationStatement* declaration )
// std::string AttributeGeneratorTraversal::generateUniqueName ( SgDeclarationStatement* declaration )
std::string
AttributeGeneratorTraversal::generateUniqueName ( SgDeclarationStatement* declaration )
   {
  // DQ (3/21/2016): The support for unique name generation for use across translation 
  // units is not refactored into the SageInterface. 
  // string s = SageInterface::generateUniqueNameForUseAsIdentifier(declaration);
     string s;

     if (SageInterface::local_node_to_name_map.find(declaration) != SageInterface::local_node_to_name_map.end())
        {
          s = SageInterface::local_node_to_name_map[declaration];
        }
       else
        {
          SgDeclarationStatement* definingDeclaration = declaration->get_definingDeclaration();
          if (definingDeclaration != NULL)
             {
#if 1
               printf ("In generateUniqueName(): Using the defining declaration = %p since %p was not in the map \n",definingDeclaration,declaration);
#endif
               s = generateUniqueName(definingDeclaration);
             }
            else
             {
               printf ("Error: declaration not in SageInterface::local_node_to_name_map: declaration = %p = %s = %s \n",
                    declaration,declaration->class_name().c_str(),SageInterface::get_name(declaration).c_str());
               ROSE_ASSERT(false);
             }
        }

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return s;
   }


void
AttributeGeneratorTraversal::modify_dsl_variable_initializers()
   {
  // Fixup the dsl variable initializers to communicate what DSL abstractions have been identified (calsses, functions, member functions, etc.).

     printf ("Fixup the dsl variable initializers to communicate what DSL abstractions have been identified \n");
  // ROSE_ASSERT(false);

     printf ("In modify_dsl_variable_initializers(): dsl_type_list.size() = %zu \n",dsl_type_list.size());

  // There are four specific variables that hold information specific to the DSL target abstractions.
  // string initializerString;
     string dsl_type_names_initializerString;
     string dsl_function_names_initializerString;
     string dsl_member_function_names_initializerString;
     string dsl_attribute_map_initializerString;

     if (dsl_type_list.empty() == false)
        {
       // initializerString = " = { ";
          dsl_type_names_initializerString            = " = { ";
          dsl_function_names_initializerString        = " = { ";
          dsl_member_function_names_initializerString = " = { ";
          dsl_attribute_map_initializerString         = " = { ";
        }

     for (size_t i = 0; i < dsl_type_list.size(); i++)
        {
          SgType* type = dsl_type_list[i];
          ROSE_ASSERT(type != NULL);

          SgClassType* classType = isSgClassType(type);
          ROSE_ASSERT(classType != NULL);

          SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
          ROSE_ASSERT(classDeclaration != NULL);
#if 0
          string type_name = classDeclaration->get_name();

          printf ("Process saved DSL type i = %zu: type = %p = %s type_name = %s \n",i,type,type->class_name().c_str(),type_name.c_str());

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif

       // string scope = SageInterface::get_name(classDeclaration->get_scope());
          string scope = isSgGlobal(classDeclaration->get_scope()) == NULL ? SageInterface::get_name(classDeclaration->get_scope()) : "global";
          string className = string("scope_") + scope + "_type_name_" + type_name;
          string className_dsl_attribute = className + "_dsl_attribute";
#if 1
          printf ("classDeclaration->get_scope() = %p = %s scope = %s \n",classDeclaration->get_scope(),classDeclaration->get_scope()->class_name().c_str(),scope.c_str());
#endif
#else
          string className = generateUniqueName (classDeclaration);
          string className_dsl_attribute = className + "_dsl_attribute";
#endif

       // Add the attribute to the initializer of the associated dsl_attribute_map_variable variable.

       // initializerString += " { {className},{className_dsl_attribute()} } ";
       // dsl_type_names_initializerString += " { \"$className\" } ";
       // dsl_type_names_initializerString += " \"$className\" ";
          dsl_type_names_initializerString += " \"" + className + "\" ";
       // dsl_function_names_initializerString += " { \"functionName\"} ";
       // dsl_member_function_names_initializerString += " { {\"className\"}, {\"functionName\"} } ";
       // dsl_attribute_map_initializerString += " { {\"$className\"},{className_dsl_attribute()} } ";
       // dsl_type_names_initializerString    = StringUtility::copyEdit(dsl_type_names_initializerString,"$className",type_name);
       // dsl_attribute_map_initializerString = StringUtility::copyEdit(dsl_attribute_map_initializerString,"$className",type_name);

          dsl_attribute_map_initializerString += " { {\"" + className + "\"},{" + className_dsl_attribute + "()} } ";

          if (i < dsl_type_list.size()-1)
             {
            // initializerString += ",";
               dsl_type_names_initializerString += ",";
            // dsl_function_names_initializerString += ",";
            // dsl_member_function_names_initializerString += ",";
               dsl_attribute_map_initializerString += ",";
             }

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     if (dsl_type_list.empty() == false && dsl_function_list.empty() == false)
        {
       // initializerString += ",";
       // dsl_type_names_initializerString += ",";
          dsl_function_names_initializerString += ",";
       // dsl_member_function_names_initializerString += ",";
          dsl_attribute_map_initializerString += ",";
        }

     for (size_t i = 0; i < dsl_function_list.size(); i++)
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(dsl_function_list[i]);
          ROSE_ASSERT(functionDeclaration != NULL);

          string function_name = functionDeclaration->get_name();

          printf ("Process saved DSL function i = %zu: function_name = %s functionDeclaration = %p = %s \n",
               i,function_name.c_str(),functionDeclaration,functionDeclaration->class_name().c_str());

       // initializerString += " { {functionName},{functionName_dsl_attribute()} } ";
          dsl_function_names_initializerString += "{\"functionName\"}";
       // dsl_member_function_names_initializerString += "{\"memberFunctionName\"}";
          dsl_attribute_map_initializerString += " { {\"functionName\"},{functionName_dsl_attribute()} } ";
          if (i < dsl_function_list.size()-1)
             {
            // initializerString += ",";
            // dsl_type_names_initializerString += ",";
               dsl_function_names_initializerString += ",";
            // dsl_member_function_names_initializerString += ",";
               dsl_attribute_map_initializerString += ",";
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     printf ("dsl_type_list.empty()            = %s \n",dsl_type_list.empty()            ? "true" : "false");
     printf ("dsl_function_list.empty()        = %s \n",dsl_function_list.empty()        ? "true" : "false");
     printf ("dsl_member_function_list.empty() = %s \n",dsl_member_function_list.empty() ? "true" : "false");

     if ( (dsl_type_list.empty() == false || dsl_function_list.empty() == false) && (dsl_member_function_list.empty() == false) )
        {
       // Only add a "'" for the dsl_attribute_map_initializerString.
       // initializerString += ",";
       // dsl_type_names_initializerString += ",";
       // dsl_function_names_initializerString += ",";
       // dsl_member_function_names_initializerString += ",";
          dsl_attribute_map_initializerString += ",";
        }

     for (size_t i = 0; i < dsl_member_function_list.size(); i++)
        {
          SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(dsl_member_function_list[i]);
          ROSE_ASSERT(memberFunctionDeclaration != NULL);

          SgClassDeclaration* classDeclaration = isSgClassDeclaration(memberFunctionDeclaration->get_associatedClassDeclaration());
          ROSE_ASSERT(classDeclaration != NULL);

#if 0
          printf ("classDeclaration (for member function) = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());

       // We might at some point want the qualified name.
          string class_name           = classDeclaration->get_name();
       // string member_function_name = memberFunctionDeclaration->get_name();
          string original_name = memberFunctionDeclaration->get_name();

       // string member_function_name = memberFunctionDeclaration->get_mangled_name();
       // string member_function_name = memberFunctionDeclaration->get_qualified_name();
       // string member_function_name = SageInterface::get_name(memberFunctionDeclaration);
          string member_function_scope = SageInterface::get_name(memberFunctionDeclaration->get_scope());
          string member_function_name_part = mangleFunctionName(original_name,"return_type");
          string member_function_name  = string("scope_") + member_function_scope + "_function_name_" + member_function_name_part;

       // DQ (3/16/2016): Detect name collisions so that we can 
       // std::map<std::string,int> dsl_attribute_name_collision_map;
          if (dsl_attribute_name_collision_map.find(member_function_name) == dsl_attribute_name_collision_map.end())
             {
               dsl_attribute_name_collision_map.insert(pair<string,int>(member_function_name,0));
             }
            else
             {
               dsl_attribute_name_collision_map[member_function_name]++;
#if 1
               int count = dsl_attribute_name_collision_map[member_function_name];
               printf ("Collision count = %d \n",count);
#endif
               member_function_name += StringUtility::numberToString(count);
             }

#else
       // string class_name           = classDeclaration->get_name();
          string className            = generateUniqueName (classDeclaration);
          string member_function_name = generateUniqueName (memberFunctionDeclaration);
       // string member_function_name_dsl_attribute = className + "_dsl_attribute";
          string member_function_name_dsl_attribute = member_function_name + "_dsl_attribute";
#endif

       // Call the mangled name support more directly.
       // string mangledName = MangledNameSupport::mangleFunctionName(original_name.str(),"return_type");
       // string mangledName = mangleFunctionName(original_name,"return_type");

       // printf ("Process saved DSL member_function i = %zu: class_name = %s member_function_name = %s functionDeclaration = %p = %s = %s = %s \n",
       //         i,class_name.c_str(),member_function_name.c_str(),memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str(),original_name.c_str(),mangledName.c_str());
          printf ("Process saved DSL member_function i = %zu: className = %s member_function_name = %s functionDeclaration = %p = %s \n",
                  i,className.c_str(),member_function_name.c_str(),memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str());

       // initializerString += " { {functionName},{functionName_dsl_attribute()} } ";
       // dsl_function_names_initializerString += "{\"functionName\"}";
       // dsl_member_function_names_initializerString += "{\"memberFunctionName\"}";
       // dsl_member_function_names_initializerString += "\"memberFunctionName\"";
       // dsl_member_function_names_initializerString += " { \"" + class_name + "\" \"" + member_function_name + "\" } ";
          dsl_member_function_names_initializerString += " { \"" + className + "\", \"" + member_function_name + "\" } ";

       // string memberFunctionName_dsl_attribute = member_function_name + "_dsl_attribute";

       // dsl_attribute_map_initializerString += " { {\"functionName\"},{functionName_dsl_attribute()} } ";
       // dsl_attribute_map_initializerString += " { \"memberFunctionName\",memberFunctionName_dsl_attribute() } ";
       // dsl_attribute_map_initializerString += " { \"" + member_function_name + "\"," + memberFunctionName_dsl_attribute + "() } ";
          dsl_attribute_map_initializerString += " { \"" + member_function_name + "\"," + member_function_name_dsl_attribute + "() } ";
          if (i < dsl_member_function_list.size()-1)
             {
            // initializerString += ",";
            // dsl_type_names_initializerString += ",";
            // dsl_function_names_initializerString += ",";
               dsl_member_function_names_initializerString += ",";
               dsl_attribute_map_initializerString += ",";
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // Add the type name to the initializer of the associated dsl_type_names_variable variable.
     ROSE_ASSERT(dsl_type_names_variable != NULL);
     SgConstructorInitializer* dsl_type_names_variable_initializer = isSgConstructorInitializer(dsl_type_names_variable->get_initializer());

     ROSE_ASSERT(dsl_type_names_variable_initializer != NULL);
     ROSE_ASSERT(dsl_type_names_variable_initializer->get_args() != NULL);
     ROSE_ASSERT(dsl_type_names_variable_initializer->get_args()->get_expressions().empty() == false);

     ROSE_ASSERT(dsl_function_names_variable != NULL);
     SgConstructorInitializer* dsl_function_names_variable_initializer = isSgConstructorInitializer(dsl_function_names_variable->get_initializer());

     ROSE_ASSERT(dsl_member_function_names_variable != NULL);
     SgConstructorInitializer* dsl_member_function_names_variable_initializer = isSgConstructorInitializer(dsl_member_function_names_variable->get_initializer());

     ROSE_ASSERT(dsl_attribute_map_variable != NULL);
     SgConstructorInitializer* dsl_attribute_map_variable_initializer = isSgConstructorInitializer(dsl_attribute_map_variable->get_initializer());

     if (dsl_type_list.empty() == false)
        {
       // initializerString += " }";
          dsl_type_names_initializerString += " }";
          dsl_function_names_initializerString += " }";
          dsl_member_function_names_initializerString += " }";
          dsl_attribute_map_initializerString += " }";

       // printf ("initializerString = %s \n",initializerString.c_str());
          printf ("dsl_type_names_initializerString            = %s \n",dsl_type_names_initializerString.c_str());
          printf ("dsl_function_names_initializerString        = %s \n",dsl_function_names_initializerString.c_str());
          printf ("dsl_member_function_names_initializerString = %s \n",dsl_member_function_names_initializerString.c_str());
          printf ("dsl_attribute_map_initializerString         = %s \n",dsl_attribute_map_initializerString.c_str());

       // SageInterface::addTextForUnparser(dsl_type_names_variable_initializer,initializerString,AstUnparseAttribute::e_replace);
          SageInterface::addTextForUnparser(dsl_type_names_variable_initializer,dsl_type_names_initializerString,AstUnparseAttribute::e_replace);
          SageInterface::addTextForUnparser(dsl_function_names_variable_initializer,dsl_function_names_initializerString,AstUnparseAttribute::e_replace);
          SageInterface::addTextForUnparser(dsl_member_function_names_variable_initializer,dsl_member_function_names_initializerString,AstUnparseAttribute::e_replace);
          SageInterface::addTextForUnparser(dsl_attribute_map_variable_initializer,dsl_attribute_map_initializerString,AstUnparseAttribute::e_replace);
        }

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


#if 1
SgNode* 
AttributeGeneratorTraversal::buildAttribute(SgType* type)
   {
     printf ("Build DSL attribute for type = %p = %s \n",type,type->class_name().c_str());

     dsl_type_list.push_back(type);

#if 1
  // SgTemplateInstantiationDecl* templateClass = NULL;

     SgClassType* classType = isSgClassType(type);
     if (classType != NULL)
        {
       // We don't have to generate code for class types (but perhaps we will to avoid template issues initially).
       // Alternative we could in this narrow non-function case just build the tempalte instantiation declaration.
#if 0
          SgClassDeclaration* ClassDeclarationFromType = isSgClassDeclaration(classType->get_declaration());
          SgName name = ClassDeclarationFromType->get_name();
          printf ("Building DSL support for ClassDeclarationFromType = %p = %s = %s \n",ClassDeclarationFromType,ClassDeclarationFromType->class_name().c_str(),name.str());
          ROSE_ASSERT(global_scope_header != NULL);

       // Don't let class names collide with constructor member function names.
       // SgName attribute_name = name + "_dsl_attribute";
          SgName attribute_name = name + "_dsl_type_attribute";
#else
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
          SgName name = classDeclaration->get_name();
          string className = generateUniqueName (classDeclaration);
          string className_dsl_attribute = className + "_dsl_attribute";
          SgName attribute_name = className_dsl_attribute;
#endif

       // SgClassDeclaration* generatedClass = SageBuilder::buildClassDeclaration(attribute_name,global_scope_header);
          SgClassDeclaration* nonDefiningDecl              = NULL;
          bool buildTemplateInstantiation                  = false; 
          SgTemplateArgumentPtrList* templateArgumentsList = NULL;

          ROSE_ASSERT(global_scope_header != NULL);
          SgClassDeclaration* generatedClass = SageBuilder::buildClassDeclaration_nfi(attribute_name,SgClassDeclaration::e_class,global_scope_header,nonDefiningDecl,buildTemplateInstantiation,templateArgumentsList);
          ROSE_ASSERT(generatedClass != NULL);

          ROSE_ASSERT(generatedClass->get_startOfConstruct() != NULL);
          ROSE_ASSERT(generatedClass->get_endOfConstruct() != NULL);

       // generatedClass->get_startOfConstruct()->display("In buildAttribute(): added class to global scope: debug");

       // Add the base class
       // Lookup the base class by name
       // SgBaseClassPtrList p_inheritances
       // SgClassSymbol* baseClassSymbol = global_scope_header->lookup_class_symbol("dsl_attribute",NULL);
          ROSE_ASSERT(global_scope_requiredSourceCode != NULL);
          SgClassSymbol* baseClassSymbol = global_scope_requiredSourceCode->lookup_class_symbol("dsl_attribute",NULL);
          ROSE_ASSERT(baseClassSymbol != NULL);

          SgClassDeclaration* baseClassDeclaration = baseClassSymbol->get_declaration();
          ROSE_ASSERT(baseClassDeclaration != NULL);

          SgClassDefinition* generatedClassDefinition = generatedClass->get_definition();
          ROSE_ASSERT(generatedClassDefinition != NULL);

       // This builder function adds it to the inheritance list.
          bool isVirtual = false;
          bool isDirect  = false;
          SgBaseClass* baseClass = SageBuilder::buildBaseClass(baseClassDeclaration,generatedClassDefinition,isVirtual,isDirect);
          ROSE_ASSERT(baseClass != NULL);
#if 0
       // For each class we want the class member declarations to be:
               public:
                 // Depending on the compile time semantics we want to leverage we may not need an object here.
                    array value;

                    array_dsl_attribute();
                    virtual ~array_dsl_attribute();

                    std::string toString();
                    std::string additionalNodeOptions();
#endif

       // DQ (2/25/2016): To simplify the inital code construction we will fill in the class with a string based unparsing mechanism.
       // This can be replaced with mechanisms to construct the AST directly when we are sure what code we want to build.
       // For now this is a simple way to tailor the code and add comments easily and quickly.
          string s = "\n \
     public: \n \
       // Depending on the compile time semantics we want to leverage we may not need an object here. \n \
          $DSL_NAME value; \n \
\n \
       // I think we can comment out the constructor and destructor. \n \
       // $DSL_NAME_dsl_attribute(); \n \
       // virtual ~$DSL_NAME_dsl_attribute(); \n \
\n \
       // Required functions \n \
          std::string toString() { return \"$DSL_NAME_attribute\"; } \n \
          std::string additionalNodeOptions() { return \"fillcolor=\\\"red2\\\",style=filled\"; } \n";

       // Substitue value defined by name for "$DSL_NAME" in string.
       // ROSE_UTIL_API std::string copyEdit(const std::string& inputString, const std::string & oldToken, const std::string & newToken);
          string new_string = StringUtility::copyEdit(s,"$DSL_NAME",name);

       // AstUnparseAttribute* astUnparseAttribute = new AstUnparseAttribute(s,AstUnparseAttribute::e_inside);
       // ROSE_ASSERT(astUnparseAttribute != NULL);
       // generatedClassDefinition->addAttribute(astUnparseAttribute);
       // SageInterface::addTextForUnparser ( SgNode* astNode, string s, AstUnparseAttribute::RelativePositionType inputlocation )
          SageInterface::addTextForUnparser(generatedClassDefinition,new_string,AstUnparseAttribute::e_inside);
#if 0
          ROSE_ASSERT(global_scope_requiredSourceCode);

       // Find the template class and build an template instantiation directive to force the instantiation (instead of generating code directly).
       // SgDeclarationStatement* declarationStatement = generatedClass;
          SgDeclarationStatement* declarationStatement = generatedClass->get_firstNondefiningDeclaration();
          ROSE_ASSERT(declarationStatement != NULL);

          ROSE_ASSERT(declarationStatement->get_startOfConstruct() != NULL);
          ROSE_ASSERT(declarationStatement->get_endOfConstruct() != NULL);

          SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = new SgTemplateInstantiationDirectiveStatement(declarationStatement);

          SageInterface::appendStatement(templateInstantiationDirective,global_scope_source);
          ROSE_ASSERT(templateInstantiationDirective->get_parent() != NULL);
       // templateInstantiationDirective->set_parent(global_scope_source);

          ROSE_ASSERT(declarationStatement->get_parent() == NULL);
          declarationStatement->set_parent(templateInstantiationDirective);
          ROSE_ASSERT(declarationStatement->get_parent() != NULL);

          ROSE_ASSERT(templateInstantiationDirective->get_startOfConstruct() != NULL);
          ROSE_ASSERT(templateInstantiationDirective->get_endOfConstruct() != NULL);
#else
       // Put the class declaration into the header file.
       // We can share this, for now, though generally it is a bad idea.
          SageInterface::appendStatement(generatedClass,global_scope_header);
       // SageInterface::appendStatement(generatedClass,global_scope_source);
#endif

       // Add the attribute to the initializer of the associated dsl_attribute_map_variable variable.
       // But we have not see the variable yet (since they are defined after the DSL abstractions's declarations.
       // So we have to put this onto a stack for processing when (or after) we see the associated variable.
       // dsl_type_name_list.push_back(name);

#if 0
       // This should be a pair<string,dsl_attribute> instead of pair<string,string>.
       // dsl_attribute_map_list.push(pair<string,string>(name,attribute_name));
       // dsl_attribute_map_list.push(pair<string,SgConstructorInitializer*>(name,generatedClass));
          SgConstructorInitializer* constructorInitializer = NULL;

       // SgConstructorInitializer* buildConstructorInitializer( 
       //    SgMemberFunctionDeclaration *declaration, SgExprListExp *args, 
       //    SgType *expression_type, bool need_name, bool need_qualifier, 
       //    bool need_parenthesis_after_name,bool associated_class_unknown);

          ROSE_ASSERT(dsl_attribute_map_variable != NULL);
          SgConstructorInitializer* dsl_attribute_map_variable_initializer = isSgConstructorInitializer(dsl_attribute_map_variable->get_initializer());

          ROSE_ASSERT(dsl_attribute_map_variable_initializer != NULL);
          ROSE_ASSERT(dsl_attribute_map_variable_initializer->get_args() != NULL);
          ROSE_ASSERT(dsl_attribute_map_variable_initializer->get_args()->get_expressions().empty() == false);

          SgConstructorInitializer* exampleConstructor = dsl_attribute_map_variable_initializer->get_args()->get_expressions()[0];
          ROSE_ASSERT(exampleConstructor != NULL);

       // constructorInitializer = ASTCopy::copy(exampleConstructor);
          constructorInitializer = exampleConstructor;
          ROSE_ASSERT(constructorInitializer != NULL);

          dsl_attribute_map_variable_initializer->get_args()->append_expression(constructorInitializer);

          ROSE_ASSERT(constructorInitializer != NULL);

          dsl_attribute_map_list.push(constructorInitializer);
#endif

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

     return NULL;
   }
#endif


#if 1
SgNode* 
AttributeGeneratorTraversal::buildAttribute(SgFunctionDeclaration* function)
   {
     ROSE_ASSERT(function != NULL);
     printf ("Build DSL attribute for function = %p = %s = %s \n",function,function->class_name().c_str(),function->get_name().str());

  // dsl_function_list.push_back(function);
     SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(function);
     if (memberFunctionDeclaration != NULL)
        {
          dsl_member_function_list.push_back(memberFunctionDeclaration);
        }
       else
        {
          dsl_function_list.push_back(function);
        }

#if 1
     if (function != NULL)
        {
#if 0
       // SgName name = function->get_name();
          SgName original_name = function->get_name();
       // SgName name = function->get_mangled_name();
       // SgName name = SageInterface::get_name(function);
          SgName name = function->get_mangled_name();

       // Call the mangled name support more directly.
       // string mangledName = MangledNameSupport::mangleFunctionName(original_name.str(),"return_type");
          string mangledName = mangleFunctionName(original_name.str(),"return_type");

          printf ("Building DSL support for function = %p = %s = %s = %s = %s \n",function,function->class_name().c_str(),original_name.str(),name.str(),mangledName.c_str());

       // SgName attribute_name = name + "_dsl_attribute";
          SgName attribute_name = name + "_dsl_function_attribute";
#else
          SgName name = function->get_name();
          string functionName = generateUniqueName(function);
          string functionName_dsl_attribute = functionName + "_dsl_attribute";
          SgName attribute_name = functionName_dsl_attribute;
#endif

       // SgClassDeclaration* generatedClass = SageBuilder::buildClassDeclaration(attribute_name,global_scope_header);
          SgClassDeclaration* nonDefiningDecl              = NULL;
          bool buildTemplateInstantiation                  = false; 
          SgTemplateArgumentPtrList* templateArgumentsList = NULL;

          ROSE_ASSERT(global_scope_header != NULL);

          SgClassDeclaration* generatedClass = SageBuilder::buildClassDeclaration_nfi(attribute_name,SgClassDeclaration::e_class,global_scope_header,nonDefiningDecl,buildTemplateInstantiation,templateArgumentsList);
          ROSE_ASSERT(generatedClass != NULL);

          ROSE_ASSERT(generatedClass->get_startOfConstruct() != NULL);
          ROSE_ASSERT(generatedClass->get_endOfConstruct() != NULL);

       // generatedClass->get_startOfConstruct()->display("In buildAttribute(): added class to global scope: debug");

       // Add the base class
       // Lookup the base class by name
       // SgBaseClassPtrList p_inheritances
       // SgClassSymbol* baseClassSymbol = global_scope_header->lookup_class_symbol("dsl_attribute",NULL);
          ROSE_ASSERT(global_scope_requiredSourceCode != NULL);
          SgClassSymbol* baseClassSymbol = global_scope_requiredSourceCode->lookup_class_symbol("dsl_attribute",NULL);
          ROSE_ASSERT(baseClassSymbol != NULL);

          SgClassDeclaration* baseClassDeclaration = baseClassSymbol->get_declaration();
          ROSE_ASSERT(baseClassDeclaration != NULL);

          SgClassDefinition* generatedClassDefinition = generatedClass->get_definition();
          ROSE_ASSERT(generatedClassDefinition != NULL);

       // This builder function adds it to the inheritance list.
          bool isVirtual = false;
          bool isDirect  = false;
          SgBaseClass* baseClass = SageBuilder::buildBaseClass(baseClassDeclaration,generatedClassDefinition,isVirtual,isDirect);
          ROSE_ASSERT(baseClass != NULL);
#if 0
       // For each class we want the class member declarations to be:
               public:
                 // Depending on the compile time semantics we want to leverage we may not need an object here.
                    array value;

                    array_dsl_attribute();
                    virtual ~array_dsl_attribute();

                    std::string toString();
                    std::string additionalNodeOptions();
#endif

       // DQ (2/25/2016): To simplify the inital code construction we will fill in the class with a string based unparsing mechanism.
       // This can be replaced with mechanisms to construct the AST directly when we are sure what code we want to build.
       // For now this is a simple way to tailor the code and add comments easily and quickly.
          string s = "\n \
     public: \n \
       // Depending on the compile time semantics we want to leverage we may not need an object here. \n \
       // $DSL_NAME value; \n \
\n \
       // I think we can comment out the constructor and destructor. \n \
       // $DSL_NAME_dsl_attribute(); \n \
       // virtual ~$DSL_NAME_dsl_attribute(); \n \
\n \
       // Required functions \n \
          std::string toString() { return \"$DSL_NAME_attribute\"; } \n \
          std::string additionalNodeOptions() { return \"fillcolor=\\\"lightblue\\\",style=filled\"; } \n";

       // Substitue value defined by name for "$DSL_NAME" in string.
       // ROSE_UTIL_API std::string copyEdit(const std::string& inputString, const std::string & oldToken, const std::string & newToken);
          string new_string = StringUtility::copyEdit(s,"$DSL_NAME",name);

       // AstUnparseAttribute* astUnparseAttribute = new AstUnparseAttribute(s,AstUnparseAttribute::e_inside);
       // ROSE_ASSERT(astUnparseAttribute != NULL);
       // generatedClassDefinition->addAttribute(astUnparseAttribute);
       // SageInterface::addTextForUnparser ( SgNode* astNode, string s, AstUnparseAttribute::RelativePositionType inputlocation )
          SageInterface::addTextForUnparser(generatedClassDefinition,new_string,AstUnparseAttribute::e_inside);

       // Put the class declaration into the header file.
       // We can share this, for now, though generally it is a bad idea.
          SageInterface::appendStatement(generatedClass,global_scope_header);
       // SageInterface::appendStatement(generatedClass,global_scope_source);

       // Add the attribute to the initializer of the associated dsl_attribute_map_variable variable.
       // But we have not see the variable yet (since they are defined after the DSL abstractions's declarations.
       // So we have to put this onto a stack for processing when (or after) we see the associated variable.
       // dsl_type_name_list.push_back(name);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

     return NULL;
   }
#endif




#define DEBUG_USING_DOT_GRAPHS 1

int main( int argc, char * argv[] )
   {
  // We don't need the constant folding to support the generation of the DSL attributes.

     bool frontendConstantFolding = false;
     SgProject* project = frontend(argc,argv,frontendConstantFolding);
     ROSE_ASSERT(project != NULL);

#if DEBUG_USING_DOT_GRAPHS
     generateDOT(*project,"_before_transformation");
#endif

  // DQ (3/21/2016): Call the support to generate unique names for class and function declarations. These
  // names will be unique across translation units (which re require to generate code for the DSL compiler).
     SageInterface::computeUniqueNameForUseAsIdentifier(project);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 1
  // AttributeGeneratorTraversal t;
     AttributeGeneratorTraversal t(project);
     AttributeGenerator_InheritedAttribute ih;

  // AttributeGenerator_SynthesizedAttribute sh = t.traverseInputFiles(project,ih);
  // t.traverseInputFiles(project,ih);
     t.traverse(project,ih);
#endif

  // At this point we should have all of the dls attribute variable initialized.
     ROSE_ASSERT(t.dsl_type_names_variable            != NULL);
     ROSE_ASSERT(t.dsl_function_names_variable        != NULL);
     ROSE_ASSERT(t.dsl_member_function_names_variable != NULL);
     ROSE_ASSERT(t.dsl_attribute_map_variable         != NULL);

     ROSE_ASSERT(t.generatedHeaderFile != NULL);

#if 1
     printf ("t.global_scope_header->get_declarations().size() = %zu \n",t.global_scope_header->get_declarations().size());
  // printf ("t.generatedHeaderFile->unparseToString() = %s \n",t.generatedHeaderFile->unparseToString().c_str());

     printf ("t.global_scope_source->get_declarations().size() = %zu \n",t.global_scope_source->get_declarations().size());
  // printf ("t.generatedSourceFile->unparseToString() = %s \n",t.generatedSourceFile->unparseToString().c_str());
#endif

#if DEBUG_USING_DOT_GRAPHS
  // generateDOT(*(t.generatedHeaderFile),"_after_transformation");
#endif

     t.unparseGeneratedCode();

     return 0;
   }
