#include "funcDecl.h"

//RMM CHANGE
//#define FUNFACT_DEBUG

//RMM
//#define RMM_TYPETRACKER
//RMM

void funcDecl::makeDeclStatic() {
  if(!SI::isStatic(fndecl)) {
    SI::setStatic(fndecl);
  }
}

funcDecl::funcDecl(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived
      ): fnName(fnName),
        paramList(paramList),
        retType(retType),
        scope(scope),
        pos(pos),
        derived(derived),
        transfName(Util::getTransfName(fnName, retType, paramList)){

  #ifdef FNDECL_DEBUG
  printf("Creating decl\n");
  printf("transf_name: %s\n", transf_name.str());
  #endif
  args = SB::buildFunctionParameterList();
  SgExpressionPtrList& exprs = paramList->get_expressions();

  #ifdef FNDECL_DEBUG
  printf("creating param list\n");
  #endif

  unsigned int argCounter = 0;
  for(SgExpressionPtrList::iterator i = exprs.begin(); i != exprs.end(); i++) {
    argCounter++;
    SgExpression* ce = *i;

    #ifdef FNDECL_DEBUG
    printf("ce: %s = %s\n", isSgNode(ce)->sage_class_name(), isSgNode(ce)->unparseToString().c_str());
    #endif
    std::string argName = "input" + boost::lexical_cast<std::string>(argCounter);
    SgInitializedName* newName = SB::buildInitializedName(SgName(argName),ce->get_type(), NULL);
    args->append_arg(newName);
    newName->set_parent(args);
  }

}

void nonDefDecl::placeTheDecl() {
  // create a non defining decl.
  // This is function is only called for externally implemented functions...


  fndecl = SB::buildNondefiningFunctionDeclaration(transfName,
                retType,
                args,
                isSgStatement(pos)->get_scope());
                  //SI::getGlobalScope(pos));

  // If its a normal func decl... using insertstmtbefore,
  // else get the class declaration... then, insertstmtbefore
  if(isSgMemberFunctionDeclaration(pos)) {
    SgScopeStatement* scope = isSgMemberFunctionDeclaration(pos)->get_scope();
    SgClassDefinition* def = isSgClassDefinition(scope);
    pos = def->get_parent();
    ROSE_ASSERT(isSgClassDeclaration(pos));
  }

  //RMM: We ran into a problem where a struct declaration was placed inside of a typedef, and a
  //typedef declaration is not a scope. This confuses the insertion routine and causes a crash.
  if(isSgScopeStatement(pos->get_parent())){
    SI::insertStatementBefore(isSgStatement(pos), fndecl);
  }  else {
    SI::insertStatementBefore(isSgStatement(pos->get_parent()),fndecl);
  }

}

nonDefDecl::nonDefDecl(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : funcDecl(fnName, paramList, retType, scope, pos, derived) {

  placeTheDecl();
}

void defDecl::placeTheDecl() {
  // create a defining decl
  if(isSgMemberFunctionDeclaration(funcDecl::pos)) {
    SgScopeStatement* scope = isSgStatement(funcDecl::pos)->get_scope();
    ROSE_ASSERT(isSgClassDefinition(scope));
    fndecl = SB::buildDefiningMemberFunctionDeclaration(transfName, retType, args, scope);
    scope->insertStatementInScope(fndecl, false);

  }
  else {
    SgScopeStatement* scope = isSgStatement(funcDecl::pos)->get_scope();
    // For C++, we should consider namespace definition as another
    // possible place for insertion of the struct.
    ROSE_ASSERT(isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope));
    fndecl = SB::buildDefiningFunctionDeclaration(transfName, retType, args, scope);


    //SI::insertStatementBefore(isSgStatement(funcDecl::pos), fndecl);

    //RMM: We ran into a problem where a struct declaration was placed inside of a typedef, and a
    //typedef declaration is not a scope. This confuses the insertion routine and causes a crash.
    if(isSgScopeStatement(funcDecl::pos->get_parent())){
      SI::insertStatementBefore(isSgStatement(funcDecl::pos), fndecl);
    }  else {
      SI::insertStatementBefore(isSgStatement(funcDecl::pos->get_parent()),fndecl);
    }



  }

}

SgStatementPtrList& defDecl::getStmts() {
  return body->get_statements();
}

SgInitializedNamePtrList& defDecl::getArgs() {
  return fndecl->get_args();
}

void defDecl::insertReturnStmt(SgExpression* ret_exp) {

  SgReturnStmt* ret_stmt = SB::buildReturnStmt(ret_exp);
  appendStmtToBody(ret_stmt);
}

void defDecl::appendStmtToBody(SgStatement* stmt) {
  body->append_statement(stmt);
}


defDecl::defDecl(SgName fnName,
    SgExprListExp* paramList,
    SgType* retType,
    SgScopeStatement* scope,
    SgNode* pos, bool derived) : funcDecl(fnName, paramList, retType, scope, pos, derived) {

  placeTheDecl();
  def = fndecl->get_definition();
  body = def->get_body();
  makeDeclStatic();
}



void createStrFn::fillOutBody() {
  // There are two types of functions which end up here.
  // 1. create_struct(orig_ptr, &orig_ptr)
  // translates to create_struct(input1, input2)
  // str output;
  //output.ptr = input1;
  //output.addr = input2;
  //return output;

  // 2. create_struct(orig_ptr, &orig_ptr, size)
  // translates to create_struct(input1, input2, input3)
  // str output;
  //output.ptr = input1;
  //output.addr = input2;
  // This create_entry takes the lock and key from the top of the stack...
  // doesn't create a new lock
  //create_entry(input2, reinterpret_cast<unsigned long long>(input1), input3);
  //return output;

  // create local variable of return type
  SgVariableDeclaration* outputVar = Util::createLocalVariable("output", retType, NULL, body);
  appendStmtToBody(outputVar);

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();
  ROSE_ASSERT(args.size() >= 2);

  // Lets pluck the first argument out of the fndecl
  SgInitializedName* arg1 = *iter;

  // Now, pluck the second argument out
  iter++;
  SgInitializedName* arg2 = *iter;

  SgExprStatement* ptrAssign = Util::createDotAssign(Util::getNameForDecl(outputVar), "ptr", SB::buildVarRefExp(arg1));
  appendStmtToBody(ptrAssign);

  SgExprStatement* addrAssign = Util::createDotAssign(Util::getNameForDecl(outputVar), "addr", SB::buildVarRefExp(arg2));
  appendStmtToBody(addrAssign);

  // Now, use buildOverloadFn to form a create_entry, if a third argument exists.
  if(args.size() == 3) {
    // Get the third arg.
    ++iter;
    SgInitializedName* arg3 = *iter;

    // the function call: create_entry(input2, reinterpret_cast<unsigned long long>(input1), input3);
    SgExpression* ceArg1 = SB::buildVarRefExp(arg2);
    SgExpression* ceArg2 = Util::castToAddr(SB::buildVarRefExp(arg1));
    // cast it to size type...
    SgExpression* ceArg3 = SB::buildCastExp(SB::buildVarRefExp(arg3), Util::getSizeOfType(), SgCastExp::e_C_style_cast);

    SgExpression* ceCall = funFact::buildDerivedFuncCall("create_entry", SB::buildExprListExp(ceArg1, ceArg2, ceArg3), SgTypeVoid::createType(), body, fndecl);

    appendStmtToBody(SB::buildExprStatement(ceCall));

  }

  // Now, for the return value
  // the return stmt: return output;
  insertReturnStmt(SB::buildVarRefExp(outputVar));
}

void createStrFromAddrFn::fillOutBody() {
  // This fn takes in an address, and create a struct out of it.
  // fn call: create_struct_from_addr(input1)
  // str output;
  // output.ptr = *(reinterpret_cast<class node**>(input1));
  // output.addr = input1;
  // return output;
  //
  // Convert the input1 to the addr of the underlying type of the return type. :)

  // Assert that we are returning a struct type...
  //RMM TEST... removing assertion
  //ROSE_ASSERT(strDecl::isValidStructType(retType, isSgFunctionDeclaration(pos)));

  // create local variable of return type
  SgVariableDeclaration* outputVar = Util::createLocalVariable("output", retType, NULL, body);
  appendStmtToBody(outputVar);

  // the args
  SgInitializedNamePtrList& args = getArgs();
  // create_struct_using_addr has only one addr as arg
  ROSE_ASSERT(args.size() == 1);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Lets pluck the first argument out of the fndecl
  SgInitializedName* arg1 = *iter;

  // Create the RHS for the first assignment above: output.ptr = *(reinterpret_cast<class node**>(input1))
  SgExpression* inputRef = SB::buildVarRefExp(arg1);
  // Pointer to the underlying ptr, in the return type
  SgType* retTypePtr = strDecl::findInUnderlyingType(retType);
  ROSE_ASSERT(retTypePtr != NULL);
  SgType* ptrOf = SgPointerType::createType(retTypePtr);
  SgExpression* inputCasted = SB::buildCastExp(inputRef, ptrOf, CAST_TYPE);
  // Now, deref it.
  SgExpression* inputCastDeref = SB::buildPointerDerefExp(inputCasted);

  // this is the ptr assignment: output.ptr = *(reinterpret_cast<class node**>(input1))
  SgExprStatement* ptrAssign = Util::createDotAssign(Util::getNameForDecl(outputVar), "ptr", inputCastDeref);
  appendStmtToBody(ptrAssign);

  // this is the addr assignment: output.addr = input
  SgExprStatement* addrAssign = Util::createDotAssign(Util::getNameForDecl(outputVar), "addr", SB::buildVarRefExp(arg1));
  appendStmtToBody(addrAssign);

  // the return stmt: return output
  insertReturnStmt(SB::buildVarRefExp(outputVar));
}

void rtcCopyFn::fillOutBody() {
  // this fn takes two pointer arguments.
  // fncall: rtc_copy(*dest, *src)
  // body: foreach pointer arg in struct
  // create_entry(dest.ptr, src.ptr)

  // Here, we need to get hold of the params in def_fn_decl.
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator i = args.begin();
  // get the first arg
  SgInitializedName* destArg = *i;
  ROSE_ASSERT(isSgPointerType(destArg->get_type()));
  SgType* destBaseType = isSgPointerType(destArg->get_type())->get_base_type();
  ROSE_ASSERT(strDecl::isOriginalClassType(destBaseType));


  // get the second arg
  ++i;
  SgInitializedName* srcArg = *i;
  ROSE_ASSERT(isSgPointerType(srcArg->get_type()));
  SgType* srcBaseType = isSgPointerType(srcArg->get_type())->get_base_type();
  ROSE_ASSERT(strDecl::isOriginalClassType(srcBaseType));

  // Now, find all the pointers in the data structure, and do a source to dest copy...
  SgClassType* strType = isSgClassType(destBaseType);

  // This part below gets us the pointers...
  SgClassDeclaration * strDecl = isSgClassDeclaration (strType->get_declaration());

  ROSE_ASSERT (strDecl);

    ROSE_ASSERT(strDecl->get_firstNondefiningDeclaration()!= NULL);

    ROSE_ASSERT(isSgClassDeclaration(strDecl->get_firstNondefiningDeclaration()) == strDecl);

  SgClassDeclaration* strDeclDef = isSgClassDeclaration(strDecl->get_definingDeclaration());

  SgClassDefinition* strDef = strDeclDef->get_definition();

  ROSE_ASSERT(strDef != NULL);

  SgDeclarationStatementPtrList& members = strDef->get_members();

  SgDeclarationStatementPtrList::iterator iter = members.begin();

  // Now, as we iterate over the members... we check if they are pointer type,
  // and if they are, perform a create_entry(dest, src)... We will accumulate all
  // these create_entry fn calls into a single expr list exp.. which will be returned
  SgExprListExp* exprs = SB::buildExprListExp();

  for(; iter != members.end(); ++iter) {

    SgDeclarationStatement* declStmt = *iter;
    // Function calls within structs/classes in C++...

    if(!isSgVariableDeclaration(declStmt)) {
      continue;
    }

    SgVariableDeclaration* varDecl = isSgVariableDeclaration(declStmt);
    SgInitializedName* varName = Util::getNameForDecl(varDecl);


    // Util::createAddressOfOpFor strips off the typedefs...
    SgExpression* destArrow = SB::buildArrowExp(SB::buildVarRefExp(destArg), SB::buildVarRefExp(varName));
    SgExpression* ce1 = Util::createAddressOfOpFor(destArrow);

    SgExpression* srcArrow = SB::buildArrowExp(SB::buildVarRefExp(srcArg), SB::buildVarRefExp(varName));
    SgExpression* ce2 = Util::createAddressOfOpFor(srcArrow);

    SgType* retType = SgTypeVoid::createType();

    SgScopeStatement* scope = SI::getScope(body);

    SgType* varType = varName->get_type();
    #ifdef STRIP_TYPEDEFS
    varType = Util::getType(varType);
    #endif

    if(isSgPointerType(varType)) {
      // Now we have a pointer member... we need to create:

      // create_entry(&(dest->var_name), &(src->var_name))
      // casting to addr
      // Both args casted, so no need to strip typedefs
      SgExprListExp* p_list = SB::buildExprListExp(Util::castToAddr(ce1), Util::castToAddr(ce2));

      SgExpression* ovl = funFact::buildDerivedFuncCall("create_entry_if_src_exists", p_list, retType, scope, fndecl);

      exprs->append_expression(ovl);
    }
    else if(strDecl::isOriginalClassType(varType)) {
      // This is a class/struct member. Generate an rtc copy for this one.

      SgExprListExp* p_list = SB::buildExprListExp(ce1, ce2);
      SgExpression* ovl = funFact::buildDerivedFuncCall("rtc_copy", p_list, retType, scope, fndecl);
      exprs->append_expression(ovl);
    }
  }

  appendStmtToBody(SB::buildExprStatement(exprs));
}

void addressOfFn::fillOutBody() {

  // input: AddressOf(addr, ptr, size, lock)
  // body: create_entry(addr, (addr_type)ptr, size, lock);
  // return create_struct(ptr, addr)

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();
  // addr
  SgInitializedName* addr = *iter;
  // ptr
  iter++;
  SgInitializedName* ptr = *iter;
  // size
  iter++;
  SgInitializedName* size = *iter;
  // lock
  iter++;
  SgInitializedName* lock = *iter;

  SgExprListExp* pList = SB::buildExprListExp(SB::buildVarRefExp(addr), Util::castToAddr(SB::buildVarRefExp(ptr)),
                      SB::buildVarRefExp(size), SB::buildVarRefExp(lock));

  SgExpression* ovl = funFact::buildDerivedFuncCall("create_entry", pList, SgTypeVoid::createType(),
                          scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl));

  SgExprListExp* pList2 = SB::buildExprListExp(SB::buildVarRefExp(ptr), SB::buildVarRefExp(addr));

  SgExpression* ovl2 = funFact::buildDerivedFuncCall("create_struct", pList2, retType, scope, fndecl);
  insertReturnStmt(ovl2);
}

void derefFn::fillOutBody() {
  // fn call: deref(input1_str)
  // check_entry((unsigned long long)input1_str.ptr, input1_str.addr)
  // return input_str.ptr;

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  // deref has only one str arg
  ROSE_ASSERT(args.size() == 1);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // the arg is of struct type.
  ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), isSgFunctionDeclaration(pos)));

  // get input1_str.ptr and cast it to unsigned long long
  SgExpression* input1Ptr = Util::castToAddr(Util::createDotExpFor(arg1, "ptr"));
  // get input1_str.addr
  SgExpression* input1Addr = Util::createDotExpFor(arg1, "addr");

  // The alternative to "pos" would be findInsertLocation(fnbody) -- this may not work with the Namespace Definition stuff
  // in C++, since the scope of this declaration won't be the same of us insertion...
  SgExpression* ce = funFact::buildDerivedFuncCall("check_entry", SB::buildExprListExp(input1Ptr, input1Addr),
                          SgTypeVoid::createType(),
                          scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ce));
  insertReturnStmt(Util::createDotExpFor(arg1, "ptr"));
}

void pntrArrRefFn::fillOutBody() {
  // fn call: pntrarrref(input1_str, index)
  // return input1_str.ptr + index;

  // the args
  SgInitializedNamePtrList& args = getArgs();
  // pntrarrref has two args. first is of struct type
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // the first arg is of struct type
  ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), fndecl));

  // create pointer dot exp
  SgExpression* ptr_exp = Util::createDotExpFor(arg1, "ptr");

  // get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  // Now, create the add op, in the return stmt...
  insertReturnStmt(SB::buildAddOp(ptr_exp, SB::buildVarRefExp(arg2)));
}

void assignFn::fillOutBody() {
  // Two cases:
  // Case 1:
  // this fn takes in two structs. first by ref, second by val
  // and performs as "assignment".
  // fncall: assign(*input1_str, input2_str)
  // input1_str->ptr = input2_str.ptr;
  // create_entry(input1_str->addr, input2_str.addr);
  // return *input1_str;
  //
  // Case 2:
  // this fn takes in one struct and a char* (string val)
  // and performs an "assignment".
  // fncall: assign(*input_str, input2)
  // input1_str->ptr = input2
  // create_entry(input1_str->addr, input1_str->ptr, sizeof(input2))
  // return *input1_str;

  // the args
  SgInitializedNamePtrList& args = getArgs();
  // assign has two args. first is of pointer type
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // first arg is pointer type -- pointer to struct
  ROSE_ASSERT(isSgPointerType(arg1->get_type()));

  SgExpression* ceCall;
  // Get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  if(strDecl::isValidStructType(arg2->get_type(), GEFD(fndecl)))
  {
    // second arg is struct type

    // input2_str.ptr
    SgExpression* ptrExp = Util::createDotExpFor(arg2, "ptr");

    // input1_str->ptr = input2_str.ptr
    SgExprStatement* ptrAssign = Util::createArrowAssign(arg1, "ptr", ptrExp);
    appendStmtToBody(ptrAssign);

    // create_entry fn call: create_entry(input1_str->addr, input2_str.addr)
    SgExpression* ceArg1 = Util::createArrowExpFor(arg1, "addr");
    SgExpression* ceArg2 = Util::createDotExpFor(arg2, "addr");

    ceCall = funFact::buildDerivedFuncCall("create_entry", SB::buildExprListExp(ceArg1, ceArg2), SgTypeVoid::createType(), scope, fndecl);

  }
  else {
    // this is a value -- like a string value..., or a char pointer

    // input1_str.ptr = input2
    SgExprStatement* ptrAssign = Util::createArrowAssign(arg1, "ptr", SB::buildVarRefExp(arg2));
    appendStmtToBody(ptrAssign);

    // create entry fn call: create_entry(input1_str->addr, input1_str.ptr, sizeof(input2))
    SgExpression* ceArg1 = Util::createArrowExpFor(arg1, "addr");
    SgExpression* ceArg2 = Util::castToAddr(Util::createArrowExpFor(arg1, "ptr"));
    SgExpression* sizeOfArg2 = SB::buildSizeOfOp(SB::buildVarRefExp(arg2));
    SgExpression* ceArg3 = SB::buildCastExp(sizeOfArg2, Util::getSizeOfType(), CAST_TYPE);

    ceCall = funFact::buildDerivedFuncCall("create_entry", SB::buildExprListExp(ceArg1, ceArg2, ceArg3), SgTypeVoid::createType(), scope, fndecl);

  }
  appendStmtToBody(SB::buildExprStatement(ceCall));

  // return *input1_str
  insertReturnStmt(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)));
}

void incrDecrFn::fillOutBody() {
  // fncall: increment/decrement(str)
  // for increment: str.ptr++
  // for decrement: str.ptr--
  // return str
  //
  // fncall: increment/decrement(**ptr)
  // for increment: (*ptr)++;
  // for decrement: (*ptr)--;
  // struct output;
  // output.ptr = *ptr;
  // output.addr = (addr type)ptr;
  // return output;

  // the args
  SgInitializedNamePtrList& args = getArgs();
  ROSE_ASSERT(args.size() == 1);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;

  if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
  {
    // fncall: incr/decr(str)
    SgExpression* incrDecr;

    SgExpression* ptrDot = Util::createDotExpFor(arg1, "ptr");

    if(isDecr) {
      incrDecr = SB::buildMinusMinusOp(ptrDot);
    }
    else {
      incrDecr = SB::buildPlusPlusOp(ptrDot);
    }
    appendStmtToBody(SB::buildExprStatement(incrDecr));

    insertReturnStmt(SB::buildVarRefExp(arg1));
  }
  else {
    // fncall: incr/decr(ptr)

    SgExpression* incrDecr;

    // (*ptr)++ / (*ptr)--
    if(isDecr) {
        incrDecr = SB::buildMinusMinusOp(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)));
    }
    else {
        incrDecr = SB::buildPlusPlusOp(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)));
    }
    appendStmtToBody(SB::buildExprStatement(incrDecr));

    // struct output
    // arg1 base type
    SgType* arg1BaseType = isSgPointerType(arg1->get_type())->get_base_type();

    // output type
    SgType* outputType = strDecl::getStructType(arg1BaseType, fndecl, true);

    SgVariableDeclaration* outputVar = SB::buildVariableDeclaration("output", outputType, NULL, SI::getScope(body));
    appendStmtToBody(outputVar);

    // output.ptr = *ptr;
    SgExpression* ptrAssign = SB::buildAssignOp(Util::createDotExpFor(Util::getNameForDecl(outputVar), "ptr"),
            SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)));
    appendStmtToBody(SB::buildExprStatement(ptrAssign));

    // output.addr = (addr type)ptr;
    SgExpression* addrAssign = SB::buildAssignOp(Util::createDotExpFor(Util::getNameForDecl(outputVar), "addr"),
            Util::castToAddr(SB::buildVarRefExp(arg1)));
    appendStmtToBody(SB::buildExprStatement(addrAssign));

    // return output
    insertReturnStmt(SB::buildVarRefExp(outputVar));
  }

}

void incrDecrDerefFn::fillOutBody() {

  // fncall: increment_deref/decrement_deref(str)
  // for increment: *str.ptr++
  // for decrement: *str.ptr--
  // return create_struct(*str.ptr, str.ptr)
  //
  // fncall: increment/decrement(ptr)
  // for increment: (*ptr)++;
  // for decrement: (*ptr)--;
  // return create_struct(*ptr, (addr_type)ptr)

  // the args
  SgInitializedNamePtrList& args = getArgs();
  ROSE_ASSERT(args.size() == 1);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // ptr/str
  SgInitializedName* arg1 = *iter;
  SgExpression* ptr;

  // str
  if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
  {
    ptr = Util::createDotExpFor(arg1, "ptr");
  }
  // ptr
  else if(Util::isQualifyingType(arg1->get_type())) {
    ptr = SB::buildVarRefExp(arg1);
  }
  else {
    printf("handleIncrDecrDerefDef - unsupported case for arg\n");
    printf("arg\n");
    Util::printNode(arg1);
    ROSE_ASSERT(0);
  }

  SgExpression* ptrDeref = SB::buildPointerDerefExp(ptr);

  SgExpression* arithOp;
  if(isDecr) {
    arithOp = SB::buildMinusMinusOp(ptrDeref);
  }
  else {
    arithOp = SB::buildPlusPlusOp(ptrDeref);
  }
  appendStmtToBody(SB::buildExprStatement(arithOp));

  // return create_struct(...)
  SgExprListExp* pList = SB::buildExprListExp(SI::copyExpression(ptrDeref), Util::castToAddr(SI::copyExpression(ptr)));
  SgExpression* ovl = funFact::buildDerivedFuncCall("create_struct", pList, retType, scope, fndecl);
  insertReturnStmt(ovl);
}

SgExpression* condFn::getExp(SgInitializedName* arg) {
  if(strDecl::isValidStructType(arg->get_type(), GEFD(fndecl)))
  {
    return Util::createDotExpFor(arg, "ptr");
  }
  else {
    // anything else, will be seen within the function as
    // a variable...
    return SB::buildVarRefExp(arg);
  }
}

void condFn::fillOutBody() {

  // this fn takes in two structs, or two pointers, or two
  // one struct + one variable or any such combination, by value
  // fncall: cond(input1_str, input2_str)
  // return input1_str.ptr (cond) input2_str.ptr;
  // fncall: cond(ptr1, ptr2)
  // return ptr1 (cond) ptr2

  // the args
  SgInitializedNamePtrList& args = getArgs();
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // Get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  // create the appropriate lhs expr --
  // if arg1 is a struct, create str.ptr
  // else just create a reference to the argument...
  SgExpression* lhsExp = getExp(arg1);
  SgExpression* rhsExp = getExp(arg2);

  SgExpression* condExp = createConditionalExp(lhsExp, rhsExp);
  insertReturnStmt(condExp);
}

SgExpression* condFn::createConditionalExp(SgExpression* lhs, SgExpression* rhs) {
  // For reference
  #if 0
  case V_SgLessThanOp: sprintf(output, "LessThan"); break;
  case V_SgGreaterThanOp: sprintf(output, "GreaterThan"); break;
  case V_SgNotEqualOp: sprintf(output, "NotEqual"); break;
  case V_SgLessOrEqualOp: sprintf(output, "LessOrEqual"); break;
  case V_SgGreaterOrEqualOp: sprintf(output, "GreaterOrEqual"); break;
  case V_SgEqualityOp: sprintf(output, "Equality"); break;
  #endif

  SgExpression* condExp;

  if(Util::compareNames(fnName, "LessThan")) {
    condExp = SB::buildLessThanOp(lhs, rhs);
  }
  else if(Util::compareNames(fnName, "GreaterThan")) {
    condExp = SB::buildGreaterThanOp(lhs, rhs);
  }
  else if(Util::compareNames(fnName, "NotEqual")) {
    condExp = SB::buildNotEqualOp(lhs, rhs);
  }
  else if(Util::compareNames(fnName, "LessOrEqual")) {
    condExp = SB::buildLessOrEqualOp(lhs, rhs);
  }
  else if(Util::compareNames(fnName, "GreaterOrEqual")) {
    condExp = SB::buildGreaterOrEqualOp(lhs, rhs);
  }
  else if(Util::compareNames(fnName, "Equality")) {
    condExp = SB::buildEqualityOp(lhs, rhs);
  }
  else {
    printf("Unsupported conditional op: %s\n", fnName.getString().c_str());
    ROSE_ASSERT(0);
  }

  return condExp;

}

void addSubFn::fillOutBody() {


  // this fn takes in a single struct, by ref
  // fncall: add/sub(input1_str, input2)
  // for addition: str.ptr += val;
  // for subtraction: str.ptr -= val;
  // return str;

  // the args
  SgInitializedNamePtrList& args = getArgs();
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // get the first arg
  SgInitializedName* arg1 = *iter;

  // get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  // If one of the two arguments is of struct type, then its
  // good enough.

  #ifdef CPP_SCOPE_MGMT
  ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)) ||
        strDecl::isValidStructType(arg2->get_type(), GEFD(fndecl)));
  #else
  ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type()) ||
        strDecl::isValidStructType(arg2->get_type()));
  #endif

  SgExpression* arithAssign;
  if(isSub) {
    if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
    {
      arithAssign = SB::buildMinusAssignOp(Util::createDotExpFor(arg1, "ptr"), SB::buildVarRefExp(arg2));
    }
    else {
      arithAssign = SB::buildMinusAssignOp(Util::createDotExpFor(arg2, "ptr"), SB::buildVarRefExp(arg1));
    }
  }
  else {
    if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
    {
      arithAssign = SB::buildPlusAssignOp(Util::createDotExpFor(arg1, "ptr"), SB::buildVarRefExp(arg2));
    }
    else {
      arithAssign = SB::buildPlusAssignOp(Util::createDotExpFor(arg2, "ptr"), SB::buildVarRefExp(arg1));
    }
  }
  appendStmtToBody(SB::buildExprStatement(arithAssign));

  // pass the struct back.. in return
  if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
  {
    insertReturnStmt(SB::buildVarRefExp(arg1));
  }
  else {
    insertReturnStmt(SB::buildVarRefExp(arg1));
  }
}

void addSubDerefFn::fillOutBody() {

  // fncall: add/sub(ptr, exp)
  // return create_struct(*ptr +/- exp, ptr);

  // the args
  SgInitializedNamePtrList& args = getArgs();
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // arg1
  SgInitializedName* arg1 = *iter;

  // arg2
  iter++;
  SgInitializedName* arg2 = *iter;

  SgExprListExp* pList;

  // one of the args should be a double pointer... since we are taking
  // it before a deref, and after the deref, it still has pointer type
  if(Util::isQualifyingType(arg1->get_type())) {
    SgPointerType* arg1Type = isSgPointerType(arg1->get_type());
    ROSE_ASSERT(Util::isQualifyingType(arg1Type->get_base_type()));

    SgExpression* ptrDeref = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));
    SgExpression* exp = SB::buildVarRefExp(arg2);

    SgExpression* arithOp;
    if(isSub) {
      arithOp = SB::buildSubtractOp(ptrDeref, exp);
    }
    else {
      arithOp = SB::buildAddOp(ptrDeref, exp);
    }

    pList = SB::buildExprListExp(arithOp, Util::castToAddr(SB::buildVarRefExp(arg1)));
  }
  else {
    ROSE_ASSERT(Util::isQualifyingType(arg2->get_type()));
    SgPointerType* arg2Type = isSgPointerType(arg2->get_type());
    ROSE_ASSERT(Util::isQualifyingType(arg2Type->get_base_type()));

    SgExpression* exp = SB::buildVarRefExp(arg1);
    SgExpression* ptrDeref = SB::buildPointerDerefExp(SB::buildVarRefExp(arg2));

    SgExpression* arithOp;
    if(isSub) {
      arithOp = SB::buildSubtractOp(exp, ptrDeref);
    }
    else {
      arithOp = SB::buildAddOp(exp, ptrDeref);
    }

    pList = SB::buildExprListExp(arithOp, Util::castToAddr(SB::buildVarRefExp(arg2)));

  }

  SgExpression* ovl = funFact::buildDerivedFuncCall("create_struct", pList, retType, scope, fndecl);
  insertReturnStmt(ovl);
}

void castFn::strToStr() {

  // this is a struct to struct cast. probably the most common type.
  // here, we cast the pointer from the input type, to the output type (return type)
  // copy the address over as is. this simple copy should be okay since
  // this operation by itself doesn't change the metadata anyways.
  // the fncall: Cast(input_str)
  // struct __Pb__Ui__Pe__ output;
  // output.ptr = (output_str.ptr type*)input_str.ptr;
  // output.addr = input_str.addr;
  // return output;

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // the first arg...
  SgInitializedName* arg1 = *iter;

  // create local variable of return type: str output;
  SgVariableDeclaration* outputVar = Util::createLocalVariable("output", retType, NULL, SI::getScope(body));
  appendStmtToBody(outputVar);

  // now: output.ptr = (output.ptr type*)input_str.ptr;
  SgExpression* inputPtrExp = Util::createDotExpFor(arg1, "ptr");
  SgType* retTypePtr = strDecl::findInUnderlyingType(retType);
  SgExpression* castedInputPtrExp = SB::buildCastExp(inputPtrExp, retTypePtr, CAST_TYPE);
  SgExprStatement* ptrAssign = Util::createDotAssign(Util::getNameForDecl(outputVar), "ptr", castedInputPtrExp);
  appendStmtToBody(ptrAssign);

  // now: output.addr = input_str.addr;
  SgExpression* inputAddrExp = Util::createDotExpFor(arg1, "addr");
  SgExprStatement* addrAssign = Util::createDotAssign(Util::getNameForDecl(outputVar), "addr", inputAddrExp);
  appendStmtToBody(addrAssign);

  #ifdef RMM_TYPETRACKER
  SgExprStatement* addStmt;
  //TMP
  addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(
    Util::castToAddr(Util::createDotExpFor(arg1, "ptr")),
    SB::buildVarRefExp("rtc_ti_" + Util::getNameForType(retTypePtr->stripTypedefsAndModifiers()))
  ), SI::getScope(body));
  appendStmtToBody(addStmt);
  #endif


  // return output;
  insertReturnStmt(SB::buildVarRefExp(outputVar));
}

void castFn::strToVal() {

  // the fncall: Cast(input_str)
  // return reinterpret_cast<(addr type)>(input_str.ptr);

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // the first arg...
  SgInitializedName* arg1 = *iter;

  SgExpression* dotExp = Util::createDotExpFor(arg1, "ptr");
  // removed cast to addr type... doesn't make sense
  //SgExpression* castedExp = buildCastExp(dotExp, getAddrType(), CAST_TYPE);
  insertReturnStmt(dotExp);
}

void castFn::valToStr() {

  // This is a value to struct cast.
  // fncall: cast(ptr, addr, size, lock)
  // body:
  // create_entry(addr, base, size, lock)
  // return create_struct(ptr, addr);



  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // the first arg... ptr
  SgInitializedName* ptr = *iter;


  SgExpression* ptrCasted = Util::castToAddr(SB::buildVarRefExp(ptr));

  // the second arg... addr
  ++iter;
  SgInitializedName* addr = *iter;

  // the third arg... size
  ++iter;
  SgInitializedName* size = *iter;

  // the fourth arg... lock
  ++iter;
  SgInitializedName* lock = *iter;


  // create_entry
  SgExprListExp* pList = SB::buildExprListExp(SB::buildVarRefExp(addr), ptrCasted,
                      SB::buildVarRefExp(size), SB::buildVarRefExp(lock));

  SgExpression* ovl = funFact::buildDerivedFuncCall("create_entry", pList, SgTypeVoid::createType(),
                        scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl));


  // create_struct
  SgExprListExp* pList2 = SB::buildExprListExp(SB::buildVarRefExp(ptr), SB::buildVarRefExp(addr));

  SgExpression* ovl2 = funFact::buildDerivedFuncCall("create_struct", pList2, retType, scope, fndecl);


  #ifdef RMM_TYPETRACKER
  SgExprStatement* addStmt;
  //std::string argTypeStr =  ptr->get_type()->get_mangled().getString();
  //addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(ptrCasted, SB::buildStringVal(argTypeStr)),SI::getScope(body));
  addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(
    ptrCasted,
    SB::buildVarRefExp("rtc_ti_" + Util::getNameForType(ptr->get_type()->stripTypedefsAndModifiers())) ),
    SI::getScope(body)
  );
  appendStmtToBody(addStmt);
  #endif



  insertReturnStmt(ovl2);

}

void castFn::varRefToStr() {

  // the fncall: Cast(var_ref, &var_ref)
  // struct_retType output;
  // output.ptr = (retType)var_ref;
  // output.addr = &var_ref;
  // return output

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // the first arg...
  SgInitializedName* arg1 = *iter;

  // the second arg...
  ++iter;
  SgInitializedName* arg2 = *iter;


  // create local variable of return type: str output;
  SgVariableDeclaration* outputVar = Util::createLocalVariable("output", retType, NULL, SI::getScope(body));
  appendStmtToBody(outputVar);

  // output.ptr
  SgExpression* outputPtr = Util::createDotExpFor(Util::getNameForDecl(outputVar), "ptr");
  SgExpression* castedVarRef = SB::buildCastExp(SB::buildVarRefExp(arg1), outputPtr->get_type(), CAST_TYPE);
  SgExprStatement* ptrAssign = SB::buildExprStatement(SB::buildAssignOp(outputPtr, castedVarRef));
  appendStmtToBody(ptrAssign);

  // output.addr
  SgExpression* outputAddr = Util::createDotExpFor(Util::getNameForDecl(outputVar), "addr");
  SgExprStatement* addrAssign = SB::buildExprStatement(SB::buildAssignOp(outputAddr, SB::buildVarRefExp(arg2)));
  appendStmtToBody(addrAssign);


  #ifdef RMM_TYPETRACKER
  SgExprStatement* addStmt;

  addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(
    Util::castToAddr(SB::buildVarRefExp(arg1)),
    SB::buildVarRefExp("rtc_ti_" + Util::getNameForType(outputPtr->get_type()->stripTypedefsAndModifiers()))),
    SI::getScope(body));

  appendStmtToBody(addStmt);

  #endif

  // return output
  insertReturnStmt(SB::buildVarRefExp(outputVar));
}

void castFn::fillOutBody() {

  // Four cases
  // 1. value_exp to struct
  // 2. qual_type_exp to struct
  // 2. struct to struct
  // 3. struct to value

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // get the first arg
  SgInitializedName* arg1 = *iter;

  if(Util::isQualifyingType(arg1->get_type()) && args.size() == 2) {
    varRefToStr();
  }
  else if(!strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
  {
    // fncall: cast(ptr, addr, sizeof(ptr), lock)
    // The output should be a struct in this case.
    ROSE_ASSERT(strDecl::isValidStructType(retType, GEFD(fndecl)));

    // This is a value to struct...
    valToStr();
  }
  else {
    // in the other two cases, the argument is a struct
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));

    if(!strDecl::isValidStructType(retType, GEFD(fndecl)))
    {
      // returning a value from a struct
      strToVal();
    }
    else {
      // struct to struct cast
      strToStr();
    }
  }

}

void plusMinusFn::fillOutBody() {

  // This function does a plus/minus assign on a struct.
  // fncall: PlusAssign(*input1_str, input2)
  // plusassign body: input1_str->ptr += input2;
  // minusassign body: input1_str->ptr -= input2;
  // return *input1_str

  // the args
  SgInitializedNamePtrList& args = getArgs();
  // assign has two args. first is of pointer type
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // first arg is pointer type -- pointer to struct
  ROSE_ASSERT(isSgPointerType(arg1->get_type()));

  // input1_str->ptr
  SgExpression* ptrExp = Util::createArrowExpFor(arg1, "ptr");

  // Get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  SgExpression* assign;
  if(isMinus) {
    assign = SB::buildMinusAssignOp(ptrExp, SB::buildVarRefExp(arg2));
  }
  else {
    assign = SB::buildPlusAssignOp(ptrExp, SB::buildVarRefExp(arg2));
  }
  appendStmtToBody(SB::buildExprStatement(assign));

}

void ptrCheckFn::fillOutBody() {

  // fn call: ptr_check(input1_str, index)
  // body: array_bound_check_using_lookup(....)
  // return input1_str.ptr;

  // the args
  SgInitializedNamePtrList& args = getArgs();
  // pntrarrref has two args. first is of struct type
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // the first arg is of struct type
  ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));

  // create pointer dot exp
  SgExpression* ptrExp = Util::createDotExpFor(arg1, "ptr");

  // get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  // create addr dot exp
  SgExpression* addrExp = Util::castToAddr(Util::createDotExpFor(arg1, "addr"));

  // arg1 is designed to be the metadata lookup using addr... this is not required
  // for arrays...
  // arg2 is the pointer/array var
  // arg3 is the index
  // arg4 is the base type for array/pointer
  // scope gives the scope in which the function needs to be inserted
  // insert specifies where to insert the forward decl/def of the function
  SgExprListExp* pList = Util::getArrayBoundCheckUsingLookupParams(addrExp, ptrExp,
                        SB::buildVarRefExp(arg2),
                        isSgPointerType(ptrExp->get_type())->get_base_type());

  SgExpression* ovl = funFact::buildDerivedFuncCall("array_bound_check_using_lookup", pList, SgTypeVoid::createType(), scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl));

  insertReturnStmt(SI::copyExpression(ptrExp));
}

void derefCheckFn::fillOutBody() {

  // fn call: deref_check(ptr, addr)
  // body: check_entry(ptr, addr)
  // null_check(ptr)
  // return ptr

  // the args
  SgInitializedNamePtrList& args = getArgs();
  // pntrarrref has two args. first is of struct type
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // Cast it to addr...
  SgExpression* ceArg1 = Util::castToAddr(SB::buildVarRefExp(arg1));

  // Get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;
  // Take a var ref for this
  SgExpression* ceArg2 = SB::buildVarRefExp(arg2);

  // insert check_entry call
  SgExprListExp* pList = SB::buildExprListExp(ceArg1, ceArg2);

  SgExpression* ovl = funFact::buildDerivedFuncCall("check_entry", pList,
                          SgTypeVoid::createType(),
                          scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl));

  // insert null_check call
  SgExprListExp* pList2 = SB::buildExprListExp(SI::copyExpression(ceArg1));
  SgExpression* ovl2 = funFact::buildDerivedFuncCall("null_check", pList2,
                          SgTypeVoid::createType(),
                          scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl2));

  // Now, return the pointer itself,for the deref operation
  insertReturnStmt(SB::buildVarRefExp(arg1));
}


void derefCheckWithStrFn::fillOutBody() {

  // fn call: deref_check_with_str(str)
  // body: return deref_check(str.ptr, str.addr)

  // the args
  SgInitializedNamePtrList& args = getArgs();
  // deref_check_with_str has 1 struct arg
  ROSE_ASSERT(args.size() == 1);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));

  // str.ptr
  SgExpression* ptrExp = Util::createDotExpFor(arg1, "ptr");

  // str.addr
  SgExpression* addrExp = Util::castToAddr(Util::createDotExpFor(arg1, "addr"));

  // create the param list for the deref_check call
  SgExprListExp* pList = SB::buildExprListExp(ptrExp, addrExp);
  SgExpression* ovl = funFact::buildDerivedFuncCall("deref_check", pList, ptrExp->get_type(),
                          scope, fndecl);
  insertReturnStmt(ovl);
}

void assignAndCopyFn::fillOutBody() {
  #ifdef RMM_TYPETRACKER
  SgExprStatement* addStmt;
  #endif

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  SgExprListExp* entryPL;
  SgExprListExp* strPL;

  SgInitializedName* arg1 = *iter;
  iter++;
  SgInitializedName* arg2 = *iter;

  if(args.size() == 3 && !strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
  {
    // input: var_ref_L = var_ref_R
    // output: assign_copy(&var_ref_L, var_ref_R, &var_ref_R)
    // body: var_ref_L =cd pe  var_ref_R -- this is required since the original variable needs to be updated
    // body: create_entry(&var_ref_L, &var_ref_R),
    // return create_struct(var_ref_R, &var_ref_L)

    // Cast the lhs of the assign op to the rhs type

    #ifdef RMM_TYPETRACKER
    //addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)), SB::buildStringVal(arg2->get_type()->get_mangled().getString())),scope);
    addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(
    Util::castToAddr(SB::buildVarRefExp(arg1)),
    SB::buildVarRefExp("rtc_ti_" + Util::getNameForType(arg2->get_type()->stripTypedefsAndModifiers()))),
    scope);
    #endif


    SgExpression* lhs = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));

    SgExpression* assignOp = SB::buildAssignOp(lhs,
        SB::buildVarRefExp(arg2));
    appendStmtToBody(SB::buildExprStatement(assignOp));

    iter++;
    SgInitializedName* arg3 = *iter;

    entryPL = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)),
        SB::buildVarRefExp(arg3));

    strPL = SB::buildExprListExp(SB::buildVarRefExp(arg2),
        Util::castToAddr(SB::buildVarRefExp(arg1)));

  }
  else if(args.size() == 2 && !strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
  {
    // input: var_ref_L = struct_R
    // output: assign_copy(&var_ref_L, struct_R)
    // body: var_ref_L = struct_R.ptr -- this is required since the original variable needs to be updated
    // body: create_entry(&var_ref_L, struct_R.addr);
    // return create_struct(struct_R.ptr, &var_ref_L)

    // Cast the lhs to the type of the rhs
    SgExpression* rhs = Util::createDotExpFor(arg2, "ptr");
    SgExpression* lhs = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));

    SgExpression* assignOp = SB::buildAssignOp(lhs,
        rhs);
    appendStmtToBody(SB::buildExprStatement(assignOp));

    entryPL = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)),
        Util::castToAddr(Util::createDotExpFor(arg2, "addr")));

    strPL = SB::buildExprListExp(Util::createDotExpFor(arg2, "ptr"),
        Util::castToAddr(SB::buildVarRefExp(arg1)));

    #ifdef RMM_TYPETRACKER
    //addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)), SB::buildStringVal(arg1->get_type()->get_mangled().getString())),scope);
    addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(
    Util::castToAddr(SB::buildVarRefExp(arg1)),
    SB::buildVarRefExp("rtc_ti_" + Util::getNameForType(arg1->get_type()->dereference()->stripTypedefsAndModifiers()))),
    scope);
    #endif


  }
  else if(args.size() == 3 && strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)))
  {
    // input: struct_L = var_ref_R
    // output: assign_copy(struct_L, var_ref_R, &var_ref_R)
    // body: struct_L.ptr = var_ref_R; -- This is required since *ptr_index = NULL still
    // requires that *ptr_index is updated!
    // body: create_entry(struct_L.addr, &var_ref_R)
    // return create_struct(var_ref_R, struct_L.addr)

    #ifdef RMM_TYPETRACKER
    //addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)), SB::buildStringVal(arg1->get_type()->get_mangled().getString())),scope);
    addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(
    Util::castToAddr(SB::buildVarRefExp(arg1)),
    SB::buildVarRefExp("rtc_ti_" + Util::getNameForType(arg1->get_type()->stripTypedefsAndModifiers()))),
    scope);
    #endif

    SgExpression* ptrAssign = SB::buildAssignOp(Util::createDotExpFor(arg1, "ptr"),
        SB::buildVarRefExp(arg2));
    appendStmtToBody(SB::buildExprStatement(ptrAssign));

    iter++;
    SgInitializedName* arg3 = *iter;

    entryPL = SB::buildExprListExp(Util::castToAddr(Util::createDotExpFor(arg1, "addr")),
        SB::buildVarRefExp(arg3));

    strPL = SB::buildExprListExp(SB::buildVarRefExp(arg2),
        Util::castToAddr(Util::createDotExpFor(arg1, "addr")));
  }
  else {
    // input: struct_L = struct_R
    // output: assign_copy(struct_L, struct_R)
    // body: struct_L.ptr = struct_R.ptr; -- This is required since *ptr_index = NULL still
    // requires that *ptr_index is updated!
    // body: create_entry(struct_L.addr, struct_R.addr)
    // return create_struct(struct_R.ptr, struct_L.addr)

    #ifdef RMM_TYPETRACKER
    //addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)), SB::buildStringVal(arg1->get_type()->get_mangled().getString())),scope);
    addStmt = SB::buildFunctionCallStmt(SgName("typetracker_add"),SgTypeVoid::createType(),SB::buildExprListExp(
    Util::castToAddr(SB::buildVarRefExp(arg1)),
    SB::buildVarRefExp("rtc_ti_" + Util::getNameForType(arg1->get_type()->stripTypedefsAndModifiers()))),
    scope);
    #endif


    SgExpression* ptrAssign = SB::buildAssignOp(Util::createDotExpFor(arg1, "ptr"),
        Util::createDotExpFor(arg2, "ptr"));
    appendStmtToBody(SB::buildExprStatement(ptrAssign));

    entryPL = SB::buildExprListExp(Util::castToAddr(Util::createDotExpFor(arg1, "addr")),
        Util::castToAddr(Util::createDotExpFor(arg2, "addr")));

    strPL = SB::buildExprListExp(Util::createDotExpFor(arg2, "ptr"),
        Util::castToAddr(Util::createDotExpFor(arg1, "addr")));
  }

  // create_entry_if_src_exists is used instead of create_entry since the rhs can be NULL..
  // and its metadata may not be recorded.
  SgExpression* entryOvl = funFact::buildDerivedFuncCall("create_entry_if_src_exists", entryPL,
                        SgTypeVoid::createType(), scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(entryOvl));

  SgExpression* strOvl = funFact::buildDerivedFuncCall("create_struct", strPL,
                        retType, scope, fndecl);

  #ifdef RMM_TYPETRACKER
  if(addStmt != NULL){
  SgFunctionDefinition* FnDef = fndecl->get_definition();
  SgBasicBlock* FirstBlock = FnDef->get_body();
  SgStatementPtrList& Stmts = FirstBlock->get_statements();
  SgStatementPtrList::iterator i = Stmts.begin();
  i += (Stmts.size() - 1);
  SI::insertStatementBefore(*i, addStmt);
  }
  #endif


  insertReturnStmt(strOvl);
}

void derefAssignAndCopyFn::fillOutBody() {

  // fncall: deref_assign_and_copy(ptr1, ptr2)
  // body: *ptr1 = *ptr2
  // body: create_entry(ptr1, ptr2)
  // return create_struct(*ptr1, ptr1)

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // ptr1
  SgInitializedName* ptr1 = *iter;

  // ptr2
  iter++;
  SgInitializedName* ptr2 = *iter;

  // *ptr1 = *ptr2
  SgExpression* assignOp = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1)),
      SB::buildPointerDerefExp(SB::buildVarRefExp(ptr2)));
  appendStmtToBody(SB::buildExprStatement(assignOp));

  // create_entry(ptr1, ptr2)
  SgExprListExp* cePL = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(ptr1)), Util::castToAddr(SB::buildVarRefExp(ptr2)));
  SgExpression* ovl = funFact::buildDerivedFuncCall("create_entry", cePL, SgTypeVoid::createType(),
                          scope, fndecl);

  appendStmtToBody(SB::buildExprStatement(ovl));

  // create_struct(*ptr1, ptr1)
  SgExpression* ptr1Deref = SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1));
  SgExprListExp* csPL = SB::buildExprListExp(ptr1Deref, Util::castToAddr(SB::buildVarRefExp(ptr1)));
  SgExpression* ovl2 = funFact::buildDerivedFuncCall("create_struct", csPL, retType, scope, fndecl);
  insertReturnStmt(ovl2);
}

void lhsDerefAssignAndCopyFn::fillOutBody() {

  // fncall: lhs_deref_assign_and_copy(ptr1, str)/lhs_deref_assign_and_copy(ptr1, ptr2, &ptr2)
  // body: *ptr1 = str.ptr / *ptr1 = ptr2
  // body: create_entry(ptr1, str.addr)/create_entry(ptr1, &ptr2)
  // return create_struct(*ptr1, ptr1)

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // ptr1
  SgInitializedName* ptr1 = *iter;
  SgExprListExp* cePL;

  if(args.size() == 2) {
    // str
    iter++;
    SgInitializedName* str = *iter;

    // *ptr1 = str.ptr
    SgExpression* assignOp = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1)), Util::createDotExpFor(str, "ptr"));
    appendStmtToBody(SB::buildExprStatement(assignOp));

    // create_entry(ptr1, str.addr)
    cePL = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(ptr1)),
        Util::castToAddr(Util::createDotExpFor(str, "addr")));
  }
  else {
    ROSE_ASSERT(args.size() == 3);
    // ptr2
    iter++;
    SgInitializedName* ptr2 = *iter;

    // &ptr2
    iter++;
    SgInitializedName* ptr2Addr = *iter;

    // *ptr1 = ptr2
    SgExpression* assignOp = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1)),
        SB::buildVarRefExp(ptr2));
    appendStmtToBody(SB::buildExprStatement(assignOp));

    // create_entry(ptr1, &ptr2)
    cePL = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(ptr1)),
        Util::castToAddr(SB::buildVarRefExp(ptr2Addr)));
  }

  SgExpression* ovl = funFact::buildDerivedFuncCall("create_entry", cePL, SgTypeVoid::createType(),
                        scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl));

  // return create_struct(*ptr1, ptr1)
  SgExprListExp* csPL = SB::buildExprListExp(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr1)), Util::castToAddr(SB::buildVarRefExp(ptr1)));
  SgExpression* ovl2 = funFact::buildDerivedFuncCall("create_struct", csPL, retType, scope, fndecl);
  insertReturnStmt(ovl2);
}

void rhsDerefAssignAndCopyFn::fillOutBody() {

  // fncall: rhs_deref_assign_and_copy(&ptr1, *ptr2), rhs_deref_assign_and_copy(str, *ptr2)
  // body: *ptr1 = *ptr2 / str.ptr = *ptr2
  // body: create_entry(ptr1, ptr2) / create_entry(str.addr, ptr2)
  // return create_struct(*ptr1, ptr1) / return str

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // ptr1 / str
  SgInitializedName* arg1 = *iter;

  // ptr2
  iter++;
  SgInitializedName* ptr2 = *iter;

  SgExprListExp* cePL;

  if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) {

    SgExpression* assignOp = SB::buildAssignOp(Util::createDotExpFor(arg1, "ptr"),
        SB::buildPointerDerefExp(SB::buildVarRefExp(ptr2)));
    appendStmtToBody(SB::buildExprStatement(assignOp));

    cePL = SB::buildExprListExp(Util::createDotExpFor(arg1, "addr"), SB::buildVarRefExp(ptr2));
  }
  else {
    ROSE_ASSERT(Util::isQualifyingType(arg1->get_type()));

    SgExpression* assignOp = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)),
        SB::buildPointerDerefExp(SB::buildVarRefExp(ptr2)));
    appendStmtToBody(SB::buildExprStatement(assignOp));

    cePL = SB::buildExprListExp(Util::castToAddr(SB::buildVarRefExp(arg1)), Util::castToAddr(SB::buildVarRefExp(ptr2)));
  }

  SgExpression* ovl = funFact::buildDerivedFuncCall("create_entry", cePL, SgTypeVoid::createType(),
                          scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl));

  // return create_struct(*ptr1, ptr1) / return str
  if(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl))) {
    insertReturnStmt(SB::buildVarRefExp(arg1));
  }
  else {
    ROSE_ASSERT(Util::isQualifyingType(arg1->get_type()));

    SgExprListExp* csPL = SB::buildExprListExp(SB::buildPointerDerefExp(SB::buildVarRefExp(arg1)),
        Util::castToAddr(SB::buildVarRefExp(arg1)));
    SgExpression* ovl2 = funFact::buildDerivedFuncCall("create_struct", csPL, retType, scope, fndecl);
    insertReturnStmt(ovl2);
  }
}

void doubleDerefCheckFn::fillOutBody() {

  // input: DoubleDerefCheck(ptr)
  // body: check_entry(*ptr, (addr_type)ptr)
  // return *ptr

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // ptr
  SgInitializedName* ptr = *iter;

  SgExprListExp* pList = SB::buildExprListExp(Util::castToAddr(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr))),
                      Util::castToAddr(SB::buildVarRefExp(ptr)));
  SgExpression* ovl = funFact::buildDerivedFuncCall("check_entry", pList, SgTypeVoid::createType(), scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl));

  // return *ptr
  insertReturnStmt(SB::buildPointerDerefExp(SB::buildVarRefExp(ptr)));
}

void derefCreateStrFn::fillOutBody() {
  // input: DerefCreateStructDef(str) / DerefCreateStruct(ptr)
  // return create_struct(*str.ptr, (addr_type)str.ptr) / create_struct(*ptr, (addr_type)ptr)

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // str/ptr
  SgInitializedName* arg1 = *iter;

  SgExpression* csArg1;
  SgExpression* csArg2;

  if(Util::isQualifyingType(arg1->get_type())) {
    csArg1 = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));
    csArg2 = Util::castToAddr(SB::buildVarRefExp(arg1));
  }
  else {
    ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));
    csArg1 = SB::buildPointerDerefExp(Util::createDotExpFor(arg1, "ptr"));
    csArg2 = Util::castToAddr(Util::createDotExpFor(arg1, "ptr"));
  }

  SgExpression* ovl = funFact::buildDerivedFuncCall("create_struct",
                        SB::buildExprListExp(csArg1, csArg2), retType,
                        scope, fndecl);
  insertReturnStmt(ovl);
}

void returnPtrFn::fillOutBody() {

  // input: return_pointer(str)
  // return str.ptr

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // str
  SgInitializedName* arg1 = *iter;

  ROSE_ASSERT(strDecl::isValidStructType(arg1->get_type(), GEFD(fndecl)));

  insertReturnStmt(Util::createDotExpFor(arg1, "ptr"));
}

void createStrAssignAndCopyFn::fillOutBody() {

  // the args
  SgInitializedNamePtrList& args = getArgs();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // lhs
  SgInitializedName* lhs = *iter;
  iter++;
  // rhs
  SgInitializedName* rhs = *iter;

  // *lhs = *rhs
  SgExpression* lhsDeref = SB::buildPointerDerefExp(SB::buildVarRefExp(lhs));
  SgExpression* rhsDeref = SB::buildPointerDerefExp(SB::buildVarRefExp(rhs));

  SgExpression* assignOp = SB::buildAssignOp(lhsDeref, rhsDeref);
  appendStmtToBody(SB::buildExprStatement(assignOp));

  // rtc_copy(lhs, rhs)
  SgExprListExp* pList = SB::buildExprListExp(SB::buildVarRefExp(lhs), SB::buildVarRefExp(rhs));
  SgExpression* ovl = funFact::buildDerivedFuncCall("rtc_copy", pList,
                          SgTypeVoid::createType(), scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl));

  // return *lhs
  insertReturnStmt(SI::copyExpression(lhsDeref));
}

void createEntryWithNewLockGenFn::fillOutBody() {

  // fncall: create_entry_with_new_lock_gen(addr, ptr, size)
  // body: create_entry_with_new_lock(addr, (unsigned long long)ptr, size)
  //       return {ptr, addr}

  // the args
  SgInitializedNamePtrList& args = getArgs();
  // assign has two args. first is of pointer type
  ROSE_ASSERT(args.size() == 3);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // addr
  SgExpression* ce1 = SB::buildVarRefExp(*iter);

  // ptr
  iter++;
  SgExpression* ce2 = SB::buildVarRefExp(*iter);
  SgExpression* ce2Casted = Util::castToAddr(ce2);

  // size
  iter++;
  SgExpression* ce3 = SB::buildVarRefExp(*iter);

  // params
  SgExprListExp* pList = SB::buildExprListExp(ce1, ce2Casted, ce3);

  // create_entry_with_new_lock
  SgExpression* ovl = funFact::buildDerivedFuncCall("create_entry_with_new_lock", pList,
                        SgTypeVoid::createType(), scope, fndecl);
  appendStmtToBody(SB::buildExprStatement(ovl));

  // {ptr, addr}
  SgAggregateInitializer* constInit = SB::buildAggregateInitializer(SB::buildExprListExp(SI::copyExpression(ce2), SI::copyExpression(ce1)),
                                  retType);

  // var = {ptr, addr}
  SgName varName("retvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
  SgVariableDeclaration* newVar = SB::buildVariableDeclaration(varName, retType, constInit, scope);
  appendStmtToBody(newVar);

  // return {ptr, addr}
  insertReturnStmt(SB::buildVarRefExp(newVar));
}


SgFunctionCallExp* funFact::buildDerivedFuncCall(SgName fnName,
  SgExprListExp* paramList,
  SgType* retType,
  SgScopeStatement* scope,
  SgNode* pos) {

  return funFact::buildOrCreate(fnName, paramList, retType, scope, pos, true);
}

SgFunctionCallExp* funFact::buildFuncCall(SgName fnName,
  SgExprListExp* paramList,
  SgType* retType,
  SgScopeStatement* scope,
  SgNode* pos) {

  return funFact::buildOrCreate(fnName, paramList, retType, scope, pos, false);
}

SgScopeStatement* funFact::getInsertScope(SgName, SgExprListExp* paramList, SgType* retType, SgNode* pos, bool derived, SgName fnName) {
  if(Util::isExternal(fnName)) {
    return SI::getGlobalScope(pos);
  }
  else {
    if(strDecl::functionInteractsWithStdOrPrimitives(paramList, retType, pos) && !derived) {
      return Util::FirstDeclStmtInFile->get_scope();
    }
    else {
      return isSgStatement(pos)->get_scope();
    }
  }
}

bool funFact::funcExists(SgName transfName, SgScopeStatement* scope) {
  return scope->symbol_exists(transfName);
}

SgFunctionDeclaration* funFact::getFuncDeclInScope(SgName transfName, SgScopeStatement* scope) {
  SgSymbol* sym = scope->lookup_symbol(transfName);
  ROSE_ASSERT(isSgFunctionSymbol(sym));
  return isSgFunctionSymbol(sym)->get_declaration();
}

SgNode* funFact::correctPositionIfNecessary(SgExprListExp* paramList, SgType* retType, SgNode* pos, bool derived) {

  #if 0
  if(strDecl::functionInteractsWithStdOrPrimitives(paramList, retType, pos) && !Util::isExternal(fnName)
    && !derived)
  #endif
  if(strDecl::functionInteractsWithStdOrPrimitives(paramList, retType, pos) && !derived)
  {
    #ifdef FUNFACT_DEBUG
    printf("changed pos to first decl stmt\n");
    #endif
    return Util::FirstDeclStmtInFile;

  }
  #ifdef FUNFACT_DEBUG
  printf("pos\n");
  Util::printNode(pos);
  #endif
  return pos;
}


SgFunctionCallExp* funFact::buildOrCreate(SgName fnName, SgExprListExp* paramList, SgType* retType, SgScopeStatement* scope, SgNode* pos, bool derived) {


  // transform the name
  SgName transfName(Util::getTransfName(fnName, retType, paramList));

  // correct the scope if necessary
  SgScopeStatement* insScope = getInsertScope(transfName, paramList, retType, pos, derived, fnName);

  SgFunctionDeclaration* decl;


  #ifdef FUNFACT_DEBUG
  printf("transfName: %s\n", transfName.getString().c_str());
  printf("scope: %s\n", insScope->sage_class_name());
  #endif

  //RMM CHANGE
  #ifdef FUNFACT_DEBUG
  printf("pos\n");
  Util::printNode(pos);
  #endif

  if(funcExists(transfName, insScope)) {
    decl = getFuncDeclInScope(transfName, insScope);
    #ifdef FUNFACT_DEBUG
    printf("Found decl in scope\n");
    #endif
  }
  else {
    // correct the pos if necessary
    pos = correctPositionIfNecessary(paramList, retType, pos, derived);

    decl = build(fnName, paramList, retType, scope, pos, derived);

    //SAM ROSE_ASSERT(funcExists(transfName, insScope));
    #ifdef FUNFACT_DEBUG
    printf("created decl in scope\n");
    printf("pos: %s\n", pos->sage_class_name());
    #endif
  }

  return SB::buildFunctionCallExp(decl->get_name(), retType, paramList, scope);
}


SgFunctionDeclaration* funFact::build(SgName fnName,
    SgExprListExp* paramList,
    SgType* retType,
    SgScopeStatement* scope,
    SgNode* pos, bool derived) {


  funcDecl* fd;
  if(Util::isExternal(fnName)) {
    fd = new nonDefDecl(fnName, paramList, retType, scope, pos, derived);
  }
  else {
    // now choose the correct def decl class...
    if(Util::compareNames(fnName, "create_struct")) {
      fd = new createStrFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "create_struct_from_addr")) {
      fd = new createStrFromAddrFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "rtc_copy")) {
      fd = new rtcCopyFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "AddressOf")) {
      fd = new addressOfFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "Deref")) {
      fd = new derefFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "PntrArrRef")) {
      fd = new pntrArrRefFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "Assign")) {
      fd = new assignFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "Increment")) {
      fd = new incrDecrFn(fnName, paramList, retType, scope, pos, false, derived);
    }
    else if(Util::compareNames(fnName, "Increment_deref")) {
      fd = new incrDecrDerefFn(fnName, paramList, retType, scope, pos, false, derived);
    }
    else if(Util::compareNames(fnName, "Decrement")) {
      fd = new incrDecrFn(fnName, paramList, retType, scope, pos, true, derived);
    }
    else if(Util::compareNames(fnName, "Decrement_deref")) {
      fd = new incrDecrDerefFn(fnName, paramList, retType, scope, pos, true, derived);
    }
    else if(Util::isCondString(fnName)) {
      fd = new condFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "Add")) {
      fd = new addSubFn(fnName, paramList, retType, scope, pos, false, derived);
    }
    else if(Util::compareNames(fnName, "Add_deref")) {
      fd = new addSubDerefFn(fnName, paramList, retType, scope, pos, false, derived);
    }
    else if(Util::compareNames(fnName, "Sub")) {
      fd = new addSubFn(fnName, paramList, retType, scope, pos, true, derived);
    }
    else if(Util::compareNames(fnName, "Sub_deref")) {
      fd = new addSubDerefFn(fnName, paramList, retType, scope, pos, true, derived);
    }
    else if(Util::compareNames(fnName, "Cast")) {
      fd = new castFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "PlusAssign")) {
      fd = new plusMinusFn(fnName, paramList, retType, scope, pos, false, derived);
    }
    else if(Util::compareNames(fnName, "MinusAssign")) {
      fd = new plusMinusFn(fnName, paramList, retType, scope, pos, true, derived);
    }
    else if(Util::compareNames(fnName, "ptr_check")) {
      fd = new ptrCheckFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "deref_check")) {
      fd = new derefCheckFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "deref_check_with_str")) {
      fd = new derefCheckWithStrFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "assign_and_copy")) {
      fd = new assignAndCopyFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "deref_assign_and_copy")) {
      fd = new derefAssignAndCopyFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "lhs_deref_assign_and_copy")) {
      fd = new lhsDerefAssignAndCopyFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "rhs_deref_assign_and_copy")) {
      fd = new rhsDerefAssignAndCopyFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "double_deref_check")) {
      fd = new doubleDerefCheckFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "deref_create_struct")) {
      fd = new derefCreateStrFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "return_pointer")) {
      fd = new returnPtrFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "cs_assign_and_copy")) {
      fd = new createStrAssignAndCopyFn(fnName, paramList, retType, scope, pos, derived);
    }
    else if(Util::compareNames(fnName, "create_entry_with_new_lock_gen")) {
      fd = new createEntryWithNewLockGenFn(fnName, paramList, retType, scope, pos, derived);
    }
    else {
      printf("Can't create def. fnName: %s\n", fnName.getString().c_str());
      ROSE_ASSERT(0);
    }

    fd->fillOutBody();
  }
  return fd->getDecl();
}
