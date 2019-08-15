#include <algorithm>

#include "preProcessAST.h"
#include "traverse.h" // RMM

namespace PPA
{
  NodeContainer NodesToProcess;
  SgProject* project;

  Util::nodeType TopBotTrack3::evaluateSynthesizedAttribute(SgNode* node,
          Util::nodeType inh, SynthesizedAttributesList) {
    #ifdef SYNTH3_DEBUG
    printf("TopBotTrack3: Ev_Synth_Attr\n");
    printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
    Util::printNodeExt(node);
    #endif

    if(inh.inhattr == Util::DOT_EXP) {
      #ifdef SYNTH3_DEBUG
      printf("synth:Util::DOT_EXP: Node added\n");
      #endif
      // This is a dot or arrow exp
      ROSE_ASSERT(Util::isDotOrArrow(node));
      NodesToProcess.push_back(node);
    }
    else if(inh.inhattr == Util::DOT_EXP_CHILD) {
      ROSE_ASSERT(Util::isDotOrArrow(node->get_parent()));
      // If the node is a var ref or this exp, no need to transform
      // If the node is the rhs of a dot or arrow, no need to transform
      if(!isSgVarRefExp(node) && !isSgThisExp(node) && !Util::isRHSOfDotOrArrow(node)) {
        // This is an expression besides a var ref exp within an enclosing dot expression... needs to be transformed.
        #ifdef SYNTH3_DEBUG
        printf("synth:WITHIN_Util::DOT_EXP: Node added\n");
        #endif
        NodesToProcess.push_back(node);
      }
      else {
        #ifdef SYNTH3_DEBUG
        printf("synth:WITHIN_Util::DOT_EXP: Node **not** added\n");
        #endif
      }
    }

    // The attribute we are looking for is inherited, not synthesized
    // so, always propagate NOT_Util::DOT_EXP and Util::INH_UNKNOWN from each node in the
    // synthesized traversal
    Util::nodeType synattr(Util::UNKNOWN,Util::INH_UNKNOWN);
    return synattr;
  }

  Util::nodeType TopBotTrack3::evaluateInheritedAttribute(SgNode* node,
                          Util::nodeType inh) {
    #ifdef INH3_DEBUG
    printf("TopBotTrack3: Ev_Inh_Attr\n");
    printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
    #endif

    if(Util::isDotOrArrow(node)) {
      #ifdef INH3_DEBUG
      printf("inh:DotOrArrow: WITHIN_Util::DOT_EXP\n");
      #endif
      Util::nodeType new_inh(Util::UNKNOWN, Util::DOT_EXP);
      return new_inh;
    }
    else if(inh.inhattr == Util::DOT_EXP) {
      // This is a node that is inheriting Util::DOT_EXP
      // from its parent... which is a dot or arrow
      ROSE_ASSERT(Util::isDotOrArrow(node->get_parent()));
      if(Util::isRHSOfDotOrArrow(node)) {
        #ifdef INH3_DEBUG
        printf("inh:RHS: Util::DOT_EXP: Util::INH_UNKNOWN\n");
        #endif
        // RHS of dot or arrow, nothing to do on this side
        Util::nodeType new_inh(Util::UNKNOWN, Util::INH_UNKNOWN);
        return new_inh;
      }
      else {
        #ifdef INH3_DEBUG
        printf("inh:LHS: Util::DOT_EXP: WITHIN_DOT_EXP\n");
        #endif
        // But since this node by itself isn't
        // a dot or arrow, nothing to do here...
        Util::nodeType new_inh(Util::UNKNOWN, Util::DOT_EXP_CHILD);
        return new_inh;
      }
    }
    else if(inh.inhattr == Util::DOT_EXP_CHILD) {
      // This is now the second generation... the child of
      // the child. Nothing to do...
      #ifdef INH3_DEBUG
      printf("inh:WITHIN_Util::DOT_EXP: Util::INH_UNKNOWN\n");
      #endif
      Util::nodeType new_inh(Util::UNKNOWN, Util::INH_UNKNOWN);
      return new_inh;
    }
    else {
      Util::nodeType new_inh(Util::UNKNOWN, Util::INH_UNKNOWN);
      return new_inh;
    }
  }

  void pushGlobalVarInitsToMain()
  {
    SgGlobal* global = SI::getFirstGlobalScope(project);

    Rose_STL_Container <SgVariableDeclaration*> global_var_decls;
    global_var_decls = SI::querySubTree<SgVariableDeclaration>(global, V_SgVariableDeclaration);

    // find Main
    SgFunctionDeclaration* MainFn = SageInterface::findMain(global);
    if(!MainFn) {
      printf("pushGlobalVarInits: can't find main -- returning");
      return;
    }

    SgBasicBlock *bb = Util::getBBForFn(MainFn);

    for(unsigned int index = 0; index < global_var_decls.size(); index++) {

      SgVariableDeclaration* var_decl = global_var_decls[index];

      if(Util::NoInitializer(var_decl) || !isSgGlobal(var_decl->get_scope())) {
        continue;
      }

      // Lets copy the initializer to a statement.. inserted at the top of main
      // input: global_var = init
      // output: global_var; main() { global_var = init; }

      SgInitializedName* var_name = Util::getNameForDecl(var_decl);
      SgAssignInitializer* assign_init = isSgAssignInitializer(var_name->get_initializer());

      // Skip constructor initializers
      if(assign_init == NULL) {
        continue;
      }

      ROSE_ASSERT(assign_init != NULL);

      // Copy the initializer
      SgExpression* init_copy = SI::copyExpression(assign_init->get_operand());

      // Set the initializer in the var decl to NULL.
      var_name->set_initializer(NULL);

      // Create an assign op.. which will be replaced by the appropriate
      // operation by the handleAssignOverloadOps function below.
      SgAssignOp* var_assign = SB::buildAssignOp(SB::buildVarRefExp(var_name), init_copy);
      SgExprStatement* assign_stmt = SB::buildExprStatement(var_assign);

      Util::insertAtTopOfBB(bb, assign_stmt);

    }

  }

  //RMM FIX
  //Ensure that all case option statements have basic blocks for their bodies to work with the instrumentation.
  void modifyCaseOptionStmts(){
    Rose_STL_Container <SgCaseOptionStmt*> case_option_stmts = SI::querySubTree<SgCaseOptionStmt>(isSgNode(project), V_SgCaseOptionStmt);
    for(unsigned int index = 0; index < case_option_stmts.size(); index++) {
      SgCaseOptionStmt* stmt = case_option_stmts[index];
      SI::ensureBasicBlockAsBodyOfCaseOption(stmt);
    }
  }

  //Ensure that all usages of the register keyword are removed as they can create unnecessary complications.
  void eliminateRegisterKeyword(){
    Rose_STL_Container <SgVariableDeclaration *> var_decls = SI::querySubTree< SgVariableDeclaration >(isSgNode(project), V_SgVariableDeclaration );
    for(unsigned int index = 0; index < var_decls.size(); index++) {
      SgVariableDeclaration* var_decl = var_decls[index];
      SgDeclarationModifier declMod = var_decl->get_declarationModifier();
      SgStorageModifier* sm = &declMod.get_storageModifier();
      if(sm->isRegister()){ //We have found a declaration that uses the "register" modifier. Now we must get rid of it.
        sm->setDefault();
        ROSE_ASSERT(!sm->isRegister());
      }
    }

  }


  void fixVarRefsInArrayDeclarations(){
    Rose_STL_Container <SgVariableDeclaration *> var_decls = SI::querySubTree< SgVariableDeclaration >(isSgNode(project), V_SgVariableDeclaration );
    for(unsigned int index = 0; index < var_decls.size(); index++) {
      SgVariableDeclaration* var_decl = var_decls[index];
      SgInitializedName* var_name = Util::getNameForDecl(var_decl);
      SgType* var_type = var_name->get_typeptr();
      SgArrayType* arrayType = isSgArrayType(var_type);
      if(arrayType) {
        SgExpression* arrayIndex = arrayType->get_index();
        if(arrayIndex){
          SgVarRefExp* ref = isSgVarRefExp(arrayIndex);
          if(ref) {
            SgVariableSymbol* symbol = ref->get_symbol();
            SgInitializedName* iname = symbol->get_declaration();
            SgInitializer* init = iname->get_initializer();
              if(init && isSgAssignInitializer(init))
              {
                SgExpression* operand = isSgAssignInitializer(init)->get_operand();
                arrayType->set_index(operand);
              }
          }

        }
      }
    }
  }

  void convertGlobalUnboundedCharArrays(){
    Rose_STL_Container <SgVariableDeclaration *> var_decls = SI::querySubTree< SgVariableDeclaration >(isSgNode(project), V_SgVariableDeclaration );
    for(unsigned int index = 0; index < var_decls.size(); index++) {
       SgVariableDeclaration* var_decl = var_decls[index];
       if(!isSgGlobal(var_decl->get_scope())) //Here we're only interested in arrays declared in global scope.
        continue;

      SgInitializedName* var_name = Util::getNameForDecl(var_decl);
      SgType* var_type = var_name->get_typeptr();
      SgType* arrayType = isSgArrayType(var_type);
      if(arrayType) {
        if(SI::getDimensionCount(arrayType) != 1) //Ignore arrays with multiple dimensions.
          continue;
        SgType* arrayElementType = SI::getArrayElementType(arrayType);
        if(isSgTypeChar(arrayElementType)){
          if(isSgNullExpression(SI::get_C_array_dimensions(arrayType).front())) //Only perform the instrumentation if the size of the array isn't explicitly given.
            var_name->set_typeptr(SB::buildPointerType(SB::buildCharType()));
        }


      }

    }

  }
  //RMM BOOKMARK
  void castFunctionRefExpressions()
  {
    Rose_STL_Container < SgAssignInitializer  *> assigninits = SI::querySubTree<  SgAssignInitializer  >(isSgNode(project), V_SgAssignInitializer );

    for(unsigned int index = 0; index < assigninits.size(); index++)
    {
      SgAssignInitializer *  assigninit = assigninits[index];
      SgExpression* rhs = assigninit->get_operand();

      if (isSgFunctionRefExp(rhs))
      {
        SgVariableDeclaration* decl = isSgVariableDeclaration(SI::getEnclosingStatement(assigninit));

        if(decl)
        {
          SgType* t = SI::getFirstVarSym(decl)->get_type(); //assumption: there's only one variable symbol. In practice, this may not be the case. e.g. "int x,y = 5,7;"

          SgCastExp* casted_rhs = SB::buildCastExp(SI::copyExpression(rhs), t, SgCastExp::e_C_style_cast);
          SI::replaceExpression(rhs, casted_rhs);
        }
      }
    }
  }

#ifdef RMM_MISSING_FUNC_DEFINITION_INSTRUMENTATION
  void modifyForeignFunctionCalls()
  {
    Rose_STL_Container <SgFunctionCallExp*> func_calls = SI::querySubTree<SgFunctionCallExp>(isSgNode(project), V_SgFunctionCallExp);

    for(unsigned int index = 0; index < func_calls.size(); index++)
    {
      SgFunctionCallExp* call = func_calls[index];
      SgFunctionSymbol* fn_symbol = call->getAssociatedFunctionSymbol(); //Note: Function pointers etc. will not have an associated function symbol.
      if(!fn_symbol)
        continue;

#ifdef RMM_FFC_DEBUG
        printf("Examining: %s\n",fn_symbol->get_name().str());
#endif /* RMM_FFC_DEBUG */

      SgFunctionDeclaration* fn_decl = call->getAssociatedFunctionDeclaration();
      if (!fn_decl->get_definingDeclaration())
      {
#ifdef RMM_FFC_DEBUG
          printf("%s has no defining declaration.\n",fn_symbol->get_name().str());
#endif /* RMM_FFC_DEBUG */
        SgType* retType = fn_decl->get_orig_return_type();
        if(isSgPointerType(retType)){
#ifdef RMM_FFC_DEBUG
          printf("Return type of %s is a pointer.\n",fn_symbol->get_name().str());
#endif /* RMM_FFC_DEBUG */

          if(strcmp(fn_symbol->get_name().str(),"malloc") == 0  || strcmp(fn_symbol->get_name().str(),"realloc") == 0) //malloc and realloc are already handled
            continue;
          else if(Trav::isQualifyingLibCall(call))
            continue;

          #ifdef RMM_FFC_DEBUG
          printf("FFC DEBUG (node identified): %s\n",fn_symbol->get_name().str());
          Util::printNode(retType);
          #endif

          #ifdef RMM_GENERATE_WRAPPER_FUNCTION
          if(!SI::lookupFunctionSymbolInParentScopes (SgName(fn_symbol->get_name().str() + SI::declarationPositionString(fn_decl)), SI::getScope (call))){

            SgFunctionDeclaration* nF_decl = SB::buildNondefiningFunctionDeclaration(SgName(fn_symbol->get_name().str() + SI::declarationPositionString(fn_decl) ),retType,fn_decl->get_parameterList(),fn_decl->get_scope(),NULL);

            SI::insertStatementBefore(SI::getEnclosingFunctionDeclaration(call),nF_decl);

            SgFunctionDeclaration* nF_decl_defining = SB::buildDefiningFunctionDeclaration(SgName(fn_symbol->get_name().str() + SI::declarationPositionString(fn_decl) ),retType,fn_decl->get_parameterList(),nF_decl->get_scope(),NULL,false,nF_decl);

            SgExpression* nF_expr = SI::copyExpression (isSgExpression(call));

            // \pp was: nF_expr->addNewAttribute(std::string("RMM_FFC"), &AstAttribute()); //A marker to indicate that more instrumentation is needed (see instrCallExps.C)

            nF_expr->addNewAttribute(std::string("RMM_FFC"), NULL); //A marker to indicate that more instrumentation is needed (see instrCallExps.C)

            SgReturnStmt* nF_retStmt = SB::buildReturnStmt(nF_expr);

            SgFunctionDefinition * def = nF_decl_defining->get_definition();
            def->append_statement(nF_retStmt);

            SI::insertStatementBefore(SI::getEnclosingFunctionDeclaration(call),isSgStatement(nF_decl_defining));
          }
          SgFunctionRefExp* e = SageBuilder::buildFunctionRefExp(SgName(fn_symbol->get_name().str()) + SI::declarationPositionString(fn_decl),SI::getScope (call) );
          call->set_function(isSgExpression(e));
          #else
          // \pp was: call->addNewAttribute(std::string("RMM_FFC"), &AstAttribute());
          call->addNewAttribute(std::string("RMM_FFC"), NULL);
          #endif
          }

        } else {
          SgDeclarationStatement* defDecl = fn_decl->get_definingDeclaration();
          #ifdef RMM_FFC_DEBUG
          printf("%s has a defining declaration.\n",fn_symbol->get_name().str());
          Util::printNode(defDecl);
          #endif
          if(!defDecl->get_file_info()->isSameFile(call->get_file_info()))
          {
            // \pp was: call->addNewAttribute(std::string("RMM_FFC"), &AstAttribute());
            call->addNewAttribute(std::string("RMM_FFC"), NULL);
            #ifdef RMM_FFC_DEBUG
            printf("The defining declaration of %s is in a different file! Marked for additional instrumentation.\n",fn_symbol->get_name().str());
            #endif
          }
        }
      }
    }
  #endif

  // This function transforms for and while loops to work with the instrumentation.
  // while(cond) { } is transformed to while(1) { if(!(cond)) { break; } }
  // We also split the cond to leave just the conditional expression, and remove
  // the other expressions and place them before the "if" after transformation.
  // For example: while(exp1, cond) { } is transformed to
  // while(1) { exp1; if(cond) { break; } }
  // for(init; cond; incr) { } is transformed to for(init;;incr) { if(!(cond)) { break; } }
  void modifyForAndWhileLoops()
  {
    Rose_STL_Container <SgWhileStmt*> while_stmts = SI::querySubTree<SgWhileStmt>(isSgNode(project), V_SgWhileStmt);

    for(unsigned int index = 0; index < while_stmts.size(); ++index)
    {
      SgWhileStmt* whilestmt = while_stmts[index];
      SgStatement* condstmt = whilestmt->get_condition();
      if(!isSgExprStatement(condstmt))
        continue; //RMM TEST

      //ROSE_ASSERT(isSgExprStatement(condstmt));

      SgExpression* cond_exp = isSgExprStatement(condstmt)->get_expression();
      // If its a comma op exp, split the lhs and rhs.
      // lhs becomes a statement in the body. rhs goes into the if stmt
      SgExpression* comma_lhs = NULL;
      SgExpression* cond = NULL;
      if(isSgCommaOpExp(cond_exp)) {
        comma_lhs = SI::copyExpression(isSgCommaOpExp(cond_exp)->get_lhs_operand());
        cond = SI::copyExpression(isSgCommaOpExp(cond_exp)->get_rhs_operand());
      }
      else {
        cond = SI::copyExpression(cond_exp);
      }

      // cond -> !cond
      SgExpression* not_cond = SB::buildNotOp(cond);
      SgStatement* not_cond_stmt = SB::buildExprStatement(not_cond);

      // if(!cond) { break; }
      SgStatement* if_true_body = SB::buildBreakStmt();
      SgStatement* if_stmt = SB::buildIfStmt(not_cond_stmt, if_true_body, NULL);

      // insert if stmt into while
      SgStatement* while_body = whilestmt->get_body();
      if(isSgBasicBlock(while_body)) {
        // prepend prepend this statement to the basic block
        isSgBasicBlock(while_body)->prepend_statement(if_stmt);
        // if its a comma op, comma_lhs won't be NULL, so prepend
        // it as well
        if(comma_lhs) {
          isSgBasicBlock(while_body)->prepend_statement(SB::buildExprStatement(comma_lhs));
        }
      }
      else
      {
        // insert a basic block in the place of the old
        // statement and insert if_stmt in that basic block.
        // if the comma_lhs is not NULL, insert it before the if_stmt
        SgBasicBlock* bb =
           comma_lhs
              ? SB::buildBasicBlock(SB::buildExprStatement(comma_lhs), if_stmt, SI::copyStatement(while_body))
              : SB::buildBasicBlock(if_stmt, SI::copyStatement(while_body));

        SI::deepDelete(while_body);
        whilestmt->set_body(bb);
        bb->set_parent(whilestmt);
      }

      // while(cond) -> while(1)
      SgExprStatement* new_cond = SB::buildExprStatement(SB::buildIntVal(1));
      whilestmt->set_condition(new_cond);
      // Remove the old condition
      SI::deepDelete(condstmt);
      // Set the parent for the new cond as the whilestmt
      new_cond->set_parent(whilestmt);
    }
  }

  void replacePlusMinusAssigns(SgBinaryOp* op, bool plus)
  {
    SgExpression* lhs = op->get_lhs_operand();
    SgExpression* rhs = op->get_rhs_operand();

    SgType* lhs_type = lhs->get_type();
    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif

    if(!Util::isQualifyingType(lhs_type)) {
      return;
    }

    SgBinaryOp* plus_minus;
    if(plus) {
      plus_minus = SB::buildAddOp(SI::copyExpression(lhs), SI::copyExpression(rhs));
    }
    else {
      plus_minus = SB::buildSubtractOp(SI::copyExpression(lhs), SI::copyExpression(rhs));
    }

    SgBinaryOp* assign_op = SB::buildAssignOp(SI::copyExpression(lhs), plus_minus);

    SI::replaceExpression(op, assign_op);

  }


  void splitPlusMinusAssigns()
  {
    Rose_STL_Container <SgPlusAssignOp*> plus_assigns =
                      SI::querySubTree<SgPlusAssignOp>(isSgNode(project), V_SgPlusAssignOp);

    for(unsigned int index = 0; index < plus_assigns.size(); index++) {

      SgPlusAssignOp* ce = plus_assigns[index];

      replacePlusMinusAssigns(ce);
    }

    Rose_STL_Container <SgMinusAssignOp*> minus_assigns =
                      SI::querySubTree<SgMinusAssignOp>(isSgNode(project), V_SgMinusAssignOp);

    for(unsigned int index = 0; index < minus_assigns.size(); index++) {

      SgMinusAssignOp* ce = minus_assigns[index];

      replacePlusMinusAssigns(ce, false);
    }

  }

  void convertArrowThisToDot() {

    // expressions of the form this->var or this->func will be converted
    // to (*this).var and (*this).func
    Rose_STL_Container <SgArrowExp*> arrows =
                      SI::querySubTree<SgArrowExp>(isSgNode(project), V_SgArrowExp);

    for(unsigned int i = 0; i < arrows.size(); i++) {
      SgArrowExp* arr = arrows[i];
      if(isSgThisExp(arr->get_lhs_operand())) {
        // this arrow needs to be converted to dot...
        SgExpression* lhs = arr->get_lhs_operand();
        SgExpression* deref_lhs = SB::buildPointerDerefExp(SI::copyExpression(lhs));
        SgExpression* rhs_copy = SI::copyExpression(arr->get_rhs_operand());
        SgDotExp* new_dot = SB::buildDotExp(deref_lhs, rhs_copy);
        SI::replaceExpression(arr, new_dot);
      }
    }
  }

  void functionCallArgCastHandler() {

    Rose_STL_Container <SgFunctionCallExp*> fn_calls;
    fn_calls = SI::querySubTree<SgFunctionCallExp>(project, V_SgFunctionCallExp);

    for(unsigned int index = 0; index < fn_calls.size(); index++) {
      SgFunctionCallExp* cf = fn_calls[index];

      SgExprListExp* args = cf->get_args();
      // This is an indexable structure (like a vector)
      SgExpressionPtrList& exprs = args->get_expressions();

      // Now get the declaration for this function... and
      // if available the definition
      // If we can't find the definition, we won't be changing
      // the function signature (through handleFuncParams), so we might as well leave this
      // as it is...

      // Associated Function Declaration.. param list types
      SgFunctionDeclaration* fn_decl = cf->getAssociatedFunctionDeclaration();
      // fn_decl may be NULL if its a function pointer being deref'ed
      // If that is the case, then we don't do anything...
      if(!fn_decl) {
        continue;
      }

      SgFunctionDeclaration* fn_def_decl = isSgFunctionDeclaration(fn_decl->get_definingDeclaration());

      if(!fn_def_decl) {
        #ifdef PANIC_DEBUG
        printf("fnArgCastHandler: Can't find defining decl for %s. Continuing\n",
            fn_decl->get_name().getString().c_str());
        #endif
        continue;
      }

      SgInitializedNamePtrList& decl_args = fn_def_decl->get_args();

      // Now, lets see if there is cast from string val to char*

      //for(unsigned int exp_index = 0; exp_index < exprs.size(); exp_index++) {
      // RMM CHANGE
      size_t minsz = std::min(exprs.size(), decl_args.size());

      for (size_t exp_index = 0; exp_index < minsz; ++exp_index)
      {
        // exprs -> func_call args
        SgExpression*      ce = exprs[exp_index];

        // decl_args -> defining decl args
        SgInitializedName* ci = decl_args[exp_index];

        // If they don't match, replace ce with casted_ce... (although we are traversing
        // the structure, it should be okay to replace the member, I guess
        SgType* ce_type = ce->get_type();
        #ifdef STRIP_TYPEDEFS
        ce_type = Util::getType(ce_type);
        #endif

        SgType* ci_type = ci->get_type();
        #ifdef STRIP_TYPEDEFS
        ci_type = Util::getType(ci_type);
        #endif

        //if(isSgStringVal(ce) && !Util::compareTypes(ce_type, ci_type)) {
        //RMM CHANGE
        if(isSgStringVal(ce) && !Util::compareTypes(ce_type, ci_type) && isSgPointerType(ci_type)) {
          // There is a cast from string to a pointer type (char* mostly)...
          ROSE_ASSERT(isSgPointerType(ci_type));
          SgExpression* casted_ce = SB::buildCastExp(SI::copyExpression(ce), ci_type,
                            SgCastExp::e_C_style_cast);
          SI::replaceExpression(ce, casted_ce);
        }
      }
    }

  }


  SgFunctionDeclaration* findDeclParent(SgNode* node) {
    if(!node) {
      return NULL;
    }
    else if(isSgFunctionDeclaration(node)) {
      return isSgFunctionDeclaration(node);
    }
    else if(isSgGlobal(node)) {
      return NULL;
    }
    else {
      return findDeclParent(node->get_parent());
    }
  }


  SgClassDeclaration* duplicateStruct(SgClassDeclaration* orig, SgName new_name, SgScopeStatement* scope, SgStatement* pos) {
    SgClassDeclaration* str_decl =
              SB::buildStructDeclaration(new_name,scope);

    SgClassDefinition* str_def = SB::buildClassDefinition(str_decl);

    SI::insertStatementBefore(pos, str_decl);

    SgDeclarationStatementPtrList& members = orig->get_definition()->get_members();

    SgDeclarationStatementPtrList::iterator iter = members.begin();

    for(; iter != members.end(); ++iter) {

      SgDeclarationStatement* decl_stmt = *iter;

      // Function calls within structs/classes in C++...
      if(!isSgVariableDeclaration(decl_stmt)) {
          ROSE_ASSERT(0 && "Member function inside a class/struct located within a function!");
          continue;
      }

      SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl_stmt);
      SgInitializedName* var_name = Util::getNameForDecl(var_decl);

      SgVariableDeclaration* new_var_decl = SB::buildVariableDeclaration(var_name->get_name(), var_name->get_type(),
              isSgInitializer(SI::deepCopyNode(var_name->get_initializer())),
              str_def);

      str_def->append_member(new_var_decl);
    }

    return str_decl;

  }

  void evictStructsFromFunctions()
  {
    // Get all the struct declarations... then, from that list, figure out which ones
    // are *defined within functions*
    Rose_STL_Container <SgClassDeclaration*> str_decls =
                      SI::querySubTree<SgClassDeclaration>(isSgNode(project), V_SgClassDeclaration);

    typedef std::map<SgClassDeclaration*, SgClassDeclaration*> ClassMap;
    ClassMap replacedStructs;

    for (size_t i = 0; i < str_decls.size(); ++i)
    {
      SgClassDeclaration* cd = str_decls[i];

      // This is a forward declaration, no need to hoist this.
      if(cd->isForward()) continue;

      // Find if the parent of this node ever hits a function declaration or class declaration
      // This struct is not within a function/class, nothing to do.
      SgFunctionDeclaration* parent = findDeclParent(cd);
      if (!parent) continue;

      SgFunctionDeclaration* par_decl = isSgFunctionDeclaration(parent);
      SgName                 str_name(par_decl->get_name().getString() + "_" + cd->get_name().getString());
      SgClassDeclaration*    str_decl = duplicateStruct(cd, str_name, parent->get_scope(), parent);

      // Now, we need to make sure that the variables of the original struct, and now variables of the new struct
      // For that, we create a map from the orig decl to the new decl.
      replacedStructs[cd] = str_decl;
    }

    // One way to go about fixing the variable declarations and references is as follows:
    // 1. Get the *relevant* var decls, replace them with new var decls. Create a map which
    // tracks the orig to new var decl.
    // 2. Get the *relevant* var refs, replace them with new var refs.
    // Here *relevant* refers to how a map is used to figure out if this is a var decl we are interested in
  }

  void castReturnExps() {

    Rose_STL_Container <SgReturnStmt*> rets = SI::querySubTree<SgReturnStmt>(isSgNode(project), V_SgReturnStmt);

    for(unsigned int i = 0; i < rets.size(); i++) {

      SgReturnStmt* r = rets[i];
      SgExpression* rexp = r->get_expression(); //RMM FIX
      if(!isSgNullExpression(rexp)) { //RMM FIX

        // get the function declaration
        SgFunctionDefinition* fn_def = SageInterface::getEnclosingProcedure(r);
        SgFunctionDeclaration* fn_decl = fn_def->get_declaration();
        SgType* ty = fn_decl->get_type()->get_return_type();
        #ifdef STRIP_TYPEDEFS
        ty = Util::getType(ty);
        #endif
        if(!Util::compareTypes(ty, r->get_expression()->get_type())) {
          // need to cast it...
          SgExpression* casted = SB::buildCastExp(SI::copyExpression(r->get_expression()), ty, CAST_TYPE);
          SI::replaceExpression(r->get_expression(), casted);
        }
      }
    }
  }


  SgVariableDeclaration* pushToLocalVar(SgExpression* exp, SgScopeStatement* scope) {
    SgName temp("temp_" + boost::lexical_cast<std::string>(Util::VarCounter++));
    return SB::buildVariableDeclaration(temp, exp->get_type(), SB::buildAssignInitializer(exp, exp->get_type()),
                    scope);
  }

  SgVariableDeclaration* takeRefAndPushToLocalVar(SgExpression* exp) {

    ROSE_ASSERT(exp->get_type());


    SgExpression* addr = Util::createAddressOfOpFor(SI::copyExpression(exp));

    #if 0
    SgName temp("temp_" + boost::lexical_cast<std::string>(Util::VarCounter++));
    return SB::buildVariableDeclaration(temp, SB::buildReferenceType(exp->get_type()), SB::buildAssignInitializer(addr, addr->get_type()),
                    Util::getScopeForExp(exp));
    #endif
    // assign this to local variable and push to prev stmt
    return pushToLocalVar(addr, Util::getScopeForExp(exp));
  }

  void simplifyDotExp(SgDotExp* dot) {

    SgVariableDeclaration* tempvar;
    SgExpression* subst;
    // If the rhs operand is a variable operand, then we take its reference
    // Need to take it as a reference since it could be an lval/rval
    if(isSgVarRefExp(dot->get_rhs_operand())) {
      // rhs is var ref... move it to expression statement prior to the current stmt...
      tempvar = takeRefAndPushToLocalVar(dot);
      subst = SB::buildPointerDerefExp(SB::buildVarRefExp(tempvar));
    }
    else if(isSgFunctionCallExp(dot->get_rhs_operand())) {
      // If its a function, then we simply take its value/ref in a variable
      // rhs is a func call... move it to the expression stmt prior to the current stmt..
      tempvar = pushToLocalVar(SI::copyExpression(dot), Util::getScopeForExp(dot));
      subst = SB::buildVarRefExp(tempvar);
    }
    else if(isSgMemberFunctionRefExp(dot->get_rhs_operand())) {
      // Nothing to do...
      return;
    }
    else {
      printf("Unsupported case in simplifyDotExp\n");
      Util::printNode(dot->get_rhs_operand());
      ROSE_ASSERT(0);
    }
    SI::insertStatementBefore(SI::getEnclosingStatement(dot), tempvar);
    SI::replaceExpression(dot, subst);
  }

  void simplifyArrowExp(SgArrowExp* arrow) {
    // Deref the lhs... and convert to dot...
    SgExpression* lhs = arrow->get_lhs_operand();
    SgExpression* lhs_deref = SB::buildPointerDerefExp(SI::copyExpression(lhs));

    // Now, SB::build the dot expression...
    SgDotExp* dot = SB::buildDotExp(lhs_deref, SI::copyExpression(arrow->get_rhs_operand()));

    // replace the original one with this one...
    SI::replaceExpression(arrow, dot);

    // simplify the dot
    simplifyDotExp(dot);
  }

  void pushToLocalVarAndReplace(SgExpression* exp) {
    SgVariableDeclaration* tempvar = pushToLocalVar(SI::copyExpression(exp), Util::getScopeForExp(exp));
    SI::insertStatementBefore(SI::getEnclosingStatement(exp), tempvar);
    SI::replaceExpression(exp, SB::buildVarRefExp(tempvar));
  }

  void simplifyNodes() {

    #ifdef SN_DEBUG
    printf("InstrumentNodes4 - begin\n");
    #endif
    for(NodeContainer::iterator nit = NodesToProcess.begin(); nit != NodesToProcess.end(); ++nit) {

      SgNode* node = *nit;


      // print statements in SgGlobal
      //Util::printStatements(getGlobalScope(node));

      #ifdef SN_DEBUG
      Sg_File_Info* File = isSgNode(node)->get_file_info();
      printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
      printf("File: (%s, %d, %d) %s = %s\n", File->get_filenameString().c_str(),
                        File->get_line(),
                        File->get_col(),
                        isSgNode(node)->sage_class_name(),
                        isSgNode(node)->unparseToString().c_str());
      #endif

      if(isSgDotExp(node)) {
        simplifyDotExp(isSgDotExp(node));
      }
      else if(isSgArrowExp(node)) {
        simplifyArrowExp(isSgArrowExp(node));
      }
      else {
        // Must be a child node... push to prev stmt...
        pushToLocalVarAndReplace(isSgExpression(node));
      }
    }

  }


  void preProcess (SgProject* proj)
  {
    PPA::project = proj;

    // Push global var inits into main -- This was required since we ended up placing the
    // create_entry function ahead of the main function in handlePointerVars. There is another
    // elegant solution where we could simply put the create_entry before the pointer var decl
    // itself. That makes sure that we put the create_entry before the first use of it, and all uses
    // of create_entry see that forward decl.
    // Earlier:
    // input: int* ptr = NULL;
    //        fn(int* ptr1)
    //        main() {
    //      }
    // output: int* ptr;
    //        fn(int_str ptr1)
    //        struct int_str {create_entry }
    //      create_entry();
    //      assign_and_copy(ptr) {... create_entry}
    //        main() {
    //      assign_and_copy(ptr, NULL);
    //      }
    // Now:
    // output: int* ptr;
    //      struct int_str {}
    //      create_entry();
    //      fn(int_str ptr1)
    //      assign_and_copy(ptr) {... create_entry}
    //      main() {
    //      assign_and_copy(ptr, NULL);
    //      }
    // We could have also solved this problem by simply making
    // sure that we placed the int_str and create_entry before
    // int* ptr rather than before the main() function.. however
    // that would've required us to tweak the handleAssignOverloadOps
    // which would've been messy!

    //RMM FIX
    //convertGlobalUnboundedCharArrays();
    //RMM FIX

    pushGlobalVarInitsToMain();



    // Modify for and while loops to work with our
    // instrumentation
    modifyForAndWhileLoops();

    // Split plus and minus assign ops to plus/minus and
    // then assign ops
    splitPlusMinusAssigns();

    // This will remove all the original expression trees -
    // generated through constant propagation/folding
    SageInterface::removeAllOriginalExpressionTrees(project);

    // Convert arrow exps where lhs is "this" to dot exps...
    convertArrowThisToDot();

    // Make string val to char* conversions (through function call args)
    // explicit
    #ifdef STR_TO_CHARPTR_ARG_CASTS
    functionCallArgCastHandler();
    #endif

    // Move structs defined within functions, out into the class/global scope
    // so that the instrumentation functions can see the definition
    #ifdef EVICT_STRUCTS_FROM_FUNCTIONS
    evictStructsFromFunctions();
    #endif

    #ifdef EXPLICIT_RETURN_STMT_CASTS
    castReturnExps();
    #endif

    #if 0
    #ifdef RETAIN_FUNC_PROTOS
    moveArgsOutOfFuncCalls(project);
    #endif
    #endif


    #ifdef SIMPLIFY_EXPRESSIONS
    TopBotTrack3 TB3;
    Util::nodeType inh3Attr(Util::UNKNOWN, Util::INH_UNKNOWN);
    TB3.traverseInputFiles(project, inh3Attr);

    simplifyNodes();

    NodesToProcess.clear();
    #endif

    //RMM FIX
    eliminateRegisterKeyword();
    modifyCaseOptionStmts();
    #ifdef RMM_MISSING_FUNC_DEFINITION_INSTRUMENTATION
    modifyForeignFunctionCalls();
    #endif
    fixVarRefsInArrayDeclarations();
    castFunctionRefExpressions();
    //RMM FIX
  }
}
