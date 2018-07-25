/**
  Question of copying and references. If replaceExpression is performed, I need to store the original enclosing. When you replace expression, check enclosing statement for attribute, if not add it, if added then don't statement. I assume I need to deepCopy it. If the statement already has the attribute then nothing needs to be done.
  When replaceExpression is performed. REPLACE EXPRESSION IS CURRENTLY NOT PROPERLY STORING ORIGINAL (NOV 10 2016).
**/

#include "instr.h"

namespace si = SageInterface;

namespace Instr
{
  VariableSymbolMap_t classStructVarRemap;
  ScopeLockKeyMap     scopeLockMap;
  VariableSymbolMap_t varRemap;
  VariableSymbolMap_t reverseMap;

  int myCounter       = 0;
  int nodePosition    = 0;
  bool STAGGERED      = false;

  // buildWrapper and replaceWrapper are from outdated implementations
  void buildWrapper(std::string uninstrumented, SgLocatedNode* instrumented)
  {
    std::cout << uninstrumented << std::endl;

    std::string tag;
    char buffer[13]; //check and 8 digits

    sprintf(buffer, "Check%d", myCounter);
    ++myCounter;
    tag = (std::string) buffer;

    std::string ifdef = "\n#ifdef " + tag + "\n";

    si::addTextForUnparser(instrumented, ifdef, AstUnparseAttribute::e_before);

    std::string endif = "\n#else\n";

    endif = endif + uninstrumented + "\n" + "#endif\n";
    si::addTextForUnparser(instrumented, endif, AstUnparseAttribute::e_after);
  }

  void replaceWrapper(SgStatement* original, SgStatement* replacement)
  {
    std::string attrname = TempNameAttr::attributeName();

    if (!original->attributeExists(attrname))
    {
      SgStatement*  originalStatement = (SgStatement*) si::deepCopy(original);
      TempNameAttr* dontdoit = new TempNameAttr(originalStatement, replacement);

      dontdoit->setCurrent(replacement);
      originalStatement->addNewAttribute(attrname, dontdoit);
      replacement->addNewAttribute(attrname, dontdoit);
      si::replaceStatement(original, replacement);
    }
    else
    {
      TempNameAttr* dontdoit = dynamic_cast<TempNameAttr*>(original->getAttribute(attrname));

      dontdoit->setCurrent(replacement);
      replacement->addNewAttribute(attrname, dontdoit);
      si::replaceStatement(original, replacement);
    }
  }

  void replaceWrapper(SgExpression* original, SgExpression* replacement)
  {
    if (!STAGGERED)
    {
      si::replaceExpression(original, replacement);
      return;
    }

    std::string attrname = TempNameAttr::attributeName();

    if (  isSgFunctionParameterList(original)
       || isSgScopeStatement(original)
       || isSgFunctionCallExp(original)
       || isSgUnaryOp(original)
       )
    {
      ROSE_ASSERT(!si::getEnclosingStatement(original)->attributeExists(attrname));
      si::replaceExpression(original, replacement);
    }
    else
    {
      SgStatement* statement = si::getEnclosingStatement(original);

      if (!statement->attributeExists(attrname))
      {
        SgStatement* originalStatement = (SgStatement*) si::deepCopy(statement);

        si::replaceExpression(original, replacement);
        statement = si::getEnclosingStatement(replacement);

        TempNameAttr* dontdoit = new TempNameAttr(originalStatement, statement);

        originalStatement->addNewAttribute(attrname, dontdoit);
        statement->addNewAttribute(attrname, dontdoit);
      }
      else
      {
        si::replaceExpression(original, replacement);
      }
    }
  }

  void iterateOverNodes(SgProject* project)
  {
    // was: AstTests::runAllTests(si::getProject());
    AstTests::runAllTests(project);

    if (STAGGERED)
    {
      for ( NodeContainer::iterator i = Trav::NodesToInstrument.begin();
            i != Trav::NodesToInstrument.end();
            ++i
          )
      {
        /****
        #ifdef INSTR_DEBUG
        printf("Current Node\n");
        Util::printNode(*i);
        #endif
        //std::cout << (*i)->sage_class_name() << std::endl;
        if(isSgFunctionParameterList(*i) || isSgScopeStatement(*i)){
                instr(*i);
        }
        else if(isSgFunctionCallExp(*i)){
          instr(*i);
        }
        else if(isSgUnaryOp(*i))
          instr(*i);
        else if(itCounter >= nodePosition && flag){
          instr(*i);
          flag = false;
          nodePosition = itCounter;
        }
        else if(itCounter == Trav::NodesToInstrument.size() - 1){
          SgNode* temp = *i;
          //std::cout << temp->sage_class_name() << std::endl;
          instr(*i);
        }
        itCounter++;
        ****/
        instr(project, *i);
      }
    }
    else
    {
      for( NodeContainer::iterator i = Trav::NodesToInstrument.begin();
           i != Trav::NodesToInstrument.end();
           ++i
         )
      {
        instr(project, *i);
      }
    }
  }

  SgExpression*
  buildMultArgOverloadFn(SgName fn_name, SgExprListExp* parameter_list, SgType* retType, SgScopeStatement* scope, SgNode* pos, bool /*derived*/)
  {
    #ifdef OVLFN_DEBUG
    printf("buildMultArgOverloadFn\n");
    #endif

    // The parameter list or the retType cannot be Typedef types.
    ROSE_ASSERT(!isSgTypedefType(retType));

    SgExpressionPtrList& exprs = parameter_list->get_expressions();
    SgExpressionPtrList::iterator it = exprs.begin();

    for(; it != exprs.end(); ++it)
    {
      SgExpression* ce = *it;
      ROSE_ASSERT(!isSgTypedefType(ce->get_type()));
    }

    return funFact::buildFuncCall(fn_name, parameter_list, retType, scope, pos);
  }

SgExpression*
createStructUsingAddressOf(SgExpression* exp, SgNode* pos)
{
  #ifdef CS_ADDROF_DEBUG
  printf("createStructUsingAddressOf\n");
  Util::printNode(exp);
  #endif

  SgScopeStatement* scope;
  if (isSgVarRefExp(exp))
  {
    scope = Util::getScopeForVarRef(isSgVarRefExp(exp));
  }
  else
  {
    scope = Util::getScopeForExp(exp);
  }

  // Lvalue should be set as false since we are using the address
  // here to create a struct. Not writing to this expression at all
  SgExpression* copy_exp = si::copyExpression(exp);
  copy_exp->set_lvalue(false);

  SgExpression* addr_of = Util::castToAddr(Util::createAddressOfOpFor(copy_exp));

//  SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp->get_type()), Util::getScopeForExp(exp), true);
//  SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp->get_type()), GEFD(si::getEnclosingStatement(exp)), true);
  #ifdef REMOVE_TYPEDEFS_FROM_VARS
  SgType* exp_type = Util::skip_Typedefs(exp->get_type());
  SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp_type), pos, true);
  #else
  SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp->get_type()), pos, true);
  #endif


  #if 0
  if(isSgArrayType(exp->get_type())) {
    // If its an array type, then we need to use the pointer to the base type
    // struct
    SgType* base_type = (isSgArrayType(exp->get_type()))->get_base_type();
    retType = strDecl::getStructType(SgPointerType::createType(base_type), true);
  }
  else {
    retType = strDecl::getStructType(exp->get_type(), true);
  }
  #endif


  #if 0
  printf("exp->get_parent()\n");
  Util::printNode(exp->get_parent());
  #endif

#ifdef OLD_NAME_GEN
  SgName fn_name(getStringForFn("create_struct_from_addr", addr_of->get_type(), NULL, NULL, retType));
//  SgExpression* createfn = buildOverloadFn(fn_name, addr_of, NULL, retType, scope);
//  SgExpression* createfn = buildOverloadFn(fn_name, addr_of, NULL, retType, scope, GEFD(si::getEnclosingStatement(exp)));
//  SgExpression* createfn = buildOverloadFn(fn_name, addr_of, NULL, retType, scope, pos);
  SgExpression* createfn = buildMultArgOverloadFn(fn_name, SB::buildExprListExp(addr_of), retType, scope, pos);
#else
//  SgExpression* createfn = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, scope);
//  SgExpression* createfn = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, scope, GEFD(si::getEnclosingStatement(exp)));
//  SgExpression* createfn = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, scope, pos);
  SgExpression* createfn = buildMultArgOverloadFn("create_struct_from_addr", SB::buildExprListExp(addr_of), retType, scope, pos);
#endif

  return createfn;
}

void insertExecFunctions(SgProject* project)
{
  SgFunctionDeclaration* mainFn = si::findMain(project);

  if (!mainFn) {
    #ifdef INSERT_EXECFN_DEBUG
    printf("Can't find Main function. Not inserting execAtFirst and execAtLast\n");
    #endif
    return;
  }

  SgScopeStatement* scope = mainFn->get_scope();

  SgFunctionDefinition* mainFnDef = mainFn->get_definition();

  SgBasicBlock* FirstBlock = mainFnDef->get_body();

  SgStatementPtrList& Stmts = FirstBlock->get_statements();


  #if 0
  SgExpression* overload = buildOverloadFn(SgName("execAtFirst"), NULL, NULL, SgTypeVoid::createType(),
             scope, mainFn);
  #endif
  SgExpression* overload = buildMultArgOverloadFn(SgName("execAtFirst"), SB::buildExprListExp(), SgTypeVoid::createType(),
             scope, mainFn);
  si::insertStatementBefore(*(Stmts.begin()), SB::buildExprStatement(overload));

  #if 0
  overload = buildOverloadFn(SgName("execAtLast"), NULL, NULL, SgTypeVoid::createType(),
             scope, mainFn);
  #endif
  overload = buildMultArgOverloadFn(SgName("execAtLast"), SB::buildExprListExp(), SgTypeVoid::createType(),
             scope, mainFn);

  SgStatementPtrList::iterator i = Stmts.begin();
  i += (Stmts.size() - 1);

  si::insertStatementBefore(*i, SB::buildExprStatement(overload));


  #ifdef RMM_TYPETRACKER
  //si::insertHeader("prototype_typetracker.h", PreprocessingInfo::after,0,scope);

  #ifdef RMM_TYPETRACKER_REPORT
  SgExprStatement* traverseStmt = SB::buildFunctionCallStmt(SgName("traverseAndPrint"),SgTypeVoid::createType(),SB::buildExprListExp(),scope);
  i = Stmts.begin(); i += (Stmts.size() - 1);
  si::insertStatementAfter(*i, traverseStmt);
  #endif

  #endif

}

void handleArgvInitialization(SgProject* project)
{
  // Adding at the top of the main function:
  // create_dummy_entry(&argv)
  // argc_type index;
  // for(index = 0; index < argc; index++)
  //      create_dummy_entry(&argv[index]);

  SgFunctionDeclaration* mainFn = si::findMain(project);

  if(!mainFn) {
    #ifdef ARGV_INIT
    printf("handleArgvInit:Can't find main\n");
    #endif
    return;
  }

  SgFunctionDefinition* mainFnDef = mainFn->get_definition();

  SgBasicBlock* BB = mainFnDef->get_body();

  // Now, check if it has args... and if so, there should be two
  SgInitializedNamePtrList& args = mainFn->get_args();

  ROSE_ASSERT((args.size() == 0) || (args.size() == 2));

  if(args.size() == 0) {
    // Nothing to do in this case
    return;
  }

  // Get the argc
  SgInitializedName* argc_var = *args.begin();

  // Get the argv
  SgInitializedName* argv_var = *(args.begin() + 1);

  // insert: create_dummy_entry(&argv)
  SgExpression* addr_argv = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(argv_var)));
  SgExprListExp* p_list = SB::buildExprListExp(addr_argv);

  SgExpression* argv_ovl = buildMultArgOverloadFn("create_dummy_entry", p_list,
              SgTypeVoid::createType(),
              si::getScope(BB),
              GEFD(si::getEnclosingStatement(BB)));

  SgExprStatement* argv_ovl_stmt = SB::buildExprStatement(argv_ovl);
  Util::insertAtTopOfBB(BB, argv_ovl_stmt);


  // Create an index variable to iterator over argv
  // argc_type index;
  SgVariableDeclaration* index_var = SB::buildVariableDeclaration(SgName("index_argc"),
                  argc_var->get_type(),
                  NULL,
                  si::getScope(BB));

  si::insertStatementAfter(argv_ovl_stmt, index_var);

  // Now, create a for loop which will iterate over the argv and create dummy entries

  // for init stmt
  // index = 0
  SgAssignOp* init_assign = SB::buildAssignOp(SB::buildVarRefExp(index_var), SB::buildIntVal(0));
  //SgForInitStatement* init_stmt = buildForInitStatement(init_assign);

  // test statement
  // index < argc
  SgExpression* test_exp = SB::buildLessThanOp(SB::buildVarRefExp(index_var), SB::buildVarRefExp(argc_var));
  SgStatement* test_stmt = SB::buildExprStatement(test_exp);

  // increment
  SgExpression* incr_exp = SB::buildPlusPlusOp(SB::buildVarRefExp(index_var));

  // Loop body
  // create_dummy_entry(&argv[index])
  SgExpression* argv_index = SB::buildPntrArrRefExp(SB::buildVarRefExp(argv_var), SB::buildVarRefExp(index_var));
  SgExpression* addr_argv_index = Util::castToAddr(Util::createAddressOfOpFor(argv_index));

  SgExprListExp* param_list = SB::buildExprListExp(addr_argv_index);

  SgExpression* overload = buildMultArgOverloadFn("create_dummy_entry", param_list, SgTypeVoid::createType(),
              si::getScope(BB), GEFD(si::getEnclosingStatement(BB)));

  SgStatement* ovl_stmt = SB::buildExprStatement(overload);

  //SgForStatement* for_stmt = SB::buildForStatement(init_stmt, test_stmt, incr_exp, ovl_stmt);
  SgForStatement* for_stmt = SB::buildForStatement(SB::buildExprStatement(init_assign), test_stmt, incr_exp, ovl_stmt);

  si::insertStatementAfter(index_var, for_stmt);
}


void iterateOverNodes(SgProject*);
void insertExecFunctions(SgProject*);
void handleArgvInitialization(SgProject*);

void instrument(SgProject* proj, int pos)
{
  nodePosition = pos;
  iterateOverNodes(proj);
  handleArgvInitialization(proj);
  insertExecFunctions(proj);
}

void instr(SgProject* project, SgExpression* exp) {
  #ifdef INSTR_DEBUG
  printf("  instr expression\n");
  #endif
  if(isSgBinaryOp(exp)) {
    #ifdef INSTR_DEBUG
    printf("    instr SgBinaryOp\n");
    #endif
    instr(isSgBinaryOp(exp));
  }
  else if(isSgCallExpression(exp)) {
      #ifdef INSTR_DEBUG
      printf("    instr SgCallExpression\n");
      #endif
    instr(project, isSgCallExpression(exp));
  }
  else if(isSgVarRefExp(exp)) {
      #ifdef INSTR_DEBUG
      printf("    instr SgVarRefExpression\n");
      #endif
    instr(isSgVarRefExp(exp));
  }
  else if(isSgUnaryOp(exp)) {
      #ifdef INSTR_DEBUG
      printf("    instr SgUnaryOp\n");
      #endif
    instr(isSgUnaryOp(exp));
  }
  else if(isSgThisExp(exp)) {
      #ifdef INSTR_DEBUG
      printf("    instr isSgThisExpression\n");
      #endif
    instr(isSgThisExp(exp));
  }
  else if(isSgNewExp(exp)) {
      #ifdef INSTR_DEBUG
      printf("    instr SgNewExpression\n");
      #endif
    instr(isSgNewExp(exp));
  }
  else if(isSgDeleteExp(exp)) {
      #ifdef INSTR_DEBUG
      printf("    instr SgDeleteExpression\n");
      #endif
    instr(isSgDeleteExp(exp));
  }
  else if(isSgSizeOfOp(exp)) {
      #ifdef INSTR_DEBUG
      printf("    instr SgSizeOfOp\n");
      #endif
    instr(isSgSizeOfOp(exp));
  }
  else if(isSgRefExp(exp)) {
      #ifdef INSTR_DEBUG
      printf("    instr SgRefExpression\n");
      #endif
    instr(isSgRefExp(exp));
  }
  #ifdef INSTR_DEBUG
  else
    printf("    no match\n");
  #endif

}

void instr(SgStatement* stmt) {
  if(isSgScopeStatement(stmt)) {
    #ifdef INSTR_DEBUG
    printf("  instr scope statement\n");
    #endif
    instr(isSgScopeStatement(stmt));
  }
  else if(isSgDeclarationStatement(stmt)) {
    #ifdef INSTR_DEBUG
    printf("  instr declaration statement\n");
    #endif
    instr(isSgDeclarationStatement(stmt));
  }
  else if(isSgReturnStmt(stmt)) {
    #ifdef INSTR_DEBUG
    printf("  instr return statement\n");
    #endif
    instr(isSgReturnStmt(stmt));
  }
}

void instr(SgProject* proj, SgNode* node) {
  #ifdef INSTR_DEBUG
  printf("instr node\n");
  #endif

  if(isSgExpression(node)) {
    instr(proj, isSgExpression(node));
  }
  else if(isSgStatement(node)) {
    instr(isSgStatement(node));
  }

}

}
