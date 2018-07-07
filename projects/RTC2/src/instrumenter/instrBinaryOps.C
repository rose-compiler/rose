#include "instr.h"

namespace Instr {

  SgExpression* createArrayBoundCheckFn(SgExpression* arg1, SgExpression* arg2,
      SgExpression* arg3, SgType* arg4, SgScopeStatement* scope,
      SgStatement* insert,
      bool array);

  SgExpression* handleAssignOverloadOps(SgNode* node) {



#ifdef BINARY_OVLOP_DEBUG
    printf("handleBinaryOverloadOps\n");
#endif

    SgBinaryOp* bop = isSgBinaryOp(node);

    // += and -= are broken down to assign and +/-
    // Cases:
    // 1. var_ref/dotarrow = var_ref/dotarrow
    // 2. struct  = var_ref/dotarrow
    // 3. var_ref/dotarrow = struct
    // 4. struct = struct
    // 5. *ptr = struct
    // 6. struct = *ptr
    // 7. var_ref/dotarrow = *ptr
    // 8. *ptr = var_ref/dotarrow
    // 9. *ptr = *ptr2
    // 10. ref = str

    SgExpression* lhs = bop->get_lhs_operand();
    SgType* lhs_type = lhs->get_type();
#ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
#endif

    SgExpression* rhs = bop->get_rhs_operand();

#ifdef CONVERT_ARG_STRUCTS_TO_PTR
    // Convert the lhs from struct of a variable (typically, a variable in the
    // param list) to a dot exp so that its updated accordingly.
#ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(lhs_type, GEFD(lhs)) && isSgVarRefExp(lhs))
#else
    if(strDecl::isValidStructType(lhs_type) && isSgVarRefExp(lhs))
#endif
    {
      SgInitializedName* var_name = isSgVarRefExp(lhs)->get_symbol()->get_declaration();
      SgExpression* dot_exp = Util::createDotExpFor(var_name, "ptr");
      replaceWrapper(lhs, dot_exp);
      ROSE_ASSERT(bop->get_lhs_operand() == dot_exp);
      return handleAssignOverloadOps(bop);
    }
#endif



    // If the rhs is a value exp, cast it to the
    // qualifying type on the lhs
    if(isSgValueExp(rhs)) {
      SgType* lhs_type_local;
      SgCastExp* casted_rhs;
      if(Util::isQualifyingType(lhs_type)) {
        lhs_type_local = lhs_type;
        // Nothing to do
      }
#ifdef CPP_SCOPE_MGMT
      else if(strDecl::isValidStructType(lhs_type, GEFD(lhs)))
#else
      else if(strDecl::isValidStructType(lhs_type))
#endif
      {
        lhs_type_local = strDecl::findInUnderlyingType(lhs_type);
      }
      else {
        printf("AssignOverloadOps: lhs is not qual/valid struct type\n");
        printf("lhs\n");
        Util::printNode(lhs);
      }

      casted_rhs = SB::buildCastExp(SI::copyExpression(rhs), lhs_type_local, SgCastExp::e_C_style_cast);

      // replace rhs with the casted_rhs
      replaceWrapper(rhs, casted_rhs);

      // Now, call the handleCastExp function with the casted_rhs
      SgExpression* transf_rhs = handleCastExp(casted_rhs);
      // Now, assign transf_rhs to rhs...
      rhs = transf_rhs;


    }


    SgExprListExp* param_list;
    SgExpression* overload;
    SgType* retType;

    // We should get the rhs_type here since we could have
    // changed the rhs to transf_rhs above
    SgType* rhs_type = rhs->get_type();
#ifdef STRIP_TYPEDEFS
    rhs_type = Util::getType(rhs_type);
#endif

    // Create entry for arrays... on rhs
    // lhs doesn't need metadata entry since it will be overwritten
    // by the metadata from rhs anyway.
#ifdef ARRAY_CREATE_ENTRY
    // Also, we need to insert create_entry for the RHS, if its
    // array type.
    if(isSgArrayType(rhs_type)) {
      insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
          SI::getEnclosingStatement(node));
    }
#endif

    SgName fn_name;
#ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(lhs_type, GEFD(lhs)) && strDecl::isValidStructType(rhs_type, GEFD(rhs)))
#else
    if(strDecl::isValidStructType(lhs_type) && strDecl::isValidStructType(rhs_type))
#endif
    {
      // This is the struct = struct case
      // input: struct_L = struct_R
      // output: assign_copy(struct_L, struct_R)

      SgExpression* lhs_copy = SI::copyExpression(lhs);
      SgExpression* rhs_copy = SI::copyExpression(rhs);

      param_list = SB::buildExprListExp(lhs_copy, rhs_copy);

      retType = lhs_type;
    }
#ifdef CPP_SCOPE_MGMT
    else if(strDecl::isValidStructType(rhs_type, GEFD(rhs)) && Util::isVarRefOrDotArrow(lhs))
#else
    else if(strDecl::isValidStructType(rhs_type) && Util::isVarRefOrDotArrow(lhs))
#endif
    {
      // This is the var_ref/dotarrow = struct case

      // input: var_ref_L = struct_R
      // output: assign_copy(&var_ref_L, struct_R)

      //SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));
      SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
      SgExpression* rhs_copy = SI::copyExpression(rhs);

      param_list = SB::buildExprListExp(lhs_addr, rhs_copy);

      retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
          GEFD(SI::getEnclosingStatement(node)), true);
    }
#ifdef CPP_SCOPE_MGMT
    else if(strDecl::isValidStructType(lhs_type, GEFD(lhs)) && Util::isVarRefOrDotArrow(rhs))
#else
    else if(strDecl::isValidStructType(lhs_type) && Util::isVarRefOrDotArrow(rhs))
#endif
    {
      // This is the struct = var_ref/dotarrow case

      // input: struct_L = var_ref_R
      // output: assign_copy(struct_L, var_ref_R, &var_ref_R);

#if 0
#ifdef ARRAY_CREATE_ENTRY
      // Also, we need to insert create_entry for the RHS, if its
      // array type.
      if(isSgArrayType(rhs->get_type())) {
        insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
            SI::getEnclosingStatement(node));
      }
#endif
#endif


      SgExpression* lhs_copy = SI::copyExpression(lhs);
      //SgExpression* rhs_copy = SI::copyExpression(rhs);
      SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs_type), SgCastExp::e_C_style_cast);
      SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

      param_list = SB::buildExprListExp(lhs_copy, rhs_copy, rhs_addr);

      retType = lhs_type;

    }
    else if(Util::isVarRefOrDotArrow(lhs) && Util::isVarRefOrDotArrow(rhs)) {
      // This is the var_ref/dotarrow = var_ref/dotarrow

      // input: var_ref_L = var_ref_R
      // output: assign_copy(&var_ref_L, var_ref_R, &var_ref_R);

#if 0
#ifdef ARRAY_CREATE_ENTRY
      // Also, we need to insert create_entry for the RHS, if its
      // array type.
      if(isSgArrayType(rhs->get_type())) {
        insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
            SI::getEnclosingStatement(node));
      }
#endif
#endif


      //SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));
      SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
      SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs_type), SgCastExp::e_C_style_cast);
      SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

      param_list = SB::buildExprListExp(lhs_addr, rhs_copy, rhs_addr);

      retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
          GEFD(SI::getEnclosingStatement(node)), true);

    }
#ifdef DOUBLE_DIM_ENABLED
    else if(isSgPointerDerefExp(lhs) && Util::isVarRefOrDotArrow(rhs)) {
      // This is the *ptr = var_ref/dotarrow

      // input: *ptr_L = var_ref_R
      // output: lhs_deref_assign_and_copy(ptr_L, var_ref_R, &var_ref_R)

#if 0
#ifdef ARRAY_CREATE_ENTRY
      // Also, we need to insert create_entry for the RHS, if its
      // array type.
      if(isSgArrayType(rhs->get_type())) {
        insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
            SI::getEnclosingStatement(node));
      }
#endif
#endif

      SgExpression* lhs_strip = SI::copyExpression(Util::stripDeref(lhs));

#ifdef STRIP_TYPEDEFS
      lhs_strip = Util::castAwayTypedef(lhs_strip);
#endif

      SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs_type),
          SgCastExp::e_C_style_cast);
      SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

      param_list = SB::buildExprListExp(lhs_strip, rhs_copy, rhs_addr);

      retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
          GEFD(SI::getEnclosingStatement(node)), true);
    }
    else if(Util::isVarRefOrDotArrow(lhs) && isSgPointerDerefExp(rhs)) {
      // This is the var_ref/dotarrow = *ptr

      // input: var_ref_L = *ptr_R
      // output: rhs_deref_assign_and_copy(&var_ref_L, ptr_R)


      SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
      SgExpression* rhs_strip = SI::copyExpression(Util::stripDeref(rhs));

#ifdef STRIP_TYPEDEFS
      rhs_strip = Util::castAwayTypedef(rhs_strip);
#endif

      param_list = SB::buildExprListExp(lhs_addr, rhs_strip);

      retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
          GEFD(SI::getEnclosingStatement(node)), true);
    }
#ifdef CPP_SCOPE_MGMT
    else if(isSgPointerDerefExp(lhs) && strDecl::isValidStructType(rhs->get_type(), GEFD(rhs)))
#else
    else if(isSgPointerDerefExp(lhs) && strDecl::isValidStructType(rhs->get_type()))
#endif
    {
      // This is the *ptr = struct case

      // input: *ptr = str_R
      // output: lhs_deref_assign_and_copy(ptr, str_R)

      SgExpression* lhs_strip = SI::copyExpression(Util::stripDeref(lhs));

#ifdef STRIP_TYPEDEFS
      lhs_strip = Util::castAwayTypedef(lhs_strip);
#endif

      SgExpression* rhs_copy = SI::copyExpression(rhs);

      param_list = SB::buildExprListExp(lhs_strip, rhs_copy);

      retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
          GEFD(SI::getEnclosingStatement(node)), true);
    }
#ifdef CPP_SCOPE_MGMT
    else if(strDecl::isValidStructType(lhs->get_type(), GEFD(lhs)) && isSgPointerDerefExp(rhs))
#else
    else if(strDecl::isValidStructType(lhs->get_type()) && isSgPointerDerefExp(rhs))
#endif
    {
      // This is the str = *ptr case

      // input: str_L = *ptr_R
      // output: rhs_deref_assign_and_copy(str_L, ptr_R)

      SgExpression* lhs_copy = SI::copyExpression(lhs);
      SgExpression* rhs_strip = SI::copyExpression(Util::stripDeref(rhs));

#ifdef STRIP_TYPEDEFS
      rhs_strip = Util::castAwayTypedef(rhs_strip);
#endif

      param_list = SB::buildExprListExp(lhs_copy, rhs_strip);

      retType = rhs_type;
    }
    else if(isSgPointerDerefExp(lhs) && isSgPointerDerefExp(rhs)) {
      // This is the *ptr = *ptr2 case

      // input: *ptr = *ptr2
      // output: deref_assign_and_copy(ptr, ptr2)

      SgExpression* lhs_strip = SI::copyExpression(Util::stripDeref(lhs));
#ifdef STRIP_TYPEDEFS
      lhs_strip = Util::castAwayTypedef(lhs_strip);
#endif

      SgExpression* rhs_strip = SI::copyExpression(Util::stripDeref(rhs));
#ifdef STRIP_TYPEDEFS
      rhs_strip = Util::castAwayTypedef(rhs_strip);
#endif

      param_list = SB::buildExprListExp(lhs_strip, rhs_strip);

      retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs_type),
          GEFD(SI::getEnclosingStatement(node)), true);
    }
#endif
    else {
      printf("Case not supported in handleAssignOverload Ops\n");
      printf("assign_op\n");
      printf("lhs_type\n");
      Util::printNode(lhs->get_type());
      printf("rhs_type\n");
      Util::printNode(rhs->get_type());
      Util::printNode(node);
      ROSE_ASSERT(0);
    }

#if 0

    if(Util::isVarRefOrDotArrow(lhs) && Util::isVarRefOrDotArrow(rhs)) {
      // This is the var_ref/dotarrow = var_ref/dotarrow

      // input: var_ref_L = var_ref_R
      // output: assign_copy(&var_ref_L, var_ref_R, &var_ref_R);

#ifdef ARRAY_CREATE_ENTRY
      // Also, we need to insert create_entry for the RHS, if its
      // array type.
      if(isSgArrayType(rhs->get_type())) {
        insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
            SI::getEnclosingStatement(node));
      }
#endif


      //SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));
      SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
      SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs->get_type()), SgCastExp::e_C_style_cast);
      SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

      param_list = SB::buildExprListExp(lhs_addr, rhs_copy, rhs_addr);

      retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs->get_type()),
          GEFD(SI::getEnclosingStatement(node)), true);

    }
    else if(Util::isVarRefOrDotArrow(lhs)) {
      ROSE_ASSERT(strDecl::isValidStructType(rhs->get_type()));
      // This is the var_ref/dotarrow = struct case

      // input: var_ref_L = struct_R
      // output: assign_copy(&var_ref_L, struct_R)

      //SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));
      SgExpression* lhs_addr = Util::createAddressOfOpFor(SI::copyExpression(lhs));
      SgExpression* rhs_copy = SI::copyExpression(rhs);

      param_list = SB::buildExprListExp(lhs_addr, rhs_copy);

      retType = strDecl::getStructType(Util::getTypeForPntrArr(lhs->get_type()),
          GEFD(SI::getEnclosingStatement(node)), true);

    }
    else if(Util::isVarRefOrDotArrow(rhs)) {
      ROSE_ASSERT(strDecl::isValidStructType(lhs->get_type()));
      // This is the struct = var_ref/dotarrow case

      // input: struct_L = var_ref_R
      // output: assign_copy(struct_L, var_ref_R, &var_ref_R);

#ifdef ARRAY_CREATE_ENTRY
      // Also, we need to insert create_entry for the RHS, if its
      // array type.
      if(isSgArrayType(rhs->get_type())) {
        insertCreateEntry(SI::copyExpression(rhs), Util::getScopeForExp(isSgExpression(node)),
            SI::getEnclosingStatement(node));
      }
#endif


      SgExpression* lhs_copy = SI::copyExpression(lhs);
      //SgExpression* rhs_copy = SI::copyExpression(rhs);
      SgExpression* rhs_copy = SB::buildCastExp(SI::copyExpression(rhs), Util::getTypeForPntrArr(rhs->get_type()), SgCastExp::e_C_style_cast);
      SgExpression* rhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(rhs)));

      param_list = SB::buildExprListExp(lhs_copy, rhs_copy, rhs_addr);

      retType = lhs->get_type();
    }
    else {
      ROSE_ASSERT(strDecl::isValidStructType(lhs->get_type()) &&
          strDecl::isValidStructType(rhs->get_type()));
      // This is the struct = struct case
      // input: struct_L = struct_R
      // output: assign_copy(struct_L, struct_R)

      SgExpression* lhs_copy = SI::copyExpression(lhs);
      SgExpression* rhs_copy = SI::copyExpression(rhs);

      param_list = SB::buildExprListExp(lhs_copy, rhs_copy);

      retType = lhs->get_type();
    }
#endif


    // 1. Modify dot/arrow to not transform when their parent is
    // an assign op.
    // 2. Def for assign_and_copy
#ifdef DOUBLE_DIM_ENABLED
    if(isSgPointerDerefExp(lhs) && isSgPointerDerefExp(rhs)) {
      overload = buildMultArgOverloadFn("deref_assign_and_copy", param_list,
          retType,
          Util::getScopeForExp(isSgExpression(node)),
          GEFD(SI::getEnclosingStatement(node)));
    }
    else if(isSgPointerDerefExp(lhs)) {
      overload = buildMultArgOverloadFn("lhs_deref_assign_and_copy", param_list,
          retType,
          Util::getScopeForExp(isSgExpression(node)),
          GEFD(SI::getEnclosingStatement(node)));
    }
    else if(isSgPointerDerefExp(rhs)) {
      overload = buildMultArgOverloadFn("rhs_deref_assign_and_copy", param_list,
          retType,
          Util::getScopeForExp(isSgExpression(node)),
          GEFD(SI::getEnclosingStatement(node)));
    }
    else {
      overload = buildMultArgOverloadFn("assign_and_copy", param_list,
          retType,
          Util::getScopeForExp(isSgExpression(node)),
          GEFD(SI::getEnclosingStatement(node)));
    }
#else
    overload = buildMultArgOverloadFn("assign_and_copy", param_list,
        retType,
        Util::getScopeForExp(isSgExpression(node)),
        GEFD(SI::getEnclosingStatement(node)));
#endif

    replaceWrapper(isSgExpression(node), overload);

    #ifdef RMM_TYPETRACKER

    SgStatement* s_stmt = SI::getEnclosingStatement(overload);
    SgScopeStatement* s_scope = SI::getScope(s_stmt);
                SgExprStatement* minalloc_call_stmt = SB::buildFunctionCallStmt(SgName("minalloc_check"),
      SB::buildVoidType(),
      SB::buildExprListExp( SB::buildCastExp(*(param_list->get_expressions().begin()),SB::buildUnsignedLongLongType())),
                s_scope);
    SI::insertStatementAfter(s_stmt,minalloc_call_stmt);
    #endif


    return overload;

  }

  SgExpression* handleCSAssignOverloadOps(SgExpression* assign_op) {
    ROSE_ASSERT(isSgAssignOp(assign_op));

    // input: lhs_str = rhs_str
    // output: cs_assign_and_copy(&lhs_str, &rhs_str)

    // In cs_assign_and_copy: *lhs_str = *rhs_str; rtc_copy(lhs_str, rhs_str); return lhs_str

    SgExpression* rhs_copy = SI::copyExpression(isSgAssignOp(assign_op)->get_rhs_operand());
#ifdef STRIP_TYPEDEFS
    rhs_copy = Util::castAwayTypedef(rhs_copy);
#endif

    SgExpression* lhs_copy = SI::copyExpression(isSgAssignOp(assign_op)->get_lhs_operand());
#ifdef STRIP_TYPEDEFS
    lhs_copy = Util::castAwayTypedef(lhs_copy);
#endif

    SgExpression* lhs_addr = Util::createAddressOfOpFor(lhs_copy);
    SgExpression* rhs_addr = Util::createAddressOfOpFor(rhs_copy);

    SgExprListExp* param_list = SB::buildExprListExp(lhs_addr, rhs_addr);

    SgType* retType = lhs_copy->get_type();
    ROSE_ASSERT(!isSgTypedefType(retType));

    SgExpression* overload = buildMultArgOverloadFn("cs_assign_and_copy", param_list, retType,
        Util::getScopeForExp(assign_op),
        GEFD(SI::getEnclosingStatement(assign_op)));

    replaceWrapper(assign_op, overload);

    return overload;

  }

  SgExpression* createArrayBoundCheckFn(SgExpression* arg1, SgExpression* arg2,
      SgExpression* arg3, SgType* arg4, SgScopeStatement* scope,
      SgStatement* insert,
      bool array) {

#ifdef STRIP_TYPEDEFS
    // We don't need to the args below since they are all casted
    // before being used.
    // The return type is void. So, nothing to do.
#endif

    // arg1 is designed to be the metadata lookup using addr... this is not required
    // for arrays...
    // arg2 is the pointer/array var
    // arg3 is the index
    // arg4 is the base type for array/pointer
    // scope gives the scope in which the function needs to be inserted
    // insert specifies where to insert the forward decl/def of the function
    // array specifies if this is an array or not. false by default

    SgExpression* overload = 0;

    if (!array)
    {
      SgExprListExp* param_list = Util::getArrayBoundCheckUsingLookupParams(arg1, arg2, arg3, arg4);

      overload = buildMultArgOverloadFn("array_bound_check_using_lookup", param_list,
          SgTypeVoid::createType(), scope, insert);
    }
    else
    {
      SgExprListExp* param_list = Util::getArrayBoundCheckParams(arg1, arg2, arg3, arg4);

      overload = buildMultArgOverloadFn("array_bound_check", param_list, SgTypeVoid::createType(),
          scope, insert);
    }

    ROSE_ASSERT(overload);
    return overload;
  }



  void instr(SgPntrArrRefExp* array_ref) {
#ifdef PNTRARRREF_DEBUG
    printf("handlePntrArrRefExp... \n");
#endif

    // lhs could be a var ref here.
    // 1. Check if its part of the params
    // 2. If so, replace the var ref with an array bound
    // check using the struct from the params
    // 3. Else, do a normal array bound check for it

    // In both cases, we need to copy over the rhs (index)
    // to a local variable.
    // Here, we first create that variable
    SgExpression* rhs = isSgPntrArrRefExp(array_ref)->get_rhs_operand();
    SgType* rhs_type = rhs->get_type();

#ifdef STRIP_TYPEDEFS
    rhs_type = Util::getType(rhs_type);
#endif

    SgName s_name(rhs_type->get_mangled() + "_index_" + boost::lexical_cast<std::string>(Util::VarCounter++));

    SgVariableDeclaration* array_index = Util::createLocalVariable(s_name, rhs_type, NULL, Util::getScopeForExp(isSgExpression(array_ref)));

#ifdef PNTRARRREF_DEBUG
    SgStatement* encl_stmt = SI::getEnclosingStatement(array_ref);
    printf("encl_stmt\n");
    Util::printNode(encl_stmt);
#endif

    SgStatement* stmt = Util::getSuitablePrevStmt(SI::getEnclosingStatement(array_ref));
    SI::insertStatementBefore(stmt, array_index);

    SgExpression* overload;
    SgExpression* pntr_arr_ref;

    // array_index = rhs
    SgExpression* assign_op = SB::buildAssignOp(SB::buildVarRefExp(array_index), SI::copyExpression(rhs));

    SgExpression* lhs = array_ref->get_lhs_operand();
    SgType* lhs_type = lhs->get_type();

#ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
#endif

#ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(lhs_type, GEFD(lhs)))
#else
    if(strDecl::isValidStructType(lhs_type))
#endif
    {
      // the pntr arr ref looks like: str[index]
      // We should change that to array_index = index; Ptr_check(str, array_index)[array_index]
      // where Ptr_check does a array bound check using lookup, and then returns str.ptr
      // Maybe we should also do a temporal check in Ptr_check

      // We should insert the array_index in a *statement* before the pntr arr ref
      // This makes sure that we don't replace an operation with a comma op. With this we can make
      // sure that we don't have to handle comma ops in the next higher operation... hence it doesn't
      // complicate the hiearchy

#ifdef STRIP_TYPEDEFS
      // No need to check if lhs/rhs is typedef type. lhs is ValidStruct
      // and rhs can't be typedef type (thanks to the rhs_type above)
#endif

      SgExprListExp* param_list = SB::buildExprListExp(SI::copyExpression(lhs),
          SB::buildVarRefExp(array_index));

      SgExpression* overload = buildMultArgOverloadFn("ptr_check", param_list,
          strDecl::findInUnderlyingType(lhs_type),
          SI::getScope(array_ref),
          GEFD(SI::getEnclosingStatement(array_ref)));


#ifdef ARRAY_CHECK_PLACEMENT
      // Since we could be changing array_ref itself in the function
      // we can't do this before the replaceExpression below... we
      // need to atleast return the new array_ref to use below
      // or, do this after that...
      insertCheckCodeForArray(array_ref, assign_op);
#else
      // insert the assign op before the current statement.
      SgExprStatement* assign_stmt = SB::buildExprStatement(assign_op);

      SgStatement* stmt = Util::getSuitablePrevStmt(SI::getEnclosingStatement(array_ref));
      SI::insertStatementBefore(stmt, assign_stmt);
#endif


      // build a new pntr arr ref to take the place of the original one
      SgExpression* pntr_arr_ref = SB::buildPntrArrRefExp(overload, SB::buildVarRefExp(array_index));
      replaceWrapper(array_ref, pntr_arr_ref);
    }
    else {
      // Case of var ref...
      if(isSgVarRefExp(lhs)) {

#ifndef RETAIN_FUNC_PROTOS
        // check in varRemap
        SgVariableSymbol* param = checkInVarRemap(lhs_var_ref->get_symbol());
        if(param != NULL) {
          // This was a param, which has been converted to struct
          // This is a pointer. Arrays don't go through fncalls
          // directly. They need to be inserted within structs
          ROSE_ASSERT(isSgPointerType(lhs_type));

          // input: ptr[index]
          // output: array_bound_check_using_lookup(param.addr, param.ptr + index*sizeof(base_type)), param.ptr[index]

          // 1. Do an array_bound_check_using_lookup here, and then
          // 2. insert an actual pntr arr ref

          // 1. Array bound check using lookup


          // param.addr
          SgExpression* param_addr = Util::castToAddr(Util::createDotExpFor(param->get_declaration(), "addr"));

          // (sizeof_type)param.ptr
          SgExpression* param_ptr = Util::createDotExpFor(param->get_declaration(), "ptr");
          //      SgExpression* param_ptr_casted = SB::buildCastExp(param_ptr, Util::getSizeOfType(), SgCastExp::e_C_style_cast);

          // index*sizeof(base_type)
          SgType* base_type = isSgPointerType(param_ptr->get_type())->get_base_type();
          //      SgExpression* sizeof_exp = SB::buildSizeOfOp(base_type);
          //      SgExpression* offset = SB::buildMultiplyOp(SB::buildVarRefExp(array_index), sizeof_exp);

          // By default, the function assumes pointers... which is true in this case.
          overload = createArrayBoundCheckFn(param_addr, param_ptr, SB::buildVarRefExp(array_index),
              base_type, SI::getScope(array_ref),
              GEFD(SI::getEnclosingStatement(array_ref))
              false /* array */);

          pntr_arr_ref = SB::buildPntrArrRefExp(SI::copyExpression(param_ptr), SB::buildVarRefExp(array_index));

          //comma_op = SB::buildCommaOpExp(overload, pntr_arr_ref);
        }
        else {
#endif
          // This is the case where the lhs is not from the param list
          // So, we build an array_bound_check_using_lookup if its a pointer
          // or just an array_bound_check if its an array

#ifdef USE_ARRAY_LOOKUP_FOR_ARGV
          if(strcmp(isSgVarRefExp(lhs)->get_symbol()->get_name().str(), "argv") == 0) {
            SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));

            //SgType* base_type = isSgArrayType(lhs_type)->get_base_type();
            SgType* base_type = lhs_type; //RMM

#ifdef STRIP_TYPEDEFS
            base_type = Util::getType(base_type);
#endif

            overload = createArrayBoundCheckFn(ptr_addr, SI::copyExpression(lhs), SB::buildVarRefExp(array_index),
                base_type,
                SI::getScope(array_ref),
                GEFD(SI::getEnclosingStatement(array_ref)),
                false /* array */);

          }
          else if(isSgArrayType(lhs_type))
#else
          if(isSgArrayType(lhs_type))
#endif
          {

            SgType* base_type = isSgArrayType(lhs_type)->get_base_type();

#ifdef STRIP_TYPEDEFS // Inserted with ENUM_TO_INT_HACK
            base_type = Util::getType(base_type);
#endif


            overload = createArrayBoundCheckFn(NULL, SI::copyExpression(lhs),
                SB::buildVarRefExp(array_index),
                base_type,
                SI::getScope(array_ref),
                GEFD(SI::getEnclosingStatement(array_ref)),
                true);
          }
          else {
            ROSE_ASSERT(isSgPointerType(lhs_type));

            SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));

            SgType* base_type = isSgPointerType(lhs_type)->get_base_type();

#ifdef STRIP_TYPEDEFS // Inserted with ENUM_TO_INT_HACK
            base_type = Util::getType(base_type);
#endif

            overload = createArrayBoundCheckFn(ptr_addr, SI::copyExpression(lhs), SB::buildVarRefExp(array_index),
                base_type,
                SI::getScope(array_ref),
                GEFD(SI::getEnclosingStatement(array_ref)),
                false /* array */);
          }

          pntr_arr_ref = SB::buildPntrArrRefExp(SI::copyExpression(lhs), SB::buildVarRefExp(array_index));
          //comma_op = SB::buildCommaOpExp(overload, pntr_arr_ref);
#ifndef RETAIN_FUNC_PROTOS
        }
#endif

        // We insert the array_bound_check and assignment as a previous statement, rather than
        // putting it in a comma op with the pntr arr ref. This should work in most cases.

        SgExprStatement* assign_ovl = SB::buildExprStatement(SB::buildCommaOpExp(assign_op, overload));
        SgStatement* stmt = Util::getSuitablePrevStmt(SI::getEnclosingStatement(array_ref));
        SI::insertStatementBefore(stmt, assign_ovl);

        //SgExpression* comma_op2 = SB::buildCommaOpExp(assign_op, comma_op);

        //replaceWrapper(array_ref, comma_op2);
        // Only diff between original array_ref and the new pntr arr ref is that the
        // array index is the local variable, rather than the original expression
        replaceWrapper(array_ref, pntr_arr_ref);

      }
      else {
        // This is the case of a dot/arrow (with var oper) expression on the lhs.

        if(isSgArrayType(lhs_type)) {
          overload = createArrayBoundCheckFn(NULL, SI::copyExpression(lhs),
              SB::buildVarRefExp(array_index),
              isSgArrayType(lhs_type)->get_base_type(),
              SI::getScope(array_ref),
              GEFD(SI::getEnclosingStatement(array_ref)),
              true);
        }
        else {
          ROSE_ASSERT(isSgPointerType(lhs_type));

          SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));

          overload = createArrayBoundCheckFn(ptr_addr, SI::copyExpression(lhs), SB::buildVarRefExp(array_index),
              isSgPointerType(lhs_type)->get_base_type(),
              SI::getScope(array_ref),
              GEFD(SI::getEnclosingStatement(array_ref)),
              false /* array */);
        }

        pntr_arr_ref = SB::buildPntrArrRefExp(SI::copyExpression(lhs), SB::buildVarRefExp(array_index));

        SgExprStatement* assign_ovl = SB::buildExprStatement(SB::buildCommaOpExp(assign_op, overload));
        SgStatement* stmt = Util::getSuitablePrevStmt(SI::getEnclosingStatement(array_ref));
        SI::insertStatementBefore(stmt, assign_ovl);

        replaceWrapper(array_ref, pntr_arr_ref);

      }

    }
  }


  void handleDotExpVarOper(SgDotExp* /*dot_exp*/) {

  }


  void handleDotExpFuncOper(SgDotExp* /*dot_exp*/) {

  }

  void handleDotExp(SgDotExp* dot_exp) {

    // So, we get wrap the dot exp in a create struct... right?
    // But that doesn't always work because if its an array we would need
    // initialization data... We need to create a struct which uses
    // the dot exp's addr, and location it points to.
    // If this is an array type, then we might need to send in the initialization
    // data for that array, since we may not have recorded it.

    // Two cases here... var operand... function operand
    if(isSgVarRefExp(dot_exp->get_rhs_operand())) {
      handleDotExpVarOper(dot_exp);
    }
    else if(isSgFunctionCallExp(dot_exp->get_rhs_operand())) {
      handleDotExpFuncOper(dot_exp);
    }
    else {
      // Nothing to do in other cases... This must be
      // a class/struct var
    }
  }

  void instr(SgDotExp* dot_exp) {

    SgType* dot_exp_type = dot_exp->get_type();
#ifdef STRIP_TYPEDEFS
    dot_exp_type = Util::getType(dot_exp_type);
#endif

    if(Util::isQualifyingType(dot_exp_type)) {
      handleDotExp(dot_exp);
    }
    else {
      printf("What is this dot exp?");
      Util::printNode(dot_exp);
      ROSE_ASSERT(0);
    }

  }

  SgExpression* handleLHSArrowExp(SgArrowExp* arrow_exp) {
#ifdef LHSARROW_DEBUG
    printf("handleLHSArrowExp\n");
#endif

    SgExpression* lhs = arrow_exp->get_lhs_operand();
    SgType* lhs_type = lhs->get_type();

#ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
#endif

    // If this is a struct/var ref/another arrow/dot expression
    // we simply call deref_check/deref_check_with_str

    if(isSgThisExp(lhs)) {
      // this->val... nothing to do...
      return arrow_exp;
    }
    else {

      SgExprListExp* param_list;
      SgExpression* overload;

      // We'll be using deref_check for the var_ref/dot/arrow
      // case
#ifdef CPP_SCOPE_MGMT
      if(strDecl::isValidStructType(lhs_type, GEFD(arrow_exp)))
#else
      if(strDecl::isValidStructType(lhs_type))
#endif
      {

        param_list = SB::buildExprListExp(SI::copyExpression(lhs));

        overload = buildMultArgOverloadFn("deref_check_with_str", param_list,
            strDecl::findInUnderlyingType(lhs_type),
            Util::getScopeForExp(arrow_exp),
            GEFD(SI::getEnclosingStatement(arrow_exp)));
      }
      else if(Util::isVarRefOrDotArrow(lhs)) {

        SgExpression* lhs_ptr = SI::copyExpression(lhs);

#ifdef STRIP_TYPEDEFS
        lhs_ptr = Util::castAwayTypedef(lhs_ptr);
#endif

        SgExpression* lhs_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(lhs)));

        param_list = SB::buildExprListExp(lhs_ptr, lhs_addr);

        overload = buildMultArgOverloadFn("deref_check", param_list,
            lhs_type,
            Util::getScopeForExp(arrow_exp),
            GEFD(SI::getEnclosingStatement(arrow_exp)));

      }
      else {
        printf("Arrow LHS not supported\n");
        printf("arrowlhs\n");
        Util::printNode(lhs);
        ROSE_ASSERT(0);
      }

      SgExpression* new_arrow_exp = SB::buildArrowExp(overload, SI::copyExpression(arrow_exp->get_rhs_operand()));

      replaceWrapper(arrow_exp, new_arrow_exp);
      return new_arrow_exp;
    }
  }

  SgExpression* handleRHSArrowExp(SgArrowExp* arrow_exp) {

#ifdef RHSARROW_DEBUG
    printf("handleRHSArrowExp\n");
#endif

    // Here, there are two cases...
    // either the rhs is a var_ref/function call
    // If its a var ref, we don't handle it here.. it'll be handled
    // at the next higher op.
    // If its a func call, its already converted to a struct
    // So, nothing to do in either case
    return arrow_exp;
  }

  void instr(SgArrowExp* arrow_exp) {

    // Handle LHS of Arrow exp... and then handle
    // the RHS...
    // This function replaces the original arrow exp with the
    // new one.
    SgExpression* new_arrow_exp = handleLHSArrowExp(arrow_exp);

    // Now to handle the RHS...
    handleRHSArrowExp(isSgArrowExp(new_arrow_exp));

  }

  void instr(SgCommaOpExp* /*cop*/) {

  }

  void instrCond(SgBinaryOp* bop) {

#ifdef COND_DEBUG
    printf("handleConditionals\n");
#endif

    //std::string op_name(Util::getStringForVariantT(bop->variantT()));

    // not defined..
#ifdef COND_BOOL_RETVAL
    SgType* retType = SgTypeBool::createType();
#else
    SgType* retType = SgTypeInt::createType();
#endif

    // Conditionals always return bool
    std::string op_name = Util::getStringForOp(isSgNode(bop), retType);

    SgExpression* LHS = bop->get_lhs_operand();
    SgExpression* LHS_Copy = SI::copyExpression(LHS);

#ifdef STRIP_TYPEDEFS
    LHS_Copy = Util::castAwayTypedef(LHS_Copy);
#endif

    SgExpression* RHS = bop->get_rhs_operand();
    SgExpression* RHS_Copy = SI::copyExpression(RHS);

#ifdef STRIP_TYPEDEFS
    RHS_Copy = Util::castAwayTypedef(RHS_Copy);
#endif

    if(isSgValueExp(RHS) || isSgValueExp(LHS)) {
      // Both can't be values -- right?
      ROSE_ASSERT(isSgValueExp(LHS) && isSgValueExp(RHS));
      op_name = "_ValueCast";
    }

#ifdef OLD_NAME_GEN
    //  SgExpression* overload = buildOverloadFn(SgName(op_name), LHS_Copy, RHS_Copy, retType, Util::getScopeForExp(bop));
    //  SgExpression* overload = buildOverloadFn(SgName(op_name), LHS_Copy, RHS_Copy, retType, Util::getScopeForExp(bop), GEFD(SI::getEnclosingStatement(bop)));
    SgExpression* overload = buildMultArgOverloadFn(SgName(op_name), SB::buildExprListExp(LHS_Copy, RHS_Copy), retType, Util::getScopeForExp(bop), GEFD(SI::getEnclosingStatement(bop)));
#else
    //  SgExpression* overload = buildOverloadFn(SgName(Util::getStringForVariantT(bop->variantT())), LHS_Copy, RHS_Copy, retType, Util::getScopeForExp(bop));
    //  SgExpression* overload = buildOverloadFn(SgName(Util::getStringForVariantT(bop->variantT())), LHS_Copy, RHS_Copy, retType, Util::getScopeForExp(bop), GEFD(SI::getEnclosingStatement(bop)));
    SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(bop->variantT())), SB::buildExprListExp(LHS_Copy, RHS_Copy),
        retType, Util::getScopeForExp(bop), GEFD(SI::getEnclosingStatement(bop)));
#endif

    replaceWrapper(bop, overload);

#ifdef INLINING_CHECK
    //  bool status = inlining(isSgFunctionCallExp(overload));
    bool status = doInline(isSgFunctionCallExp(overload));
#ifdef COND_DEBUG
    printf("INLINING_CHECK: status: %s\n", status ? "true" : "false");
#endif
#endif
  }

  void instrAddSub(SgBinaryOp* bop) {
    // Cases:
    // 1. var +/- exp
    // 2. struct +/- exp
    // 3. *ptr +/- exp

    SgType* bop_type = bop->get_type();
#ifdef STRIP_TYPEDEFS
    bop_type = Util::getType(bop_type);
#endif


    SgExpression* bop_lhs = bop->get_lhs_operand();
    SgType* bop_lhs_type = bop_lhs->get_type();
#ifdef STRIP_TYPEDEFS
    bop_lhs_type = Util::getType(bop_lhs_type);
#endif

    SgExpression* bop_rhs = bop->get_rhs_operand();
    SgType* bop_rhs_type = bop_rhs->get_type();
#ifdef STRIP_TYPEDEFS
    bop_rhs_type = Util::getType(bop_rhs_type);
#endif



    if(isSgArrayType(bop_lhs_type)) {
      insertCreateEntry(SI::copyExpression(bop_lhs), Util::getScopeForExp(bop),
          SI::getEnclosingStatement(bop));
    }

    if(isSgArrayType(bop_rhs_type)) {
      insertCreateEntry(SI::copyExpression(bop_rhs), Util::getScopeForExp(bop),
          SI::getEnclosingStatement(bop));
    }


#ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(bop_lhs_type, GEFD(bop_lhs)) ||
        strDecl::isValidStructType(bop_rhs_type, GEFD(bop_rhs)))
#else
      if(strDecl::isValidStructType(bop_lhs_type) ||
          strDecl::isValidStructType(bop_rhs_type))
#endif
      {

        // input: struct +/- exp
        // output: add/sub(struct, exp);

        SgExpression* lhs_copy = SI::copyExpression(bop_lhs);
#ifdef STRIP_TYPEDEFS
        lhs_copy = Util::castAwayTypedef(lhs_copy);
#endif

        SgExpression* rhs_copy = SI::copyExpression(bop_rhs);
#ifdef STRIP_TYPEDEFS
        rhs_copy = Util::castAwayTypedef(rhs_copy);
#endif

        SgExprListExp* param_list = SB::buildExprListExp(lhs_copy, rhs_copy);
        SgType* retType;

#ifdef CPP_SCOPE_MGMT
        if(strDecl::isValidStructType(bop_lhs_type, GEFD(bop_lhs)))
#else
          if(strDecl::isValidStructType(bop_lhs_type))
#endif
          {
            retType = bop_lhs_type;
          }
          else {
            retType = bop_rhs_type;
          }

        SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(bop->variantT())),
            param_list,
            retType, Util::getScopeForExp(bop),
            GEFD(SI::getEnclosingStatement(bop)));

        replaceWrapper(bop, overload);


      }
      else if(Util::isVarRefOrDotArrow(bop_lhs) ||
          Util::isVarRefOrDotArrow(bop_rhs)) {
        // input: var +/- exp
        // output: create_struct(var +/- exp, &var)

#if 0
        Util::printNode(bop);
        Util::printNode(bop->get_type());
#endif

        SgExpression* arg1;
        if(isSgArrayType(bop_type)) {
          // this needs to be converted to the corresponding pointer type
          arg1 = SB::buildCastExp(SI::copyExpression(bop), Util::getTypeForPntrArr(bop_type),
              SgCastExp::e_C_style_cast);
        }
        else {
          arg1 = SI::copyExpression(bop);
        }

        SgExpression* arg2;
        if(Util::isVarRefOrDotArrow(bop_lhs)) {
          arg2 = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(bop_lhs)));

          // done above
#if 0
#ifdef ARRAY_CREATE_ENTRY
          // Also, we need to insert create_entry for the lhs, if its
          // array type.
          if(isSgArrayType(bop_lhs->get_type())) {
            insertCreateEntry(SI::copyExpression(bop_lhs), Util::getScopeForExp(bop),
                SI::getEnclosingStatement(bop));
          }
#endif
#endif

        }
        else {
          arg2 = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(bop_rhs)));

          // Done above
#if 0
#ifdef ARRAY_CREATE_ENTRY
          // Also, we need to insert create_entry for the rhs, if its
          // array type.
          if(isSgArrayType(bop_rhs->get_type())) {
            insertCreateEntry(SI::copyExpression(bop_rhs), Util::getScopeForExp(bop),
                SI::getEnclosingStatement(bop));
          }
#endif
#endif

        }

        SgExprListExp* param_list = SB::buildExprListExp(arg1, arg2);

#if 0
        printf("typename: %s\n", Util::getNameForType(bop->get_type()).getString().c_str());
        printf("PntrArr - typename: %s\n", Util::getNameForType(Util::getTypeForPntrArr(bop->get_type())).getString().c_str());
#endif


#if 0
        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(bop_lhs->get_type()),
            GEFD(SI::getEnclosingStatement(bop)),
            true);
#endif

        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(bop_type),
            GEFD(SI::getEnclosingStatement(bop)),
            true);

#if 0
        printf("ret typename: %s\n", Util::getNameForType(retType).getString().c_str());
#endif

        SgExpression* overload = buildMultArgOverloadFn("create_struct", param_list,
            retType, Util::getScopeForExp(bop),
            GEFD(SI::getEnclosingStatement(bop)));

        replaceWrapper(bop, overload);
      }
#ifdef DOUBLE_DIM_ENABLED
      else if(isSgPointerDerefExp(bop_lhs) || isSgPointerDerefExp(bop_rhs)) {
        // input: *ptr +/- exp
        // output: add/sub(ptr, exp)

        SgExprListExp* p_list;

        // array create entry done above

        if(isSgPointerDerefExp(bop_lhs)) {

          SgExpression* arg1 = SI::copyExpression(Util::stripDeref(bop_lhs));
#ifdef STRIP_TYPEDEFS
          arg1 = Util::castAwayTypedef(arg1);
#endif

          SgExpression* arg2 = SI::copyExpression(bop_rhs);
#ifdef STRIP_TYPEDEFS
          arg2 = Util::castAwayTypedef(arg2);
#endif

          p_list = SB::buildExprListExp(arg1, arg2);
        }
        else {

          SgExpression* arg1 = SI::copyExpression(bop_lhs);
#ifdef STRIP_TYPEDEFS
          arg1 = Util::castAwayTypedef(arg1);
#endif

          SgExpression* arg2 = SI::copyExpression(Util::stripDeref(bop_rhs));
#ifdef STRIP_TYPEDEFS
          arg2 = Util::castAwayTypedef(arg2);
#endif


          p_list = SB::buildExprListExp(arg1, arg2);
        }

        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(bop_type),
            GEFD(SI::getEnclosingStatement(bop)),
            true);

        std::string op_name = Util::getStringForVariantT(bop->variantT()) ;

        SgExpression* overload = buildMultArgOverloadFn(SgName(op_name + "_deref"),
            p_list,
            retType, Util::getScopeForExp(bop),
            GEFD(SI::getEnclosingStatement(bop)));

        replaceWrapper(bop, overload);

      }
#endif

      else {
        printf("Case not supported in handleAddSubtractOverloadOps\n");
        printf("bop\n");
        Util::printNode(bop);
        ROSE_ASSERT(0);
      }
  }

  bool isClassStructAssign(SgAssignOp* aop) {
    SgExpression* lhs = aop->get_lhs_operand();
    SgType* lhs_type = lhs->get_type();
    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif
    return strDecl::isOriginalClassType(lhs_type);
  }

  void instr(SgAssignOp* aop) {
    // is class/struct assign
    if(isClassStructAssign(aop)) {
      handleCSAssignOverloadOps(aop);
    }
    else {
      handleAssignOverloadOps(aop);
    }
  }

  void instr(SgBinaryOp* bop) {
    #ifdef INSTR_DEBUG
    printf(" instr binary ops\n");
    #endif

    if(isSgPntrArrRefExp(bop)) {
      instr(isSgPntrArrRefExp(bop));
    }
    else if(isSgDotExp(bop)) {
      instr(isSgDotExp(bop));
    }
    else if(isSgArrowExp(bop)) {
      instr(isSgArrowExp(bop));
    }
    else if(isSgCommaOpExp(bop)) {
      instr(isSgCommaOpExp(bop));
    }
    else if(Util::isConditionalOp(bop)) {
      instrCond(bop);
    }
    else if(isSgAddOp(bop) || isSgSubtractOp(bop)) {
      instrAddSub(bop);
    }
    else if(isSgAssignOp(bop)) {
      instr(isSgAssignOp(bop));
    }

  }
}
