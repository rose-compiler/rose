
#include "broadway.h"
#include "ref_clone_changer.h"

using namespace std;

actionAnn::actionAnn(exprAnn * condition, int line)
  : Ann(line),
    _kind(Inline),
    _condition(condition),
    _replacement(0),
    _candidates()
{}

actionAnn::actionAnn(exprAnn * condition, stmtNode * replacement, int line)
  : Ann(line),
    _kind(Replace),
    _condition(condition),
    _replacement(replacement),
    _candidates()
{}

/** @brief Mini ID lookup walker
 *
 *  This walker is used to look up local declarations inside the
 *  replacement code in Replace actions. Our goal here is to prevent those
 *  variables from being captured by variables in the application. The name
 *  mangler will fix the names later. */

class miniIdLookupWalker : public Walker
{
private:

  Identifiers_table _ids;

public:

  miniIdLookupWalker()
    : Walker(Both, Subtree),
      _ids()
  {}

  // -- Enter and exit scopes

  virtual void at_block(blockNode * the_block, Order ord)
  {
    if (ord == Preorder)
      _ids.enter_scope();

    if (ord == Postorder)
      _ids.exit_scope();
  }

  // -- Store declarations

  virtual void at_decl(declNode * the_decl, Order ord)
  {
    if (ord == Postorder)
      _ids.insert(the_decl->name(), the_decl);
  }

  // -- Lookup identifiers

  virtual void at_id(idNode * the_id, Order ord)
  {
    if (ord == Postorder) {
      declNode * d = _ids.lookup(the_id->name());
      if (d)
	the_id->decl(d);
    }
  }
};


class actionLookupWalker : public Walker
{
private:

  procedureAnn * _procedure;
  Annotations * _annotations;

public:

  actionLookupWalker(procedureAnn * procedure,
		     Annotations * annotations)
    : Walker(Preorder, Subtree),
      _procedure(procedure),
      _annotations(annotations)
  {}

  virtual void at_expr(exprNode * the_expr, Order ord)
  {
    if (the_expr->typ() == Meta) {
      exprPatternNode * the_expr_pattern = (exprPatternNode *) the_expr;
      the_expr_pattern->lookup(_procedure, _annotations);
    }
  }

  virtual void at_stmt(stmtNode * the_stmt, Order ord)
  {
    if (the_stmt->typ() == Meta) {
      stmtPatternNode * the_stmt_pattern = (stmtPatternNode *) the_stmt;
      the_stmt_pattern->lookup(_procedure, _annotations);
    }
  }

  virtual void at_type(typeNode * the_type, Order ord)
  {
    if (the_type->typ() == Meta) {
      typePatternNode * the_type_pattern = (typePatternNode *) the_type;
      the_type_pattern->lookup(_procedure, _annotations);
    }
  }
};  

void actionAnn::lookup(procedureAnn * procedure,
		       Annotations * annotations)
{
  // -- Invoke lookup on the condition

  _condition->lookup((enumPropertyAnn *)0, procedure, annotations);

  // -- If this is a replace action, lookup the meta-variables in the
  // replacement code

  if (_replacement) {

    // -- Look up the local variables

    miniIdLookupWalker idw;
    _replacement->walk(idw);

    // -- Look up the meta variables

    actionLookupWalker slw(procedure, annotations);
    _replacement->walk(slw);

  }
}

void actionAnn::test(callsiteEntry * callsite,
		     procLocation * where,
		     propertyAnalyzer * property_analyzer)
{
  bool old_verbose_val = Annotations::Verbose_properties;
  if (Annotations::Show_actions) {
    Annotations::Verbose_properties = true;
    cout << " -- Action at " << callsite->call()->coord() << " ------" << endl;
    cout << "  + Test condition: ";
    _condition->print(cout);
    cout << endl;
  }
  
  // -- Evaluate the condition

  bool trigger = _condition->test(where, property_analyzer);

  if (Annotations::Show_actions) {
    if (trigger)
      cout << " => true at " << * (where) << endl; 
    else
      cout << " => false at " << * (where) << endl; 

    Annotations::Verbose_properties = false;
  }

  // -- Look up the callsite

  candidate_map_p p = _candidates.find(callsite);
  if (p == _candidates.end()) {

    // -- It doesn't exist, so create a new entry

    _candidates[callsite] = trigger;
  }
  else {

    // -- It does exist; add in this trigger using logical AND

    _candidates[callsite] &= trigger;
  }
}

bool actionAnn::is_applicable(callsiteEntry * callsite)
{
  candidate_map_p p = _candidates.find(callsite);

  if (p == _candidates.end())
    return false;
  else
    return (*p).second;
}

stmtNode * actionAnn::generate_replacement(callsiteEntry * callsite,
					   propertyAnalyzer * analyzer,
                                           Linker &linker,
					   Annotations * annotations)
{
  if (_kind == Replace) {
    if (_replacement) {

      // -- Make a copy of the replacement pattern. Use the
      // ref_clone_changer to preserve the idNode-declNode links created by
      // the miniIdLookupWalker.

      stmtNode * replacement_copy = (stmtNode *) ref_clone_changer::clone(_replacement, false);

      // -- Expand the meta-variables with their bindings

      patternExpander pe(callsite, analyzer, annotations);
      replacement_copy->change(pe);
    
      return replacement_copy;
    }
  }

  if (_kind == Inline) {

    // -- Retrieve the library function, make sure there is one

    operandNode * call = callsite->call();
    procNode * proc = NULL;
    if(call->var()->typ()==Id) {
      declNode *call_decl = ((idNode*)call->var())->decl();
      proc = linker.lookup_procedure(call_decl);
    }

    if ( proc ) {

      funcNode * func = (funcNode *) proc->decl()->type();

      // -- Clone the procedure. Here we use the ref_clone_changer because
      // it preserves the internal references (that is, the local variable
      // declaration pointers). That way we can use the name mangler to fix
      // any conflicting variable names.

      procNode * dup_proc = (procNode *) ref_clone_changer::clone(proc, false);

      // -- Remove the body...this is the return object. BUT, get the exit
      // basic block first.

      basicblockNode * exit = dup_proc->exit();
      blockNode * body = dup_proc->get_body();

      // -- Fix the parameter passing: remove each formal parameter from
      // the funcNode, use the actual argument from the callNode as the
      // initializer, and add it to the block declarations of the body.

      //    Formal arguments...

      funcNode * fd = (funcNode *) dup_proc->decl()->type();
      decl_list & formal_args = fd->args();

      //    Local declarations...

      decl_list & topdecls = body->decls();

      //    Clone the call so we can canibalize the actual arguments (memory leak?)

      operandNode * dup_call = (operandNode *)  ref_clone_changer::clone(call, false);
      operand_list actual_args = callsite->stmt()->arg_list();
      assert( formal_args.size() == actual_args.size() );

      while (! formal_args.empty()) {
	declNode * one_arg = formal_args.back();
	operandNode * one_act = (operandNode*)
                                ref_clone_changer::clone(actual_args.back(),false);

	// -- Use the initialize to pass the actual value

	one_arg->init(one_act);

	// -- Important: set the decl location to BLOCK

	one_arg->decl_location(declNode::BLOCK);

	topdecls.push_front(one_arg);

	formal_args.pop_back();
	actual_args.pop_back();
      }

      // -- Assumption: last statement of the exit basic block is the
      // return statement. This is true after the regular dismantle/CFG
      // phase. Remove it.

      returnNode * ret = (returnNode *) exit->stmts().back();
      exit->stmts().pop_back();

      assert(ret->typ() == Return);

      // -- If there is a returned value and a left-hand side at the
      // callsite, then set up the appropriate assignment.

      if (ret->expr() && callsite->stmt()->lhs()) {

	// -- Get the returned value

	exprNode * returned_expr = ret->get_expr();
        assert(returned_expr->typ() == Id || returned_expr->typ() == Const);

	// -- Copy the left-hand side of the binary node

	operandNode * lhs = (operandNode *)
                            ref_clone_changer::clone(callsite->stmt()->lhs(),
                                                     false);

	// -- Create a new assignment statement and place it at the end of
	// the basic block.

	threeAddrNode * assignment =
          new threeAddrNode(lhs, new operandNode((indexNode*) returned_expr),
                            call->coord());
	exit->stmts().push_back(assignment);
      }

      return body;
    }
  }

  return 0;
}


