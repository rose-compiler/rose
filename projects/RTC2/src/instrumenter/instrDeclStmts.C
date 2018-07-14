#include "instr.h"
//RMM
//#define ARRAY_VAR_DEBUG
namespace Instr {

  SgExpression* buildCreateEntryFnForArrays(SgExpression* array, SgScopeStatement* scope, SgStatement* pos) {

#ifdef STRIP_TYPEDEFS
    // No need to cast out typedefs.. since all the args are casted to the appropriate types
#endif

    // create_entry(&array, array, sizeof(array), dummy_lock)
    SgExpression* addr = Util::castToAddr(Util::createAddressOfOpFor(array));
    SgExpression* ptr = Util::castToAddr(SI::copyExpression(array));
    SgExpression* sizeof_array = SB::buildCastExp(SB::buildSizeOfOp(SI::copyExpression(array)),
        Util::getSizeOfType(),
        SgCastExp::e_C_style_cast);

    SgExpression* dummy_lock = Util::castToAddr(Util::getDummyLock());

    SgExprListExp* param_list = SB::buildExprListExp(addr, ptr, sizeof_array, dummy_lock);

    return buildMultArgOverloadFn("create_entry", param_list,
        SgTypeVoid::createType(),
        scope, pos);
  }


  void insertCreateEntry(SgExpression* array, SgScopeStatement* scope,
      SgStatement* pos) {

    SgExpression* overload = buildCreateEntryFnForArrays(array, scope, GEFD(pos));
    SI::insertStatementBefore(Util::getSuitablePrevStmt(pos), SB::buildExprStatement(overload));

  }



  void handlePointerVars(SgVariableDeclaration* var_decl) {

    // input: int* ptr = assign_init
    // output: int* ptr;
    // assign_and_copy(ptr....);


    // Nothing to do if pointer is not initialized
    if(Util::NoInitializer(var_decl)) {
      return;
    }

    // Only assign initializers supported
    SgInitializedName* var_name = Util::getNameForDecl(var_decl);

    SgAssignInitializer* assign_init = isSgAssignInitializer(var_name->get_initializer());
    ROSE_ASSERT(assign_init != NULL);

    // Copy the initializer
    SgExpression* init_copy = SI::copyExpression(assign_init->get_operand());

    // Set the initializer in the var decl to NULL.
    var_name->set_initializer(NULL);

    // Create an assign op.. which will be replaced by the appropriate
    // operation by the handleAssignOverloadOps function below.
    SgAssignOp* var_assign = SB::buildAssignOp(SB::buildVarRefExp(var_name), init_copy);
    SgExprStatement* assign_stmt = SB::buildExprStatement(var_assign);


    // If its a global variable, then insert the initialization in the main function
    SgScopeStatement* var_scope = var_decl->get_scope();
    if(isSgGlobal(var_scope)) {
      // Find the main function, and insert the var_assign there.
      SgFunctionDeclaration* MainFn = SageInterface::findMain(isSgGlobal(var_scope));
      if(!MainFn) {
        printf("handlePointerVars: can't find main");
        printf("var_scope\n");
        Util::printNode(var_scope);
        ROSE_ASSERT(0);
      }
      SgBasicBlock *bb = Util::getBBForFn(MainFn);
      Util::insertAtTopOfBB(bb, assign_stmt);
    }
    else {
      SI::insertStatementAfter(var_decl, assign_stmt);
    }

    // assign_and_copy(ptr...)
    handleAssignOverloadOps(var_assign);

  }

  void handleArrayVars(SgVariableDeclaration* var_decl) {

    // This is an array declaration ... which means that
    // the bounds are set by the declaration itself. To find the
    // the bounds, we will use the fields in the ArrayType

    SgInitializedName* var_name = Util::getNameForDecl(var_decl);
    // Check that we only ever get array variables here.
    SgType* var_type = var_name->get_type();

#ifdef ARRAY_VAR_DEBUG
    printf("var_name\n");
    Util::printNode(var_name);
    if(var_name->get_initializer()) {
      printf("Initializer:\n");
      Util::printNode(var_name->get_initializer());
    }
#endif

#ifdef STRIP_TYPEDEFS
    var_type = Util::getType(var_type);
#endif

    //ROSE_ASSERT(isSgArrayType(var_name->get_type()));
    ROSE_ASSERT(isSgArrayType(var_type));

    //  SgType* var_type = var_name->get_type();
#ifdef ARRAY_VAR_DEBUG
    printf("var_type\n");
    Util::printNode(var_type);
#endif


#ifdef ARRAY_VAR_DEBUG
    SgArrayType* arr_type = isSgArrayType(var_type);
    printf("arr_type\n");
    Util::printNode(arr_type);

    printf("rank\n");
    printf("%u\n", arr_type->get_rank());

    printf("base_type\n");
    Util::printNode(arr_type->get_base_type());

    if(arr_type->get_dim_info()) {
      printf("dim_info\n");
      Util::printNode(arr_type->get_dim_info());
    }

    if(arr_type->get_index()) {
      printf("index\n");
      Util::printNode(arr_type->get_index());
    }
#endif

#ifdef ARRAY_VAR_DEBUG
    SgType* base_type = arr_type->get_base_type();
    //ROSE_ASSERT(!isSgPointerType(base_type));
    printf("base_type\n");
    Util::printNode(base_type);
#endif
  }

  void handleClassStructVars(SgVariableDeclaration* var_decl) {

    // Here, we use a technique similar to assign and copy to transfer metadata from rhs to lhs. Since we
    // don't want to increase the number of uses of rhs, and we need the actual variable from the rhs, we'll
    // take both the lhs and rhs by reference into cs_assign_and_copy. Inside cs_assign_and copy, we copy
    // rhs to lhs (the actual operation) and then perform an rtc_copy.
    // input: str_type lhs_str = rhs_str
    // output: str_type lhs_str; cs_assign_and_copy(&lhs_str, &rhs_str)

    // Lets remove the initializer for the var_decl
    // Only assign initializers supported
    SgInitializedName* var_name = Util::getNameForDecl(var_decl);
    SgAssignInitializer* assign_init = isSgAssignInitializer(var_name->get_initializer());
    ROSE_ASSERT(assign_init != NULL);

    // Copy the initializer
    SgExpression* init_copy = SI::copyExpression(assign_init->get_operand());

    // Set the initializer in the var decl to NULL.
    var_name->set_initializer(NULL);

    // Create an assign op.. which will be replaced by the appropriate
    // operation by the handleAssignOverloadOps function below.
    SgAssignOp* var_assign = SB::buildAssignOp(SB::buildVarRefExp(var_name), init_copy);

    SI::insertStatementAfter(var_decl, SB::buildExprStatement(var_assign));

    // cs_assign_and_copy(ptr...)
    handleCSAssignOverloadOps(var_assign);
  }


  void instr(SgVariableDeclaration* varDecl) {
    #ifdef VAR_DECL_DEBUG
    printf("inst var decl stmt\n");
    #endif

    SgInitializedName* var_name = Util::getNameForDecl(varDecl);

    SgType* var_type = var_name->get_type();

    #ifdef STRIP_TYPEDEFS
    var_type = Util::getType(var_type);
    #endif

    if(isSgPointerType(var_type)) {
      #ifdef VAR_DECL_DEBUG
      printf("variable is a pointer\n");
      #endif
      handlePointerVars(varDecl);
    }
    #if 1
    else if(isSgArrayType(var_type)) {
      #ifdef VAR_DECL_DEBUG
      printf("variable is an array\n");
      #endif
      handleArrayVars(varDecl);
    }
    #ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(var_type)) {
      handleClassStructVars(varDecl);
    }
    #endif
    else {
      printf("What is this variable decl doing here?\n");
      Util::printNode(varDecl);
      ROSE_ASSERT(0);
    }
    #endif
  }

  void instr(SgFunctionDeclaration* fn_decl) {
    #ifdef INSTR_DEBUG
    printf("inst func decl stmt\n");
    #endif

#ifdef RETAIN_FUNC_PROTOS
    // Nothing to do, since we retain the return type
    return;
#endif

    // Changes to parameter lists alter the body of the function -- to insert
    // instructions that update metadata. This work is done in handleFuncParams
    // since doing it here would be too late --- we arrive at this function after
    // having transformed the body of the function.
    // This function only changes the return type to struct type if its of
    // qualifying type.
    // Order of traversal: param_list, basic_block, fndecl
    // we transform params and update varRemap and other structures in param_list
    // so that we can use them while we traverse the basic_block

#ifdef FUNCDECL_DEBUG
    printf("handleFuncDecls\n");
    printf("fn_decl\n");
    Util::printNode(fn_decl);
#endif

    SgName fn_name = fn_decl->get_name();

    SgScopeStatement* scope = fn_decl->get_scope();
#ifdef FUNCDECL_DEBUG
    printf("Got scope\n");
#endif

    SgFunctionParameterList* param_list = fn_decl->get_parameterList();
    ROSE_ASSERT(param_list != NULL);

#ifdef FUNCDECL_DEBUG
    printf("Got paramList\n");
#endif

    SgFunctionDefinition* fndef = fn_decl->get_definition();
    //bool defining_decl = (fndef != NULL);


    // Return type is given by strDecl::getStructType on the current poitner type
    SgFunctionType* fn_type = fn_decl->get_type();
    SgType* ret_type = fn_type->get_return_type();

#ifdef STRIP_TYPEDEFS
    ret_type = Util::getType(ret_type);
#endif

    SgType* new_ret_type;

#ifdef FUNCDECL_DEBUG
    printf("Return type is qualifying type?\n");
#endif
    if(Util::isQualifyingType(ret_type)) {
#ifdef FUNCDECL_DEBUG
      printf("Yes\n");
#endif
      // fn_decl is a scope in itself
      new_ret_type = strDecl::getStructType(Util::getTypeForPntrArr(ret_type), fn_decl, true);
    }
#ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(ret_type)) {
#ifdef FUNCDECL_DEBUG
      printf("Original Class Type\n");
#endif
      // return void type.. since we will be using the arguments to return this value...
      new_ret_type = SgTypeVoid::createType();
    }
#endif
    else {
#ifdef FUNCDECL_DEBUG
      printf("No. Retaining old ret type\n");
#endif
      new_ret_type = ret_type;
      // FIXME: Can't we just leave here... ? Nothing has changed...
    }

    // Since the parameter work is done in handleFuncParams already, just copy over the parameter list
    SgFunctionParameterList* new_args = isSgFunctionParameterList(SI::deepCopyNode(isSgNode(param_list)));

    // Keeping decorator list the same
    SgExprListExp* dec_list = fn_decl->get_decoratorList();
    SgExprListExp* new_dec_list = isSgExprListExp(SI::deepCopyNode(isSgNode(dec_list)));

    SgFunctionDeclaration* new_fn_decl;


    // Already doing this in cloneFunctions
#if 0
    //#ifdef CLONE_FUNCTIONS
    std::string new_fn_name_string = fn_name.getString() + "_mod";
    SgName new_fn_name(new_fn_name_string);
#else
    SgName new_fn_name = fn_name;
#endif

    if(fndef) {
#ifdef FUNCDECL_DEBUG
      printf("Defining fn decl\n");
#endif
      // This is a defining func decl.
      new_fn_decl = SB::buildDefiningFunctionDeclaration(new_fn_name,
          new_ret_type,
          new_args,
          scope,
          new_dec_list);

      SgFunctionDefinition* new_fndef = isSgFunctionDefinition(SI::deepCopyNode(isSgNode(fndef)));

      new_fn_decl->set_definition(new_fndef);
      isSgNode(new_fndef)->set_parent(new_fn_decl);

#ifdef FUNCDECL_DEBUG
      printf("fn_decl\n");
      Util::printNode(fn_decl);

      printf("new_fn_decl\n");
      Util::printNode(new_fn_decl);
#endif

      replaceWrapper(fn_decl, new_fn_decl);

    }
    else {
#ifdef FUNCDECL_DEBUG
      printf("Nondefining fn decl\n");
#endif
      // Nondefining fn decl
      new_fn_decl = SB::buildNondefiningFunctionDeclaration(new_fn_name,
          new_ret_type,
          new_args,
          scope,
          new_dec_list);
#ifdef FUNCDECL_DEBUG
      printf("fn_decl\n");
      Util::printNode(fn_decl);

      printf("new_fn_decl\n");
      Util::printNode(new_fn_decl);
#endif

      replaceWrapper(fn_decl, new_fn_decl);

    }


    // Checking that we did the job correctly
    if(Util::isQualifyingType(ret_type)) {
#ifdef CPP_SCOPE_MGMT
      ROSE_ASSERT(strDecl::isValidStructType(new_fn_decl->get_type()->get_return_type(), new_fn_decl));
#else
      ROSE_ASSERT(strDecl::isValidStructType(new_fn_decl->get_type()->get_return_type()));
#endif
    }
#ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(ret_type)) {
      ROSE_ASSERT(Util::compareTypes(SgTypeVoid::createType(), new_fn_decl->get_type()->get_return_type()));
    }
#endif
    else {
      ROSE_ASSERT(Util::compareTypes(ret_type, new_fn_decl->get_type()->get_return_type()));
    }

  }


  void handleSymbolTable(SgInitializedName* orig_name, SgInitializedName* str_name, bool addToMap) {

//    SgScopeStatement* orig_scope = orig_name->get_scope();

    SgSymbol* orig_symbol = orig_name->get_symbol_from_symbol_table();
    SgVariableSymbol *orig_var_symbol = isSgVariableSymbol(orig_symbol);
#ifdef HANDLE_SYMBOL_TABLE_DEBUG
    printf("Got scope. Now, symbolTable\n");
#endif

    SgSymbol* str_symbol = str_name->get_symbol_from_symbol_table();

    if(str_symbol == NULL) {

      SgVariableSymbol* str_var_symbol = new SgVariableSymbol(str_name);
      isSgNode(str_var_symbol)->set_parent(isSgNode(orig_var_symbol)->get_parent());
      SgSymbolTable* currSymbolTable = isSgSymbolTable(isSgNode(orig_var_symbol)->get_parent());
      currSymbolTable->insert(str_name->get_name(), str_var_symbol);

      // Check that this worked
      str_symbol = str_name->get_symbol_from_symbol_table();
      ROSE_ASSERT(str_symbol);
    }

    if(addToMap) {
      varRemap.insert(VariableSymbolMap_t::value_type(orig_var_symbol, isSgVariableSymbol(str_symbol)));

      reverseMap.insert(VariableSymbolMap_t::value_type(isSgVariableSymbol(str_symbol), orig_var_symbol));
    }
  }

  void handleSymbolTable(SgVariableDeclaration* orig_decl, SgVariableDeclaration* str_decl, bool addToMap) {

    SgInitializedName* orig_name = Util::getNameForDecl(orig_decl);
    SgInitializedName* str_name = Util::getNameForDecl(str_decl);

    handleSymbolTable(orig_name, str_name, addToMap);
  }

  SgInitializedName* createStructVariableFor(SgInitializedName* name) {

    std::string new_name_string;

    if(strcmp(name->get_name().getString().c_str(), "") == 0) {
      new_name_string = "";
    }
    else {
      new_name_string = name->get_name().getString() + "_str";
    }

    SgName new_name_sg(new_name_string);
#ifdef STRUCT_VAR_DEBUG
    printf("created new name: %s\n", new_name_string.c_str());
#endif

    // FIXME: Assuming no initializer
    SgInitializer* name_init = name->get_initializer();
    ROSE_ASSERT(name_init == NULL);

#if 0
    // Get the VoidStruct Type
    SgType* new_name_type = strDecl::getStructType(SgPointerType::createType(SgTypeVoid::createType()));
#endif

    // create one if necessary

    // Working one
    //  SgType* new_name_type = strDecl::getStructType(Util::getTypeForPntrArr(name->get_type()), name, true);
    // Remove the typedefs before creating a struct...

    SgType* name_type = name->get_type();

#ifdef STRIP_TYPEDEFS
    name_type = Util::getType(name_type);
#endif

    SgType* new_name_type = strDecl::getStructType(Util::getTypeForPntrArr(name_type), GEFD(name), true);

#ifdef STRUCT_VAR_DEBUG
    printf("name_type: %s\n", name_type->unparseToString().c_str());
    printf("new_name_type: %s\n", new_name_type->unparseToString().c_str());
#endif

    SgInitializedName* new_name = SB::buildInitializedName(new_name_sg, new_name_type, NULL);

    ROSE_ASSERT(new_name != NULL);

    new_name->set_scope(name->get_scope());

    return new_name;
  }

  void updateMetaDataAndReassignAddr(SgInitializedName* str,
      SgInitializedName* /*orig*/,
      SgFunctionDeclaration* fn_decl) {

#ifdef UPDATE_MD_DEBUG
    printf("updateMetaDataAndReassignAddr\n");
#endif
    // create_entry(dest, src)
    // this will be: create_entry(&str.ptr, str.addr);
    // At this stage, str.addr contains the caller variable's address.
    // so we copy over the metadata
    SgExpression* ptr_expr = Util::createDotExpFor(str, "ptr");
    SgExpression* dest_addr = Util::castToAddr(Util::createAddressOfOpFor(ptr_expr));

    SgExpression* src_addr = Util::createDotExpFor(str, "addr");

    SgScopeStatement* scope = str->get_scope();

    // create_entry function call.
    //  SgExpression* createfn = buildOverloadFn("create_entry", dest_addr, src_addr, SgTypeVoid::createType(), scope);
    //  SgExpression* createfn = buildOverloadFn("create_entry", dest_addr, src_addr, SgTypeVoid::createType(), scope, GEFD(fn_decl));


    //  SgExpression* createfn = buildOverloadFn("create_entry", dest_addr, src_addr, SgTypeVoid::createType(), scope, fn_decl);
    SgExpression* createfn = buildMultArgOverloadFn("create_entry", SB::buildExprListExp(dest_addr, src_addr), SgTypeVoid::createType(), scope, fn_decl);

#ifdef UPDATE_MD_DEBUG
    printf("createfn\n");
    Util::printNode(createfn);
#endif

    // Now, we make the following assignment:
    // str.addr = &str.ptr;

    SgExpression* lhs = SI::copyExpression(src_addr);
    SgExpression* rhs = SI::copyExpression(dest_addr);

    SgExpression* assign_op = SB::buildAssignOp(lhs, rhs);
#ifdef UPDATE_MD_DEBUG
    printf("assign_op\n");
    Util::printNode(assign_op);
#endif

    // Now, create a comma op to hold both these exprs
    SgExpression* comma_op = SB::buildCommaOpExp(createfn, assign_op);

    // Create a statement from this
    SgStatement* expr_stmt = SB::buildExprStatement(comma_op);


    // Insert this statement at the top of the body
#if 0
    SgFunctionDefinition* fn_def = fn_decl->get_definition();
    ROSE_ASSERT(fn_def != NULL);
    SgBasicBlock* bb = fn_def->get_body();
#endif
    SgBasicBlock* bb = Util::getBBForFn(fn_decl);
    SgStatementPtrList& stmts = bb->get_statements();

    SI::insertStatementBefore(*(stmts.begin()), expr_stmt);

#ifdef UPDATE_MD_DEBUG
    printf("expr_stmt\n");
    Util::printNode(expr_stmt);
#endif


    //handleSymbolTable(orig, str); -- already completed in parent function ---
  }


  // The problem with this function is as follows. We are trying to
  // create a new param list to replace the current one. However, what we are doing
  // is that we are actually creating a new param list, setting the parent for the
  // new args as the old param list, and adding them to the new param list... which
  // seems completely wrong... Through out the duration of this function, the new param
  // list considers the fn_decl as its parent, and so does the old one... the fn_decl only
  // thinks the old one is its child, until at the very end, we set the new param list
  // as the child. I think there should be a much cleaner way to handle this transition
  // and to handle the symbol tables. It looks like this is the only big function
  // which actually handleSymbolTable. There should be a clean solution to how func
  // params are handled, and how we can replace them without bringing the house down
  void instr(SgFunctionParameterList* param_list) {

    // Change those parameters which are pointers, to VoidStruct.
    // Add patch up code to the body
#ifdef FUNCPARAM_DEBUG
    printf("handleFuncParams\n");
#endif



    // 2. parameter list
    // Instead of using the type list, we are using the initialized name
    // list, since we would need to look up the symbol in case we
    // chnage the arg in a defining declaration
    SgInitializedNamePtrList& args = param_list->get_args();

    SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(isSgNode(param_list)->get_parent());
    ROSE_ASSERT(fn_decl != NULL);

    SgFunctionDefinition* fndef = fn_decl->get_definition();
    bool defining_decl = (fndef != NULL);


    // If its a forward decl, nothing to do in the case of RETAIN_FUNC_PROTOS
#ifdef RETAIN_FUNC_PROTOS
    if(!defining_decl) {
      return;
    }
#endif

    SgFunctionParameterList* new_param_list = SB::buildFunctionParameterList();
    SgInitializedNamePtrList& new_args = new_param_list->get_args();

#ifdef FUNCPARAM_DEBUG
    printf("created empty func params\n");
#endif

    // Doing this to ensure that new_args has a parent... this is required when
    // printing out where the node is...
    //  new_args->set_parent(isSgNode(fn_decl));
    new_param_list->set_parent(isSgNode(fn_decl));

#ifdef RETAIN_FUNC_PROTOS
    unsigned int arg_num = 0;
#endif


#ifdef FUNCPARAM_DEBUG
    printf("Traversing parameter list\n");
#endif

    for(SgInitializedNamePtrList::iterator iter = args.begin(); iter != args.end(); iter++) {

      // Get the initialize name
      SgInitializedName* arg_name = *iter;

#ifdef FUNCPARAM_DEBUG
      printf("arg_name: %s = %s\n", isSgNode(arg_name)->sage_class_name(), isSgNode(arg_name)->unparseToString().c_str());
#endif

      // Check if this arg is a pointer
      SgType* arg_type = arg_name->get_type();

#ifdef STRIP_TYPEDEFS
      arg_type = Util::getType(arg_type);
#endif

      //    printf("pointer type?");
#ifdef FUNCPARAM_DEBUG
      printf("is qualifying type?");
#endif
      if(Util::isQualifyingType(arg_type)) {
#ifdef FUNCPARAM_DEBUG
        printf("Yes\n");
        printf("arg_type: %s\n", arg_type->unparseToString().c_str());
#endif


#ifdef RETAIN_FUNC_PROTOS
        // Can't be a forward decl if we got till here...
        ROSE_ASSERT(defining_decl);
        // First up, we don't have to change the arg... it retains its original type
        // transform from: fn(ptr) to: fn(ptr) { create_entry_if_src_exists(&ptr, get_from_stack(arg_num) }
        SgInitializedName* new_arg_name = isSgInitializedName(SI::deepCopyNode(isSgNode(arg_name)));

        new_arg_name->set_parent(arg_name->get_parent());
        new_param_list->append_arg(new_arg_name);

        handleSymbolTable(arg_name, new_arg_name, false);

        // reading metadata sources off the stack, and insert at the address of the args
        // get src - get_from_stack(argnum)
        SgExpression* src = buildMultArgOverloadFn("get_from_stack", SB::buildExprListExp(SB::buildIntVal(arg_num)),
            Util::getAddrType(), SI::getScope(param_list), fn_decl);
        // increment argnum..
        arg_num++;

        // get the dest...
        SgExpression* dest = Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(new_arg_name)));

        // create_entry_if_src_exists(&ptr, get_from_stack(arg_num))
        SgExpression* overload = buildMultArgOverloadFn("create_entry_if_src_exists", SB::buildExprListExp(dest, src),
            SgTypeVoid::createType(),SI::getScope(param_list) , fn_decl);

        // { create_entry(&ptr, get_from_stack(arg_num)) }
        SgBasicBlock* bb = Util::getBBForFn(fn_decl);
        //RMM recommended fix: prepend to basic block. Otherwise we risk trying to put
        //a statement before a non-existent statement.
        //SgStatementPtrList& stmts = bb->get_statements();
        //SI::insertStatementBefore(*(stmts.begin()), SB::buildExprStatement(overload));
        bb->prepend_statement(SB::buildExprStatement(overload));
#else
        // Now, we have to replace this arg with a VoidStruct version of it
        SgInitializedName* new_arg_name = createStructVariableFor(arg_name);

        // The underlying type shouldn't be typedef
        SgType* under_type = strDecl::findInUnderlyingType(new_arg_name->get_type());
        ROSE_ASSERT(!isSgTypedefType(under_type));

#ifdef FUNCPARAM_DEBUG
        printf("new_arg_type: %s\n", new_arg_name->get_type()->unparseToString().c_str());
#endif

        //      new_args->append_arg(new_arg_name);
        //      new_arg_name->set_parent(new_args);
        //      new_args.push_back(new_arg_name);
        new_arg_name->set_parent(arg_name->get_parent());
        new_param_list->append_arg(new_arg_name);

        // FIXME: If its not a forward declaration, we would need to make
        // further changes -- like create a local version of the original argument
        // and then copy over the pointer data from VoidStruct and update
        // VoidStruct metadata
        if(defining_decl) {
#ifdef FUNCPARAM_DEBUG
          printf("Defining decl\n");
#endif
          //        createLocalVariableAndUpdateMetadata(new_arg_name, arg_name, fn_decl);
          // Replace all uses of the original arg_name with the new_arg_name
          // This creates a symbol for new_arg_name, and creates the necessary
          // mappings
          // By default, handleSymbolTable creates a mapping. But, just to be sure..
          // insert true at the end of the arg list -- addToMap = true
          handleSymbolTable(arg_name, new_arg_name, true);

          ROSE_ASSERT(new_arg_name->get_symbol_from_symbol_table() != NULL);

          // Things to do: 1. create local variable
          // 2. create entry at address given by local variable.
          // 3. initialize this variable with local variable address
          // FIXME: 4. copy over any initialization from orig variable, and updatePointer afterwards
          // also add new struct variable at the end... since return for the whole expression
          // has to be the new struct variable

          //createLocalVariableAndUpdateMetadata(new_arg_name, arg_name, fn_decl);

          // CHANGES FROM HERE ----

          // The changes are as follows:
          // 1. Instead of creating a local and then transfering metadata to its address in the TrackingDB,
          // we use the struct variable itself... and the address of str.ptr to keep a local copy of
          // the metadata
          // 2. We create a mapping from the original parameter (arg_name) and the str variable. Whenever
          // we encounter the use of the arg_name in handlePointerVarRefs/handleArrayVarRefs, we will
          // replace it with the str variable. No need to create a struct variable at that point. We already
          // have that.
          // 3. overloaded ops will have an update pointer at the end... in cases of ++/-- and assign.
          // This can be left as is, since it will only cause an update of hte pointer to itself...
          // *str.addr = str.ptr is harmless, and in this case, its equal to str.ptr = str.ptr since
          // str.addr = &str.ptr !

          // arg_name is used to update the symbol table mapping in varRemap
          updateMetaDataAndReassignAddr(new_arg_name, arg_name, fn_decl);

        }
#endif

      }
#if defined(CLASS_STRUCT_COPY) && !defined(RETAIN_FUNC_PROTOS)
      else if(strDecl::isOriginalClassType(arg_type)) {
#ifdef FUNCPARAM_DEBUG
        printf("Original Struct/Class object copy\n");
#endif

        // Here we have a struct var being passed to a function. We need to convert it to
        // a ref... and then create a local variable within the function, which takes the value of this
        // ref... That variable will be fed to ClassStructVarRemap so that it can replace this original
        // variable. Also, the whole point of this operation is to copy over the internal pointers
        // within the ref to the local variable newly created...

        // Now, replace this arg with a ref version of it...
        SgInitializedName* new_arg_name = createRefVersionFor(arg_name);
        new_arg_name->set_parent(arg_name->get_parent());
        new_param_list->append_arg(new_arg_name);

        if(defining_decl) {

#if 1
          // Symbol tables only exist for defining decls... if I am not wrong..
          // Add this new arg to the symbol table
          SgSymbol* arg_symbol = arg_name->get_symbol_from_symbol_table();
          SgVariableSymbol *arg_var_symbol = isSgVariableSymbol(arg_symbol);
#ifdef FUNCPARAM_DEBUG
          printf("Got scope. Now, symbolTable\n");
#endif
#endif

          // This is probably required since we need to do an assignment below...
          // and the assignment depends on inserting the new_arg_name into the symbol
          // table. But why not add it to class Remap here itself?
          //addToSymbolTable(new_arg_name, isSgNode(arg_var_symbol)->get_parent());

          SgName name= new_arg_name->get_name();
          SgScopeStatement* scope = arg_name->get_scope();
          ROSE_ASSERT(scope == new_arg_name->get_scope());
          // symbol table
          ROSE_ASSERT(scope != NULL);
          SgVariableSymbol* varSymbol = scope->lookup_variable_symbol(name);
          if (varSymbol==NULL)
          {
            varSymbol = new SgVariableSymbol(new_arg_name);
            ROSE_ASSERT(varSymbol);
            scope->insert_symbol(name, varSymbol);
          }

          // Here, we do the assigning from ref to the local variable... and then the ClassStructVarRemap
          // and finally, the element to element copy

          SgBasicBlock* bb = Util::getBBForFn(fn_decl);
          // Assign ref to local variable - instead of handling it this way, we could
          // use handleCSAssignOverloadOps - which will call the necessary functions.
          SgName s_name(arg_name->get_name().getString() + "_local" + boost::lexical_cast<std::string>(Util::VarCounter++));
          SgVariableDeclaration* local_var = SB::buildVariableDeclaration(s_name,
              arg_name->get_type(), NULL,
              isSgScopeStatement(bb));
          //Util::insertAtTopOfBB(bb, local_var);
          bb->prepend_statement(local_var);

          // local_var = *new_arg_name
          SgPointerDerefExp* new_arg_deref = SB::buildPointerDerefExp(SB::buildVarRefExp(new_arg_name));
          SgAssignOp* assign_op = SB::buildAssignOp(SB::buildVarRefExp(local_var), new_arg_deref);
          SI::insertStatementAfter(local_var, SB::buildExprStatement(assign_op));

          ROSE_ASSERT(arg_var_symbol);
          SgVariableSymbol* local_var_symbol = bb->lookup_variable_symbol(Util::getNameForDecl(local_var)->get_name());
          ROSE_ASSERT(local_var_symbol);

          ClassStructVarRemap.insert(VariableSymbolMap_t::value_type(arg_var_symbol, local_var_symbol));
          handleCSAssignOverloadOps(assign_op);

#if 0
#if 0
          Util::insertAtTopOfBB(bb, local_var);

          // local_var = *new_arg_name
          SgExpression* new_arg_deref = SB::buildPointerDerefExp(SB::buildVarRefExp(new_arg_name));
          SgExpression* assign_op = SB::buildAssignOp(SB::buildVarRefExp(local_var), new_arg_deref);
          SI::insertStatementAfter(local_var, SB::buildExprStatement(assign_op));
#endif

          ROSE_ASSERT(arg_var_symbol);
          //SgSymbol* local_symbol = Util::getNameForDecl(local_var)->get_symbol_from_symbol_table();
          //SgSymbol* local_symbol = Util::getNameForDecl(local_var)->search_for_symbol_from_symbol_table();

          SgVariableSymbol* local_var_symbol = bb->lookup_variable_symbol(Util::getNameForDecl(local_var)->get_name());
          ROSE_ASSERT(local_var_symbol);

          ClassStructVarRemap.insert(VariableSymbolMap_t::value_type(arg_var_symbol, local_var_symbol));

          // FIXME: Needs to be uncommented
          // Now, call handleCSAssignOverloadOps to do the work
          //handleCSAssignOverloadOps(assign_op);
#endif


        }
      }
#endif
      else {
#ifdef FUNCPARAM_DEBUG
        printf("No\n");
#endif
        SgInitializedName* new_arg_name = isSgInitializedName(SI::deepCopyNode(isSgNode(arg_name)));

        //      new_args->append_arg(new_arg_name);
        //      new_arg_name->set_parent(new_args);
        //      new_args.push_back(new_arg_name);
        new_arg_name->set_parent(arg_name->get_parent());
        new_param_list->append_arg(new_arg_name);
        if(defining_decl) {
          handleSymbolTable(arg_name, new_arg_name, false);
        }

        // Nothing else to do in this case
      }
    }

    // We hit this parameter list before we change the function decl, so we can check if the
    // fn_decl has a OriginalClassType return type, and make the necessary changes...

//    bool modifiedArgSize = false;
#if defined(CLASS_STRUCT_COPY) && !defined(RETAIN_FUNC_PROTOS)
    SgType* fn_retType = fn_decl->get_type()->get_return_type();

#ifdef STRIP_TYPEDEFS
    fn_retType = Util::getType(fn_retType);
#endif

    if(strDecl::isOriginalClassType(fn_retType)) {
      modifiedArgSize = true;
      // We need to add a ref variable to the end of arg list...
      // If its a defining decl, we also need to replace all return statements
      // with assignments, following by return void stmts. The return type will
      // be changed to void in handleFuncDecls.
      // original code: class_type fn1(....)
      // transformed code: class_type fn1(...., class_type *retvar)


      std::string new_name_string;
      if(!defining_decl) {
        new_name_string = "";
      }
      else {
        new_name_string = "retvar" + boost::lexical_cast<std::string>(Util::VarCounter++);
      }
      SgName new_name(new_name_string);

      SgType* new_arg_type = SgPointerType::createType(fn_retType);

      // Using param_list as scope. Shoudl be okay I guess...
      SgInitializedName* new_arg_name = SB::buildInitializedName(new_name, new_arg_type, NULL);

      new_arg_name->set_scope(SI::getScope(param_list));

      new_arg_name->set_parent(param_list);
      new_param_list->append_arg(new_arg_name);

      if(defining_decl) {

        // Symbol tables only exist for defining decls... if I am not wrong..
        // Add this new arg to the symbol table
        SgScopeStatement* scope = SI::getScope(param_list);
        SgSymbolTable* symtab = scope->get_symbol_table();

        addToSymbolTable(new_arg_name, symtab);


        // We change return stmts in handleReturnStmts. This ensures that all the modifications on
        // the expression in the ret stmt take place as usual.
#if 0
        // Now change all the return statements to return void, and do an assignment instead.
        // Get hold of all the return stmts
        Rose_STL_Container <SgNode*> stmts = NodeQuery::SI::querySubTree(fn_decl, V_SgReturnStmt);

        Rose_STL_Container<SgNode*>::iterator i;
        for(i = stmts.begin(); i != stmts.end(); i++) {
          SgReturnStmt* cur_stmt = isSgReturnStmt(*i);
          ROSE_ASSERT(cur_stmt);
          // paranoid checks...
          ROSE_ASSERT(isSgReturnStmt(cur_stmt));
          ROSE_ASSERT(isSgReturnStmt(cur_stmt)->get_expression()->get_type() == fn_decl->get_type()->get_return_type());

          // this copies the expression in the return statement and uses it as rhs of the assign op
          // *retvar = ret_exp;
          SgAssignOp* assign_op = SB::buildAssignOp(SB::buildPointerDerefExp(SB::buildVarRefExp(new_arg_name)), SI::copyExpression(isSgReturnStmt(cur_stmt)->get_expression()));
          SgExprStatement* assign_stmt = SB::buildExprStatement(assign_op);

          SgStatement* retstmt = SB::buildReturnStmt();

          // original: return ret_exp
          // transformed: *retvar = ret_exp
          //              return
          replaceWrapper(cur_stmt, assign_stmt);
          SI::insertStatementAfter(assign_stmt, retstmt);
          // Need to do this in a separate function for return stmts...
          // It can't be done here since the expression hasn't yet been modified, and whatever modifications need
          // to take place on the expression, haven't yet taken place. So, its best if we catch these return stmts
          // in TopBotTrack2 and handle the return stmts like we handle assign ops, and other such stmts.
          // Right now, the bug is: *retvar = copy_this, instead of *retvar = copy_this_local
        }
#endif
      }


    }
#endif



    //  SI::deepDelete(isSgNode(param_list));
    fn_decl->set_parameterList(new_param_list);

#ifdef FUNCPARAM_DEBUG
    printf("fn_decl\n");
    Util::printNode(fn_decl);

    printf("size: new_args: %d, args: %d\n", new_args.size(), args.size());
#endif

#ifdef CLASS_STRUCT_COPY
    // We shouldn't add more than 1 in any case
    if(modifiedArgSize) {
      ROSE_ASSERT(new_args.size() == args.size() + 1);
    }
    else {
      ROSE_ASSERT(new_args.size() == args.size());
    }
#else
    ROSE_ASSERT(new_args.size() == args.size());
#endif

#if 0
    //  new_args->set_parent(isSgNode(fn_decl));

    //  fn_decl->set_parameterList(new_args);
    fn_decl->set_parameterList(new_param_list);
#endif

  }

  void instr(SgDeclarationStatement* decl) {
    if(isSgVariableDeclaration(decl)) {
      instr(isSgVariableDeclaration(decl));
    }
    else if(isSgFunctionDeclaration(decl)) {
      instr(isSgFunctionDeclaration(decl));
    }
    else if(isSgFunctionParameterList(decl)) {
      instr(isSgFunctionParameterList(decl));
    }
  }
}
