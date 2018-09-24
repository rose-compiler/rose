#include "instr.h"


namespace Instr {

  void handlePointerVarRefs(SgVarRefExp* var_ref) {

#ifdef POINTER_VAR_REF_DEBUG
    printf("handlePointerVarRefs\n");
#endif

    // Here, we call create_struct function, which will
    // create a struct variable using the pointer value
    // and the pointer variable's address
    // Any initialization data for this pointer would already be
    // in the TrackingDB if this pointer was initialized.

    // do the old trick for cases where the symbol is found
    // in the varRemap -- these are variables in the parameter
    // list that have been converted to structs.
    // If we retain function prototypes, original params stay in
    // place
#ifndef RETAIN_FUNC_PROTOS
    if(findInVarRemap(var_ref)) {
      handleVarRefs(var_ref);
      return;
    }
#endif



    // If the parent to this var_ref is an arrow/dot expression, don't
    // touch it... this variable is not a local/global variable... its
    // address and metadata is given by the result of the dot/arrow exp
    if(Util::isDotOrArrow(var_ref->get_parent())) {
#ifdef POINTER_VAR_REF_DEBUG
      printf("Parent is dot/arrow exp\n");
      Util::printNode(var_ref->get_parent());
      printf("returning\n");
#endif
      return;
    }




#ifdef DELAYED_INSTR
    if(isSgPntrArrRefExp(var_ref->get_parent())) {
      // This should be the LHS operand of the Pntr Arr Ref
      ROSE_ASSERT(isSgPntrArrRefExp(var_ref->get_parent())->get_lhs_operand()
          == var_ref);
      // Do nothing, we'll handle it Pntr Arr Ref level
      return;
    }
    if(isSgSizeOfOp(var_ref->get_parent())) {
      // Do nothing. sizeof needs no handling
      return;
    }
    if(isSgPointerDerefExp(var_ref->get_parent())) {
      // Do nothing. Handle this in PointerDeref
      return;
    }
    if(Trav::NeedsToBeOverloaded(var_ref->get_parent())) {
      // Do nothing. Handle this in handleOverloadOps
      return;
    }
#endif

    // Why do we need this?
#ifndef NO_INSTR_AT_VAR_REFS
    // This part is not active but is being updated with
    // STRIP_TYPEDEFS for later use... if necessary

    SgExpression* createfn = createStructForVarRef(var_ref);

    SgType* var_ref_type = var_ref->get_type();
#ifdef STRIP_TYPEDEFS
    var_ref_type = Util::getType(var_ref_type);
#endif

    mapExprToOriginalType(createfn, var_ref_type);
#ifndef ONLY_ADD_ARRAYS_TO_OVR
    mapExprToOriginalVarRef(createfn, var_ref);
#ifdef POINTER_VAR_REF_DEBUG
    printf("handlePointerVarRefs\n");
    printf("mapping\n");
    printf("createfn\n");
    Util::printNode(createfn);
    printf("key: %llu\n", (unsigned long long)createfn);
    printf("var_ref\n");
    Util::printNode(var_ref);
#endif
#endif

    replaceWrapper(var_ref, createfn);
#endif
  }

  void handleArrayVarRefs(SgVarRefExp* var_ref) {

#ifndef RETAIN_FUNC_PROTOS
    // Array variables can be passed without turning into pointers at the other end...
    if(findInVarRemap(var_ref)) {
      handleVarRefs(var_ref);
      return;
    }
#endif


    // If the parent to this var_ref is an arrow/dot expression, don't
    // touch it... this variable is not a local/global variable... its
    // address and metadata is given by the result of the dot/arrow exp
    if(Util::isDotOrArrow(var_ref->get_parent())) {
#ifdef ARRAY_VAR_REF_DEBUG
      printf("Parent is dot/arrow exp\n");
      Util::printNode(var_ref->get_parent());
      printf("returning\n");
#endif
      return;
    }

#ifdef DELAYED_INSTR
    if(isSgPntrArrRefExp(var_ref->get_parent())) {
      // This should be the LHS operand of the Pntr Arr Ref
      ROSE_ASSERT(isSgPntrArrRefExp(var_ref->get_parent())->get_lhs_operand()
          == var_ref);
      // Do nothing, we'll handle it Pntr Arr Ref level
      return;
    }
    if(isSgSizeOfOp(var_ref->get_parent())) {
      // Do nothing. sizeof needs no handling
      return;
    }
    if(isSgPointerDerefExp(var_ref->get_parent())) {
      // Do nothing. Handle this in PointerDeref
      return;
    }
    if(Trav::NeedsToBeOverloaded(var_ref->get_parent())) {
      // Do nothing. Handle this in handleOverloadOps
      return;
    }
#endif

    // WHY IS THIS STUFF NECESSARY?
#ifndef NO_INSTR_AT_VAR_REFS
    // This part is not used right now... but updating it with
    // STRIP_TYPEDEFS if it is used later...

    SgExpression* createfn = createStructForVarRef(var_ref);

#ifdef ARRAY_VAR_REF_DEBUG
    printf("handleArrayVarRefs\n");
#endif

    SgType* var_ref_type = var_ref->get_type();

#ifdef STRIP_TYPEDEFS
    var_ref_type = Util::getType(var_ref_type);
#endif

    mapExprToOriginalType(createfn, var_ref_type);
    mapExprToOriginalVarRef(createfn, var_ref);
#ifdef ARRAY_VAR_REF_DEBUG
    printf("mapping\n");
    printf("createfn\n");
    Util::printNode(createfn);
    printf("key: %llu\n", (unsigned long long)createfn);
    printf("var_ref\n");
    Util::printNode(var_ref);
#endif


    replaceWrapper(var_ref, createfn);

#endif
  }

  void handleClassStructVarRefs(SgVarRefExp* var_ref) {
    // This function replaces class object var refs/struct var refs
    // with the correct version.. since, the object/var might have been
    // replaced in the func param list by a ref... this replacement
    // is done so that we can copy over the pointers... The actual
    // pointer copy occurs when the func param is replaced. At that
    // place, we insert into the ClassStructVarRemap. Here, we check that
    // map to see if we need to replace the current var ref with a new one

#ifdef CLASS_STRUCT_VAR_REF_DEBUG
    printf("handleClassStructVarRefs\n");
#endif

    SgVariableSymbol* var_symbol = var_ref->get_symbol();
    ROSE_ASSERT(var_symbol != NULL);
    VariableSymbolMap_t::iterator iter = classStructVarRemap.find(var_ref->get_symbol());
    // FIXME: Should not be equal to NULL... correct?
    if(iter == classStructVarRemap.end()) {
      // This var ref need not be replaced
      return;
    }

    SgVariableSymbol* str_symbol = iter->second;

    ROSE_ASSERT(str_symbol != NULL);
    SgVarRefExp* str_var_ref = SB::buildVarRefExp(isSgVariableSymbol(str_symbol));
    ROSE_ASSERT(str_var_ref != NULL);
    replaceWrapper(var_ref, str_var_ref);
#ifdef CLASS_STRUCT_VAR_REF_DEBUG
    printf("Done\n");
#endif


  }

  bool existsInSLKM(SgScopeStatement* scope) {

    ScopeLockKeyMap::iterator it = scopeLockMap.find(scope);
    return (it != scopeLockMap.end());
  }

  LockKeyPair findInSLKM(SgScopeStatement* scope) {
    ScopeLockKeyMap::iterator it = scopeLockMap.find(scope);
    ROSE_ASSERT(it != scopeLockMap.end());
    return it->second;
  }

  SgType* getLockType() {
    return SgTypeLongLong::createType();
  }

  SgType* getKeyType() {
    return SgTypeLongLong::createType();
  }



  // For normal (non-pointer) variables, check if the variable
  // has been initialized before this use.
  void insertInitCheckAtVarRef(SgVarRefExp* var_ref) {

    // 1. Find the scope lock for this variable
    // 2. If var is LVal, insert update.
    // 3. If var is not LVal, insert check.

    // 1. Find the scope for this variable
    SgInitializedName* name = var_ref->get_symbol()->get_declaration();
    // FIXME: The scope for function parameters is different from
    // the variable in function body. Lock and key are in the function body
    // so, we might not find the lock and key for the function body here
    // if the "name" is a parameter
    // Simple check for this would be to see the parent of the name, and
    // if its a param list, then just use the scope of the function body
    // to find the lock and key.
    SgScopeStatement* scope = name->get_scope();
    LockKeyPair lock_key = findInSLKM(scope);
    SgVariableDeclaration* lock = lock_key.first;

    // check/update(lock, &var)
    SgExpression* ce1 = SB::buildVarRefExp(lock);
    SgExpression* ce2 = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(var_ref)));
    SgExprListExp* param_list = SB::buildExprListExp(ce1, ce2);

    SgExpression* overload;
    // 2. If the var is LVal, insert update
    if(var_ref->get_lvalue()) {
      // LValues write to the location, so they update initialization info.
      overload = buildMultArgOverloadFn("update_initinfo", param_list, SgTypeVoid::createType(),
          SI::getScope(var_ref), GEFD(SI::getEnclosingStatement(var_ref)));
    }
    else {
      // Check if location is initialized before accessing it.
      overload = buildMultArgOverloadFn("check_initinfo", param_list, SgTypeVoid::createType(),
          SI::getScope(var_ref), GEFD(SI::getEnclosingStatement(var_ref)));
    }

    // original: var
    // transformed: check/update(lock, &var), var
    SgCommaOpExp* comma_op = SB::buildCommaOpExp(overload, SI::copyExpression(var_ref));

    replaceWrapper(var_ref, comma_op);

  }

  void handleNormalVarRefs(SgVarRefExp* var_ref) {

    if(Util::isDotOrArrow(var_ref->get_parent())) {
#ifdef NORMAL_VAR_REF_DEBUG
      printf("Parent is dot/arrow exp\n");
      Util::printNode(var_ref->get_parent());
      printf("returning\n");
#endif
      return;
    }

    insertInitCheckAtVarRef(var_ref);
  }



  void instr(SgVarRefExp* ref) {
    #ifdef INSTR_DEBUG
    printf("instr var ref exp\n");
    #endif

    // FIXME: Skip cases where parent is
    // arrow or dot exp... and this is the rhs!
#ifdef VAR_REF_DEBUG
    printf("handleVarRefs4\n");
#endif
    ROSE_ASSERT(ref->get_symbol() != NULL);

    SgType* var_type = ref->get_type();

#ifdef STRIP_TYPEDEFS
    var_type = Util::getType(var_type);
#endif

    // Removing these checks since parameter inputs are
    // removed from the function, and replaced by structs.
    // using get_type() seg faults.
#if 1
#ifdef SUPPORT_REFERENCES
    if(isSgPointerType(var_type) || isSgReferenceType(var_type))
#else
    if(isSgPointerType(var_type))
#endif
    {
      handlePointerVarRefs(ref);
    }
    else if(isSgArrayType(var_type)) {
      handleArrayVarRefs(ref);
    }
#ifdef CLASS_STRUCT_COPY
    else if(isSgClassType(var_type)) {
      handleClassStructVarRefs(ref);
    }
#endif
    else {
#ifdef INIT_CHECKS
      handleNormalVarRefs(ref);
#else
      printf("What is this var ref doing here?\n");
      Util::printNode(ref);
      ROSE_ASSERT(0);
#endif
    }
#endif

  }
}
