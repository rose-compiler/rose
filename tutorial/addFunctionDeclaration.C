// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top of the source file.

#include "rose.h"

#define TRANSFORMATION_FILE_INFO Sg_File_Info::generateDefaultFileInfoForTransformationNode()

class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:
          void visit ( SgNode* astNode );
   };

void
SimpleInstrumentation::visit ( SgNode* astNode )
   {
     SgGlobal* globalScope = isSgGlobal(astNode);
     if (globalScope != NULL)
        {
       // *****************************************************
       // Create the functionDeclaration 
       // *****************************************************
          SgType * func_return_type           = new SgTypeInt();
          SgName func_name                    = "my_function";
          SgFunctionType * func_type          = new SgFunctionType(func_return_type,false);
          SgFunctionDeclaration * func        = new SgFunctionDeclaration(TRANSFORMATION_FILE_INFO, func_name, func_type);
          SgFunctionDefinition *  func_def    = new SgFunctionDefinition(TRANSFORMATION_FILE_INFO, func);
          SgBasicBlock         * func_body    = new SgBasicBlock(TRANSFORMATION_FILE_INFO);

       // set the end source position as transformation generated   
       // since the constructors only set the beginning source position by default
	  func->set_endOfConstruct(TRANSFORMATION_FILE_INFO);                        
	  func->get_endOfConstruct()->set_parent(func);

	  func_def->set_endOfConstruct(TRANSFORMATION_FILE_INFO);                        
	  func_def->get_endOfConstruct()->set_parent(func_def);

	  func_body->set_endOfConstruct(TRANSFORMATION_FILE_INFO);                        
	  func_body->get_endOfConstruct()->set_parent(func_body);

       // Sets the body into the definition
          func_def->set_body(func_body);
       // Sets the defintion's parent to the declaration
          func_def->set_parent(func);

       // DQ (9/8/2007): Fixup the defining and non-defining declarations
          ROSE_ASSERT(func->get_definingDeclaration() == NULL);
          func->set_definingDeclaration(func);
          ROSE_ASSERT(func->get_definingDeclaration()         != NULL);
          ROSE_ASSERT(func->get_firstNondefiningDeclaration() != func);

       // DQ (9/8/2007): We have not build a non-defining declaration, so this should be NULL.
          ROSE_ASSERT(func->get_firstNondefiningDeclaration() == NULL);

       // DQ (9/8/2007): Need to add function symbol to global scope!
          //printf ("Fixing up the symbol table in scope = %p = %s for function = %p = %s \n",globalScope,globalScope->class_name().c_str(),func,func->get_name().str());
          SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(func);
          globalScope->insert_symbol(func->get_name(),functionSymbol);
          ROSE_ASSERT(globalScope->lookup_function_symbol(func->get_name()) != NULL);

       // ********************************************************************
       // Create the InitializedName for a parameter within the parameter list
       // ********************************************************************
          SgName var1_name = "var_name";

          SgTypeInt * var1_type     = new SgTypeInt();
          SgReferenceType *ref_type = new SgReferenceType(var1_type);
          SgInitializer * var1_initializer = NULL;

          SgInitializedName *var1_init_name = new SgInitializedName(var1_name, ref_type, var1_initializer, NULL);
          var1_init_name->set_file_info(TRANSFORMATION_FILE_INFO);

       // DQ (9/8/2007): We now test this, so it has to be set explicitly.
          var1_init_name->set_scope(func_def);

       // DQ (9/8/2007): Need to add variable symbol to global scope!
          //printf ("Fixing up the symbol table in scope = %p = %s for SgInitializedName = %p = %s \n",globalScope,globalScope->class_name().c_str(),var1_init_name,var1_init_name->get_name().str());
          SgVariableSymbol *var_symbol = new SgVariableSymbol(var1_init_name);
          func_def->insert_symbol(var1_init_name->get_name(),var_symbol);
          ROSE_ASSERT(func_def->lookup_variable_symbol(var1_init_name->get_name()) != NULL);
          ROSE_ASSERT(var1_init_name->get_symbol_from_symbol_table() != NULL);

       // Done constructing the InitializedName variable

       // Insert argument in function parameter list
          ROSE_ASSERT(func != NULL);
       // Sg_File_Info * parameterListFileInfo   = new Sg_File_Info();
       // Sg_File_Info* parameterListFileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
          SgFunctionParameterList* parameterList = new SgFunctionParameterList(TRANSFORMATION_FILE_INFO);
          ROSE_ASSERT(parameterList != NULL);
          parameterList->set_definingDeclaration (NULL);
          parameterList->set_firstNondefiningDeclaration (parameterList);

          func->set_parameterList(parameterList);
          ROSE_ASSERT(func->get_parameterList() != NULL);
          func->get_parameterList()->append_arg(var1_init_name);

       // ********************************************************
       // Insert a statement in the function body
       // *******************************************************

       // create a VarRefExp
       // SgVariableSymbol *var_symbol = new SgVariableSymbol(var1_init_name);
          SgVarRefExp *var_ref = new SgVarRefExp(TRANSFORMATION_FILE_INFO,var_symbol);
          var_ref->set_endOfConstruct(TRANSFORMATION_FILE_INFO);
          var_ref->get_endOfConstruct()->set_parent(var_ref);

       // create a ++ expression, 0 for prefix ++
          SgPlusPlusOp *pp_expression = new SgPlusPlusOp(TRANSFORMATION_FILE_INFO,var_ref,0);
          pp_expression->set_endOfConstruct(TRANSFORMATION_FILE_INFO);
          pp_expression->get_endOfConstruct()->set_parent(pp_expression);
          markLhsValues (pp_expression);

       // create an expression statement
          SgExprStatement* new_stmt = new SgExprStatement(TRANSFORMATION_FILE_INFO,pp_expression);
          new_stmt->set_endOfConstruct(TRANSFORMATION_FILE_INFO);
          new_stmt->get_endOfConstruct()->set_parent(new_stmt);
			   
#if 0
       // DQ (9/8/2007): This is no longer required, SgExpressionRoot is not longer used in the ROSE IR.
       // create an expression type
          SgTypeInt* expr_type = new SgTypeInt();

       // create an expression root
          SgExpressionRoot * expr_root = new SgExpressionRoot(TRANSFORMATION_FILE_INFO,pp_expression,expr_type);
          expr_root->set_parent(new_stmt);

       // DQ (11/8/2006): Modified to reflect use of SgExpression instead of SgExpressionRoot
          new_stmt->set_expression(expr_root);

          pp_expression->set_parent(new_stmt->get_expression());
#endif
          pp_expression->set_parent(new_stmt);

       // insert a statement into the function body
          func_body->prepend_statement(new_stmt);

       // setting the parent explicitly is not required since it would be done within AST post-processing
          func->set_parent(globalScope);

       // scopes of statments must be set explicitly since within C++ they are not guaranteed 
       // to be the same as that indicated by the parent (see ChangeLog for Spring 2005).
          func->set_scope(globalScope);

       // DQ (6/15/2013): There should be a non-defining declaration (but we need to migrate this example to instead use the AST Builder API).
       // ROSE_ASSERT(func->get_firstNondefiningDeclaration() != NULL);
          if (func->get_firstNondefiningDeclaration() == NULL)
             {
               printf ("WARNING: func->get_firstNondefiningDeclaration() == NULL for case of func = %p = %s (allowed for tutorial example transformations only) \n",func,func->class_name().c_str());
             }

       // ********************************************************
       // Insert the function declaration in the code
       // *******************************************************
          globalScope->prepend_declaration(func);

       // Required post processing of AST required to set parent pointers and fixup template names, etc.
       // temporaryAstFixes(globalScope);
          AstPostProcessing(globalScope);
        }
   }

int
main ( int argc, char * argv[] )
   {
     // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

     SimpleInstrumentation treeTraversal;
     treeTraversal.traverseInputFiles ( project, preorder );

     AstTests::runAllTests(project);
     return backend(project);
   }

