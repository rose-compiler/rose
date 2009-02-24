#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



/**
\brief This function creates a function and tries to insert and InitializeName variable into its parameter list. It
fails to insert the variable. 
*/
void create_function_and_insert_arg(SgProject * projectNode)
   {
     
     SgFilePtrListPtr  ptr_file_ptr= projectNode->get_fileList();
     ROSE_ASSERT(ptr_file_ptr!=NULL);
     ROSE_ASSERT(ptr_file_ptr->size()>0);
     
     
     SgGlobal * global =(*(ptr_file_ptr->begin()))->get_root();
     

  // *****************************************************
  // Create the functionDeclaration 
  // *****************************************************
     SgType * func_return_type           =   new SgTypeInt();


     Sg_File_Info * file_info=new Sg_File_Info(global->get_file_info()->get_filename(),0,0);

  // DQ: Must mark the newly built node to be a part of a transformation so that it will be unparsed!
     file_info->set_isPartOfTransformation(true);

     SgName func_name                    =   "my_function";

     SgFunctionType * func_type          =   new SgFunctionType(func_return_type,false);

     SgFunctionDeclaration * func        =   new SgFunctionDeclaration(file_info, func_name, func_type);

     SgFunctionDefinition *  func_def    =   new SgFunctionDefinition(file_info, func);

     SgBasicBlock         * func_body    =   new SgBasicBlock(file_info);

  // Sets the body into the definition
     func_def->set_body(func_body);
  // Sets the defintion's parent to the declaration
     func_def->set_parent(func);


  // ********************************************************
  // Create the InitializedName
  // *******************************************************
     SgName var1_name = "var_name";

     SgTypeInt * var1_type = new SgTypeInt();

     SgReferenceType *ref_type=new SgReferenceType(var1_type);

     SgInitializer * var1_initializer = 0;

  // DQ (7/27/2004): In an investigation with Alin, the following code can only work if the declaration input is NULL
  // SgInitializedName *var1_init_name=new SgInitializedName(var1_name, var1_type, var1_initializer,func->get_parameterList());
     SgInitializedName *var1_init_name=new SgInitializedName(var1_name, ref_type, var1_initializer, NULL);

    // Done constructing the InitializedName variable

     // Insert argument in function parameter list
     func->get_parameterList()->append_arg(var1_init_name);



  // ********************************************************
  // Insert a statement in the function body
  // *******************************************************

     // create a VarRefExp
     // CAUTION : the SgInitializedName
     SgVariableSymbol *var_symbol=new SgVariableSymbol(var1_init_name);

     SgVarRefExp *var_ref=new SgVarRefExp(SgNULL_FILE,var_symbol);

     // create a ++ expression, 0 for prefix ++
     SgPlusPlusOp *pp_expression=new SgPlusPlusOp(SgNULL_FILE,var_ref,0);

   // create an expression statement
     SgExprStatement *new_stmt;

     // create an expression type

     SgTypeInt* expr_type=new SgTypeInt();

     // create an expression root
     SgExpressionRoot * expr_root= new SgExpressionRoot(SgNULL_FILE,pp_expression,expr_type,new_stmt);

     new_stmt=new SgExprStatement(SgNULL_FILE,pp_expression);

     expr_root->set_parent(new_stmt);

     new_stmt->set_expression_root(expr_root);

     pp_expression->set_parent(new_stmt->get_expression_root());

     //insert a statement into the function body
     func_body->prepend_statement(new_stmt);
 // ********************************************************
  // Insert the function declaration in the code
  // *******************************************************

     func->set_parent(global);

     global->prepend_declaration(func);


  // ********************************************************
  // Create the  function call
  // *******************************************************

     SgFunctionSymbol * func_symbol=new SgFunctionSymbol(func);
          
     SgFunctionRefExp * func_ref_exp=new SgFunctionRefExp(SgNULL_FILE,func_symbol,func_type);
     
     SgExprListExp * exp_list_exp=new SgExprListExp(SgNULL_FILE);
     
     SgFunctionCallExp *  func_call_expr=new SgFunctionCallExp(SgNULL_FILE,func_ref_exp,exp_list_exp,func_type);
		
     SgExprStatement * func_expr;

     SgExpressionRoot * expr_root_func_call=new SgExpressionRoot(SgNULL_FILE,func_call_expr,func_type,func_expr);
    


     func_expr=new SgExprStatement(SgNULL_FILE, func_call_expr);

     func_call_expr->set_parent(func_expr->get_expression_root());
  // ********************************************************
  // Initialize the parameters in a function call
  // *******************************************************

     list<SgNode*> var_decl_list=NodeQuery::querySubTree(projectNode,NodeQuery::VariableDeclarations);

     // I know there is 1 variable declaration  in the main, and it's the last one in the list
     printf("\n VarDecl %zu",var_decl_list.size());

     ROSE_ASSERT(var_decl_list.size()!=0);
     SgVariableDeclaration * var_decl=isSgVariableDeclaration((*(var_decl_list.begin())));

     ROSE_ASSERT(var_decl!=NULL);
     SgInitializedNamePtrList &init_list=var_decl->get_variables();

     ROSE_ASSERT(init_list.size()!=0);

     SgInitializedName * var_initialized_name=*(--(var_decl->get_variables().end()));

     SgVariableSymbol *var_symbol_param_list=new SgVariableSymbol(var_initialized_name);

     SgVarRefExp * exp_param_list=new SgVarRefExp(SgNULL_FILE,var_symbol_param_list);

     exp_list_exp->append_expression(exp_param_list);

  // ********************************************************
  // Insert the function call in the code
  // *******************************************************

     list<SgStatement*> l;

     l.push_back(func_expr);
     
     LowLevelRewrite::insert(var_decl,l,false);


}

int
main ( int argc, char * argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

     create_function_and_insert_arg(project);

     project->unparse();
     AstPDFGeneration pdftest;
     pdftest.generateInputFiles(project);

     AstDOTGeneration dotgen;
     dotgen.generateInputFiles(project,AstDOTGeneration::PREORDER);

     return 0;//backend(project);
   }



