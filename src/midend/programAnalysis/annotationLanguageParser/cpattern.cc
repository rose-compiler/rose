
#include "broadway.h"

using namespace std;

// ----------------------------------------------------------------------
//  Expression pattern node
// ----------------------------------------------------------------------

/** @brief Create a new pattern node */

exprPatternNode::exprPatternNode(const parserID * var)
  : exprNode(Meta, 0, Coord(var->line(),0,-1)),
    _name(var->name()),
    _variable(0)
{}

/** @brief Lookup annotation variable */

void exprPatternNode::lookup(procedureAnn * procedure,
			     Annotations * annotations)
{
  _variable = procedure->lookup(name(), false);
  if ( ! _variable )
    annotations->Error(coord().line(), string("Undeclared identifier \"") + name() + "\"");
}

void exprPatternNode::visit(Visitor * the_visitor)
{
  the_visitor->at_expr(this);
}

void exprPatternNode::walk(Walker & the_walker)
{
  Walker::Order ord = the_walker.order(); 

  if (ord == Walker::Preorder || ord == Walker::Both)
    the_walker.at_expr(this, Walker::Preorder);

  if (the_walker.depth() == Walker::Subtree) {
    // -- Visit the children 

    if (type())
      type()->walk(the_walker);
  }

  if (ord == Walker::Postorder || ord == Walker::Both)
    the_walker.at_expr(this, Walker::Postorder);
}

Node * exprPatternNode::change(Changer & the_changer, bool redispatch)
{
  Changer::Order ord = the_changer.order(); 
  exprPatternNode * the_exprpattern = this;

  if ((ord == Changer::Preorder || ord == Changer::Both) && ! redispatch)
    the_exprpattern = (exprPatternNode *) the_changer.at_expr(the_exprpattern, Changer::Preorder);

  if (the_exprpattern) {

    if (the_exprpattern != this)
      return the_exprpattern->change(the_changer, true);

    typeNode * old_type = the_exprpattern->type();
    if (old_type) {
      typeNode * new_type = (typeNode *) old_type->change(the_changer);
      if (old_type != new_type) {
	//if (the_changer.delete_old())
	  //delete old_type;
        the_exprpattern->type(new_type);
      }
    }

  }

  if ((ord == Changer::Postorder || ord == Changer::Both) && ! redispatch)
    the_exprpattern = (exprPatternNode *) the_changer.at_expr(the_exprpattern, Changer::Postorder);

  return the_exprpattern;
}

void exprPatternNode::output_expr(output_context & ct, Node * par, int prec, Assoc assoc)
{
}

// ----------------------------------------------------------------------
//  Statement pattern node
// ----------------------------------------------------------------------

/** @brief Create a new pattern node */

stmtPatternNode::stmtPatternNode(const parserID * var)
  : stmtNode(Meta, Coord(var->line(),0,-1)),
    _name(var->name()),
    _variable(0)
{}

/** @brief Lookup annotation variable */

void stmtPatternNode::lookup(procedureAnn * procedure,
			     Annotations * annotations)
{
  _variable = procedure->lookup(name(), false);
  if ( ! _variable )
    annotations->Error(coord().line(), string("Undeclared identifier \"") + name() + "\"");
}

void stmtPatternNode::visit(Visitor * the_visitor)
{
  the_visitor->at_stmt(this);
}

void stmtPatternNode::walk(Walker & the_walker)
{
  Walker::Order ord = the_walker.order(); 

  if (ord == Walker::Preorder || ord == Walker::Both)
    the_walker.at_stmt(this, Walker::Preorder);

  if (the_walker.depth() == Walker::Subtree) {
    // -- Visit the children 

    if (type())
      type()->walk(the_walker);
  }

  if (ord == Walker::Postorder || ord == Walker::Both)
    the_walker.at_stmt(this, Walker::Postorder);
}

Node * stmtPatternNode::change(Changer & the_changer, bool redispatch)
{
  Changer::Order ord = the_changer.order(); 
  stmtPatternNode * the_stmtpattern = this;

  if ((ord == Changer::Preorder || ord == Changer::Both) && ! redispatch)
    the_stmtpattern = (stmtPatternNode *) the_changer.at_stmt(the_stmtpattern, Changer::Preorder);

  if (the_stmtpattern) {

    if (the_stmtpattern != this)
      return the_stmtpattern->change(the_changer, true);

  }

  if ((ord == Changer::Postorder || ord == Changer::Both) && ! redispatch)
    the_stmtpattern = (stmtPatternNode *) the_changer.at_stmt(the_stmtpattern, Changer::Postorder);

  return the_stmtpattern;
}


void stmtPatternNode::output_stmt(output_context & ct, Node * par)
{

}

// ----------------------------------------------------------------------
//  Type pattern node
// ----------------------------------------------------------------------

typePatternNode::typePatternNode(const parserID * var)
  : typeNode(Meta, NONE, (typeNode *)0, Coord(var->line(),0,-1)),
    _name(var->name()),
    _variable(0)
{}

void typePatternNode::lookup(procedureAnn * procedure,
			     Annotations * annotations)
{
  _variable = procedure->lookup(name(), false);
  if ( ! _variable )
    annotations->Error(coord().line(), string("Undeclared identifier \"") + name() + "\"");
}

void typePatternNode::visit(Visitor * the_visitor)
{
  the_visitor->at_type(this);
}

void typePatternNode::walk(Walker & the_walker)
{
  Walker::Order ord = the_walker.order(); 

  if (ord == Walker::Preorder || ord == Walker::Both)
    the_walker.at_type(this, Walker::Preorder);

  if (the_walker.depth() == Walker::Subtree) {
    // -- Visit the children 

    if (type())
      type()->walk(the_walker);
  }

  if (ord == Walker::Postorder || ord == Walker::Both)
    the_walker.at_type(this, Walker::Postorder);
}

Node * typePatternNode::change(Changer & the_changer, bool redispatch)
{
  Changer::Order ord = the_changer.order(); 
  typePatternNode * the_typepattern = this;

  if ((ord == Changer::Preorder || ord == Changer::Both) && ! redispatch)
    the_typepattern = (typePatternNode *) the_changer.at_type(the_typepattern, Changer::Preorder);

  if (the_typepattern) {

    if (the_typepattern != this)
      return the_typepattern->change(the_changer, true);

    typeNode * old_type = the_typepattern->type();
    if (old_type) {
      typeNode * new_type = (typeNode *) old_type->change(the_changer);
      if (old_type != new_type) {
	//if (the_changer.delete_old())
	  //delete old_type;
        the_typepattern->type(new_type);
      }
    }

  }

  if ((ord == Changer::Postorder || ord == Changer::Both) && ! redispatch)
    the_typepattern = (typePatternNode *) the_changer.at_type(the_typepattern, Changer::Postorder);

  return the_typepattern;
}

void typePatternNode::output_type(output_context & ct, Node * par,
				  Assoc context, Type_qualifiers q)
{

}

// ----------------------------------------------------------------------
//  Pattern expander
// ----------------------------------------------------------------------

patternExpander::patternExpander(callsiteEntry * callsite,
				 propertyAnalyzer * analyzer,
				 Annotations * annotations)
  : Changer(Postorder, Subtree, true),
    _callsite(callsite),
    _analyzer(analyzer),
    _annotations(annotations)
{}

Node * patternExpander::at_expr(exprNode * the_expr, Order ord)
{
  ostringstream ost;

  // -- Make sure it's a pattern

  if (the_expr->typ() == Meta) {
    exprPatternNode * the_expr_pattern = (exprPatternNode *) the_expr;

    // -- Get the variable from the annotations

    annVariable * var = the_expr_pattern->variable();

    // -- Special case: the "return" variable will expand to the left-hand
    // side of the function call.

    if (var->name() == "return") {

      // -- Make sure we have a LHS

      threeAddrNode * stmt = (threeAddrNode *) _callsite->stmt();
      assert(stmt->typ() == ThreeAddr);

      if (stmt->lhs() && stmt->op() && stmt->op()->id() == Operator::FUNC_CALL)
        // -- We have a left-hand side, so clone it

        return ref_clone_changer::clone(stmt->lhs(), false);

      // -- Fall through: no left-hand side, so issue an error

      ost << "Expansion of $return failed: no left-hand side at the callsite " 
	  << _callsite->call()->coord();
      _annotations->Error(the_expr_pattern->coord().line(), ost.str());

      return the_expr;
    }

    // -- Look up it's binding at this callsite. TBD: it should be the same
    // binding in all the contexts. Right now, we'll arbitrary choose the
    // first one.

    procLocation * where = * (_callsite->contexts().begin());

    // -- Get the binding

    pointerValue & var_binding = var->get_binding(where);
    
    // -- Generate the expansion. This is another weird area because the
    // variable could be bound to multiple objects, which would make it
    // impossible to expand properly. Right now, issue an error.

    if (var_binding.blocks.size() > 1) {
      ost << "Improper binding for replacement of \"" << var->name() << "\" at " <<
	_callsite->call()->coord();
      _annotations->Error(the_expr_pattern->coord().line(), ost.str());
      return the_expr_pattern;
    }

    // -- Special case: if the variable is a formal parameter, and it has
    // no binding, then assume it's a constant.

    if (var->is_formal() && (var_binding.blocks.size() == 0)) {

      // -- Look up the value in this context

      const constant * val = _analyzer->lookup_constant(var, where);
      constant temp = *val;

      // -- Return a constNode

      return new constNode(temp, "", _callsite->call()->coord());
    }

    // -- Regular variable substitution

    memoryBlock * mb = * (var_binding.blocks.begin());

    // -- Special case: the variable is bound to the special null
    // object.

    /*
    if (mb == _analyzer->null())
      return new constNode(constant(0), "0", _callsite->call()->coord());
    */

    // -- The expansion is just a new idNode generated from the declNode
    // associated with the given memoryBlock.
	
    return new idNode(mb->decl(), _callsite->call()->coord());
  }

  return the_expr;
}

Node * patternExpander::at_stmt(stmtNode * the_stmt, Order ord)
{
  return the_stmt;
}

Node * patternExpander::at_type(typeNode * the_type, Order ord)
{
  ostringstream ost;

  // -- Make sure it's a pattern

  if (the_type->typ() == Meta) {
    typePatternNode * the_type_pattern = (typePatternNode *) the_type;
 
    // -- Get the variable from the annotations

    annVariable * var = the_type_pattern->variable();

    // -- Special case: the "return" variable will expand to the left-hand
    // side of the function call.

    if (var->name() == "return") {

      // -- Make sure we have a LHS

      threeAddrNode * stmt = (threeAddrNode *) _callsite->stmt();
      assert(stmt->typ() == ThreeAddr);

      if(stmt->lhs() && stmt->op() && stmt->op()->id()==Operator::FUNC_CALL &&
         stmt->lhs()->type())
        // -- We have a left-hand side, check for a type
        return ref_clone_changer::clone(stmt->lhs()->type(), false);

      // -- Fall through: no left-hand side, so issue an error

      ost << "Expansion of $type:return failed: no left-hand side at the callsite " 
	  << _callsite->call()->coord();
      _annotations->Error(the_type_pattern->coord().line(), ost.str());

      return the_type;
    }
    else {

      // -- Look up it's binding at this callsite. TBD: it should be the same
      // binding in all the contexts. Right now, we'll arbitrary choose the
      // first one.

      procLocation * where = * (_callsite->contexts().begin());

      // -- Get the binding

      pointerValue & var_binding = var->get_binding(where);

      // -- Generate the expansion. This is another weird area because the
      // variable could be bound to multiple objects, which would make it
      // impossible to expand properly. Right now, issue an error.
      
      if (var_binding.blocks.size() != 1) {
	ost << "Improper binding for replacement of \"" << var->name() << "\" at " <<
	  _callsite->call()->coord();
	_annotations->Error(the_type_pattern->coord().line(), ost.str());
      }

      memoryBlock * mb = * (var_binding.blocks.begin());

      // -- The expansion is a copy of the type on the declNode of the memoryBlock.

      return ref_clone_changer::clone(mb->decl()->type(), false);
    }
  }

  return the_type;
}

