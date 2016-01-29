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
AttributeGeneratorTraversal::AttributeGeneratorTraversal()
   {
  // SgSourceFile* buildSourceFile(string,SgProject = NULL)
     generatedHeaderFile    = NULL;
     generatedSourceFile    = NULL;
     requiredSourceCodeFile = NULL;

  // This has to be a *.C file since we are not supporting witing heder files just yet (rename afterward).
     generatedHeaderFile = SageBuilder::buildSourceFile("generated_dsl_attributes_header.C");
     generatedSourceFile = SageBuilder::buildSourceFile("generated_dsl_attributes.C");

     global_scope_header = generatedHeaderFile->get_globalScope();
     ROSE_ASSERT(global_scope_header != NULL);
     global_scope_source = generatedSourceFile->get_globalScope();
     ROSE_ASSERT(global_scope_source != NULL);

  // Read in the file containing code that we want to use (e.g. templates).
  // build the file name of the required with the path (use the source directory as the path)
     string requiredCodeFileNameWithPath = ROSE_AUTOMAKE_TOP_SRCDIR + "/projects/ShiftCalculus4/" + "requiredCode.C";

     printf ("Using requiredCodeFileNameWithPath = %s \n",requiredCodeFileNameWithPath.c_str());

  // requiredSourceCodeFile = SageBuilder::buildSourceFile(requiredCodeFileNameWithPath);
     ROSE_ASSERT(requiredSourceCodeFile == NULL);

     bool frontendConstantFolding = false;
     std::vector<string> argList;
     argList.push_back("-c");
     argList.push_back(requiredCodeFileNameWithPath);

  // SgProject* requiredSourceCodeProject = frontend(argc,argv,frontendConstantFolding);
     SgProject* requiredSourceCodeProject = frontend(argList,frontendConstantFolding);
     ROSE_ASSERT(requiredSourceCodeProject != NULL);

     ROSE_ASSERT(requiredSourceCodeProject->get_fileList_ptr() != NULL);
     ROSE_ASSERT(requiredSourceCodeProject->get_fileList_ptr()->get_listOfFiles().empty() == false);
  // requiredSourceCodeFile = requiredSourceCodeProject->get_fileList_ptr()[0]->get_globalscope();
  // requiredSourceCodeFile = requiredSourceCodeProject->get_file(0)->get_globalscope();
  // requiredSourceCodeFile = isSgSourceFile(requiredSourceCodeProject->[0]);
     requiredSourceCodeFile = isSgSourceFile(requiredSourceCodeProject->operator[](0));
     ROSE_ASSERT(requiredSourceCodeFile != NULL);

     global_scope_requiredSourceCode = requiredSourceCodeFile->get_globalScope();
     ROSE_ASSERT(global_scope_requiredSourceCode);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void
AttributeGeneratorTraversal::unparseGeneratedCode()
   {
     printf ("In unparseGeneratedCode(): unparse the header file \n");
     generatedHeaderFile->unparse();

     printf ("In unparseGeneratedCode(): unparse the source file \n");
     generatedSourceFile->unparse();

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


AttributeGenerator_InheritedAttribute
AttributeGeneratorTraversal::evaluateInheritedAttribute   (SgNode* astNode, AttributeGenerator_InheritedAttribute inheritedAttribute )
   {
     ROSE_ASSERT(astNode != NULL);

     AttributeGenerator_InheritedAttribute return_inheritedAttribute;

#if 0
     printf ("In evaluateInheritedAttribute(): astNode = %p = %s: DSLnodes.size() = %zu \n",astNode,astNode->class_name().c_str(),DSLnodes.size());
#endif

     switch (astNode->variantT())
        {
          case V_SgPragmaDeclaration:
             {
            // Statements after a pragma will be detected and be used as DSL keywords.

               SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(astNode);
               ROSE_ASSERT(pragmaDeclaration != NULL);

               printf ("Detected marking pragma for DSL keyword: pragmaDeclaration = %p = %s = %s \n",pragmaDeclaration,pragmaDeclaration->class_name().c_str(),pragmaDeclaration->get_pragma()->get_pragma().c_str());

            // There should always be a next statement after a pragma declaration
               SgStatement* DSLKeywordStatement = SageInterface::getNextStatement(pragmaDeclaration);
               if (DSLKeywordStatement == NULL)
                  {
                    printf ("Error: There should always be a next statement after a pragma declaration \n");
                  }
               ROSE_ASSERT(DSLKeywordStatement != NULL);

               DSLnodes.insert(DSLKeywordStatement);

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

                    SgType* type = classDeclaration->get_type();
                    ROSE_ASSERT(type != NULL);

                    SgNode* ast_fragment = buildAttribute(type);
                  }

               break;
             }

          case V_SgFunctionDeclaration:
          case V_SgTemplateFunctionDeclaration:
             {
            // This case is more complex since functions have arguments (using std::function support in C++11).

               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
               ROSE_ASSERT(functionDeclaration != NULL);

               if (DSLnodes.find(functionDeclaration) != DSLnodes.end())
                  {
                    printf ("Build DSL attribute for function = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());

                    SgType* type = functionDeclaration->get_type();
                    ROSE_ASSERT(type != NULL);

                    SgNode* ast_fragment = buildAttribute(type);
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
             }
            else
             {
            // Skip adding #include and comments to an empty file (thoough it should work).
               printf ("Generating an empty file: not extra processing required \n");
             }
        }


  // return return_SynthesizedAttribute(generated_ast);
     return AttributeGenerator_SynthesizedAttribute (generated_ast);
   }


SgNode* 
AttributeGeneratorTraversal::buildAttribute(SgType* type)
   {
     printf ("Build DSL attribute for type = %p = %s \n",type,type->class_name().c_str());

     SgTemplateInstantiationDecl* templateClass = NULL;


     SgClassType* classType = isSgClassType(type);
     if (classType != NULL)
        {
       // We don't have to generate code for class types (but perhaps we will to avoid template issues initially).
       // Alternative we could in this narrow non-function case just build the tempalte instantiation declaration.
          SgClassDeclaration* ClassDeclarationFromType = isSgClassDeclaration(classType->get_declaration());

          SgName name = ClassDeclarationFromType->get_name();
          printf ("Building DSL support for ClassDeclarationFromType = %p = %s = %s \n",ClassDeclarationFromType,ClassDeclarationFromType->class_name().c_str(),name.str());
          ROSE_ASSERT(global_scope_header != NULL);

          name += "_dsl_attribute";

       // SgClassDeclaration* generatedClass = SageBuilder::buildClassDeclaration(name,global_scope_header);
          SgClassDeclaration* nonDefiningDecl              = NULL;
          bool buildTemplateInstantiation                  = false; 
          SgTemplateArgumentPtrList* templateArgumentsList = NULL;

          SgClassDeclaration* generatedClass = SageBuilder::buildClassDeclaration_nfi(name,SgClassDeclaration::e_class,global_scope_header,nonDefiningDecl,buildTemplateInstantiation,templateArgumentsList);
          ROSE_ASSERT(generatedClass != NULL);

#if 1
          ROSE_ASSERT(global_scope_requiredSourceCode);

       // Find the template class and build an template instantiation directive to force the instantiation (instead of generating code directly).
       // SgDeclarationStatement* declarationStatement = generatedClass;
          SgDeclarationStatement* declarationStatement = generatedClass->get_firstNondefiningDeclaration();
          ROSE_ASSERT(declarationStatement != NULL);

          SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = new SgTemplateInstantiationDirectiveStatement(declarationStatement);

          SageInterface::appendStatement(templateInstantiationDirective,global_scope_source);
          ROSE_ASSERT(templateInstantiationDirective->get_parent() != NULL);
       // templateInstantiationDirective->set_parent(global_scope_source);

          ROSE_ASSERT(declarationStatement->get_parent() == NULL);
          declarationStatement->set_parent(templateInstantiationDirective);
          ROSE_ASSERT(declarationStatement->get_parent() != NULL);
#else
       // We can share this, for now, though generally it is a bad idea.
       // SageInterface::appendStatement(generatedClass,global_scope_header);

          SageInterface::appendStatement(generatedClass,global_scope_source);
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
        }

     return NULL;
   }


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

#if 1
     AttributeGeneratorTraversal t;
     AttributeGenerator_InheritedAttribute ih;

  // AttributeGenerator_SynthesizedAttribute sh = t.traverseInputFiles(project,ih);
     t.traverseInputFiles(project,ih);
#endif

     t.unparseGeneratedCode();

     return 0;
   }
