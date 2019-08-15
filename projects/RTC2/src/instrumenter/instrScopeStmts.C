#include "instr.h"

namespace Instr {

  void instr(SgBasicBlock* bb) {

    // 1. Add Enter Scope -- This will be removed when RTED ScopeGuard is used
    // 2. Insert lock and key variables at the top of stack.
    // 3. Create a map for lock and key with each scope
    // 4. Insert ExitScope statement if the last statement in the list isn't
    // a return stmt. Return stmts are handled in handleReturnStmts

    SgScopeStatement* scope = isSgScopeStatement(bb);

    // 1. Add Enter Scope
#if 0
    SgExpression* overload = buildOverloadFn("EnterScope", NULL, NULL, SgTypeVoid::createType(), scope,
        GEFD(bb));
#endif
    SgExpression* overload = buildMultArgOverloadFn("EnterScope", SB::buildExprListExp(), SgTypeVoid::createType(), scope,
        GEFD(bb));

    SgStatement* enter_scope = SB::buildExprStatement(overload);
    Util::insertAtTopOfBB(bb, enter_scope);

    // 2. Insert lock and key variables
    // lock calls "getTopLock"
    // key calls "getTopKey"
#if 0
    overload = buildOverloadFn("getTopLock", NULL, NULL, getLockType(), scope,
        GEFD(bb));
#endif
    overload = buildMultArgOverloadFn("getTopLock", SB::buildExprListExp(), getLockType(), scope,
        GEFD(bb));
    // **** IMPORTANT: Using same counter value for lock and key
    SgName lock_name("lock_var" + boost::lexical_cast<std::string>(Util::VarCounter));
    //SgVariableDeclaration* lock_var = Util::createLocalVariable(lock_name, getLockType(), overload, scope);
    // Insert this lock declaration after the EnterScope
    //SI::insertStatementAfter(enter_scope, lock_var); //RMM COMMENTED OUT

    // For the key...
#if 0
    overload = buildOverloadFn("getTopKey", NULL, NULL, getKeyType(), scope,
        GEFD(bb));
#endif
    overload = buildMultArgOverloadFn("getTopKey", SB::buildExprListExp(), getKeyType(), scope,
        GEFD(bb));

    // **** IMPORTANT: Using same counter value for lock and key
    SgName key_name("key_var" + boost::lexical_cast<std::string>(Util::VarCounter++));
    //SgVariableDeclaration* key_var = Util::createLocalVariable(key_name, getKeyType(), overload, scope);
    // Insert this key decl after the lock decl
    //SI::insertStatementAfter(lock_var, key_var); //RMM COMMENTED OUT

    // Add to scope -> lock and key map
    //LockKeyPair lock_key = std::make_pair(lock_var, key_var);
    //SLKM[scope] = lock_key;

    //ROSE_ASSERT(existsInSLKM(scope));

    // 4. Insert ExitScope if last stmt is not return.
    SgStatementPtrList& stmts = bb->get_statements();
    SgStatementPtrList::iterator it = stmts.begin();
    it += (stmts.size() - 1);

    if(!isSgReturnStmt(*it)) {
      // Last statement is not return. So, add exit scope...
      // If its a break/continue statement, insert statement before,
      // otherwise, add exit_scope afterwards.
      //SgExpression* overload = buildOverloadFn("ExitScope", NULL, NULL, SgTypeVoid::createType(), scope, GEFD(bb));
      SgExpression* overload = buildMultArgOverloadFn("ExitScope", SB::buildExprListExp(), SgTypeVoid::createType(), scope, GEFD(bb));
      // check if its break/continue
      if(isSgBreakStmt(*it) || isSgContinueStmt(*it)) {
        SI::insertStatementBefore(*it, SB::buildExprStatement(overload));
      }
      else {
        SI::insertStatementAfter(*it, SB::buildExprStatement(overload));
      }
    }

    return;
  }

  void instr(SgGlobal* global) {

    // Create the lock and key variables in global scope.
    // In main:
    // EnterScope();
    // lock = getTopLock();
    // key = getTopKey();
    // .... rest of main
    // ExitScope();
    // return;
    // FIXME: Add case where we handle arbitrary exits from main
    // This can be handled similar to the way returns are handled
    // for basic blocks.

    SgScopeStatement* scope = isSgScopeStatement(global);

    // Insert lock and key variables at the top of the global scope
    // lock variable
    std::cout << "VarCounter: " << Util::VarCounter << std::endl;
    SgName lock_name("lock_var" + boost::lexical_cast<std::string>(Util::VarCounter));
    SgVariableDeclaration* lock_var = Util::createLocalVariable(lock_name, getLockType(), NULL, scope);
    // Add declaration at the top of the scope
    scope->prepend_statement(lock_var);

    // key variable
    // **** IMPORTANT: Using same counter value for lock and key
    SgName key_name("key_var" + boost::lexical_cast<std::string>(Util::VarCounter));
    Util::VarCounter++;
    SgVariableDeclaration* key_var = Util::createLocalVariable(key_name, getKeyType(), NULL, scope);
    // Insert this key decl after the lock decl
    SI::insertStatementAfter(lock_var, key_var);


    // Now, find the main function and insert...
    // EnterScope();
    // lock = getTopLock();
    // key = getTopKey();
    // .... rest of main
    // ExitScope()
    // return; -- this already exists...
    // see FIXME above

    // find main function...
    SgFunctionDeclaration* MainFn = SI::findMain(global);
    if(!MainFn) {
#ifdef HANDLE_GLOBAL_SCOPE_DEBUG
      printf("Can't find Main function. Not inserting Global Enter and Exit Scopes\n");
#endif
      return;
    }

    SgBasicBlock *bb = Util::getBBForFn(MainFn);

    // insert EnterScope()
#if 0
    SgExpression* overload = buildOverloadFn("EnterScope", NULL, NULL, SgTypeVoid::createType(), scope,
        GEFD(bb));
#endif
    SgExpression* overload = buildMultArgOverloadFn("EnterScope", SB::buildExprListExp(), SgTypeVoid::createType(), scope,
        GEFD(bb));

    SgStatement* enter_scope = SB::buildExprStatement(overload);
    Util::insertAtTopOfBB(bb, enter_scope);

    // insert lock = getTopLock();
    //overload = buildOverloadFn("getTopLock", NULL, NULL, getLockType(), scope, GEFD(bb));
    overload = buildMultArgOverloadFn("getTopLock", SB::buildExprListExp(), getLockType(), scope, GEFD(bb));
    //SgStatement* lock_assign = SB::buildExprStatement(SB::buildAssignOp(SB::buildVarRefExp(lock_var), overload));
    //SI::insertStatementAfter(enter_scope, lock_assign); //RMM COMMENTED OUT

    // insert key = getTopKey();
    // overload = buildOverloadFn("getTopKey", NULL, NULL, getKeyType(), scope, GEFD(bb));
    overload = buildMultArgOverloadFn("getTopKey", SB::buildExprListExp(), getKeyType(), scope, GEFD(bb));
    //SgStatement* key_assign = SB::buildExprStatement(SB::buildAssignOp(SB::buildVarRefExp(key_var), overload));
    //SI::insertStatementAfter(lock_assign, key_assign); //RMM COMMENTED OUT

    // add to scope -> lock and key map... SLKM
    LockKeyPair lock_key = std::make_pair(lock_var, key_var);
    scopeLockMap[scope] = lock_key;

    ROSE_ASSERT(existsInSLKM(scope));

    // Insert ExitScope if last stmt is not return.
    SgStatementPtrList& stmts = bb->get_statements();
    SgStatementPtrList::iterator it = stmts.begin();
    it += (stmts.size() - 1);

    // A little iffy on the scope part here... lets check that.
    if(!isSgReturnStmt(*it)) {
      // Last statement is not return. So, add exit scope...
      // If its a break/continue statement, insert statement before,
      // otherwise, add exit_scope afterwards.
      //SgExpression* overload = buildOverloadFn("ExitScope", NULL, NULL, SgTypeVoid::createType(), scope, GEFD(bb));
      SgExpression* overload = buildMultArgOverloadFn("ExitScope", SB::buildExprListExp(), SgTypeVoid::createType(), scope, GEFD(bb));

      // check if its break/continue
      if(isSgBreakStmt(*it) || isSgContinueStmt(*it)) {
        SI::insertStatementBefore(*it, SB::buildExprStatement(overload));
      }
      else {
        SI::insertStatementAfter(*it, SB::buildExprStatement(overload));
      }
    }

  }

  void instr(SgScopeStatement* scope) {
    if(isSgBasicBlock(scope)) {
      #ifdef LOCK_KEY_INSERT
      instr(isSgBasicBlock(scope));
      #endif
    }
    else if(isSgGlobal(scope)) {
      instr(isSgGlobal(scope));
    }
  }
}
