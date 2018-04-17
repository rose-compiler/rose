/*
 * SageTools.cpp: This file is part of the PolyOpt project.
 *
 * PolyOpt: a Polyhedral Optimizer for the ROSE compiler
 *
 * Copyright (C) 2011 the Ohio State University
 *
 * This program can be redistributed and/or modified under the terms
 * of the license specified in the LICENSE.txt file at the root of the
 * project.
 *
 * Contact: P. Sadayappan <saday@cse.ohio-state.edu>
 *
 */
/**
 * @file: SageTools.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */


// LNP: To avoid redefined macro errors in pocc.
/// LNP: FIXME: Find a better solution!
#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_NAME
#undef PACKAGE_VERSION
#include <rose.h>
#ifndef SCOPLIB_INT_T_IS_LONGLONG
# define SCOPLIB_INT_T_IS_LONGLONG
#endif
#include <scoplib/scop.h>

#include <AstTraversal.h>
#include <polyopt/SageTools.hpp>


using namespace std;
using namespace SageInterface;


/**
 * Private constructor. The class is not meant to be instantiated.
 */
SageTools::~SageTools()
{ }


/*****************************************************************************/
/************************ Variable reference handling ************************/
/*****************************************************************************/

/**
 * Collect read-only symbols.
 * Create a unique fake symbol for SgDotExp(variable, field).
 * Ensure no pointer arithmetic for any of the symbols used.
 *
 */
bool
SageTools::collectReadOnlySymbols(SgStatement* node,
				  std::set<SgVariableSymbol*>& symbols,
				  std::map<std::string, SgVariableSymbol*>&
				  fakeSymbolMap,
				  bool allowMathFunc)
{
  std::vector<SgNode*> readRefs;
  std::vector<SgNode*> writeRefs;
  if (! collectReadWriteRefs(node, readRefs, writeRefs, allowMathFunc))
    return false;
  std::set<SgVariableSymbol*> readSymbs = convertToSymbolSet(readRefs,
							     fakeSymbolMap);
  std::set<SgVariableSymbol*> writeSymbs = convertToSymbolSet(writeRefs,
							      fakeSymbolMap);

  // read only = read - write
  set_difference(readSymbs.begin(), readSymbs.end(),
		 writeSymbs.begin(), writeSymbs.end(),
		 std::inserter(symbols, symbols.begin()));
  return true;
}


/**
 * Collect all references in the subtree 'node'.
 * Ensure no pointer arithmetic for any of the symbols used.
 *
 */
bool
SageTools::collectReadWriteRefs(SgStatement* node,
				std::vector<SgNode*>& readRefs,
				std::vector<SgNode*>& writeRefs,
				bool allowMathFunc)
{
  // 1- Call ROSE interface to collect R/W refs.
  //if (! SageInterface::collectReadWriteRefs(node, readRefs, writeRefs))
  //   return false;
   if (! collectReadWriteRefsFastImpl(node, readRefs, writeRefs, allowMathFunc))
   return false;
  // 2- Sanity check: ensure no pointer reference, no offset in array
  // accesses.
  std::vector<SgNode*>::const_iterator i;
  for (i = readRefs.begin(); i != readRefs.end(); ++i)
    {
      if (isSgVarRefExp(*i))
	continue;
      SgDotExp* dexp = isSgDotExp(*i);
      if (dexp)
	{
	  if (isSgVarRefExp(dexp->get_lhs_operand()) &&
	      isSgVarRefExp(dexp->get_rhs_operand()))
	    continue;
	}
      SgPntrArrRefExp* arref = isSgPntrArrRefExp(*i);
      SgPntrArrRefExp* last = arref;
      if (arref)
	{
	  while ((last = isSgPntrArrRefExp(arref->get_lhs_operand())))
	    arref = last;
	  if (isSgVarRefExp(arref->get_lhs_operand()))
	    continue;
	  else
	    {
	      SgDotExp* dexp = isSgDotExp(arref->get_lhs_operand());
	      if (dexp)
		{
		  if (isSgVarRefExp(dexp->get_lhs_operand()) &&
		      isSgVarRefExp(dexp->get_rhs_operand()))
		    continue;
		}
	    }
	}
      return false;
    }
  for (i = writeRefs.begin(); i != writeRefs.end(); ++i)
    {
      if (isSgVarRefExp(*i))
	continue;
      SgDotExp* dexp = isSgDotExp(*i);
      if (dexp)
	{
	  if (isSgVarRefExp(dexp->get_lhs_operand()) &&
		      isSgVarRefExp(dexp->get_rhs_operand()))
	    continue;
	}
      SgPntrArrRefExp* arref = isSgPntrArrRefExp(*i);
      SgPntrArrRefExp* last = arref;
      if (arref)
	{
	  while ((last = isSgPntrArrRefExp(arref->get_lhs_operand())))
	    arref = last;
	  if (isSgVarRefExp(arref->get_lhs_operand()))
	    continue;
	  else
	    {
	      SgDotExp* dexp = isSgDotExp(arref->get_lhs_operand());
	      if (dexp)
		{
		  if (isSgVarRefExp(dexp->get_lhs_operand()) &&
		      isSgVarRefExp(dexp->get_rhs_operand()))
		    continue;
		}
	    }
	}
      return false;
    }
  return true;
}



/**
 * Collect R/W references in the subtree 'node'.
 * Efficient re-implementation of SageInterface::collectReadWriteRefs
 *
 * FIXME: Does not deal properly with pointer arithmetics.
 *
 */
bool
SageTools::collectReadWriteRefsFastImpl(SgStatement* node,
					std::vector<SgNode*>& readRefs,
					std::vector<SgNode*>& writeRefs,
					bool allowMathFuncs)
{
  // 1- Traverse the tree, collect all refs.
      // Visit the sub-tree, collecting all referenced variables.
  class ReferenceVisitor : public AstPrePostOrderTraversal {
  public:
    virtual void preOrderVisit(SgNode* node) {};
    virtual void postOrderVisit(SgNode* node) {
      // Ensure function call is allowed math one.
      SgFunctionCallExp* fe = isSgFunctionCallExp(node);
      if (fe && (!_allowMathFuncs ||
		 ! isMathFunc(fe->getAssociatedFunctionSymbol())))
	_is_error = true;
      if (! _is_error &&
	  (isSgVarRefExp(node) || isSgPntrArrRefExp(node)) ||
	  isSgDotExp(node))
	{
	  SgNode* curref = node;
	  bool is_write = false;
	  bool is_read = true;
	  node = node->get_parent();

	  // Special case 1: Array reference, skip inner dimensions
	  // of arrays.
	  SgPntrArrRefExp* last;
	  if ((last = isSgPntrArrRefExp(node)))
	    if (last->get_lhs_operand() == curref)
	      return;

	  // Special case 2: Cast, skip them.
	  SgCastExp* cexp = isSgCastExp(node);
	  while (isSgCastExp(node))
	    node = node->get_parent();

	  switch (node->variantT()) {
	    // Unary ops.
	  case V_SgMinusMinusOp:
	  case V_SgPlusPlusOp:
	  case V_SgUserDefinedUnaryOp:
	    {
	      is_write = true;
	      break;
	    }
	    // Assign ops.
	  case V_SgPlusAssignOp:
	  case V_SgMinusAssignOp:
	  case V_SgAndAssignOp:
	  case V_SgIorAssignOp:
	  case V_SgMultAssignOp:
	  case V_SgDivAssignOp:
	  case V_SgModAssignOp:
	  case V_SgXorAssignOp:
	  case V_SgLshiftAssignOp:
	  case V_SgRshiftAssignOp:
	    {
	      SgBinaryOp* bop = isSgBinaryOp(node);
	      if (bop->get_lhs_operand() == curref)
		is_write = true;
	      break;
	    }
	  case V_SgAssignOp:
	    {
	      SgBinaryOp* bop = isSgBinaryOp(node);
	      if (bop->get_lhs_operand() == curref)
		{
		  is_write = true;
		  is_read = false;
		}
	      else
		{
		  is_write = false;
		  is_read = true;
		}
	      break;
	    }
	    // Skip safe constructs.
	  case V_SgPntrArrRefExp:
	  case V_SgExprListExp:
	  case V_SgDotExp:
	  case V_SgExprStatement:
	  case V_SgConditionalExp:
	    break;
	    // Skip binop
	  case V_SgEqualityOp:
	  case V_SgLessThanOp:
	  case V_SgGreaterThanOp:
	  case V_SgNotEqualOp:
	  case V_SgLessOrEqualOp:
	  case V_SgGreaterOrEqualOp:
	  case V_SgAddOp:
	  case V_SgSubtractOp:
	  case V_SgMultiplyOp:
	  case V_SgDivideOp:
	  case V_SgIntegerDivideOp:
	  case V_SgModOp:
	  case V_SgAndOp:
	  case V_SgOrOp:
	  case V_SgBitXorOp:
	  case V_SgBitAndOp:
	  case V_SgBitOrOp:
	  case V_SgCommaOpExp:
	  case V_SgLshiftOp:
	  case V_SgRshiftOp:
	  case V_SgScopeOp:
	  case V_SgExponentiationOp:
	  case V_SgConcatenationOp:
	    // Skip safe unops.
	  case V_SgExpressionRoot:
	  case V_SgMinusOp:
	  case V_SgUnaryAddOp:
	  case V_SgNotOp:
	  case V_SgBitComplementOp:
	  case V_SgCastExp:
	  case V_SgThrowOp:
	  case V_SgRealPartOp:
	  case V_SgImagPartOp:
	  case V_SgConjugateOp:
	    break;
	  case V_SgFunctionCallExp:
	    {
	      SgFunctionCallExp* f = isSgFunctionCallExp(node);
	      if (! _allowMathFuncs ||
		  ! isMathFunc(f->getAssociatedFunctionSymbol()))
		_is_error = true;
	    }
	    break;
	  case V_SgPointerDerefExp:
	  case V_SgAddressOfOp:
	  default:
	      _is_error = true;
	    break;
	  }
	  if (is_read)
	    _readRefs.push_back(curref);
	  if (is_write)
	    _writeRefs.push_back(curref);
	}
    }
    std::vector<SgNode*> _readRefs;
    std::vector<SgNode*> _writeRefs;
    SgNode*		 _top;
    bool		 _is_error;
    bool		 _allowMathFuncs;
  };

  ReferenceVisitor lookupReference;
  lookupReference._top = node;
  lookupReference._is_error = false;
  lookupReference._allowMathFuncs = allowMathFuncs;
  lookupReference.traverse(node);

  readRefs.insert(readRefs.end(), lookupReference._readRefs.begin(),
		  lookupReference._readRefs.end());
  writeRefs.insert(writeRefs.end(), lookupReference._writeRefs.begin(),
		  lookupReference._writeRefs.end());

  return !lookupReference._is_error;
}


/**
 * Helper. Convert std::vector<SgNode*> constaining references to
 * std::set<SgVariableSymbol*>
 *
 *
 */
std::set<SgVariableSymbol*>
SageTools::convertToSymbolSet(std::vector<SgNode*>& refs,
			      std::map<std::string, SgVariableSymbol*>&
			      fakeSymbolMap)
{
  std::set<SgVariableSymbol*> result;

  std::vector<SgNode*>::const_iterator i;
  for (i = refs.begin(); i != refs.end(); ++i)
    {
      if (*i == NULL)
	continue;
      SgVariableSymbol* symbol = getSymbolFromReference(*i, fakeSymbolMap);
      if (symbol)
	result.insert(symbol);
    }

  return result;
}


/**
 * Create a fake symbol for a SgDotExp(variable, field) node. Store
 * the symbol in the map if not already existing.
 *
 */
SgVariableSymbol*
SageTools::getFakeSymbol(SgDotExp* node,
			 std::map<std::string, SgVariableSymbol*>& map)
{
  if (! node)
    return NULL;
  SgVarRefExp* v1 = isSgVarRefExp(node->get_lhs_operand());
  SgVarRefExp* v2 = isSgVarRefExp(node->get_rhs_operand());
  if (! (v1 && v2))
    return NULL;
  std::string fakename = v1->get_symbol()->get_name().getString() +
    "." + v2->get_symbol()->get_name().getString();
  std::map<std::string, SgVariableSymbol*>::iterator i =
    map.find(fakename);
  if (i == map.end())
    {
      SgInitializedName* iname = new SgInitializedName();
      iname->set_name(SgName(fakename));
      SgVariableSymbol* symbol = new SgVariableSymbol(iname);
      std::pair<std::string, SgVariableSymbol*> newelt(fakename, symbol);
      map.insert(newelt);
      return symbol;
    }
  return i->second;
}


/**
 * Get a symbol from a reference. If the reference is part of a C
 * structure/field reference, return the associated fake symbol
 * (create it if needed).
 * Return NULL if the symbol cannot be extracted.
 *
 */
SgVariableSymbol*
SageTools::getSymbolFromReference(SgNode* node,
				  std::map<std::string, SgVariableSymbol*>& map)
{
  SgVarRefExp* v = isSgVarRefExp(node);
  SgDotExp* dexp = isSgDotExp(node);;
  if (v)
    {
      dexp = isSgDotExp(v->get_parent());
      if (dexp)
	return getFakeSymbol(dexp, map);
      else
	return v->get_symbol();
    }
  else if (dexp)
    return getFakeSymbol(dexp, map);

  return NULL;
}



/**
 * Get the symbol associated with the loop iterator.
 *
 * Works only for loops with increment of the form
 *  'iterator++'
 *  '++iterator'
 *  'iterator += 1'
 *  'iterator = ...'
 *
 */
SgVariableSymbol*
SageTools::getLoopIteratorSymbol(SgForStatement* fornode)
{
  if (! fornode)
    return NULL;

  SgPlusPlusOp* ppop = isSgPlusPlusOp(fornode->get_increment());
  SgBinaryOp* bop = isSgPlusAssignOp(fornode->get_increment());
  if (! bop)
    bop = isSgAssignOp(fornode->get_increment());
  SgVariableSymbol* forSymb = NULL;
  if (ppop)
    forSymb = isSgVarRefExp(ppop->get_operand_i())->get_symbol();
  else if (bop)
    forSymb = isSgVarRefExp(bop->get_lhs_operand())->get_symbol();

  return forSymb;
}




/*****************************************************************************/
/************************* Node and tree recognition *************************/
/*****************************************************************************/



/**
 * Returns true if 'node' is a math function call.
 *
 *
 */
bool
SageTools::isMathFunc(SgFunctionSymbol* fs)
{
  if (fs)
    {
      std::string symb = fs->get_name();
      if (symb == "pow" || symb == "round" || symb == "sqrt" ||
	  symb == "modf" || symb == "fmod" || symb == "frexp" ||
	  symb == "ceild" || symb == "ceil" || symb == "floord" ||
	  symb == "floor" || symb == "min" || symb == "max" ||
	  symb == "cos" || symb == "sin" || symb == "tan" ||
	  symb == "cosh" || symb == "sinh" || symb == "tanh" ||
	  symb == "acos" || symb == "asin" || symb == "atan" ||
	  symb == "atan2" ||
	  symb == "ln" || symb == "log" || symb == "exp" || symb == "log10" ||
	  symb == "abs" || symb == "fabs" || symb == "ldexp")
	return true;
    }

  return false;
}


/**
 * Return true if the node is '>', '>=', '<', '<=' or '=='.
 *
 */
bool
SageTools::isCompareOp(SgNode* node)
{
  if (isSgGreaterThanOp(node) ||
      isSgLessThanOp(node) ||
      isSgGreaterOrEqualOp(node) ||
      isSgLessOrEqualOp(node) ||
      isSgEqualityOp(node))
    return true;

  return false;
}


/**
 * Returns true if the value 'expr' can be promoted to an integer
 * type.
 *
 */
bool
SageTools::isIntegerTypeValue(SgNode* expr)
{
   if (expr == NULL)
     return false;
  switch (expr->variantT())
    {
    case V_SgBoolValExp:
    case V_SgShortVal:
    case V_SgCharVal:
    case V_SgUnsignedCharVal:
    case V_SgWcharVal:
    case V_SgUnsignedShortVal:
    case V_SgIntVal:
    case V_SgUnsignedIntVal:
    case V_SgLongIntVal:
    case V_SgLongLongIntVal:
    case V_SgUnsignedLongLongIntVal:
    case V_SgUnsignedLongVal:
      return true;
    default:
      return false;
    }
}

/**
 * Returns an inter value of an integer type node into a long long int.
 * Careful of overflow with integer types larger than long int.
 *
 */
long int
SageTools::getIntegerTypeValue(SgNode* expr)
{
  ROSE_ASSERT(isIntegerTypeValue(expr));

  switch (expr->variantT())
    {
    case V_SgBoolValExp:
      return isSgBoolValExp(expr)->get_value();
    case V_SgShortVal:
      return isSgShortVal(expr)->get_value();
    case V_SgCharVal:
      return isSgCharVal(expr)->get_value();
    case V_SgUnsignedCharVal:
      return isSgUnsignedCharVal(expr)->get_value();
    case V_SgWcharVal:
      return isSgWcharVal(expr)->get_value();
    case V_SgUnsignedShortVal:
      return isSgUnsignedShortVal(expr)->get_value();
    case V_SgIntVal:
      return isSgIntVal(expr)->get_value();
    case V_SgUnsignedIntVal:
      return isSgUnsignedIntVal(expr)->get_value();
    case V_SgLongIntVal:
      return isSgLongIntVal(expr)->get_value();
    case V_SgLongLongIntVal:
      return isSgLongLongIntVal(expr)->get_value();
    case V_SgUnsignedLongLongIntVal:
      return isSgUnsignedLongLongIntVal(expr)->get_value();
    case V_SgUnsignedLongVal:
      return isSgUnsignedLongVal(expr)->get_value();
    default:
      return 0;
    }
}


/**
 * Check if the node is a min(x,y) expression using x < y ? x : y
 *
 */
bool
SageTools::isMinFuncCall(SgNode* node)
{
  SgConditionalExp* tern = isSgConditionalExp(node);

  if (tern)
    {
      SgLessThanOp* lop = isSgLessThanOp(tern->get_conditional_exp());
      if (! lop)
	return false;
      bool xOK = false;
      bool yOk = false;
      xOK = checkTreeAreEquivalent(lop->get_lhs_operand(),
				   tern->get_true_exp());
      if (xOK)
	yOk = checkTreeAreEquivalent(lop->get_rhs_operand(),
				     tern->get_false_exp());
      return xOK && yOk;
    }

  return false;
}


/**
 * Check if the node is a max(x,y) expression using x > y ? x : y
 *
 */
bool
SageTools::isMaxFuncCall(SgNode* node)
{
  SgConditionalExp* tern = isSgConditionalExp(node);

  if (tern)
    {
      SgGreaterThanOp* gop = isSgGreaterThanOp(tern->get_conditional_exp());
      if (! gop)
	return false;
      bool xOK = false;
      bool yOk = false;
      xOK = checkTreeAreEquivalent(gop->get_lhs_operand(),
				   tern->get_true_exp());
      if (xOK)
	yOk = checkTreeAreEquivalent(gop->get_rhs_operand(),
				     tern->get_false_exp());
      return xOK && yOk;
    }
  return false;
}


/**
 * Check if the node is floord(n, d), defined as:
 * (((n*d)<0) ? (((d)<0) ? -((-(n)+(d)+1)/(d)) : -((-(n)+(d)-1)/(d))) : (n)/(d))
 *
 */
bool
SageTools::isFloorFuncCall(SgNode* node)
{

  SgConditionalExp* tern = isSgConditionalExp(node);
  if (! tern)
    return false;
  // Parse first conditional: n * d < 0 OR 0|1 (when n and d are constants)
  SgLessThanOp* ltop = isSgLessThanOp(tern->get_conditional_exp());
  if (!ltop)
    return false;
  SgMultiplyOp* mmop = isSgMultiplyOp(ltop->get_lhs_operand());
  SgIntVal* val = isSgIntVal(ltop->get_rhs_operand());
  if (! (mmop && val && val->get_value() == 0))
    return false;
  SgExpression* n;
  SgIntVal* d;
  d = isSgIntVal(mmop->get_rhs_operand());
  n = isSgExpression(mmop->get_lhs_operand());
  if (! (d && n))
    return false;
  // Parse first true clause / 2nd conditional: d < 0 OR 0|1
  SgConditionalExp* tern2 = isSgConditionalExp(tern->get_true_exp());
  if (! tern2)
    return false;
  SgLessThanOp* ltop2 = isSgLessThanOp(tern2->get_conditional_exp());
  SgIntVal* vl = isSgIntVal(tern2->get_conditional_exp());
  SgIntVal* v1;
  SgIntVal* v2;
  if (!vl && !ltop2)
    return false;
  if (vl && vl->get_value() != 0 && vl->get_value() != 1)
    return false;
  else if (ltop2)
    {
      v1 = isSgIntVal(ltop2->get_lhs_operand());
      v2 = isSgIntVal(ltop2->get_rhs_operand());
      if (! (v1 && v2 && v1->get_value() == d->get_value()
	     && v2->get_value() == 0))
	return false;
    }
  // Parse 2nd true clause: -((-(n)+(d)+1)/(d))
  SgMinusOp* mop = isSgMinusOp(tern2->get_true_exp());
  if (! mop)
    return false;
  SgDivideOp* dop = isSgDivideOp(mop->get_operand_i());
  if (! dop)
    return false;
  v1 = isSgIntVal(dop->get_rhs_operand());
  SgAddOp* aop = isSgAddOp(dop->get_lhs_operand());
  if (aop == NULL || v1->get_value() != d->get_value())
    return false;
  SgAddOp* aop2 = isSgAddOp(aop->get_lhs_operand());
  v1 = isSgIntVal(aop->get_rhs_operand());
  if (! (aop2 && v1 && v1->get_value() == 1))
    return false;
  mop = isSgMinusOp(aop2->get_lhs_operand());
  v1 = isSgIntVal(aop2->get_rhs_operand());
  if (! (mop && val && v1->get_value() == d->get_value()))
    return false;
  SgExpression* e = isSgExpression(mop->get_operand_i());
  if (! (e && checkTreeAreEquivalent(e, n)))
    return false;
  // Parse 2nd false clause: -((-(n)+(d)-1)/(d)))
  mop = isSgMinusOp(tern2->get_false_exp());
  if (! mop)
    return false;
  dop = isSgDivideOp(mop->get_operand_i());
  if (! dop)
    return false;
  SgSubtractOp* sop = isSgSubtractOp(dop->get_lhs_operand());
  v1 = isSgIntVal(dop->get_rhs_operand());
  if (! (sop && v1->get_value() == d->get_value()))
    return false;
  v1 = isSgIntVal(sop->get_rhs_operand());
  aop = isSgAddOp(sop->get_lhs_operand());
  if (! (aop && v1 && v1->get_value() == 1))
    return false;
  mop = isSgMinusOp(aop->get_lhs_operand());
  v1 = isSgIntVal(aop->get_rhs_operand());
  if (! (mop && v1 && v1->get_value() == d->get_value()))
    return false;
  e = isSgExpression(mop->get_operand_i());
  if (! (e && checkTreeAreEquivalent(e, n)))
    return false;
  // Parse 1st false clause: (n)/(d)
  dop = isSgDivideOp(tern->get_false_exp());
  if (! dop)
    return false;
  v1 = isSgIntVal(dop->get_rhs_operand());
  e = isSgExpression(dop->get_lhs_operand());
  if (! (v1 && e && v1->get_value() == d->get_value() &&
	 checkTreeExpAreEquivalent(e, n)))
    return false;

  return true;
}


/**
 * Check if the node is ceil(n, d), defined as:
 * (((n*d)<0) ? -((-(n))/(d)) : (((d)<0) ? (((-n)+(-d)-1)/(-d)) :  ((n)+(d)-1)/(d)))
 *
 */
bool
SageTools::isCeilFuncCall(SgNode* node)
{
  SgConditionalExp* tern = isSgConditionalExp(node);
  if (! tern)
    return false;

  // Demangle first conditional: n * d < 0
  SgLessThanOp* ltop = isSgLessThanOp(tern->get_conditional_exp());
  if (! ltop)
    return false;
  SgMultiplyOp* mmop = isSgMultiplyOp(ltop->get_lhs_operand());
  SgIntVal* val = isSgIntVal(ltop->get_rhs_operand());
  SgExpression* n;
  SgIntVal* d;
  if (! (mmop && val && val->get_value() == 0))
    return false;
  d = isSgIntVal(mmop->get_rhs_operand());
  n = isSgExpression(mmop->get_lhs_operand());
  if (! (d && n))
    return false;

  // Demangle first true clause: -((-(n))/(d))
  SgMinusOp* mop = isSgMinusOp(tern->get_true_exp());
  if (! mop)
    return false;
  SgDivideOp* dop = isSgDivideOp(mop->get_operand_i());
  if (! dop)
    return false;
  mop = isSgMinusOp(dop->get_lhs_operand());
  SgIntVal* v1 = isSgIntVal(dop->get_rhs_operand());
  if (! (mop && v1 && v1->get_value() == d->get_value()))
    return false;
  SgExpression* e = isSgExpression(mop->get_operand_i());
  if (! checkTreeExpAreEquivalent(e, n))
    return false;

  // Demangle first false clause / 2nd conditional: d < 0 OR 0|1
  SgConditionalExp* tern2 = isSgConditionalExp(tern->get_false_exp());
  if (! tern2)
    return false;
  SgLessThanOp* ltop2 = isSgLessThanOp(tern2->get_conditional_exp());
  SgIntVal* vl = isSgIntVal(tern2->get_conditional_exp());
  SgIntVal* v2;
  if (!vl && !ltop2)
    return false;
  if (vl && vl->get_value() != 0 && vl->get_value() != 1)
    return false;
  else if (ltop2)
    {
      v1 = isSgIntVal(ltop2->get_lhs_operand());
      v2 = isSgIntVal(ltop2->get_rhs_operand());
      if (! (v1 && v2 && v1->get_value() == d->get_value() &&
	     v2->get_value() == 0))
    return false;
    }
  // Demangle 2nd true clause: (((-n)+(-d)-1)/(-d))
  dop = isSgDivideOp(tern2->get_true_exp());
  if (! dop)
    return false;
  mop = isSgMinusOp(dop->get_rhs_operand());
  SgSubtractOp* sop = isSgSubtractOp(dop->get_lhs_operand());
  if (! (mop && sop))
    return false;
  v1 = isSgIntVal(mop->get_operand_i());
  v2 = isSgIntVal(sop->get_rhs_operand());
  if (! (v1 && v2 && v1->get_value() == d->get_value() && v2->get_value() == 1))
    return false;
  SgAddOp* aop = isSgAddOp(sop->get_lhs_operand());
  if (! (aop))
    return false;
  mop = isSgMinusOp(aop->get_lhs_operand());
  SgMinusOp* mop2 = isSgMinusOp(aop->get_rhs_operand());
  if (! (mop && mop2))
    return false;
  e = isSgExpression(mop->get_operand_i());
  v1 = isSgIntVal(mop2->get_operand_i());
  if (! (e && v1 && v1->get_value() == d->get_value() &&
	 checkTreeAreEquivalent(e, n)))
    return false;

  // Demangle 2nd false clause: ((n)+(d)-1)/(d)))
  dop = isSgDivideOp(tern2->get_false_exp());
  if (! dop)
    return false;
  v1 = isSgIntVal(dop->get_rhs_operand());
  sop = isSgSubtractOp(dop->get_lhs_operand());
  if (! (sop && v1 &&  v1->get_value() == d->get_value()))
    return false;
  aop = isSgAddOp(sop->get_lhs_operand());
  v1 = isSgIntVal(sop->get_rhs_operand());
  if (! (aop && v1 && v1->get_value() == 1))
    return false;
  e = isSgExpression(aop->get_lhs_operand());
  v1 = isSgIntVal(aop->get_rhs_operand());
  if (! (e && v1 && v1->get_value() == d->get_value() &&
	 checkTreeAreEquivalent(e, n)))
    return false;

  return true;
}

/**
 * Return true if two trees are identical.
 *
 */
static bool checkGeneralEquivalence(SgNode* n1,
				    SgNode* n2)
{
  class AllVisitor : public AstPrePostOrderTraversal {
  public:
    virtual void preOrderVisit(SgNode* node) {
      nodes.push_back(node);
    };
    virtual void postOrderVisit(SgNode* node) { };
    std::vector<SgNode*> nodes;
  };

  AllVisitor cn1;
  cn1.traverse(n1);
  AllVisitor cn2;
  cn2.traverse(n2);

  if (cn1.nodes.size() != cn2.nodes.size())
    return false;
  std::vector<SgNode*>::const_iterator i1 = cn1.nodes.begin();
  std::vector<SgNode*>::const_iterator i2 = cn2.nodes.begin();
  while (i1 != cn1.nodes.end())
    {
      // Check types are equal.
      if ((*i1)->variantT() != (*i2)->variantT())
	return false;
      if (SageTools::isIntegerTypeValue(*i1))
	{
	  long int val1 = SageTools::getIntegerTypeValue(*i1);
	  long int val2 = SageTools::getIntegerTypeValue(*i2);
	  if (val1 != val2)
	    return false;
	}
      if (isSgVarRefExp(*i1))
	{
	  SgVariableSymbol* v1 = isSgVarRefExp(*i1)->get_symbol();
	  SgVariableSymbol* v2 = isSgVarRefExp(*i2)->get_symbol();
	  if (v1->get_name().getString() !=
	      v2->get_name().getString())
	    return false;
	}
      ++i1;
      ++i2;
    }

  return true;
}

/**
 * Return true if two expression trees are identical.
 *
 */
bool SageTools::checkTreeExpAreEquivalent(SgNode* e1,
					  SgNode* e2)
{
  return checkGeneralEquivalence(e1, e2);
}


/**
 * Return true if two general trees are identical.
 *
 *
 */
bool SageTools::checkTreeAreEquivalent(SgNode* e1,
				       SgNode* e2)
{
  return checkGeneralEquivalence(e1, e2);
}


/**
 * Check if a node is dominated by a specific node.
 *
 *
 */
bool
SageTools::isDominatedBy(SgNode* child, SgNode* dominator)
{
  SgNode* parent = child;
  while (parent && parent != dominator)
    parent = parent->get_parent();

  return parent != NULL;
}




/*****************************************************************************/
/***************************** Output verbosifier ****************************/
/*****************************************************************************/


/**
 *
 *
 */
bool
SageTools::invalidNode(SgNode* node, const char* message)
{
  if (message)
    std::cerr << message << std::endl;
  std::cerr << node->unparseToCompleteString() << std::endl;

  return false;
}

/**
 *
 *
 */
bool
SageTools::validNode(SgNode* node, const char* message)
{
  if (message)
    std::cerr << message << std::endl;
  std::cerr << node->unparseToCompleteString() << std::endl;
  return true;
}


/**
 * Prints a tree and the type of its root.
 *
 */
void
SageTools::debugPrintNode(SgNode* node)
{
  std::cout << "node: " << node->unparseToString() << std::endl;
  std::cout << "node type: " << node->sage_class_name() << std::endl;
}
