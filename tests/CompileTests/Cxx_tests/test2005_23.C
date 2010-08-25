/*

Hi Dan,

Attached is a code (file main.cpp) that builds an AST from a very simple inputfile (file testfile.cpp).
To this AST I try to add a function prototype declaration of "printf" and then a function call to "printf". There are a couple of problems/difficulties with doing this.

More specific comments and questions are in the file itself (main.cpp).

Please let me know if I should add more comments or specify the difficulties more.

Vera



// test file to demonstrate incorrect string argument into the printf function call
// when building the function call in the AST.
// (the printf call will be appended after the return statement. This is not correct. However, nothing
// is done to fix it since that is not of any concern in this case.)


int main(){

  int a;
  int b = a;  // One SgVarRefExp
  return 0;

}

*/

#if 0
#include "rose.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



/*********************************************************************************
 * To demonstrate problems/ difficulties with adding a printf-call in the AST.
 *
 * This code builds an AST of an (simple) input file (testfile.cpp).
 *
 * Then a prototype of the function declaration for the function "printf" is prepended to the sgproject.
 * Here is problem #1: I don't manage to add the wanted ; (semicolon) after the function declaration.
 * I neither manage to add "extern" in front of the prototype (if this is necessary).
 *
 * Next a function call to "printf" is build. Here I find two problems/difficulties: 
 * Problem #1: I want the first argument to printf to be a string (char*). However, in the unparsed
 * code this string seems to take random values? In other word, the wanted string is not outputted correctly.
 * Problem #2: As the other arguments to "printf" I use all of the SgVarRefExp nodes in the AST. I know in
 * my input file I have only one SgVarRefExp. However, in the switch-case structure, this SgVarRefExp
 * which I expect being of type int, matches all three cases (int, float and string).
 *  
 * There are comments with more specific questions further down in this file.
 **********************************************************************************/


void createFunctionDeclaration(SgGlobal* global, list<SgNode*> var_list, SgFunctionDeclaration*& func_decl){
  // create SgfunctionDeclaration 

  // The name of the function we want to make a prototype of is "printf()", type void.
  SgName func_name = "printf";
  SgType* func_return_type = new SgTypeVoid();
  SgFunctionType *func_type = new SgFunctionType(func_return_type);//, false); // why false??
  
  Sg_File_Info* file_info = global->get_file_info();
  func_decl =   new SgFunctionDeclaration(file_info, func_name, func_type);
  func_decl->set_parent(global);
  func_decl->set_definition(NULL); // Definition is NULL since this is a prototype.
  
  // append first argument which is a string
  SgTypeString* type_string = new SgTypeString();
  SgInitializedName* string = new SgInitializedName();
  string->set_type(type_string);
  func_decl->append_arg(string);
  
  // append arguements from var_list
  for(list<SgNode*>::const_iterator it= var_list.begin(); it!=var_list.end(); it++){
    SgInitializedName* ini = new SgInitializedName();
    ini->set_type(isSgVarRefExp(*it)->get_type());
    SgName name = isSgVarRefExp(*it)->get_symbol()->get_name();
    func_decl->append_arg(ini);
  }
  
  // insert function declaration
  global->prepend_declaration(func_decl);  // maybe not this, but all the others?
  

  // QUESTIONS:
  // How do I get the "extern" in front of the function declaration to make it a prototype?
  // Do I need at all this "extern"?
  // How do I get the semicolon after the function declaration? (I don't want to have any function 
  // definition of this function.)
  
  // One ad hoc solution:
  // Since I can't get the function prototype to have "extern" written in front and no semicolon is
  // added, then I can simply avoid adding the whole expression to the AST. However, we need to have
  // the AST node of the function declaration to get the right name for the later function call. We can 
  // have this without actually adding the AST node to the sgproject.
  // For this printf function call to work we just have to include correct header files. These header files
  // can be added manually before the whole process...
}


void createFunctionCall(SgProject* sgproject){
  // Create the actual function call which will be (improperly) added to the AST.
  // (Improperly because I add it at the end of the code, after the return-statement of
  // the main function. This is not fixed, since this example is for demonstrating the result of
  // the printf()-statement. I don't intend to compile or run the resulting code.

  SgFile &sageFile = sgproject->get_file(0);
  SgGlobal *root = sageFile.get_root();
  
  list<SgNode*> var_list = NodeQuery::querySubTree(root, V_SgVarRefExp);
  SgFunctionDeclaration* func_decl;	
  // need to have the function declaration, so that we can get the correct function symbol.  
  createFunctionDeclaration(root, var_list, func_decl);
  
  SgFunctionSymbol* func_symbol = new SgFunctionSymbol(func_decl);
  SgFunctionType* func_type = func_decl->get_type();
  SgFunctionRefExp* func_ref_exp = new SgFunctionRefExp(SgNULL_FILE, func_symbol, func_type);
  SgExprListExp* exp_list_exp = new SgExprListExp(SgNULL_FILE);
  SgFunctionCallExp *  func_call_expr=new SgFunctionCallExp(SgNULL_FILE,func_ref_exp,
							    exp_list_exp,func_type);
  SgFunctionCallExp* func_call = new SgFunctionCallExp(SgNULL_FILE,func_ref_exp,
                                                       exp_list_exp, func_type);
  SgExpressionRoot* expr_root_func_call = new SgExpressionRoot();
  
  
  SgExprStatement* func_expr = new SgExprStatement(SgNULL_FILE, func_call_expr);
  func_call_expr->set_parent(func_expr->get_expression_root());
  
  // add parameters in the function call
  char s[25]= "";
  for(list<SgNode*>::const_iterator it= var_list.begin(); it!=var_list.end(); it++){
    VariantT variant = isSgVarRefExp(*it)->get_symbol()->get_type()->variantT();
    
    switch(variant){
    case V_SgTypeInt:{
      cout << "V_SgTypeInt" << endl;
      char *d = "%d ";
      strcat(s,d);
    }
    case V_SgTypeFloat:{
      cout << "V_SgTypeFloat" << endl;
      char *f = "%f ";
      strcat(s,f);
    }
    case V_SgTypeString:{
      cout << "V_SgTypeString" << endl;
      char *s2 = "%s ";
      strcat(s,s2);
    }
    }
  }
  
  SgStringVal* string = new SgStringVal();
  string->set_value(s);
  cout << "The string value is: " << string->get_value() << endl;
  // Why is it not just "%d", but "%d %f %s"?

  // Here something is wrong...
  // SgCastExp* cast_exp = new SgCastExp(SgNULL_FILE, isSgExpression(string),
  //  string->get_type(),SgCastExp::c_cast_e);
  
  exp_list_exp->append_expression(isSgExpression(string));
  // output not correct...
  
  // append arguements from var_list
  for(list<SgNode*>::const_iterator it= var_list.begin(); it!=var_list.end(); it++){
    // Since I earlier ask for SgVarRefExp, they are all SgVarRefExp
      exp_list_exp->append_expression(isSgVarRefExp(*it));
  }
  
  
  list<SgNode*> bb_list = NodeQuery::querySubTree(root, V_SgBasicBlock);
  SgBasicBlock* bb = isSgBasicBlock(*(bb_list.begin()));
  bb->append_statement(func_expr);
 
  // QUESTION 1:
  // I know I have a SgVarRefExp of type Int in my test file. Why do all of the cases in the switch-
  // case construction happen? (Why does the variant of SgTypeInt match all variants?)
  // In other words: Why is it not the output of the string i make just "%d", but "%d %f %s"?

  // QUESTION 2:
  // How to append the string correctly, so that it appears in the unparsed code (rose_testfile.cpp)?

}

int main(int argc, char *argv[]){
  
  SgProject *sgproject = frontend(argc, argv);
  ROSE_ASSERT(sgproject != NULL);
	
  createFunctionCall(sgproject);
  
  sgproject->unparse();
  
  cout << "Finishes ok." << endl;
  return 0;
}

#endif
