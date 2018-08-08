#include <iostream>

#include "instr.h"

#define FUNCCALL_DEBUG 1

namespace Instr
{
  SgFunctionDeclaration* createDefiningDecl(SgName transf_name, SgType* retType, SgExprListExp* param_list,
      SgScopeStatement* /*scope*/, SgProject* /*project*/, SgNode* pos, bool derived, SgName s_name) {
#ifdef DEFDECL_DEBUG
    std::cerr << ("Creating Defining decl\n");
    std::cerr << "transf_name: << " <<  transf_name << std::endl;
#endif

#if 0
    SgGlobal* globalScope = SI::getFirstGlobalScope(project);
    pushScopeStack(globalScope);
#endif

    SgFunctionParameterList* new_param_list = SB::buildFunctionParameterList();

    SgExpressionPtrList& exprs = param_list->get_expressions();

#ifdef DEFDECL_DEBUG
    std::cerr << ("creating param_list\n");
#endif

    unsigned int arg_counter = 0;
    for(SgExpressionPtrList::iterator iter = exprs.begin(); iter != exprs.end();
        iter++) {
      arg_counter++;
      SgExpression* ce = *iter;
#ifdef DEFDECL_DEBUG
      std::cerr << "ce: " << isSgNode(ce)->sage_class_name()
                << " = " << isSgNode(ce)->unparseToString()
                << std::endl;
#endif
      std::string arg_name = "input" + boost::lexical_cast<std::string>(arg_counter);
      SgInitializedName* new_name = SB::buildInitializedName(SgName(arg_name),ce->get_type(), NULL);
#ifdef STRIP_STATIC_PARAMS
      SgStorageModifier& sm = new_name->get_storageModifier();
      std::cerr << "storage_modifier: " << sm.get_modifier() << std::endl;
#endif


#ifdef DEFDECL_DEBUG
      std::cerr << ("created new_name\n");
#endif
      // FIXME: Scope should be fndecl, not globalScope.
      //new_name->set_scope(globalScope);
      //new_name->set_scope(new_param_list->get_scope());
      new_param_list->append_arg(new_name);
      new_name->set_parent(new_param_list);
    }

#ifdef DEFDECL_DEBUG
    std::cerr << ("creating defdecl\n");
#endif

    // Working one
#if 0
    SgFunctionDeclaration* defdecl = SB::buildDefiningFunctionDeclaration(transf_name,
        retType,
        new_param_list);
#else

#ifdef CPP_SCOPE_MGMT
    if(strDecl::functionInteractsWithStdOrPrimitives(param_list, retType, pos) && !Util::isExternal(s_name) && !derived) {
      pos = Util::FirstDeclStmtInFile;
#ifdef DEFDECL_DEBUG
      std::cerr << ("changed pos to first decl stmt\n");
#endif
    }
#ifdef DEFDECL_DEBUG
    std::cerr << ("pos\n");
    Util::printNode(pos);
#endif
    SgFunctionDeclaration* defdecl;
    if(isSgMemberFunctionDeclaration(pos)) {
      SgScopeStatement* scope = isSgStatement(pos)->get_scope();
      ROSE_ASSERT(isSgClassDefinition(scope));
      defdecl = SB::buildDefiningMemberFunctionDeclaration(transf_name, retType, new_param_list, scope);
      scope->insertStatementInScope(defdecl, false);

    }
    else {
      SgScopeStatement* scope = isSgStatement(pos)->get_scope();
      // For C++, we should consider namespace definition as another
      // possible place for insertion of the struct.
      ROSE_ASSERT(isSgGlobal(scope) || isSgNamespaceDefinitionStatement(scope));
      defdecl = SB::buildDefiningFunctionDeclaration(transf_name, retType, new_param_list, scope);
      SI::insertStatementBefore(isSgStatement(pos), defdecl);
    }

#else
    SgFunctionDeclaration* defdecl = SB::buildDefiningFunctionDeclaration(transf_name,
        retType,
        new_param_list,
        isSgStatement(pos)->get_scope());

    // Instead of simply placing the defdecl at the top of the file... we'll
    // place it as close to the use as possible. This should ensure that all the
    // necessary data structures used, are declared.
    //  prependStatement(isSgStatement(defdecl));
    //  SgStatement* loc = findInsertLocationFromParams(param_list);
    //  SI::insertStatementBefore(loc, defdecl);
    //  SI::insertStatementBefore(scope, defdecl);

#if 0
    insertStmtAfterLast(defdecl, globalScope);
#else

#ifdef USE_INSERT_AFTER
    SI::insertStatementAfter(isSgStatement(pos), defdecl);
#else
    SI::insertStatementBefore(isSgStatement(pos), defdecl);
#endif

#endif
#endif
#endif

#ifdef MAKE_DECLS_STATIC
    Util::makeDeclStatic(isSgDeclarationStatement(defdecl));
#endif

#if 0
    popScopeStack();
#endif

    return defdecl;
  }

  static
  SgFunctionRefExp*
  getOrCreateWrapperFunction( SgProject* project,
                              SgFunctionRefExp* fn_ref,
                              SgExpression* arg,
                              SgFunctionCallExp* fncall
                            )
  {
    SgFunctionSymbol* fn_sym = fn_ref->get_symbol();
    SgName            new_fn_name(fn_sym->get_name().getString() + "_wrap");
    SgType*           retType = SgPointerType::createType(SgTypeVoid::createType());

    // First check, if we have already created the wrapper before...
    // Check if the wrapper function exists. If it does exist, it'll exist
    // in the same scope (hence the same symbol table) as the orig func
    SgScopeStatement* scope = fn_sym->get_declaration()->get_scope();
    if (SgFunctionSymbol* new_fn_sym = scope->lookup_function_symbol(new_fn_name))
    {
      // Wrapper function already created...
     return SB::buildFunctionRefExp(new_fn_sym);
    }

    // Wrapper functions needs to be created
    SgExprListExp* param_list = SB::buildExprListExp(arg);
    SgFunctionDeclaration* wrapper_fn = createDefiningDecl(new_fn_name, retType, param_list, scope,
        project,
        GEFD(SI::getEnclosingStatement(fncall)), false, fn_sym->get_name());

    // Get the function definition...
    SgFunctionDefinition* fndef = Util::getFuncDef(wrapper_fn);
    SgBasicBlock* fnbody = Util::getFuncBody(fndef);

    SgType* var_type = strDecl::getStructType(retType, wrapper_fn, true);

    // the args
    SgInitializedNamePtrList& args = wrapper_fn->get_args();
    // Get the first arg
    SgInitializedName* arg1 = *(args.begin());
    // first arg is pointer type -- pointer to struct
    ROSE_ASSERT(isSgPointerType(arg1->get_type()));

    // the arg is pointer to void struct type... but its been cast to void pointer type... so, cast
    // it to pointer to void struct type
    // This is the void* argument casted to pointer to void struct type...
    SgExpression* arg_casted = SB::buildCastExp(SB::buildVarRefExp(arg1), SgPointerType::createType(var_type), CAST_TYPE);

    // Dereference the void* to reveal a void struct
    //SgExpression* deref_arg = SB::buildPointerDerefExp(SB::buildVarRefExp(arg1));
    SgExpression* deref_arg = SB::buildPointerDerefExp(arg_casted);

    // now create a function call using the original fn ref...
//      SgFunctionCallExp* transf_fn_call = SB::buildFunctionCallExp(SI::copyExpression(fn_ref), SB::buildExprListExp(deref_arg));
    SgFunctionCallExp* transf_fn_call = SB::buildFunctionCallExp(SI::copyExpression(fncall->get_function()), SB::buildExprListExp(deref_arg));

    // create local variable of return type... which is initialized by the function call above
    SgVariableDeclaration* output_var = Util::createLocalVariable("output", var_type, transf_fn_call, fnbody);
    fnbody->append_statement(output_var);

    // return the output_var.ptr
    Util::appendReturnStmt(Util::createDotExpFor(Util::getNameForDecl(output_var), "ptr"), fnbody);

    // get the new_fn_ref from the wrapper_fn
    return SB::buildFunctionRefExp(wrapper_fn);
  }

  SgExpression* insertPushCall(SgExpression* ins, SgExpression* top) {

    if(!ins) {
      // Nothing to do...
      return NULL;
    }
    if(top) {
      return SB::buildCommaOpExp(top, ins);
    }
    else {
      return ins;
    }
  }



  void instr(SgProject* project, SgFunctionCallExp* fncall)
  {
#ifdef FUNCCALL_DEBUG
    std::cerr << "handleFuncCalls >> "
              << fncall->unparseToString()
              << std::endl;
#endif

    if(fncall->getAssociatedFunctionDeclaration() == NULL)
    {
#ifdef FUNCCALL_DEBUG
      std::cerr << ("Can't find associated fn decl for... ");
      Util::printNode(fncall);
      std::cerr << ("Must be virtual function call. Returning... \n");
#endif
      return;
    }

#ifdef FUNCCALL_DEBUG
    std::cerr << "Function: " << fncall->getAssociatedFunctionDeclaration()->get_name()
              << std::endl;;
    Util::printNode(fncall);
#endif

    SgType* fncall_type = fncall->get_type();

#ifdef STRIP_TYPEDEFS
    fncall_type = Util::getType(fncall_type);
#endif

    // Replace malloc calls with malloc_overload
    if (strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "malloc") == 0)
    {
#ifdef FUNCCALL_DEBUG
      std::cerr << ("Handling malloc\n");
#endif
      SgStatement* stmt = Util::getSurroundingStatement(*isSgExpression(fncall));
      SgScopeStatement* scope = stmt->get_scope();

      SgExprListExp* args = fncall->get_args();
      SgExpressionPtrList& exprs = args->get_expressions();
      ROSE_ASSERT(exprs.size() == 1);
      SgExpression* arg1 = *(exprs.begin());

      SgExpression* arg1_copy = SI::copyExpression(arg1);
      // Do a c_Style_cast to unsigned int
      SgExpression* arg1_copy_casted = SB::buildCastExp(arg1_copy, Util::getSizeOfType(), SgCastExp::e_C_style_cast);
      SgExprListExp* params = SB::buildExprListExp(arg1_copy_casted);

      SgType* retType = strDecl::getStructType(fncall_type, GEFD(SI::getEnclosingStatement(fncall)), true);

      SgExpression* overload = buildMultArgOverloadFn("malloc_overload", params, retType, scope,
          GEFD(SI::getEnclosingStatement(fncall)));

      replaceWrapper(fncall, overload);
    }
    else if (strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "realloc") == 0)
    {
#ifdef FUNCCALL_DEBUG
      std::cerr << ("Handling realloc\n");
#endif
      SgStatement* stmt = Util::getSurroundingStatement(*isSgExpression(fncall));
      SgScopeStatement* scope = stmt->get_scope();

      SgExprListExp* args = fncall->get_args();
      SgExpressionPtrList& exprs = args->get_expressions();
      ROSE_ASSERT(exprs.size() == 2);
      SgExpressionPtrList::iterator iter = exprs.begin();
      SgExpression* arg1 = *iter;
      SgType* arg1_type = arg1->get_type();
      arg1_type = Util::getType(arg1_type);
#ifdef CPP_SCOPE_MGMT
      ROSE_ASSERT(strDecl::isValidStructType(arg1_type, GEFD(fncall)));
#else
      ROSE_ASSERT(strDecl::isValidStructType(arg1_type));
#endif

      SgExpression* arg1_copy = SI::copyExpression(arg1);

#ifdef STRIP_TYPEDEFS
      // No need to do any typedef checks here since arg1 is ValidStructType
      // and arg2 is casted to SizeOfType
#endif

      ++iter;
      SgExpression* arg2 = *iter;

      SgExpression* arg2_copy = SI::copyExpression(arg2);
      // Do a c_Style_cast to unsigned int
      SgExpression* arg2_copy_casted = SB::buildCastExp(arg2_copy, Util::getSizeOfType(), SgCastExp::e_C_style_cast);
      SgExprListExp* params = SB::buildExprListExp(arg1_copy, arg2_copy_casted);

      SgType* retType = strDecl::getStructType(fncall_type, GEFD(SI::getEnclosingStatement(fncall)), true);

      SgExpression* overload = buildMultArgOverloadFn("realloc_overload", params, retType, scope,
          GEFD(SI::getEnclosingStatement(fncall)));

      replaceWrapper(fncall, overload);
    }
    else if (Util::isFree(fncall))
    {
#ifdef FUNCCALL_DEBUG
      std::cerr << ("Handling free\n");
#endif
      SgStatement* stmt = Util::getSurroundingStatement(*isSgExpression(fncall));
      SgScopeStatement* scope = stmt->get_scope();

      SgExprListExp* args = fncall->get_args();
      SgExpressionPtrList& exprs = args->get_expressions();
      ROSE_ASSERT(exprs.size() == 1);
      SgExpression* arg1 = *(exprs.begin());
      SgExpression* arg1_copy = SI::copyExpression(arg1);

      // assert that the input type is valid struct type
      SgType* arg1_type = arg1->get_type();
#ifdef STRIP_TYPEDEFS
      arg1_type = Util::getType(arg1_type);
#endif

#ifdef CPP_SCOPE_MGMT
      ROSE_ASSERT(strDecl::isValidStructType(arg1_type, GEFD(fncall)));
#else
      ROSE_ASSERT(strDecl::isValidStructType(arg1_type));
#endif

      SgExprListExp* params = SB::buildExprListExp(arg1_copy);

      ROSE_ASSERT(fncall->get_type()->get_mangled() == SgTypeVoid::createType()->get_mangled());
      SgType* retType = SgTypeVoid::createType();
      SgExpression* overload = buildMultArgOverloadFn("free_overload", params, retType, scope,
          GEFD(SI::getEnclosingStatement(fncall)));

      replaceWrapper(fncall, overload);

    }
#ifdef WRAP_PTHREAD_ROUTINE_CALLS
    else if (strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "pthread_create") == 0)
    {
#ifdef FUNCCALL_DEBUG
      std::cerr << ("Handling pthread_create by wrapping routine calls\n");
#endif /* FUNCCALL_DEBUG */
      // Get the args
      SgExprListExp* args = fncall->get_args();
      SgExpressionPtrList& exprs = args->get_expressions();
      ROSE_ASSERT(exprs.size() == 4);
      SgExpressionPtrList::iterator i = exprs.begin();
      SgExpression* arg1 = *i;
      SgExpression* arg2 = *++i;
      SgExpression* arg3 = *++i;
      SgExpression* arg4 = *++i;

      // Check that the third arg is a function ref
      ROSE_ASSERT(isSgFunctionRefExp(arg3));

      // Check that the fourth arg is a qual type if its a var ref (not converted to str yet)
      // otherwise, its a valid struct type
#ifdef CPP_SCOPE_MGMT
      ROSE_ASSERT(Util::isQualifyingType(arg4->get_type()) || strDecl::isValidStructType(arg4->get_type(), GEFD(arg4)));
#else /* CPP_SCOPE_MGMT */
      ROSE_ASSERT(Util::isQualifyingType(arg4->get_type()) || strDecl::isValidStructType(arg4->get_type()));
#endif /* CPP_SCOPE_MGMT */

      // Now, we need to create a def func decl prior for the wrapper func... this wrapper
      // will be called from pthread_create, and the wrapper calls the original function in turn
      // this ensures that the penalty of conforming to pthread_create arg specs is only paid once
      // when the pthread_create call is made, and not always
      // In addition, if multiple functions call the same routine, we can use the same wrapper function
      // We can use our overload function system to include this function


      // Now, comes the tricky part... converting the argument to the pthread_routine to a void pointer
      // while passing metadata through...

      // In the caller... (func that is calling pthread_create)
      // The strategy here is to push the arg before the function call, and assign it a local variable.
      // This local variable will be of valid struct type
      // We pass the address of the local variable, casted to void pointer

      // In the wrapper... (func called by pthread_create)
      // the void pointer is derefed to yield a void struct... We don't know what the
      // underlying type is, while in the wrapper. So, we'll assume its a void struct...
      // This works because when the transf function is called, it is written in such a way that it can
      // cast void struct to the appropriate struct type... and proceed.

      // In the trans func... (func originally meant to be called by pthread_create... this has been
      // transformed to receive void struct and return void struct.
      // This function won't notice any difference, and hence can be transformed as usual.
      SgVariableDeclaration* pthread_arg_var;
      SgExpression* init;
      SgType* var_ty;

      if (Util::isQualifyingType(arg4->get_type()))
      {
        // Not considering double dim arrays here yet...
        ROSE_ASSERT(Util::isVarRefOrDotArrow(arg4));

        SgType* arg4_type = arg4->get_type();

#ifdef STRIP_TYPEDEFS
        arg4_type = Util::getType(arg4_type);
#endif /* STRIP_TYPEDEFS */

        // We might also need to do a create_entry if the arg is
        // of array type
#ifdef ARRAY_CREATE_ENTRY
        // Also, we need to insert create_entry for the arg, if its
        // array type.
        if(isSgArrayType(arg4_type)) {
          insertCreateEntry(SI::copyExpression(arg4), Util::getScopeForExp(fncall),
              SI::getEnclosingStatement(fncall));
        }
#endif /* ARRAY_CREATE_ENTRY */

        SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(arg4)));
        SgExpression* ptr = SI::copyExpression(arg4);

#ifdef STRIP_TYPEDEFS
        ptr = Util::castAwayTypedef(ptr);
        // ptr_addr is already casted
#endif /* STRIP_TYPEDEFS */

        var_ty = strDecl::getStructType(Util::getTypeForPntrArr(arg4_type),
            GEFD(SI::getEnclosingStatement(fncall)),
            true);

        SgExprListExp* cs_p = SB::buildExprListExp(ptr, ptr_addr);


        init = buildMultArgOverloadFn("create_struct", cs_p,
            var_ty, Util::getScopeForExp(fncall),
            GEFD(SI::getEnclosingStatement(fncall)));

      }
      else
      {
#ifdef CPP_SCOPE_MGMT
        ROSE_ASSERT(strDecl::isValidStructType(arg4->get_type(), GEFD(arg4)));
#else /* CPP_SCOPE_MGMT */
        ROSE_ASSERT(strDecl::isValidStructType(arg4->get_type()));
#endif /* CPP_SCOPE_MGMT */

        // Expressions using pointers are converted to struct temporaries by our scheme.
        // Since these are temporaries, we cannot take their address.
        // TODO: We might be able
        // to place them before the fncall exp and store the result in a local variable
        // and pass the address of the local variable through the function call.. haven't
        // done that yet.
        init = SI::copyExpression(arg4);
        var_ty = arg4->get_type();
      }

      // name, type, init, scope
      SgName s_name(var_ty->get_mangled() + "_pthread_arg_" + boost::lexical_cast<std::string>(Util::VarCounter++));
      pthread_arg_var = Util::createLocalVariable(s_name, var_ty, SB::buildAssignInitializer(init, init->get_type()), SI::getScope(fncall));
      SI::insertStatementBefore(Util::getSuitablePrevStmt(SI::getEnclosingStatement(fncall)), pthread_arg_var);

      // Now that we have the local variable, take the address of that variable, and cast it to void*
      SgExpression* addr_of_arg = Util::createAddressOfOpFor(SB::buildVarRefExp(pthread_arg_var));
      SgExpression* void_arg = SB::buildCastExp(addr_of_arg, SgPointerType::createType(SgTypeVoid::createType()), CAST_TYPE);

      // We have the arg in the right format now. What we need is the wrap function which can take arg now.
      // the second arg here is reqd to use createDefiningDecl to create the function def... not for actual use in the
      // wrapper function.. since it will be called by pthread_create
      // fncall is required to determine where to insert the wrapper function
      SgFunctionRefExp* arg3ref = isSgFunctionRefExp(arg3);
      SgExpression*     voidArgCopy = SI::copyExpression(void_arg);
      SgFunctionRefExp* new_fn_ref = getOrCreateWrapperFunction(project, arg3ref, voidArgCopy, fncall);

      // Now, change the old pthread_create call with a new one which has new_fn_ref and void_arg as the arguments.
      // The first arg is a struct... call return_pointer to get the original pointer back... the pointer is basically
      // &pthread_t... which has been converted to a struct
      SgType* arg1_type = arg1->get_type();
#ifdef STRIP_TYPEDEFS
      arg1_type = Util::getType(arg1_type);
#endif /* STRIP_TYPEDEFS */

      SgExpression* arg1_ptr = buildMultArgOverloadFn("return_pointer", SB::buildExprListExp(SI::copyExpression(arg1)),
          strDecl::findInUnderlyingType(arg1_type),
          Util::getScopeForExp(fncall),
          GEFD(SI::getEnclosingStatement(fncall)));

      //SgExprListExp* p_list = SB::buildExprListExp(SI::copyExpression(arg1), SI::copyExpression(arg2), new_fn_ref, void_arg);
      SgExprListExp* p_list = SB::buildExprListExp(arg1_ptr, SI::copyExpression(arg2), new_fn_ref, void_arg);
      SgExpression* new_fncall = SB::buildFunctionCallExp("pthread_create", fncall->get_type(), p_list, SI::getScope(fncall));

      replaceWrapper(fncall, new_fncall);
    }
#endif /* WRAP_PTHREAD_ROUTINE_CALLS */

    // This should work in general for all "qualifying lib calls"
    //RMM TEST
#ifdef RMM_MISSING_FUNC_DEFINITION_INSTRUMENTATION
    else if (Trav::isQualifyingLibCall(fncall) || fncall->attributeExists(std::string("RMM_FFC")) )
#else
    else if (Trav::isQualifyingLibCall(fncall)) )
#endif /* RMM_MISSING_FUNC_DEFINITION_INSTRUMENTATION */
    {
#ifdef FUNCCALL_DEBUG
      std::cerr << ("Handling qual lib call\n");
#endif /* FUNCCALL_DEBUG */

      // Basically, deref all the struct arguments
      SgExprListExp* args = fncall->get_args();
      SgExpressionPtrList& exprs = args->get_expressions();

      SgExpressionPtrList::iterator iter = exprs.begin();
      SgExprListExp* new_args = SB::buildExprListExp();

      for(; iter != exprs.end(); ++iter)
      {
        SgExpression* ce = *iter;
        SgExpression* ce_copy = SI::copyExpression(ce);

        SgType* ce_type = ce->get_type();

#ifdef STRIP_TYPEDEFS
        ce_type = Util::getType(ce_type);
#endif /* STRIP_TYPEDEFS */

#ifdef CPP_SCOPE_MGMT
        if(strDecl::isValidStructType(ce_type, GEFD(fncall)))
#else /* CPP_SCOPE_MGMT */
        if(strDecl::isValidStructType(ce_type))
#endif /* CPP_SCOPE_MGMT */
        {
          // Shouldn't do deref here since it will involve a check... and thats not
          // necessary since we are not derefencing anything here. Instead,
          // we should call a function which returns the pointer from the struct
          SgExprListExp* p_list = SB::buildExprListExp(SI::copyExpression(ce));
          SgType* retType = strDecl::findInUnderlyingType(ce_type);

          SgExpression* new_exp = buildMultArgOverloadFn("return_pointer", p_list, retType,
              Util::getScopeForExp(fncall),
              GEFD(SI::getEnclosingStatement(fncall)));

          new_args->append_expression(new_exp);
        }
        else
        {
          new_args->append_expression(ce_copy);
        }
      }

      SgName fn_name(fncall->getAssociatedFunctionDeclaration()->get_name());
      // if this get_type doesn't work, use fn->getAssociatedFunctionDeclaration()->get_orig_return_type()
      SgType* retType = fncall->get_type();

      // This can change the return type of
      // a library function call...
#ifdef STRIP_TYPEDEFS
      retType = Util::getType(retType);
#endif /* STRIP_TYPEDEFS */

#ifdef FUNCCALL_DEBUG
      std::cerr << ("fncall\n");
      Util::printNode(fncall);
      std::cerr << ("retType\n");
      Util::printNode(retType);
#endif

      // if return type is of struct type, return the underlying type... this is a library call
      // we don't change the return types for these things
#ifdef CPP_SCOPE_MGMT
      if(strDecl::isValidStructType(retType, GEFD(fncall)))
#else
        if(strDecl::isValidStructType(retType))
#endif
        {
          // We will never return a struct type --- correct?
          ROSE_ASSERT(0);
        }


//      SgExpression* new_fncall = SB::buildFunctionCallExp(fn_name, retType, new_args, SI::getScope(fncall));
      SgExpression* new_fncall = SB::buildFunctionCallExp(SI::copyExpression(fncall->get_function()), new_args);

      replaceWrapper(fncall, new_fncall);
#ifdef FUNCCALL_DEBUG
      std::cerr << ("new_fncall\n");
      Util::printNode(new_fncall);
#endif

      if(Util::isQualifyingType(retType)) {
        // Need to create a struct out of it.
        // First create a local variable... which will take the return value of the function
        // then, create_entry from that local variable.
        // retType libcall_ret;
        // libcall_ret = qual_lib_call(), create_entry(&ret, ret, sizeof(ret)), create_struct_from_addr(&ret)
        #ifdef FUNCCALL_DEBUG
        std::cerr << ("further processing since return type is qualifying type\n");
        #endif

#ifdef STRIP_TYPEDEFS
        ROSE_ASSERT(!isSgTypedefType(retType));
#endif

        // name, type, init, scope
        SgName s_name(retType->get_mangled() + "_libcall_ret_" + boost::lexical_cast<std::string>(Util::VarCounter++));
        SgVariableDeclaration* var_decl = Util::createLocalVariable(s_name, retType, NULL, SI::getScope(new_fncall));
        SI::insertStatementBefore(Util::getSuitablePrevStmt(SI::getEnclosingStatement(new_fncall)), var_decl);

        // libcall_ret = qual_lib_call()
        SgExpression* assign_op = SB::buildAssignOp(SB::buildVarRefExp(var_decl), SI::copyExpression(new_fncall));

        // create_entry
        SgExpression* ce1 = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(var_decl)));
        SgExpression* ce2 = SB::buildCastExp(SB::buildVarRefExp(var_decl), Util::getAddrType(), SgCastExp::e_C_style_cast);
        SgExpression* ce3 = SB::buildCastExp(SB::buildSizeOfOp(SB::buildVarRefExp(var_decl)), Util::getSizeOfType(), SgCastExp::e_C_style_cast);

        SgExprListExp* params = SB::buildExprListExp(ce1, ce2, ce3);
        SgExpression* ce_fncall = buildMultArgOverloadFn("create_entry", params, SgTypeVoid::createType(), SI::getScope(new_fncall),
            GEFD(SI::getEnclosingStatement(new_fncall)));

        // create_struct_from_addr(&ret)
        SgExpression* cs_from_addr = createStructUsingAddressOf(SB::buildVarRefExp(var_decl), GEFD(var_decl));

        SgExpression* init_ce = SB::buildCommaOpExp(assign_op, ce_fncall);
        SgExpression* init_ce_cs = SB::buildCommaOpExp(init_ce, cs_from_addr);

        replaceWrapper(new_fncall, init_ce_cs);
      }

    }
    else {

      // Four things to do here.
      // 1. If one or more args is of qualifying type, convert to appropriate struct
      // 2. If return type is qualifying type, convert it to relevant struct type
      // 3. CLASS_STRUCT_COPY: If one or more arguments is an original class/struct,
      // convert it to its ref version
      // 4. CLASS_STRUCT_COPY: If return type is original class/struct, add additional
      // arg to the param_list (&retvar) and change return type to void.
#ifdef FUNCCALL_DEBUG
      std::cerr << ("Normal function call: Check the args and return type\n");
      std::cerr << fncall->unparseToString() << std::endl;
#endif

      SgExprListExp* param_list = fncall->get_args();
      SgExprListExp* new_param_list = SB::buildExprListExp();

      SgExpressionPtrList& exprs = param_list->get_expressions();
      SgExpressionPtrList::iterator it = exprs.begin();

#ifdef RETAIN_FUNC_PROTOS
      SgExpression* pcalls = NULL;
      unsigned int arg_num = 0;
#endif

      for(; it != exprs.end(); ++it)
      {
        SgExpression* ce = *it;
        SgType* ce_type = ce->get_type();

#ifdef STRIP_TYPEDEFS
        ce_type = Util::getType(ce_type);
#endif


#ifdef FUNCCALL_DEBUG
        std::cerr << ("params: ce: \n");
        Util::printNode(ce);
        std::cerr << ("param_type: ce_type:\n");
        Util::printNode(ce_type);
#endif

#ifdef CLASS_STRUCT_COPY
        // 3. CLASS_STRUCT_COPY: If one or more arguments is an original class/struct,
        // convert it to its ref version
        if(strDecl::isOriginalClassType(ce_type)) {
#ifdef FUNCCALL_DEBUG
          std::cerr << ("param is orig class/struct type\n");
#endif
          // This needs to be converted to its ref version.
          SgExpression* ce_addr = Util::createAddressOfOpFor(SI::copyExpression(ce));
          new_param_list->append_expression(ce_addr);
          continue;
        }
#endif

        // 1. If one or more args is of qualifying type, convert to appropriate struct
        if (Util::isQualifyingType(ce_type))
        {
#ifdef FUNCCALL_DEBUG
          std::cerr << ("FuncCalls-qual_type: ce\n");
          Util::printNode(ce);
#endif


          // We might also need to do a create_entry if the arg is
          // of array type
#ifdef ARRAY_CREATE_ENTRY
          // Also, we need to insert create_entry for the arg, if its
          // array type.
          if(isSgArrayType(ce_type)) {
            insertCreateEntry(SI::copyExpression(ce), Util::getScopeForExp(fncall),
                SI::getEnclosingStatement(fncall));
          }
#endif


          // If its var ref or dot arrow, then create_struct(ptr, &ptr) --
          // if RETAIN_FUNC_PROTOS is defined, then insert push(&ptr), fn(ptr)
          if(Util::isVarRefOrDotArrow(ce)) {
            SgExpression* ptr_addr = Util::castToAddr(Util::createAddressOfOpFor(SI::copyExpression(ce))); //original
            SgExpression* ptr = SI::copyExpression(ce);

#ifdef STRIP_TYPEDEFS
            ptr = Util::castAwayTypedef(ptr);

            // ptr_addr is already casted
#endif

#ifdef RETAIN_FUNC_PROTOS

            SgExprListExp* cs_p = SB::buildExprListExp(ptr_addr);

            SgExpression* pushcall = buildMultArgOverloadFn("push_to_stack", cs_p,
                SgTypeVoid::createType(), Util::getScopeForExp(fncall),
                GEFD(SI::getEnclosingStatement(fncall)));
            pcalls = insertPushCall(pushcall, pcalls);

            new_param_list->append_expression(ptr);

            arg_num++;
#else
            SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(ce_type),
                GEFD(SI::getEnclosingStatement(fncall)),
                true);

            SgExprListExp* cs_p = SB::buildExprListExp(ptr, ptr_addr);


            SgExpression* overload = buildMultArgOverloadFn("create_struct", cs_p,
                retType, Util::getScopeForExp(fncall),
                GEFD(SI::getEnclosingStatement(fncall)));
            new_param_list->append_expression(overload);
#endif
          }

#ifdef DOUBLE_DIM_ENABLED
          else if(isSgPointerDerefExp(ce))
          {
            // Something like *ptr / *str
            // 1. Strip the deref.
            // 2. Call deref_create_struct(ptr), deref_create_struct(str)
#ifdef RETAIN_FUNC_PROTOS
            std::cerr << ("Not retaining function prototypes for double dim pointer derefs... quitting\n");
            exit(1);
#endif

            SgExpression* ptr = SI::copyExpression(Util::stripDeref(ce));

#ifdef STRIP_TYPEDEFS
            ptr = Util::castAwayTypedef(ptr);
#endif

            SgExpression* overload = buildMultArgOverloadFn("deref_create_struct", SB::buildExprListExp(ptr),
                strDecl::getStructType(Util::getTypeForPntrArr(ce_type),
                  GEFD(SI::getEnclosingStatement(fncall)), true),
                Util::getScopeForExp(fncall),
                GEFD(SI::getEnclosingStatement(fncall)));
            new_param_list->append_expression(overload);

          }
          else
          {
            std::cerr << ("Unsupported case in handleFuncCalls - normal function calls\n");
            std::cerr << ("ce:\n");
            Util::printNode(ce);
            ROSE_ASSERT(0);
          }
#endif
          continue;
        }
#ifdef RETAIN_FUNC_PROTOS
#ifdef CPP_SCOPE_MGMT
        else if(strDecl::isValidStructType(ce_type, GEFD(ce)))
#else
        else if(strDecl::isValidStructType(ce_type))
#endif
        {
#ifdef FUNCCALL_DEBUG
          std::cerr << ("valid struct type found...\n");
#endif
          // the transf will be from: fn(str)
          // to: str argvar; argvar = str, push(argvar.addr), fn(argvar.ptr)

          // Create a variable declaration
          // 1. create a local varible... argvar
          SgName var_name("argvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
          SgVariableDeclaration* argvar = SB::buildVariableDeclaration(var_name, ce->get_type(), NULL, SI::getScope(fncall));



          SI::insertStatementBefore(SI::getEnclosingStatement(fncall), argvar);

          // 2. argvar = str;
          SgExpression* argvar_assign = SB::buildAssignOp(SB::buildVarRefExp(argvar), SI::copyExpression(ce));

          // 3. push(argvar.addr)
          SgExprListExp* cs_p = SB::buildExprListExp(Util::createDotExpFor(Util::getNameForDecl(argvar), "addr"));
          SgExpression* pushcall = buildMultArgOverloadFn("push_to_stack", cs_p,
              SgTypeVoid::createType(), Util::getScopeForExp(fncall),
              GEFD(SI::getEnclosingStatement(fncall)));

          // 4. create comma op exp with the argvar_assign and pushcall
          pushcall = SB::buildCommaOpExp(argvar_assign, pushcall);

          // 5. add it to the list of exps to be inserted before the fncall
          pcalls = insertPushCall(pushcall, pcalls);

          // 6. add argvar.ptr to the new param list
          new_param_list->append_expression(Util::createDotExpFor(Util::getNameForDecl(argvar), "ptr"));

          arg_num++;

          continue;
        }
#endif

#ifdef FUNCCALL_DEBUG
        std::cerr << ("Nothing to do with ce\n");
#endif

        // If nothing is done to the struct, simply add it to the new_param_list
        new_param_list->append_expression(SI::copyExpression(ce));
      }

      SgType* fn_retType = fncall->get_type();

#ifdef STRIP_TYPEDEFS
      fn_retType = Util::getType(fn_retType);
#endif

#ifdef FUNCCALL_DEBUG
      std::cerr << ("fncall -- mid way through...\n");
      Util::printNode(fncall);
#endif

      // 2. If return type is qualifying type, convert it to relevant struct type
      if(Util::isQualifyingType(fn_retType)) {
        // convert return type to corresponding struct type
        SgType * new_retType = strDecl::getStructType(Util::getTypeForPntrArr(fn_retType),
            GEFD(SI::getEnclosingStatement(fncall)), true);

#ifdef RETAIN_FUNC_PROTOS
        // pop_from_stack returns the top most, while popping a total of arg_num + 1 out...
        // This will return the metadata we are looking for, while removing the args from the stack...
        // transform from: fn(...) to int t; ret* ptr; (pcalls, ptr = new_fn(...), t = pop_from_stack(arg_num), create_struct(ptr, t))
        // new_fn(...)
#if 1
        SgExpression* new_fncall = SB::buildFunctionCallExp(SI::copyExpression(fncall->get_function()),
            new_param_list);

#else
        SgExpression* new_fncall = SB::buildFunctionCallExp(Util::getFunctionName(fncall),
            fn_retType,
            new_param_list,
            SI::getScope(fncall));
#endif

#ifdef FUNCCALL_DEBUG
        std::cerr << ("new_fncall\n");
        Util::printNode(new_fncall);
#endif

        // ret* ptr
        SgName var_name("retvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
        SgVariableDeclaration* retvar = SB::buildVariableDeclaration(var_name, fn_retType, NULL, SI::getScope(fncall));
        SI::insertStatementBefore(SI::getEnclosingStatement(fncall), retvar);

        // ptr = new_fn(...)
        SgExpression* fn_ret = SB::buildAssignOp(SB::buildVarRefExp(retvar), new_fncall);



        // pop()
        SgExprListExp* cs_p = SB::buildExprListExp(SB::buildIntVal(arg_num + 1));
        SgExpression* popcall = buildMultArgOverloadFn("pop_from_stack", cs_p,
            Util::getAddrType(), Util::getScopeForExp(fncall),
            GEFD(SI::getEnclosingStatement(fncall)));

        // int t
        SgName popvar_name("popvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
        SgVariableDeclaration* popvar = SB::buildVariableDeclaration(popvar_name, Util::getAddrType(), NULL, SI::getScope(fncall));
        SI::insertStatementBefore(SI::getEnclosingStatement(fncall), popvar);

        // t = pop()
        SgExpression* pop_assign = SB::buildAssignOp(SB::buildVarRefExp(popvar), popcall);

        // (pcalls, ptr = new_fn())
        if(pcalls) {
          fn_ret = SB::buildCommaOpExp(pcalls, fn_ret);
        }

        // (pcalls, ptr = new_fn(...), t = pop_from_stack(arg_num))
        fn_ret = SB::buildCommaOpExp(fn_ret, pop_assign);

        // Instead of using the aggregate initializer, we can call create_struct...
        //new_fncall = SB::buildAggregateInitializer(SB::buildExprListExp(new_fncall, popcall), new_retType);
#if 0
        new_fncall = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(new_fncall, popcall),
            new_retType, Util::getScopeForExp(fncall),
            GEFD(SI::getEnclosingStatement(fncall)));
#endif
        // create_struct(ptr, t)
        SgExpression* cs = buildMultArgOverloadFn("create_struct", SB::buildExprListExp(SB::buildVarRefExp(retvar), SB::buildVarRefExp(popvar)),
            new_retType, Util::getScopeForExp(fncall),
            GEFD(SI::getEnclosingStatement(fncall)));

        // t here contains the address of the stack variable returned, but the
        // op above this one should handle any copying necessary, properly
        // (pcalls, ptr = new_fn(...), t = pop_from_stack(arg_num), create_struct(ptr, t)
        new_fncall = SB::buildCommaOpExp(fn_ret, cs);

        // replace fncall with new_fncall
        replaceWrapper(fncall, new_fncall);

#else

        SgExpression* new_fncall = SB::buildFunctionCallExp(Util::getFunctionName(fncall),
            new_retType,
            new_param_list,
            SI::getScope(fncall));

        replaceWrapper(fncall, new_fncall);
#endif
      }
#ifdef CLASS_STRUCT_COPY
      // 4. CLASS_STRUCT_COPY: If return type is original class/struct, add additional
      // arg to the param_list (&retvar) and change return type to void.
      else if(strDecl::isOriginalClassType(fn_retType)) {
        // Here we are returning a variable/object of struct/class type... other than the ones we created... so,
        // we should create a local variable to hold the ret value, and pass it by reference.
        // original code: struct A var = fn(...)
        // transformed code: struct A retvar; struct A var = (fn(..., &retvar), retvar)

        // 1. create a local varible... of the same type as the return type.
        SgName var_name("retvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
        SgVariableDeclaration* retvar = SB::buildVariableDeclaration(var_name, fn_retType, NULL, SI::getScope(fncall));
        SI::insertStatementBefore(SI::getEnclosingStatement(fncall), retvar);

        // 2. add &retvar to the end of a parameters list
        new_param_list->append_expression(Util::createAddressOfOpFor(SB::buildVarRefExp(retvar)));


        SgScopeStatement* scope = Util::getScopeForExp(fncall);
        SgName fn_name(Util::getFunctionName(fncall));

        // 3. this part requires the final fncall expr... fncall(..., &retvar), retvar
        // 4. Also convert return type to void

        SgExpression* new_fncall = SB::buildFunctionCallExp(SI::copyExpression(fncall->get_function()),
            SgTypeVoid::createType(),
            new_param_list,
            SI::getScope(fncall));
        #if 0
        SgExpression* new_fncall = SB::buildFunctionCallExp(Util::getFunctionName(fncall),
            SgTypeVoid::createType(),
            new_param_list,
            SI::getScope(fncall));
        #endif

        SgExpression* comma_op = SB::buildCommaOpExp(new_fncall, SB::buildVarRefExp(retvar));
        replaceWrapper(fncall, comma_op);
      }
#endif
      else {

#ifdef RETAIN_FUNC_PROTOS
        // transform from: fn(...) to pcalls, new_fn(...), pop_from_stack(arg_num)
#if 0
        SgExpression* new_fncall = SB::buildFunctionCallExp(Util::getFunctionName(fncall),
            fn_retType,
            new_param_list,
            SI::getScope(fncall));
#else
        SgExpression* new_fncall = SB::buildFunctionCallExp(SI::copyExpression(fncall->get_function()),
            new_param_list);
#endif

#ifdef FUNCCALL_DEBUG
        std::cerr << ("new_fncall\n");
        Util::printNode(new_fncall);
#endif

#ifdef FUNCCALL_DEBUG
        std::cerr << ("fn_sym\n");
        Util::printNode(fncall->getAssociatedFunctionSymbol());
#endif

        // (pcalls, new_fn(..))
        if(pcalls) {
          new_fncall = SB::buildCommaOpExp(pcalls, new_fncall);
        }

        // In this case, pop_from_stack will return the top most, but it will be discarded anyway...
        // pop_from_stack
        SgExpression* popcall = buildMultArgOverloadFn("pop_from_stack", SB::buildExprListExp(SB::buildIntVal(arg_num)),
            Util::getAddrType(), Util::getScopeForExp(fncall),
            GEFD(SI::getEnclosingStatement(fncall)));

        // ((pcalls, new_fn(..)), pop_from_stack(arg_num))
        new_fncall = SB::buildCommaOpExp(new_fncall, popcall);

        // replace fn(...) with (pcalls, new_fn())
        replaceWrapper(fncall, new_fncall);

#else
        // retain the original return type, and create a new func call with the
        // new param list
#if 0
        SgExpression* new_fncall = SB::buildFunctionCallExp(Util::getFunctionName(fncall),
            fn_retType,
            new_param_list,
            Util::getScopeForExp(fncall));
#endif
        // get scope above only gives the scope of the fncall expression, not of the
        // function call... which is what is required.
        SgExpression* new_fncall = SB::buildFunctionCallExp(fncall->getAssociatedFunctionSymbol(),
            new_param_list);
        replaceWrapper(fncall, new_fncall);
#endif
      }
    }
#ifdef FUNCCALL_DEBUG
    std::cerr << ("handleFuncCalls - Done\n");
#endif
  }

  void instr(SgProject* project, SgCallExpression* ce) {
    #ifdef INSTR_DEBUG
    std::cerr << (" instr call exp\n");
    #endif
    ROSE_ASSERT(isSgFunctionCallExp(ce));
    instr(project, isSgFunctionCallExp(ce));
  }
}
