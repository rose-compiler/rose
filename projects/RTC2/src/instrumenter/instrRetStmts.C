#include "instr.h"

namespace Instr {

  void handleClassStructReturnStmts(SgReturnStmt* retstmt) {

#ifdef CLASSSTRUCT_RETURN_DEBUG
  printf("handleClassStructReturnStmts\n");
#endif

#ifdef RETAIN_FUNC_PROTOS
  // Nothing to do for now...
  // FIXME: Should push onto the stack, element
  // by element for C structs
  return;
#endif

  SgType* retType = retstmt->get_expression()->get_type();

#ifdef STRIP_TYPEDEFS
  retType = Util::getType(retType);
#endif

  ROSE_ASSERT(strDecl::isOriginalClassType(retType));

//  SgFunctionDeclaration* fn_decl = getFuncDeclForNode(retstmt);
  SgFunctionDeclaration* fn_decl = GEFD(retstmt);

  SgInitializedNamePtrList& args = fn_decl->get_args();

  // Now, find the last arg in the list.
  SgInitializedNamePtrList::iterator i = args.begin();
  i += args.size() - 1;
  SgInitializedName* last_arg = *i;

#ifdef CLASSSTRUCT_RETURN_DEBUG
  printf("arg check:\n");
  Util::printNode(last_arg);
#endif

  // Check that its type is of the pointer type of the ret stmt type
  SgType* last_arg_type = last_arg->get_type();
#ifdef STRIP_TYPEDEFS
  last_arg_type = Util::getType(last_arg_type);
#endif
  ROSE_ASSERT(isSgPointerType(last_arg->get_type()));

  SgType* last_arg_base_type = isSgPointerType(last_arg_type)->get_base_type();
#ifdef STRIP_TYPEDEFS
  last_arg_base_type = Util::getType(last_arg_base_type);
#endif
  ROSE_ASSERT(last_arg_base_type == retType);

  // Now, do the assignment and void return substitution
  // original: return ret_exp
  // transformed: *retvar = ret_exp
  //              return

  // this copies the expression in the return statement and uses it as rhs of the assign op
  // *retvar = ret_exp;
  SgAssignOp* assign_op = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(last_arg)), SI::copyExpression(retstmt->get_expression()));
  SgExprStatement* assign_stmt = SB::buildExprStatement(assign_op);

  SgStatement* voidretstmt = SB::buildReturnStmt();

  replaceWrapper(retstmt, assign_stmt);

#ifdef LOCK_KEY_INSERT
#ifdef CLASSSTRUCT_RETURN_DEBUG
  printf("LOCK_KEY_INSERT: inserting ExitScope before last return\n");
#endif
  //SgExpression* overload = buildOverloadFn("ExitScope", NULL, NULL, SgTypeVoid::createType(), SI::getScope(retstmt), GEFD(retstmt));
  SgExpression* overload = buildMultArgOverloadFn("ExitScope", SB::buildExprListExp(), SgTypeVoid::createType(), SI::getScope(retstmt), GEFD(retstmt));

  SgStatement* exit_scope = SB::buildExprStatement(overload);
  // assign_stmt
  // exit_scope
  // return
  SI::insertStatementAfter(assign_stmt, exit_scope);
  SI::insertStatementAfter(exit_scope, voidretstmt);
#else
  SI::insertStatementAfter(assign_stmt, voidretstmt);
#endif

  return;
}


  void handleNormalReturnStmts(SgReturnStmt* retstmt) {

    // We handle normal return statements (int/char returns... this also handles returns for main... for global scope)
    // So, add one extra ExitScope if the current function is main... this ExitScope removes the global scope from the
    // scope stack...
    //SgExpression* overload = buildOverloadFn("ExitScope", NULL, NULL, SgTypeVoid::createType(), SI::getScope(retstmt), GEFD(retstmt));
    SgExpression* overload = buildMultArgOverloadFn("ExitScope", SB::buildExprListExp(), SgTypeVoid::createType(), SI::getScope(retstmt), GEFD(retstmt));
    SI::insertStatementBefore(retstmt, SB::buildExprStatement(overload));

    // if the current function is main...
    SgFunctionDeclaration* parent_fn = Util::getFnFor(retstmt);
    if(SI::isMain(parent_fn)) {
      // insert ExitScope again... to remove the global scope lock and key
      SgExpression* exit_scope_2 = SI::copyExpression(overload);
      SI::insertStatementBefore(retstmt, SB::buildExprStatement(exit_scope_2));
    }
  }

  void handleQualReturnStmts(SgReturnStmt* retstmt) {

    SgExpression* exp = retstmt->get_expression();
    SgType* exp_type = exp->get_type();
#ifdef STRIP_TYPEDEFS
    exp_type = Util::getType(exp_type);
#endif

    if(isSgArrayType(exp_type)) {
      // Need to insert create entry as well...
      insertCreateEntry(SI::copyExpression(exp), SI::getScope(retstmt),
          SI::getEnclosingStatement(retstmt));
    }

    // call create_struct
    SgExpression* ptr = SI::copyExpression(exp);
#ifdef STRIP_TYPEDEFS
    ptr = Util::castAwayTypedef(ptr);
#endif

    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(exp)));


#ifdef RETAIN_FUNC_PROTOS
    // transform from: return ptr to: return (push(addr), ptr)
    // 1. push(addr)
    SgExpression* pushcall = buildMultArgOverloadFn("push_to_stack", SB::buildExprListExp(addr),
        SgTypeVoid::createType(), SI::getScope(retstmt),
        GEFD(retstmt));

    // 2. push(addr), ptr
    SgExpression* ret_exp = SB::buildCommaOpExp(pushcall, ptr);
    // 3. replace exp with ret_exp
    replaceWrapper(exp, ret_exp);

#else
    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(exp_type),
        GEFD(SI::getEnclosingStatement(retstmt)),
        true);

    SgExprListExp* param_list = SB::buildExprListExp(ptr, addr);

    SgExpression* fncall = buildMultArgOverloadFn("create_struct", param_list,
        retType, SI::getScope(retstmt),
        GEFD(SI::getEnclosingStatement(retstmt)));

    replaceWrapper(exp, fncall);
#endif
  }

  void unwrapStructAndPush(SgStatement* retstmt) {

    SgExpression* exp = isSgReturnStmt(retstmt)->get_expression();

    // transform from: str_var to: str temp; return ((temp = exp, push(temp.addr)), str.ptr)

    // read the struct into a variable...
    // 1. create a local varible... argvar
    SgName var_name("temp_" + boost::lexical_cast<std::string>(Util::VarCounter++));
    SgVariableDeclaration* temp = SB::buildVariableDeclaration(var_name, exp->get_type(), NULL, SI::getScope(retstmt));
    SI::insertStatementBefore(retstmt, temp);

    // 2. temp = exp;
    SgExpression* temp_assign = SB::buildAssignOp(SB::buildVarRefExp(temp), SI::copyExpression(exp));

    // 3. push(temp.addr)
    SgExprListExp* cs_p = SB::buildExprListExp(Util::createDotExpFor(Util::getNameForDecl(temp), "addr"));
    SgExpression* pushcall = buildMultArgOverloadFn("push_to_stack", cs_p,
        SgTypeVoid::createType(), SI::getScope(retstmt),
        GEFD(retstmt));

    // 4. temp=exp, push(temp.addr)
    SgExpression* comma_op = SB::buildCommaOpExp(temp_assign, pushcall);

    // 5. ((temp=exp, push(temp.addr)), str.ptr)
    comma_op = SB::buildCommaOpExp(comma_op, Util::createDotExpFor(Util::getNameForDecl(temp), "ptr"));

    // 6. replace exp with comma_op
    replaceWrapper(exp, comma_op);
  }

  void instr(SgReturnStmt* retstmt) {

#ifdef RETURN_STMT_DEBUG
    printf("handleReturnStmts\n");
    Util::printNode(retstmt);
#endif

    SgExpression* exp = retstmt->get_expression();
    SgType* exp_type = exp->get_type();
#ifdef STRIP_TYPEDEFS
    exp_type = Util::getType(exp_type);
#endif

    if(strDecl::isOriginalClassType(exp_type)) {
      // This would need to be handled slightly differently...
#ifdef RETURN_STMT_DEBUG
      printf("exp is orig class type\n");
#endif
      handleClassStructReturnStmts(retstmt);
    }
    else if(Util::isQualifyingType(exp_type)) {
      // This needs to be converted to a struct
#ifdef RETURN_STMT_DEBUG
      printf("exp is qual type\n");
#endif
      handleQualReturnStmts(retstmt);
    }
#ifdef CPP_SCOPE_MGMT
    else if(strDecl::isValidStructType(exp_type, GEFD(retstmt)))
#else
    else if(strDecl::isValidStructType(exp_type))
#endif
    {
#ifdef RETAIN_FUNC_PROTOS
      // Should return the pointer, and push the
      // metadata onto the stack
      unwrapStructAndPush(retstmt);
#endif
      // The exp has been converted to valid struct type
      // No need to convert
#ifdef LOCK_KEY_INSERT
      handleNormalReturnStmts(retstmt);
#endif
    }
#ifdef LOCK_KEY_INSERT
    else {
      // Here, we simply insert exit_scope
      handleNormalReturnStmts(retstmt);
    }
#else
    else {
      // We shouldn't be here if we are not inserting exit scope
      printf("handleReturnStmts: retstmt: ");
      Util::printNode(retstmt);
      printf("What are we doing here?");
      ROSE_ASSERT(0);
    }
#endif
    return;
  }

}
