#include "instr.h"

namespace Instr
{
  SgExpression* createDerefCheckWithStr(SgExpression* str, SgScopeStatement* scope,
      SgStatement* pos) {

#ifdef STRIP_TYPEDEFS
    // No need to do any casting here since the type is
    // valid struct type
#endif

    // Expect a struct if its not a var ref
#ifdef CPP_SCOPE_MGMT
    ROSE_ASSERT(strDecl::isValidStructType(str->get_type(), GEFD(pos)));
#else
    ROSE_ASSERT(strDecl::isValidStructType(str->get_type()));
#endif

    SgExprListExp* param_list = SB::buildExprListExp(str);

    // The underlying type will give the actual pointer type that will be returned
    SgType* retType = strDecl::findInUnderlyingType(str->get_type());

    SgExpression* overload = buildMultArgOverloadFn("deref_check_with_str", param_list,
        retType, scope,
        pos);

    return overload;
  }

  SgExpression* createDerefCheck(SgExpression* ptr, SgExpression* addr, SgScopeStatement* scope,
      SgStatement* pos) {

#ifdef STRIP_TYPEDEFS
    ptr = Util::castAwayTypedef(ptr);
    // This casting should make sure that the retType
    // and ptr_casted are not typedefs. We are basically
    // trying to create a boundary... by removing all typedefs
    // before going into the create function part. So all the
    // functions we have generated wouldn't have a typedef anywhere in them
#endif

    SgType* retType = Util::getTypeForPntrArr(ptr->get_type());

    SgType* ptr_type = ptr->get_type();

    SgExpression* ptr_casted;

#ifdef SUPPORT_REFERENCES
    if(isSgReferenceType(ptr_type)) {
      ptr_casted = ptr;
    }
    else if(isSgPointerType(ptr_type))
#else
      if(isSgPointerType(ptr_type))
#endif
      {
        ptr_casted = ptr;
      }
      else if(isSgArrayType(ptr_type)) {
        ptr_casted = SB::buildCastExp(ptr, retType, SgCastExp::e_C_style_cast);
      }
      else {
        printf("createDerefCheck: ptr is not qual type\n");
        printf("ptr\n");
        Util::printNode(ptr);
        ROSE_ASSERT(0);
      }

    SgExprListExp* param_list = SB::buildExprListExp(ptr_casted, addr);

    SgExpression* overload = buildMultArgOverloadFn("deref_check", param_list,
        retType, scope, pos);

    return overload;
  }


  SgExpression* handleCastExp(SgCastExp* cast_exp) {

#ifdef CAST_DEBUG
    printf("handleCastExp\n");
#endif

    // This function handles four different types of casts
    // 1. struct to struct    - already implemented
    // 2. value_exp to struct - already implemented
    // 3. qual_type to struct - NEW!
    // 4. struct to value     - already implemented, possibly broken!
    // 5. exp to struct - This handles the case where the expression is not a value
    // exp but rather a computed one.

    SgType* cast_type = cast_exp->get_type();
#ifdef STRIP_TYPEDEFS
    cast_type = Util::getType(cast_type);
#endif

    SgExpression* oper = cast_exp->get_operand();
    SgType* oper_type = oper->get_type();

#ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
#endif

    SgExpression* oper_copy = SI::copyExpression(oper);

#ifdef STRIP_TYPEDEFS
    oper_copy = Util::castAwayTypedef(oper_copy);
#endif

    oper_copy->set_lvalue(false);

    SgExprListExp* param_list;
    SgType* retType;


    // 1. struct to struct
#ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(oper_type, GEFD(oper)) &&
        Util::isQualifyingType(cast_type))
#else
      if(strDecl::isValidStructType(oper_type) &&
          Util::isQualifyingType(cast_type))
#endif

      {

        param_list = SB::buildExprListExp(oper_copy);
        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_type),
            GEFD(SI::getEnclosingStatement(cast_exp)),
            true);
      }
    // 2. value_exp to struct
      else if(isSgValueExp(oper) && Util::isQualifyingType(cast_type)) {

        // Lets do something slightly different for a value cast... instead
        // of just converted the value to a pointer, lets do how a traditional
        // compiler would handle... capture the value in a global variable
        // and then we will create a struct out of it.
        // input: cast("abc")
        // output: global_var = "abc"
        // output: cast(&global_var, &global_ptr, sizeof(global_var), dummy_lock)
        // Using global_ptr to get a global address

        std::string global_var_name_string = "global_var" + boost::lexical_cast<std::string>(Util::VarCounter++);
#ifdef FILE_SPECIFIC_VARNAMES
        global_var_name_string = global_var_name_string + "_" + boost::lexical_cast<std::string>(oper->get_file_info()->get_line())
          + "_" + boost::lexical_cast<std::string>(oper->get_file_info()->get_col());
#endif

        std::string global_ptr_name_string = "global_ptr" + boost::lexical_cast<std::string>(Util::VarCounter++);
#ifdef FILE_SPECIFIC_VARNAMES
        global_ptr_name_string = global_ptr_name_string + "_" + boost::lexical_cast<std::string>(oper->get_file_info()->get_line())
          + "_" + boost::lexical_cast<std::string>(oper->get_file_info()->get_col());
#endif

#ifdef CPP_SCOPE_MGMT
        // This could insert the variable as a static variable within a class... lets not do that...
        // instead of using the "getGlobalScope()".. lets do it slightly differently, to make sure
        // that the variable is inserted in the global scope... in all cases (even member func cases)
        SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(GEFD(SI::getEnclosingStatement(cast_exp)));
        SgStatement* pos;
        // Now, if its a member func decl, find the location before the class..
        if(SgMemberFunctionDeclaration* mem = isSgMemberFunctionDeclaration(fn_decl)) {
          SgClassDefinition* def = isSgClassDefinition(mem->get_scope());
          pos = isSgStatement(isSgClassDeclaration(def->get_parent()));
        }
        else {
          pos = fn_decl;
        }
        ROSE_ASSERT(isSgGlobal(pos->get_scope()));

        SgVariableDeclaration* global_var = SB::buildVariableDeclaration(
            SgName(global_var_name_string),
            oper_type,
            SB::buildAssignInitializer(SI::copyExpression(oper_copy), oper_type),
            pos->get_scope());
        SI::insertStatementBefore(pos, global_var);

#else
        SgVariableDeclaration* global_var = SB::buildVariableDeclaration(
            SgName(global_var_name_string),
            oper_type,
            SB::buildAssignInitializer(SI::copyExpression(oper_copy), oper_type),
            getGlobalScope(cast_exp));

        SI::insertStatementBefore(GEFD(SI::getEnclosingStatement(cast_exp)), global_var);
#endif

        // Added to make sure that each file has its own set of generated global variables
        Util::makeDeclStatic(global_var);

        // We could've used a lookup ticket... right?
#ifdef INSERT_GLOBAL_PTR_FOR_ADDR
        SgVariableDeclaration* global_ptr = SB::buildVariableDeclaration(
            SgName(global_ptr_name_string),
            oper_type,
            NULL,
            getGlobalScope(cast_exp));

        SI::insertStatementBefore(GEFD(SI::getEnclosingStatement(cast_exp)), global_ptr);
#endif


        // global_var
        SgExpression* global_var_casted;

        // This handles cases where NULL is converted to a void pointer
        if(Util::getNameForDecl(global_var)->get_type() != cast_type) {
          global_var_casted = SB::buildCastExp(SB::buildVarRefExp(global_var), cast_type, SgCastExp::e_C_style_cast);
        }
        else {
          global_var_casted = SB::buildVarRefExp(global_var);
        }

        // &global_var
        SgExpression* addr_global_var = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(global_var)));

#ifdef INSERT_GLOBAL_PTR_FOR_ADDR
        // &global_ptr
        SgExpression* addr_global_ptr = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(global_ptr)));
#endif

        // sizeof(global_var)
        SgExpression* sizeof_global_var = SB::buildCastExp(SB::buildSizeOfOp(SB::buildVarRefExp(global_var)),
            Util::getSizeOfType(), SgCastExp::e_C_style_cast);

        // lock
        SgExpression* lock = Util::castToAddr(Util::getDummyLock());

#ifdef INSERT_GLOBAL_PTR_FOR_ADDR
        // the global ptr supplies the address -- using address of
        // the global variable supplies the pointer -- using address of
        // the size is given by sizeof(global_var)
        // lock is a dummy lock for now
        param_list = SB::buildExprListExp(addr_global_var, addr_global_ptr, sizeof_global_var, lock);
#else
        param_list = SB::buildExprListExp(global_var_casted, addr_global_var, sizeof_global_var, lock);
#endif

        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_type),
            GEFD(SI::getEnclosingStatement(cast_exp)),
            true);

#if 0
        param_list = SB::buildExprListExp(oper_copy);
        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_exp->get_type()),
            GEFD(SI::getEnclosingStatement(cast_exp)),
            true);
#endif
      }
    // 3. qual_type to struct - NEW!
      else if(Util::isQualifyingType(oper_type) &&
          Util::isQualifyingType(cast_type)) {


#ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the arg, if its
        // array type.
        if(isSgArrayType(oper_type)) {
          insertCreateEntry(SI::copyExpression(oper_copy), Util::getScopeForExp(cast_exp),
              SI::getEnclosingStatement(cast_exp));
        }
#endif


        SgExpression* ptr = oper_copy;
        SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper_copy)));

        param_list = SB::buildExprListExp(ptr, addr);
        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_type),
            GEFD(SI::getEnclosingStatement(cast_exp)),
            true);
      }
    // 4. struct to value
#ifdef CPP_SCOPE_MGMT
      else if(strDecl::isValidStructType(oper_type, GEFD(oper)) &&
          !(Util::isQualifyingType(cast_type) ||
            strDecl::isValidStructType(cast_type, GEFD(oper))))
#else
      else if(strDecl::isValidStructType(oper_type) &&
          !(Util::isQualifyingType(cast_type) ||
            strDecl::isValidStructType(cast_type)))
#endif
      {
        param_list = SB::buildExprListExp(oper_copy);
        retType = cast_type;
      }
    // 5. primitive/original class types to qualifying type -- unlike value exps above
    // these are not known statically, they are computed at runtime (like ret vals from fns)
      else if(!Util::isQualifyingType(oper_type) && Util::isQualifyingType(cast_type)) {
        // For this case, we don't have any metadata... so, we create a struct... and we don't
        // create an entry in the TrackingDB. This will make sure that any check_entry occuring
        // by dereferencing this value will fail...
        // If we wanted to allow a deref, we could always use a dummy entry instead.

        // create_struct((ptr_type) oper_copy, (addr_type)Util::getLookupTicket())
        SgExpression* casted_oper_copy = SB::buildCastExp(oper_copy, cast_type,
            SgCastExp::e_C_style_cast);
        SgExpression* casted_lookup_ticket = Util::castToAddr(Util::getLookupTicket());

        param_list = SB::buildExprListExp(casted_oper_copy, casted_lookup_ticket);
        retType = strDecl::getStructType(Util::getTypeForPntrArr(cast_type),
            GEFD(SI::getEnclosingStatement(cast_exp)),
            true);

        SgExpression* overload = buildMultArgOverloadFn("create_struct", param_list, retType,
            Util::getScopeForExp(cast_exp),
            GEFD(SI::getEnclosingStatement(cast_exp)));

        replaceWrapper(cast_exp, overload);
        return overload;
      }
      else {
        printf("Unsupported case in Cast Exp\n");
        printf("cast_exp");
        Util::printNode(cast_exp);
        printf("cast_exp->get_type(): %s", cast_type->unparseToString().c_str());
        printf("cast_exp->get_operand()->get_type(): %s",
            oper_type->unparseToString().c_str());
        ROSE_ASSERT(0);
      }

    SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(cast_exp->variantT())),
        param_list,
        retType,
        Util::getScopeForExp(cast_exp),
        GEFD(SI::getEnclosingStatement(cast_exp)));


    replaceWrapper(cast_exp, overload);

    return overload;
  }

  void instr(SgCastExp* cast_exp) {
    SgType* cast_type = cast_exp->get_type();

#ifdef STRIP_TYPEDEFS
    cast_type = Util::getType(cast_type);
#endif

    SgExpression* oper = cast_exp->get_operand();
    SgType* oper_type = oper->get_type();

#ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
#endif

#if 1
#ifdef CPP_SCOPE_MGMT
    if(Util::isQualifyingType(cast_type) ||
        strDecl::isValidStructType(oper_type, GEFD(cast_exp)))
#else
    if(Util::isQualifyingType(cast_type) ||
        strDecl::isValidStructType(oper_type))
#endif
    {
      handleCastExp(cast_exp);
    }
    else {
      printf("What is this cast exp?");
      Util::printNode(cast_exp);
      ROSE_ASSERT(0);
    }
#endif

  }

  void instr(SgPointerDerefExp* ptr_deref) {

#ifdef DEREF_DEBUG
    printf("handleDerefExp\n");
#endif


    // Here, we expect a var ref as the operand... or a struct.
    // First, the var ref case.
    // Single function call does deref_check and returns the
    // appropriate pointer. This handles only single dimensional
    // pointers/arrays for now.

    SgExpression* overload;

    SgExpression* oper = ptr_deref->get_operand();
    SgType* oper_type = oper->get_type();

#ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
#endif

#ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(oper_type, GEFD(oper)))
#else
    if(strDecl::isValidStructType(oper_type))
#endif
    {
      // We need to insert deref_check_with_str
      // input: *str
      // output: *deref_check_with_str(str);
      overload = createDerefCheckWithStr(SI::copyExpression(ptr_deref->get_operand()),
          Util::getScopeForExp(ptr_deref),
          GEFD(SI::getEnclosingStatement(ptr_deref)));
    }
    else {
      // If this is a var ref
      if(isSgVarRefExp(oper)) {


#ifndef RETAIN_FUNC_PROTOS
        SgVarRefExp* var_ref = isSgVarRefExp(oper);
        SgVariableSymbol* param = checkInVarRemap(var_ref->get_symbol());
        if(param != NULL) {
          // This was a param, which has been converted to struct
          // This is a pointer. Arrays don't go through fncalls
          // directly. They need to be inserted within structs

          SgType* var_ref_type = var_ref->get_type();

#ifdef STRIP_TYPEDEFS
          var_ref_type = Util::getType(var_ref_type);
#endif
          ROSE_ASSERT(isSgPointerType(var_ref_type));

          // input: *ptr
          // output: *deref_check_with_str(ptr_str)

          overload = createDerefCheckWithStr(SB::buildVarRefExp(param),
              SI::getScope(ptr_deref),
              GEFD(SI::getEnclosingStatement(ptr_deref)));
        }
        else {
#endif
          // This is var ref which is not part of params
          // input: *var_ref
          // output: *deref_check(var_ref, &var_ref)

          // We might also need to do a create_entry if the operand is
          // of array type
#ifdef ARRAY_CREATE_ENTRY
          // Also, we need to insert create_entry, if its
          // array type.
          if(isSgArrayType(oper_type)) {
            insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(ptr_deref),
                SI::getEnclosingStatement(ptr_deref));
          }
#endif


          overload = createDerefCheck(SI::copyExpression(oper),
              Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper))),
              SI::getScope(ptr_deref),
              GEFD(SI::getEnclosingStatement(ptr_deref)));
#ifndef RETAIN_FUNC_PROTOS
        }
#endif
      }
      else if(isSgDotExp(oper) || isSgArrowExp(oper)) {
        // This is a dot/arrow beneath... with a var ref on the rhs.
        // input: *var.ptr or *var->ptr
        // output: *deref_check(var.ptr, &var.ptr) or *deref_check(var->ptr, &var->ptr)

        // We might also need to do a create_entry if the operand is
        // of array type
#ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry, if its
        // array type.
        if(isSgArrayType(oper_type)) {
          insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(ptr_deref),
              SI::getEnclosingStatement(ptr_deref));
        }
#endif

        overload = createDerefCheck(SI::copyExpression(oper),
            Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper))),
            SI::getScope(ptr_deref),
            GEFD(SI::getEnclosingStatement(ptr_deref)));
      }
#ifdef DOUBLE_DIM_ENABLED
      else if(isSgPointerDerefExp(oper)) {
        // This is a double pointer deref... **ptr
        // input: **(deref_check(ptr, &ptr))
        // output: *(double_deref(deref_check(ptr, &ptr)))

#ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry, if its
        // array type.
        if(isSgArrayType(oper_type)) {
          insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(ptr_deref),
              SI::getEnclosingStatement(ptr_deref));
        }
#endif

        SgExpression* strip_deref = SI::copyExpression(Util::stripDeref(ptr_deref->get_operand()));

#ifdef STRIP_TYPEDEFS
        strip_deref = Util::castAwayTypedef(strip_deref);
#endif

        overload = buildMultArgOverloadFn("double_deref_check",
            SB::buildExprListExp(strip_deref),
            oper_type,
            Util::getScopeForExp(ptr_deref),
            GEFD(SI::getEnclosingStatement(ptr_deref)));

      }
      else if(isSgPntrArrRefExp(oper)) {
        // This is deref of a pntr arr ref... *array[index]
        // input: *array[index]
        // output: *deref_check(array[index], &array[index])

        // We have performed the array bound check for
        // the pntr arr ref. Here, we need to perform a deref check on that.

#ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry, if its
        // array type.
        if(isSgArrayType(oper_type)) {
          insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(ptr_deref),
              SI::getEnclosingStatement(ptr_deref));
        }
#endif

        overload = createDerefCheck(SI::copyExpression(oper),
            Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper))),
            SI::getScope(ptr_deref),
            GEFD(SI::getEnclosingStatement(ptr_deref)));
      }
#endif
      else if(isSgThisExp(oper)) {
        // "this" would be derefed before being used in a dot exp...
        // nothing to do here..
        return;
      }
      else {
        printf("Unsupported case in DerefExp\n");
        printf("ptr_deref\n");
        Util::printNode(ptr_deref);
        ROSE_ASSERT(0);
      }
    }

    // Need to figure out here if the ptr_deref output type is also of qualifying type
    // If it is, then we call DoublePointerDeref on the overload created above
    // In the DoublePointerDeref...
    // for structs, we'll say return create_struct(*str.ptr, str.ptr)
    // for variables, we'll say return create_struct(*ptr, ptr)

    SgExpression* new_exp;
    // Make sure that overload is of qualifying type
    //ROSE_ASSERT(isSgPointerType(overload->get_type()));
    ROSE_ASSERT(Util::isQualifyingType(overload->get_type()));

    new_exp = SB::buildPointerDerefExp(overload);

    replaceWrapper(ptr_deref, new_exp);

  }

  void instr(SgAddressOfOp* aop) {

    SgExpression* oper = aop->get_operand();
    SgType* oper_type = oper->get_type();

    SgType* aop_type = aop->get_type();

#ifdef STRIP_TYPEDEFS
    aop_type = Util::getType(aop_type);
#endif

#ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
#endif

#ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(oper_type, GEFD(aop)))
#else
    if(strDecl::isValidStructType(oper_type))
#endif
    {
      printf("handleAddressOfOp: valid struct type as operand???\n");
      printf("aop\n");
      Util::printNode(aop);
      ROSE_ASSERT(0);
    }
    else {
      // Address of op creates a value, so what we need to do
      // is to get the metadata, if available, for the location
      // we are taking the address of. If that isn't available then,
      // we create an entry
      // using a lookup ticket (we need a location in TrackingDB, and we'll
      // use a lookup ticket since there's really no address here to use)

      SgExprListExp* param_list;

      if(isSgArrayType(oper_type)) {
        // For arrays, it is straightforward. We'll create an entry for it
        // and use its address for the struct.addr
        // input: &array
        // output: address_of(&array, &array, sizeof(array), dummy_lock)
        // Interestingly... here, we'll simply do: output.ptr = &array
        // and output.addr = &array since the lookup for the metadata is
        // the addr of the array, and the pointer will point to the array...
        // same thing, basically!

#if 0
        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(oper->get_type()),
            GEFD(SI::getEnclosingStatement(aop)),
            true);
#endif


        SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(oper)));
        SgExpression* ptr = Util::createAddressOfOpFor(SI::copyExpression(oper));
        SgExpression* size = SB::buildCastExp(SB::buildSizeOfOp(SI::copyExpression(oper)), Util::getSizeOfType(), SgCastExp::e_C_style_cast);
        SgExpression* lock = Util::castToAddr(Util::getDummyLock());

        param_list = SB::buildExprListExp(addr, ptr, size, lock);

      }
      else if(isSgPointerType(oper_type)) {

        // We are taking the address of a pointer variable here,
        // So, we need to generate metadata for the memory location given by
        // the pointer itself...
        // Here, there isn't an address at which we can store this info...

        SgExpression* addr = Util::castToAddr(Util::getLookupTicket());
        SgExpression* ptr = Util::createAddressOfOpFor(SI::copyExpression(oper));
        SgExpression* size = SB::buildCastExp(SB::buildSizeOfOp(SI::copyExpression(oper)), Util::getSizeOfType(),
            SgCastExp::e_C_style_cast);
        SgExpression* lock = Util::castToAddr(Util::getDummyLock());

        param_list = SB::buildExprListExp(addr, ptr, size, lock);
      }
      else {
        // Same as pointer above...
        // We are taking the address of a pointer variable here,
        // So, we need to generate metadata for the memory location given by
        // the pointer itself...
        // Here, there isn't an address at which we can store this info...

        SgExpression* addr = Util::castToAddr(Util::getLookupTicket());
#ifndef COUT_FIX
        SgExpression* ptr = Util::createAddressOfOpFor(SI::copyExpression(oper));
#else
        SgExpression* copy = SI::copyExpression(oper);
        copy->set_lvalue(false);
        SgExpression* ptr = SB::buildAddressOfOp(copy);
#endif

        SgExpression* size = SB::buildCastExp(SB::buildSizeOfOp(SI::copyExpression(oper)), Util::getSizeOfType(),
            SgCastExp::e_C_style_cast);
        SgExpression* lock = Util::castToAddr(Util::getDummyLock());

        param_list = SB::buildExprListExp(addr, ptr, size, lock);
      }

      // FIXME: Util::getTypeForPntrArr may not be the right thing to use here
      SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(aop_type),
          GEFD(SI::getEnclosingStatement(aop)),
          true);

      SgExpression* overload = buildMultArgOverloadFn("AddressOf", param_list,
          retType, Util::getScopeForExp(aop),
          GEFD(SI::getEnclosingStatement(aop)));

      replaceWrapper(aop, overload);


    }

    return;
  }

  void instrOvlUnary(SgUnaryOp* uop) {

#ifdef UNARY_OVLOP_DEBUG
    printf("handleUnaryOverloadOps\n");
#endif

    SgType* uop_type = uop->get_type();
#ifdef STRIP_TYPEDEFS
    uop_type = Util::getType(uop_type);
#endif

    SgExpression* oper = uop->get_operand();
    SgType* oper_type = oper->get_type();
#ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
#endif

    // For a var ref/ dot / arrow, overload the op
    // and update the varible in the overloading fn
#ifdef CPP_SCOPE_MGMT
    if(strDecl::isValidStructType(oper_type, GEFD(uop)))
#else
      if(strDecl::isValidStructType(oper_type))
#endif
      {

#ifdef CONVERT_ARG_STRUCTS_TO_PTR
        // Its possible that the struct type exp is actually a reference to a variable
        // . one case is when the variable is a parameter to the function, and has been
        // converted to a struct. Here, we should actually manipulate the ptr within the
        // struct, not just pass the struct by value. So, this is what we do. We'll
        // create a dot exp in place of the struct type exp of the variable. We can then simply
        // call this function again, so that it handles the expression appropriately.
        if(isSgVarRefExp(oper)) {
          // This is the case we are looking for. So, lets create a dot exp to replace the
          // the oper.
          SgVarRefExp* var_ref_exp = isSgVarRefExp(oper);
          SgInitializedName* var_name = var_ref_exp->get_symbol()->get_declaration();
          SgExpression* dot_exp = Util::createDotExpFor(var_name, "ptr");
          replaceWrapper(oper, dot_exp);
          ROSE_ASSERT(uop->get_operand() == dot_exp);
          // Now call handleUnaryOverloadOps and it should be able to handle the dot exp correctly
          instrOvlUnary(uop);
          return;
        }
#endif


        // Its a struct. Overload the operation
        // and return the updated struct
        // The struct needs to be passed by reference
        // since we would -- its a struct .. so its a value
        // so no need to pass by ref. we only need the value out... RIGHT?
        SgExprListExp* param_list = SB::buildExprListExp(SI::copyExpression(oper));
        SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(uop->variantT())),
            param_list, oper_type,
            Util::getScopeForExp(uop),
            GEFD(SI::getEnclosingStatement(uop)));

        replaceWrapper(uop, overload);
      }
      else if(Util::isVarRefOrDotArrow(oper)) {

        // We might also need to do a create_entry if the arg is
        // of array type
#ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the arg, if its
        // array type.
        if(isSgArrayType(oper_type)) {
          insertCreateEntry(SI::copyExpression(oper), Util::getScopeForExp(uop),
              SI::getEnclosingStatement(uop));
        }
#endif

        // input: oper++, oper--
        // output: plusplus(&oper), minusminus(&oper)

        SgExprListExp* param_list = SB::buildExprListExp(Util::createAddressOfOpFor(SI::copyExpression(oper)));

        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(oper_type),
            GEFD(SI::getEnclosingStatement(uop)),
            true);

        SgExpression* overload = buildMultArgOverloadFn(SgName(Util::getStringForVariantT(uop->variantT())),
            param_list, retType,
            Util::getScopeForExp(uop),
            GEFD(SI::getEnclosingStatement(uop)));

        replaceWrapper(uop, overload);

        // Nothing to do
#ifdef UNARY_OVLOP_DEBUG
        printf("Var ref on unary op. Nothing to do\n");
#endif
      }
#ifdef DOUBLE_DIM_ENABLED
      else if(isSgPointerDerefExp(oper)) {
        // input: *ptr++ / *ptr--
        // output: deref_incr_decr(ptr)

        // 1. strip the deref
        // 2. call deref_incr_decr(ptr)

        SgExpression* ptr = SI::copyExpression(Util::stripDeref(oper));
#ifdef STRIP_TYPEDEFS
        ptr = Util::castAwayTypedef(ptr);
#endif

        SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(oper_type),
            GEFD(SI::getEnclosingStatement(uop)),
            true);

        std::string op_name = Util::getStringForVariantT(uop->variantT()) ;

        SgExpression* overload = buildMultArgOverloadFn(SgName(op_name + "_deref"),
            SB::buildExprListExp(ptr), retType,
            Util::getScopeForExp(uop),
            GEFD(SI::getEnclosingStatement(uop)));

        replaceWrapper(uop, overload);
      }
#endif
      else {
        printf("Case not supported in UnaryOverloadOps\n");
        printf("unaryop\n");
        Util::printNode(uop);
        ROSE_ASSERT(0);
      }
  }

  void instr(SgUnaryOp* uop) {
    #ifdef INSTR_DEBUG
    printf("inst unary op\n");
    #endif
    if(isSgCastExp(uop)) {
      instr(isSgCastExp(uop));
    }
    else if(isSgPointerDerefExp(uop)) {
      instr(isSgPointerDerefExp(uop));
    }
    else if(isSgAddressOfOp(uop)) {
      instr(isSgAddressOfOp(uop));
    }
    else if(Trav::NeedsToBeOverloaded(uop)) {
      instrOvlUnary(uop);
    }
  }
}
