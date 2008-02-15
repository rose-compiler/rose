/* unparse_stmt_fort.C
 *
 * Code to unparse Sage/Fortran statement nodes.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rose.h>
#include "unparser_opt.h"
#include "unparser_fort.h"

//----------------------------------------------------------------------------
//  Unparser::unparseStatement
//
//  General unparse function for statements. Routes work to the
//  appropriate helper function.
//----------------------------------------------------------------------------
void
UnparserFort::unparseStatement(SgStatement* stmt, SgUnparse_Info& info)
{
  ROSE_ASSERT(stmt != NULL);

  // If line_to_unparse is greater than 0, unparse only the specified
  // line. Otherwise, unparse everything.
  if ((line_to_unparse > 0) 
      && (strcmp(getFileName(stmt), getCurOutFileName()) == 0) 
      && (ROSE::getLineNumber(stmt) != line_to_unparse)) {
    return;
  }
  
  // -------------------------------------------------------
  // Debug output
  // -------------------------------------------------------
  string sgnm = stmt->sage_class_name();
  if (isDebug()) {
    cout << "===== unparseStatement (" << sgnm 
	 << ", 0x" << hex << stmt << dec << ") " << " =====" << endl;
    cout << "file:line: " << getFileName(stmt) 
	 << ":" << ROSE::getLineNumber(stmt) << endl;
  }
#if 0
  cur << "! unparseStatement: sg_class = " << sgnm << "\n";
#endif
  
#if 0
  // the following section is for debugging purposes. Set a breakpoint
  // after the conditional to stop at the line number in the input
  // source file. Modify the value of debugline as needed.
  int debugline = 29;
  if (ROSE::getLineNumber(stmt) == debugline) {
    printDebugInfo("stopped at line ", FALSE);
    printDebugInfo(debugline, TRUE);
  }
#endif

  // -------------------------------------------------------
  // unparse the statement
  // -------------------------------------------------------

  // unparse directives before the current statement
  unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);  

  cur.format(stmt, info, FORMAT_BEFORE_STMT);
  if (repl && repl->unparse_statement(stmt, info, cur)) {
    return;
  }   

  switch (stmt->variantT())
    {
      // scope
    case V_SgGlobal:                 unparseGlobalStmt(stmt, info); break;
    case V_SgScopeStatement:         unparseScopeStmt(stmt, info); break;

      // program units
    case V_SgModuleStatement:          unparseModuleStmt(stmt, info); break;
    case V_SgProgramHeaderStatement:   unparseProgHdrStmt(stmt, info); break;
    case V_SgProcedureHeaderStatement: unparseProcHdrStmt(stmt, info); break;
      
      // declarations
    case V_SgInterfaceStatement:     unparseInterfaceStmt(stmt, info); break;
    case V_SgCommonBlock:            unparseCommonBlock(stmt, info); break;
    case V_SgVariableDeclaration:    unparseVarDeclStmt(stmt, info); break;
    case V_SgVariableDefinition:     unparseVarDefnStmt(stmt, info); break;
    case V_SgParameterStatement:     unparseParamDeclStmt(stmt, info); break;
    case V_SgUseStatement:           unparseUseStmt(stmt, info); break;
      
      // executable statements, control flow
    case V_SgBasicBlock:             unparseBasicBlockStmt(stmt, info); break;
    case V_SgIfStmt:                 unparseIfStmt(stmt, info); break;
    case V_SgFortranDo:              unparseDoStmt(stmt, info); break;
    case V_SgWhileStmt:              unparseWhileStmt(stmt, info); break;
    case V_SgSwitchStatement:        unparseSwitchStmt(stmt, info); break;
    case V_SgCaseOptionStmt:         unparseCaseStmt(stmt, info); break;
    case V_SgDefaultOptionStmt:      unparseDefaultStmt(stmt, info); break;
    case V_SgBreakStmt:              unparseBreakStmt(stmt, info); break;
    case V_SgLabelStatement:         unparseLabelStmt(stmt, info); break;
    case V_SgGotoStatement:          unparseGotoStmt(stmt, info); break;
    case V_SgStopOrPauseStatement:   unparseStopOrPauseStmt(stmt, info); break;
    case V_SgReturnStmt:             unparseReturnStmt(stmt, info); break;

      // executable statements, IO
    case V_SgIOStatement:            unparseIOStmt(stmt, info); break;
    case V_SgIOControlStatement:     unparseIOCtrlStmt(stmt, info); break;

      // executable statements, other
    case V_SgExprStatement:          unparseExprStmt(stmt, info); break;
      
      // pragmas
    case V_SgPragmaDeclaration:      unparsePragmaDeclStmt(stmt, info); break;

    default:
      printf("UnparserFort::unparseStatement: Error: No handler for %s (variant: %d)\n",
	     stmt->sage_class_name(), stmt->variantT());
      ROSE_ASSERT(false);
      break;
    }
  
  cur.format(stmt, info, FORMAT_AFTER_STMT);
  unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);

  
  if (isDebug()) {
    cout << "===== end unparseStatement (" << sgnm 
	 << ", 0x" << hex << stmt << dec << ") " << " =====" << endl;
  }
#if 0
  cur << "\n! End unparseStatement: sg_class = " << sgnm << "\n";
#endif
}


//----------------------------------------------------------------------------
//  UnparserFort::<scope>
//----------------------------------------------------------------------------

void
UnparserFort::unparseGlobalStmt (SgStatement* stmt, SgUnparse_Info& info)
{
  SgGlobal* globalScope = isSgGlobal(stmt);
  ROSE_ASSERT(globalScope != NULL);

  // Setup an iterator to go through all the statements in the top
  // scope of the file.
  SgDeclarationStatementPtrList& stmtList = globalScope->get_declarations();
  SgDeclarationStatementPtrList::iterator stmtIt = stmtList.begin();
  for ( ; stmtIt != stmtList.end(); stmtIt++) {
    SgStatement* curStmt = *stmtIt;
    ROSE_ASSERT(curStmt != NULL);
    
    bool unparseStmt = isStmtFromFile(curStmt, getCurOutFileName());
    if (ROSE_DEBUG > 5) {
      cout << "unparseStmt = " << ((unparseStmt) ? "true" : "false") << endl;
    }
    
    if (unparseStmt) {
      if (ROSE_DEBUG > 3) {
	cout << "In run_unparser(): getLineNumber(curStmt) = "
	     << ROSE::getLineNumber(curStmt) 
	     << " getFileName(curStmt) = " << ROSE::getFileName(curStmt)
	     << " cur_index = " << cur_index
	     << endl;
      }
      unparseStatement(curStmt, info);
    }
  }
}

void 
UnparserFort::unparseScopeStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node is not used in Fortran
  SgScopeStatement* scope_stmt = isSgScopeStatement(stmt);
  ROSE_ASSERT(scope_stmt != NULL);
}


//----------------------------------------------------------------------------
//  UnparserFort::<program units>
//----------------------------------------------------------------------------

void 
UnparserFort::unparseModuleStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran module

  SgModuleStatement* mod = isSgModuleStatement(stmt);
  ROSE_ASSERT(mod);
  
  cur << "MODULE " << mod->get_name().str();
  
  // body
  SgUnparse_Info ninfo(info);
  unparseStatement(mod->get_body(), ninfo);
  
  cur << "END MODULE";  
}

void 
UnparserFort::unparseProgHdrStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran program

  SgProgramHeaderStatement* proghdr = isSgProgramHeaderStatement(stmt);
  ROSE_ASSERT(proghdr);
  
  // 1. program header
  cur << "PROGRAM ";
  
  // FIXME: eventually must support more qualifiers

  // 1b. name
  cur << proghdr->get_name().str();
  
  // 2. automatic statements
  genPUAutomaticStmts(stmt, info);

  // 3. body
  SgUnparse_Info ninfo(info);
  unparseStatement(proghdr->get_body(), ninfo);
  
  // 4. program end
  cur << "END PROGRAM";  
}

void
UnparserFort::unparseProcHdrStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran procedure program unit

  //printf ("Inside of unparseProcHdrStmt() \n");
  SgProcedureHeaderStatement* prochdr = isSgProcedureHeaderStatement(stmt);
  ROSE_ASSERT(prochdr);
  
  SgType* rty = NULL;
  string prockind = "SUBROUTINE";
  
  if (prochdr->isFunction()) {
    rty = prochdr->get_result_type();
    prockind = "FUNCTION";
  }
  
  // 1. procedure header
  
  // 1a. procedure kind, qualifiers
  // do not print out type for functions that return results; mfef90 will complain.
  // unparseTypeForConstExprs unparses the type but instead of unparsing into the
  // more portable and friendly ROSE__TYPES we must use the explicit REAL*4, etc.
  // because mfef90 will otherwise generate the error message '"ROSE_TY_R4" is used 
  // in a constant expression, therefore it must be a constant" if we use a ROSE__TYPE
  // in an interface block.
  // FIXME:  ifort doesn't have this limitation.  In the long term, it would be best 
  // to make mfef90 accept the more abstract notation everywhere.
  SgVariableSymbol* result = prochdr->get_function_result();
  if (!result && rty) {
    unparseTypeForConstExprs(rty, info);
    cur << " ";
  }
  cur << prockind << " ";

  // FIXME: eventually must support more qualifiers
  SgFunctionType* ty = prochdr->get_type();
  // printDeclModifier(funcdecl, ninfo);
  
  // 1b. name
  cur << prochdr->get_name().str();

  // 1c. arguments
  SgUnparse_Info ninfo1(info);
  ninfo1.set_inArgList();
    
  cur << "(";
  SgInitializedNamePtrList& args = prochdr->get_args();
  unparseFuncArgs(&args, ninfo1);
  cur << ") ";
  
  // 1d. result
  if (result) {
    cur << "RESULT(" << result->get_name().str() << ")";
  }

  // 2. automatic statements
  genPUAutomaticStmts(stmt, info);
  
  // 3. body (In INTERFACE statements, the body will not exist.)
  if (prochdr->get_body()) {
    SgUnparse_Info ninfo2(info);
    unparseStatement(prochdr->get_body(), ninfo2);
  }
  
  // 4. procedure end
  cur << "END " << prockind;  
}


//----------------------------------------------------------------------------
//  UnparserFort::<declarations>
//----------------------------------------------------------------------------

void
UnparserFort::unparseInterfaceStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran interface statement
  SgInterfaceStatement* if_stmt = isSgInterfaceStatement(stmt);
  ROSE_ASSERT(if_stmt != NULL);
  
  // don't generate an explicit interface if the body is empty

  if (if_stmt->get_body() != NULL) {

    string nm = if_stmt->get_name().str();
    cur << "INTERFACE " << nm;
 
    // body
    unparseStatement(if_stmt->get_body(), info);
  
    // scope
    SgStatement* scope = if_stmt->get_scope();
    if (scope) {
      unparseStatement(scope, info);
    }
  
    cur << "END INTERFACE";

    // FIXME:eraxxon: format should handle this... but the problem is
    // that INTERFACE blocks do not necessarily contain any regular
    // statements, which means the 'prev node' pointer may still think
    // the last thing we've seen is a program-header node...
    cur.insert_newline(1); 
  }
}

void
UnparserFort::unparseCommonBlock(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran common block
  SgCommonBlock* comblk = isSgCommonBlock(stmt);
  ROSE_ASSERT(comblk != NULL);

  string nm = comblk->get_name().str();
  SgInitializedNamePtrList& vars = comblk->get_variables();
  
  cur << "COMMON /" << nm << "/ ";
  unparseInitNamePtrList(&vars, info);
}

void
UnparserFort::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran variable declaration
  
  SgVariableDeclaration* vardecl = isSgVariableDeclaration(stmt);
  ROSE_ASSERT(vardecl != NULL);
  
  // In Fortran we should never have to deal with a type declaration
  // inside a variable declaration (e.g. struct A { int x; } a;)
  ROSE_ASSERT(!vardecl->get_variableDeclarationContainsBaseTypeDefiningDeclaration());  
  
  // Build a new SgUnparse_Info object to represent formatting options
  // for this statement
  SgUnparse_Info ninfo(info);

  // FIXME: we may need to do something analagous for modules?
  // Check to see if this is an object defined within a class
  int inClass = false;
  SgName inCname;
  ROSE_ASSERT(vardecl->get_parent());
  SgClassDefinition *cdefn = isSgClassDefinition(vardecl->get_parent());
  if (cdefn) {
    inClass = true;
    inCname = cdefn->get_declaration()->get_name();
    if (cdefn->get_declaration()->get_class_type() 
	== SgClassDeclaration::e_class)
      ninfo.set_CheckAccess();
  }
  
  printAccessModifier(vardecl, ninfo); 
  
  // Save the input information
  SgUnparse_Info saved_ninfo(ninfo);

  // Setup the SgUnparse_Info object for this statement
  ninfo.unset_CheckAccess();
  info.set_access_attribute(ninfo.get_access_attribute());
  
  SgInitializedNamePtrList::iterator p = vardecl->get_variables().begin();
  while (p != vardecl->get_variables().end()) {
    SgInitializedName* decl_item = *p;
    
    printStorageModifier(vardecl, saved_ninfo);
    unparseVarDecl(vardecl, decl_item, ninfo);
    
    p++;
    
    if (p != vardecl->get_variables().end()) {
      ROSE_ASSERT(false && "Unimplemented");
      if (!ninfo.inArgList())
	ninfo.set_SkipBaseType();
      cur << ",";
    }
  }
}

void
UnparserFort::unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node has no Fortran correspondence
  SgVariableDefinition* vardefn_stmt = isSgVariableDefinition(stmt);
  ROSE_ASSERT(vardefn_stmt != NULL);
  ROSE_ASSERT(false && "UnparserFort::unparseVarDefnStmt");
}

void
UnparserFort::unparseParamDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran parameter declaration
  ROSE_ASSERT(false && "UnparserFort::unparseParamDeclStmt");
}

void
UnparserFort::unparseUseStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran use statement
  
  SgUseStatement* useStmt = isSgUseStatement(stmt);
  ROSE_ASSERT (useStmt != NULL);
  
  SgExprListExp* u_rename = useStmt->get_rename_list();
  SgUseOnlyExpression* u_only = useStmt->get_use_only();
  
  cur << "USE " << useStmt->get_name().str();
  if (u_rename) {
    cur << ",";
    unparseExprList(u_rename, info, false /*paren*/);
  }
  else if (u_only) {
    unparseUseOnly(u_only, info);
  }
}


//----------------------------------------------------------------------------
//  UnparserFort::<executable statements, control flow>
//----------------------------------------------------------------------------

void
UnparserFort::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
  ROSE_ASSERT(basic_stmt != NULL);
  
  cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
  cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
  
  SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
  for ( ; p != basic_stmt->get_statements().end(); ++p) { 
    //cout << "stmt: " << hex << (*p) << dec << endl;
    ROSE_ASSERT((*p) != NULL);
    unparseStatement((*p), info);
  }
  
  cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);
  cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
}

void 
UnparserFort::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran 'if'
  //
  // Assume: If nodes always have a true and false body which are
  // possibly empty basic block nodes.
  
  SgIfStmt* if_stmt = isSgIfStmt(stmt);
  ROSE_ASSERT(if_stmt != NULL);
  ROSE_ASSERT(if_stmt->get_conditional());
  
  // condition
  cur << "IF (";
  info.set_inConditional();
  unparseStatement(if_stmt->get_conditional(), info);
  info.unset_inConditional();
  cur << ") THEN";
  
  // true body
  ROSE_ASSERT(if_stmt->get_true_body());
  unparseStatement(if_stmt->get_true_body(), info);
  
  // false body: unparse only if non-empty basic block
  SgBasicBlock* fbb = isSgBasicBlock(if_stmt->get_false_body());
  if (fbb && fbb->get_statements().size() > 0) {
    cur << "ELSE";
    cur.set_prev(if_stmt); // reset formatting context info
    unparseStatement(if_stmt->get_false_body(), info);
  }
  
  cur << "END IF";
}

void 
UnparserFort::unparseDoStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran 'do'
    
  SgFortranDo* doloop = isSgFortranDo(stmt);
  ROSE_ASSERT(doloop != NULL);

  // NOTE: for now we are responsible for unparsing the
  // initialization, condition and update expressions into a triplet.
  // We assume that these statements are of a very restricted form.
  SgExpression* initExp = 
    isSgExprStatement(doloop->get_initialization())->get_the_expr();
  SgAssignOp* init = isSgAssignOp(initExp);
  ROSE_ASSERT(init);

  SgExpression* condExp = 
    isSgExprStatement(doloop->get_condition())->get_the_expr();
  SgBinaryOp* cond = (isSgLessOrEqualOp(condExp) ? 
		      isSgBinaryOp(isSgLessOrEqualOp(condExp)) : 
		      isSgBinaryOp(isSgGreaterOrEqualOp(condExp)));
  ROSE_ASSERT(cond);
  
  SgExpression* updateExp = 
    isSgExprStatement(doloop->get_increment())->get_the_expr();
  SgAssignOp* update = isSgAssignOp(updateExp);
  ROSE_ASSERT(update);

  SgBinaryOp* update1 = (isSgAddOp(update->get_rhs_operand()) ? 
			 isSgBinaryOp(isSgAddOp(update->get_rhs_operand())) : 
			 isSgBinaryOp(isSgSubtractOp(update->get_rhs_operand())));
  ROSE_ASSERT(update1);


  // induction var: (i = lb)
  SgVarRefExp* inducVar = isSgVarRefExp(init->get_lhs_operand());
  ROSE_ASSERT(inducVar);
  
  // lower bound: (i = lb)
  SgExpression* lb = init->get_rhs_operand();

  // upper bound: (i <= ub) or (i >= ub)
  SgExpression* ub = cond->get_rhs_operand();

  // step: (i = i +/- step)
  SgExpression* step = update1->get_rhs_operand();
  
  cur << "DO ";

  unparseExpression(inducVar, info);
  cur << " = ";
  unparseExpression(lb, info);
  cur << ", ";
  unparseExpression(ub, info);
  cur << ", ";
  unparseExpression(step, info);
  
  // loop body (must always exist)
  SgStatement *body = doloop->get_body();
  unparseStatement(body, info);
  
  cur << "END DO";
}

void 
UnparserFort::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info) 
{
  // Sage node corresponds to Fortran 'do while' (pre-test)
  
  SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
  ROSE_ASSERT(while_stmt != NULL);

  // conditional
  cur << "DO WHILE " << "(";
  info.set_inConditional(); // prevent printing line and file info
  unparseStatement(while_stmt->get_condition(), info);
  info.unset_inConditional();
  cur << ")";
  
  // loop body (must always exist)
  unparseStatement(while_stmt->get_body(), info);
  
  cur << "END DO";
}

void
UnparserFort::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran 'select'
  SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt);
  ROSE_ASSERT(switch_stmt != NULL);

  cur << "SELECT CASE(";
  unparseExpression(switch_stmt->get_item_selector(), info);
  cur << ")";
  
  if (switch_stmt->get_body()) {
    unparseStatement(switch_stmt->get_body(), info);
  }
  
  cur << "END SELECT";
}

void
UnparserFort::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran 'case'
  SgCaseOptionStmt* case_stmt = isSgCaseOptionStmt(stmt);
  ROSE_ASSERT(case_stmt != NULL);

  cur << "CASE (";
  unparseExpression(case_stmt->get_key(), info);
  cur << ")";
  
  if (case_stmt->get_body()) {
    unparseStatement(case_stmt->get_body(), info);
  }
}

void 
UnparserFort::unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran 'case default'
  SgDefaultOptionStmt* default_stmt = isSgDefaultOptionStmt(stmt);
  ROSE_ASSERT(default_stmt != NULL);
  
  cur << "CASE DEFAULT";
  if (default_stmt->get_body()) {
    unparseStatement(default_stmt->get_body(), info);
  }
}

void 
UnparserFort::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran 'exit'
  SgBreakStmt* break_stmt = isSgBreakStmt(stmt);
  ROSE_ASSERT(break_stmt != NULL);
  cur << "EXIT";
}

void 
UnparserFort::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info) 
{
  // Sage node corresponds to Fortran 'label continue'
  SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
  ROSE_ASSERT(label_stmt != NULL);

  cur << label_stmt->get_label().str() << " CONTINUE";
}

void
UnparserFort::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info) 
{
  // Sage node corresponds to Fortran 'goto'
  SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
  ROSE_ASSERT(goto_stmt != NULL);
  cur << "GOTO " << goto_stmt->get_label()->get_label().str();
}

void
UnparserFort::unparseStopOrPauseStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran 'stop' or 'pause'
  SgStopOrPauseStatement* sp_stmt = isSgStopOrPauseStatement(stmt);
  ROSE_ASSERT(sp_stmt != NULL);
  
  SgStopOrPauseStatement::stop_or_pause knd = (SgStopOrPauseStatement::stop_or_pause)sp_stmt->get_stmt_kind();
  
  if (knd == SgStopOrPauseStatement::stop) {
    cur << "STOP " << sp_stmt->get_code().str();
  }
  else {
    cur << "PAUSE";
  }
}

void
UnparserFort::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran 'return'
  SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
  ROSE_ASSERT(return_stmt != NULL);
  
  cur << "RETURN";

  // The expression can only be a scalar integer for an alternate return
  SgExpression* altret = return_stmt->get_return_expr();
  if (altret) {
    ROSE_ASSERT(isSgValueExp(altret));
    cur << " ";
    unparseExpression(altret, info);
  }
}


//----------------------------------------------------------------------------
//  void UnparserFort::<executable statements, IO>
//----------------------------------------------------------------------------

void 
UnparserFort::unparseIOStmt(SgStatement* stmt, SgUnparse_Info& info) 
{
  // Sage node corresponds to Fortran input/output statement
  SgIOStatement* io_stmt = isSgIOStatement(stmt);
  ROSE_ASSERT(io_stmt != NULL);

  SgIOStatement::io_stmts iofn = (SgIOStatement::io_stmts)io_stmt->get_io_function();
  SgIOControlStatement* ioctrl = io_stmt->get_io_control();
  SgExprListExp* iolist = io_stmt->get_io_stmt_list();

  const char* iofn_nm = NULL;
  switch (iofn) {
    case SgIOStatement::print:     iofn_nm = "PRINT"; break;
    case SgIOStatement::read:      iofn_nm = "READ"; break;
    case SgIOStatement::write:     iofn_nm = "WRITE"; break;
    case SgIOStatement::open:      iofn_nm = "OPEN"; break;
    case SgIOStatement::close:     iofn_nm = "CLOSE"; break;
    case SgIOStatement::rewind:    iofn_nm = "REWIND"; break;
    case SgIOStatement::backspace: iofn_nm = "BACKSPACE"; break;
    case SgIOStatement::inquire:   iofn_nm = "INQUIRE"; break;
    default: ROSE_ASSERT(false && "Invalid value for get_io_function()");
  }
  cur << iofn_nm;

  unparseIOCtrlStmt(ioctrl, info);
  if (iolist) {
    cur << " ";
    unparseExprList(iolist, info, false /*paren*/);
  }
}


void 
UnparserFort::unparseIOCtrlStmt(SgStatement* stmt, SgUnparse_Info& info) 
{
  // Sage node corresponds to Fortran IO control info
  SgIOControlStatement* ioctrl_stmt = isSgIOControlStatement(stmt);
  ROSE_ASSERT(ioctrl_stmt != NULL);

  SgExpression* unit = ioctrl_stmt->get_unit();
  SgExpression* err_lbl = ioctrl_stmt->get_err_label();
  SgExpression* iostat = ioctrl_stmt->get_iostat();

  cur << "(UNIT=";  
  unparseExpression(unit, info);
  
  if (err_lbl) {
    cur << ", ERR=";
    unparseExpression(err_lbl, info);
  }
  if (iostat) {
    cur << ", IOSTAT=";
    unparseExpression(iostat, info);
  }

  if (isSgInputOutputStatement(ioctrl_stmt)) {
    unparseInOutStmt(ioctrl_stmt, info);
  }

  cur << ")";
}


void 
UnparserFort::unparseInOutStmt(SgStatement* stmt, SgUnparse_Info& info) 
{
  // Sage node corresponds to Fortran input/output statement
  SgInputOutputStatement* io_stmt = isSgInputOutputStatement(stmt);
  ROSE_ASSERT(io_stmt != NULL);

  SgVarRefExp* fmt = io_stmt->get_format();
  SgVarRefExp* nmlst = io_stmt->get_namelist_nm();
  SgExpression* adv = io_stmt->get_advance();
  SgExpression* end_lbl = io_stmt->get_end_label();
  SgExpression* eor_lbl = io_stmt->get_eor_label();
  SgExpression* rec = io_stmt->get_rec();
  SgExpression* sz = io_stmt->get_size();

  if (fmt) {
    cur << ", FMT=\"";
    unparseExpression(fmt, info);
    cur << "\"";
  }
  if (nmlst) {
    cur << ", NML=";
    unparseExpression(nmlst, info);
  }
  if (adv) {
    cur << ", ADVANCE=";
    unparseExpression(adv, info);
  }
  if (end_lbl) {
    cur << ", END=";
    unparseExpression(end_lbl, info);
  }
  if (eor_lbl) {
    cur << ", EOR=";
    unparseExpression(eor_lbl, info);
  }
  if (rec) {
    cur << ", REC=";
    unparseExpression(rec, info);
  }
  if (sz) {
    cur << ", SIZE=";
    unparseExpression(sz, info);
  }
}


//----------------------------------------------------------------------------
//  void UnparserFort::<executable statements, other>
//----------------------------------------------------------------------------

void 
UnparserFort::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info) 
{
  // Sage node corresponds to Fortran expression
  SgExprStatement* expr_stmt = isSgExprStatement(stmt);
  ROSE_ASSERT(expr_stmt != NULL);
  ROSE_ASSERT(expr_stmt->get_the_expr());
  
  SgUnparse_Info ninfo(info);
  
  // Never unparse class definition in expression stmt
  ninfo.set_SkipClassDefinition();

  printDebugInfo(getSgVariant(expr_stmt->get_the_expr()->variant()), TRUE); 
  unparseExpression(expr_stmt->get_the_expr(), ninfo);

  if (ninfo.inVarDecl()) {
    cur << ",";
  }
}


//----------------------------------------------------------------------------
//  UnparserFort::<pragmas>
//----------------------------------------------------------------------------

void
UnparserFort::unparsePragmaDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran convention !pragma
  SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(stmt);
  ROSE_ASSERT(pragmaDeclaration != NULL);
  
  SgPragma* pragma = pragmaDeclaration->get_pragma();
  ROSE_ASSERT(pragma != NULL);
  
  const char* txt = (pragma->get_pragma()) ? pragma->get_pragma() : "";
  cur << "!pragma " << txt;
}


//----------------------------------------------------------------------------
//  UnparserFort::unparseAttachedPreprocessingInfo
//----------------------------------------------------------------------------

void
UnparserFort::unparseAttachedPreprocessingInfo(SgStatement* stmt,
					       SgUnparse_Info& info,
					       PreprocessingInfo::RelativePositionType whereToUnparse)
{
  AttachedPreprocessingInfoType *ppInfo = stmt->getAttachedPreprocessingInfo();
  if (!ppInfo) {
    return;
  }

  // Continue only if options indicate
  if (info.SkipComments()) {
    return;
  }
  
  // Traverse the container of PreprocessingInfo objects, unparsing if
  // necessary.
  AttachedPreprocessingInfoType::iterator i;
  for (i = ppInfo->begin(); i != ppInfo->end(); ++i) {
    // Assert that i points to a valid preprocssingInfo object
    ROSE_ASSERT ((*i) != NULL);
    ROSE_ASSERT ((*i)->getTypeOfDirective()  != PreprocessingInfo::CpreprocessorUnknownDeclaration);
    ROSE_ASSERT ((*i)->getRelativePosition() == PreprocessingInfo::before || 
		 (*i)->getRelativePosition() == PreprocessingInfo::after);
    
    // Check and see if the statement should be printed.
    if ((*i)->getRelativePosition() == whereToUnparse) {
      cur.format(stmt, info, FORMAT_BEFORE_DIRECTIVE);
      
      switch ((*i)->getTypeOfDirective()) {
	// Comments don't have to be further commented
      case PreprocessingInfo::C_StyleComment:
      case PreprocessingInfo::CplusplusStyleComment:
	if ( !info.SkipComments() ) {
	  cur << "! " << (*i)->getString();
	}
	break;
	
      default:
	printf ("Error: UnparserFort::unparseAttachedPreprocessingInfo(): default switch reached\n");
	ROSE_ABORT();
      }
      
      cur.format(stmt, info, FORMAT_AFTER_DIRECTIVE);      
    }
  }
}


//----------------------------------------------------------------------------
//  Program unit helpers
//----------------------------------------------------------------------------

void
UnparserFort::genPUAutomaticStmts(SgStatement* stmt, SgUnparse_Info& info)
{
  // For formatting purposes, pretend we have a small basic block
  cur.format(stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
  cur << "USE ROSE__TYPES";

  cur.format(stmt, info, FORMAT_BEFORE_STMT);
  //cur << "IMPLICIT NONE";

  cur.format(stmt, info, FORMAT_AFTER_BASIC_BLOCK1);
}

void
UnparserFort::unparseFuncArgs(SgInitializedNamePtrList* args, 
			      SgUnparse_Info& info)
{
  unparseInitNamePtrList(args, info);
}

void
UnparserFort::unparseInitNamePtrList(SgInitializedNamePtrList* args, 
				     SgUnparse_Info& info)
{
  SgInitializedNamePtrList::iterator it = args->begin();
  while (it != args->end()) {
    SgInitializedName* arg = *it;
    cur << arg->get_name().str();
    
    // Move to the next argument
    it++;
    
    // Check if this is the last argument (output a "," separator if not)
    if (it != args->end()) {
      cur << ", ";
    }
  }
}

//----------------------------------------------------------------------------
//  Declarations helpers
//----------------------------------------------------------------------------

void
UnparserFort::unparseVarDecl(SgStatement* stmt, 
			     SgInitializedName* initializedName,
			     SgUnparse_Info& info)
{
  // General format:
  //   <type> <attributes> :: <variable>
  
  SgName name = initializedName->get_name();
  SgType* type = initializedName->get_type();
  SgInitializer* init = initializedName->get_initializer();  
  ROSE_ASSERT(name.str());
  ROSE_ASSERT(type);
  
  // FIXME: eventually we will probably use this
  SgStorageModifier& storage = initializedName->get_storageModifier();
  
  unparseType(type, info);  
  cur << " :: ";
  cur << name.str();
  
  // Unparse the initializers if any exist
  if (init) {
    cur << " = ";
    unparseExpression(init, info);
  }
}

//----------------------------------------------------------------------------
//  void Unparser::printDeclModifier
//  void Unparser::printAccessModifier   
//  void Unparser::printStorageModifier
//  
//  The following 2 functions: printAccessModifier and printStorageModifier,
//  are just the two halves from printDeclModifier. These two functions
//  are used in the unparse functions for SgMemberFunctionDeclarations
//  and SgVariableDeclaration.  printAccessModifier is first called before
//  the format function. If "private", "protected", or "public" is to
//  be printed out, it does so here. Then I format which will put me
//  in position to unparse the declaration. Then I call
//  printSpecifer2, which will print out any keywords if the option is
//  turned on.  Then the declaration is printed in the same line. If I
//  didnt do this, the printing of keywords would be done before
//  formatting, and would put the declaration on another line (and
//  would look terribly formatted).
//----------------------------------------------------------------------------

void
UnparserFort::printDeclModifier(SgDeclarationStatement* decl_stmt, SgUnparse_Info & info)
{
  printAccessModifier(decl_stmt, info);
  printStorageModifier(decl_stmt, info);
}

void
UnparserFort::printAccessModifier(SgDeclarationStatement * decl_stmt, SgUnparse_Info & info)
{
  // FIXME: this will look different for full-featured Fortran
  
  if (info.CheckAccess()) {
    ROSE_ASSERT (decl_stmt != NULL);
    bool flag = false;
    if (info.isPrivateAccess()) {
      if (!decl_stmt->get_declarationModifier().get_accessModifier().isPrivate())
	flag = true;
    }
    else {
      if (info.isProtectedAccess()) {
	if (!decl_stmt->get_declarationModifier().get_accessModifier().isProtected())
	  flag = true;
      }
      else {
	if (info.isPublicAccess()) {
	  if (!decl_stmt->get_declarationModifier().get_accessModifier().isPublic())
	    flag = true;
	}
	else
	  flag = true;
      }
    }
    
    info.set_isUnsetAccess();
    
    if (decl_stmt->get_declarationModifier().get_accessModifier().isPrivate()) {
      info.set_isPrivateAccess();
      if (flag) {
	cur << "private: ";
      }
    }
    else {
      if (decl_stmt->get_declarationModifier().get_accessModifier().isProtected()) {
	info.set_isProtectedAccess();
	if (flag) {
	  cur << "protected: ";
	}
      }
      else {
	/* default, always print Public */
	ROSE_ASSERT (decl_stmt->get_declarationModifier().get_accessModifier().isPublic() == true);
	info.set_isPublicAccess();
	if (flag) {
	  cur << "public: ";
	}
      }
    }
  }
}

void
UnparserFort::printStorageModifier(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info) 
{
  // FIXME: this will look different for full-featured Fortran
  
  if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && decl_stmt->get_linkage()) 
    {
      cur << "extern \"" << decl_stmt->get_linkage() << "\" ";
      if (decl_stmt->isExternBrace()) {
	cur << "{ ";
      }
    }

  if (decl_stmt->get_declarationModifier().isFriend()) {
    cur << "friend ";
  }
  
  SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(decl_stmt);
  if (functionDeclaration != NULL) {
    if (functionDeclaration->get_functionModifier().isVirtual()) {
      cur << "virtual ";
    }

    if (functionDeclaration->get_functionModifier().isInline()) {
      cur << "inline ";
    }

    if ((!info.SkipFunctionDefinition()) && functionDeclaration->get_functionModifier().isExplicit()) {
      cur << "explicit ";
    }
  }

  if (decl_stmt->get_declarationModifier().get_storageModifier().isStatic()) {
    cur << "static ";
  }

  if (decl_stmt->get_declarationModifier().get_storageModifier().isExtern() && !decl_stmt->get_linkage()) {
    cur << "extern ";
  }

  if (opt.get_auto_opt()) {    // checks option status before printing auto
    if (decl_stmt->get_declarationModifier().get_storageModifier().isAuto()) {
      cur << "auto ";
    }
  }
  
  if (decl_stmt->get_declarationModifier().get_storageModifier().isRegister()) {
    cur << "register ";
  }

  if (decl_stmt->get_declarationModifier().get_storageModifier().isMutable()) {
    cur << "mutable ";
  }
}


