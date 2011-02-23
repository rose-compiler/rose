#include "rose.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "InsertPragma.h"

void InsertPragma:: visit(SgNode* astNode){
  //  cout << "start of visit" << endl;
  // cout << "astNode: " << astNode->sage_class_name() << endl;
  int index; 
  if(isSgBasicBlock(astNode)!=NULL){
    if(!astNode->attribute.exists("local_index")){
      astNode->attribute.add("local_index", new BasicBlockLocalIndex(0));
      astNode->attribute.set("local_index",0);
      // cout << "adding a new attribute!" << endl;
    }
    else if(astNode->attribute.exists("local_index")){
      // need to reset the local index counter     
      astNode->attribute.set("local_index",0);
    }
  }
  // If it is a statement in a basic block:
  if(isSgBasicBlock(astNode->get_parent())!=NULL){
    if(astNode->get_parent()->attribute.exists("local_index")){
      AstAttribute* attri = (astNode->get_parent())->attribute["local_index"];
      int local_index = (int) attri;
      local_index++;
      // cout << "The local index is " << local_index << endl;
      (astNode->get_parent())->attribute.set("local_index",(AstAttribute*) local_index);
      
      int global_index=0;
      if(astNode->attribute.exists("global_index")){
        AstAttribute* attri = astNode->attribute["global_index"];
        global_index = (int) attri;
        //      cout << "Global index: " << global_index <<endl;
      } 
      // if global index is equal to the statement number which is to be "wrapped" in pragmas
      if(global_index==no_statement){
        
        // build the two pragma declaration
        SgPragma* start = new SgPragma("start_slicing_criterion", astNode->get_file_info());
        SgPragmaDeclaration* start_pdecl = new SgPragmaDeclaration();
        start_pdecl->set_pragma(start);
        start_pdecl->set_parent(astNode->get_parent());

        SgPragma* end = new SgPragma("end_slicing_criterion", astNode->get_file_info());
        SgPragmaDeclaration* end_pdecl = new SgPragmaDeclaration();
        end_pdecl->set_pragma(end);
        end_pdecl->set_parent(astNode->get_parent()); 

        SgBasicBlock* bb = isSgBasicBlock(astNode->get_parent());
        SgStatementPtrList *stmt_list = &bb->get_statements();
        SgStatementPtrList::iterator it = stmt_list->begin();

        // Increment the statement pointer iterator, so we can place the pragmas correct
        for(int j=1; j<local_index;j++){it++;}
        //ROSE_ASSERT(bb);
        bb->prepend_statement(it,start_pdecl);
        //      bb->append_statement(it,end_pdecl);

        // get the variables which occur in it!
        list<SgNode*> var_list = NodeQuery::querySubTree(isSgNode(*it), V_SgVarRefExp);
        
        SgGlobal* global =  TransformationSupport::getGlobalScope(astNode);
        SgFunctionDeclaration* printf_func;
        SgFunctionDeclaration *fopen_func, *fclose_func;
        createFunctionDeclaration(global, var_list, printf_func, fopen_func, fclose_func);
        // build an output statement outputting the variables within the statements between the pragmas.
        SgExprStatement* func_expr;

        createFunctionCallprintf(global, printf_func, var_list, func_expr);
        bb->append_statement(it, func_expr);
        it++; // increment statement-list pointer

        // append end pragma declaration
        bb->append_statement(it,end_pdecl);

      }
    }
  }  
}

// move this function and do this after we know how many arguements we will need.
void InsertPragma::createFunctionDeclaration(SgGlobal* global,
                                             list<SgNode*> var_list, SgFunctionDeclaration*& printf_func,
                                             SgFunctionDeclaration*& fopen_func,
                                             SgFunctionDeclaration*& fclose_func){
  // create SgfunctionDeclaration 
  SgName func_name = "printf";
  SgType* func_return_type = new SgTypeVoid();
  SgFunctionType *func_type = new SgFunctionType(func_return_type);//, false); // why false??
  
  Sg_File_Info* file_info = global->get_file_info();
 
  printf_func =   new SgFunctionDeclaration(file_info, func_name, func_type);
  
  printf_func->set_parent(global);
  printf_func->set_definition(NULL);

  // append first string argument
  SgTypeString* type_string = new SgTypeString();
  SgInitializedName* string = new SgInitializedName();
  string->set_type(type_string);
  printf_func->append_arg(string);

  // append arguements from var_list
  for(list<SgNode*>::const_iterator it= var_list.begin(); it!=var_list.end(); it++){
    
    SgInitializedName* ini = new SgInitializedName();
    ini->set_type(isSgVarRefExp(*it)->get_type());
    //don't append if it is of type bool
    //if(isSgTypeBool(isSgVarRefExp(*it)->get_symbol()->get_type())==NULL){
    // cout << "- type bool...not append" << endl;
    // SgName name = isSgVarRefExp(*it)->get_symbol()->get_name();
    // cout << "The name is:"  << name.str() << endl;
      printf_func->append_arg(ini);
      //}
  }
  // insert function declaration
  //global->prepend_declaration(printf_func);  // maybe not this, but all the others?


  // Do the same for fopen
  func_name = "fopen";
  func_return_type = new SgTypeVoid();
  func_type = new SgFunctionType(func_return_type);//, false); // why false??
  
  file_info = global->get_file_info();
  
  fopen_func =   new SgFunctionDeclaration(file_info, func_name, func_type);
  fopen_func->set_parent(global);
  fopen_func->set_definition(NULL);

  // append two string arguments
  type_string = new SgTypeString();
  string = new SgInitializedName();
  string->set_type(type_string);
  fopen_func->append_arg(string);
  fopen_func->append_arg(string);
 
  //global->prepend_declaration(fopen_func);
  

  // Do the same for fclose
  
  func_name = "fclose";
  func_return_type = new SgTypeVoid();
  func_type = new SgFunctionType(func_return_type);//, false); // why false??
  
  file_info = global->get_file_info();
  
  fclose_func =   new SgFunctionDeclaration(file_info, func_name, func_type);
  
  fclose_func->set_parent(global);
  fclose_func->set_definition(NULL);

  // append first string argument
  SgType* base_type = new SgTypeUnknown();
  //SgPointerType* type = new SgPointerType();
  SgInitializedName* ini = new SgInitializedName();
  ini->set_type(base_type);
  fclose_func->append_arg(ini);
  
  // global->prepend_declaration(fclose_func);

  // Don't need to prepend these function declarations as long as I have the...


  // have correct argument list with names?
  // have the type "extern" in front of the function declaration!!

  // since I cant get the function prototype to have "extern" written in front + no semicolon is
  // added, then the whole expression is not added to the AST. However, we use the reference to the
  // function declaration AST node to get the right name for the later function call. For this
  // function call to work we have to include correct header files. These header files are added
  // manually before the whole process...

}


void InsertPragma::createFunctionCallprintf(SgGlobal*& root,SgFunctionDeclaration* func_decl,list<SgNode*> var_list, SgExprStatement*& func_expr){

  SgFunctionSymbol* func_symbol = new SgFunctionSymbol(func_decl);
  SgFunctionType* func_type = func_decl->get_type();
  SgFunctionRefExp* func_ref_exp = new SgFunctionRefExp(SgNULL_FILE, func_symbol, func_type);
  SgExprListExp* exp_list_exp = new SgExprListExp(SgNULL_FILE);
  SgFunctionCallExp *  func_call_expr=new SgFunctionCallExp(SgNULL_FILE,func_ref_exp,
                                                            exp_list_exp,func_type);
  SgFunctionCallExp* func_call = new SgFunctionCallExp(SgNULL_FILE,func_ref_exp,
                                                       exp_list_exp, func_type);
  SgExpressionRoot* expr_root_func_call = new SgExpressionRoot();
  func_expr = new SgExprStatement(SgNULL_FILE, func_call_expr);
  func_call_expr->set_parent(func_expr->get_expression_root());
  
  // add parameters in the function call
  char s[256]= "";

  // First add the %d or %s etc of the printf-function.
  for(list<SgNode*>::const_iterator it= var_list.begin(); it!=var_list.end(); it++){
    //cout << "it is " << (*it)->unparseToString() << endl;
    
    switch(isSgVarRefExp(*it)->get_symbol()->get_type()->variantT()){
    case V_SgTypeInt:{
      //cout << "V_SgTypent" << endl;
      sprintf(s, "%s %s ",s, "%d");  
      break;
    }
    case V_SgTypeFloat:{
      //cout << "V_SgTypeFloat" << endl;
      sprintf(s, "%s %s ",s, "%f");       
      break;
    }
    case V_SgTypeString:{
      // cout << "V_SgTypeString" << endl;
      sprintf(s, "%s %s ",s, "%s");      
      break;
    }
    case V_SgPointerType:{
      //cout <<" V_SgPointerType" << endl;
      if(isSgVarRefExp(*it)->get_symbol()->get_type()->findBaseType()->variantT()==V_SgTypeChar){
        //cout << "V_SgTypeChar" << endl;
        sprintf(s, "%s %s", s, "%s");
      }
      // add for more base type of pointers...
      break;
    }
    }
  } 
    SgStringVal* string = new SgStringVal(SgNULL_FILE, s);
    //cout << "The string value is: " << string->get_value() << endl;
    exp_list_exp->append_expression(isSgExpression(string));
    

    // append arguements from var_list 
    for(list<SgNode*>::const_iterator it= var_list.begin(); it!=var_list.end(); it++){
      // They are all SgVarRefExp
      exp_list_exp->append_expression(isSgVarRefExp(*it));

    }
    
  
  }
 
