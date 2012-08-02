
#include <rose.h>

using namespace std;

#include <rosepoly/RosePollyInterface.h>


// CUSTOMIZING INTERFACE

bool RosePollyCustom::isAffineExpression( SgExpression * exp, const vector<string>& legalVars ) const
{
	if ( exp == NULL )
		return true;
	
	VariantT varT = exp->variantT();
	
	switch (varT)
	{
		case V_SgIntVal :
			return true;
		case V_SgVarRefExp :
		{string name = isSgVarRefExp(exp)->get_symbol()->get_name().getString();
			for ( int i = 0 ; i < legalVars.size() ; i++ ) {
				if ( name == legalVars[i] )
					return true;
			}}
			return false;
		case V_SgMultiplyOp :
			if ( ( isAffineExpression( isSgBinaryOp(exp)->get_lhs_operand(), legalVars ) && isIntExpression(isSgBinaryOp(exp)->get_rhs_operand()) ) ||
				( isIntExpression(isSgBinaryOp(exp)->get_lhs_operand()) && isAffineExpression( isSgBinaryOp(exp)->get_rhs_operand(), legalVars ) )
				)
				return true;
			else
				return false;
		case V_SgDivideOp:
		case V_SgIntegerDivideOp:
			if ( isAffineExpression( isSgBinaryOp(exp)->get_lhs_operand(), legalVars ) && isIntExpression(isSgBinaryOp(exp)->get_rhs_operand()) )
				return true;
			else
				return false;
		case V_SgAddOp :
		case V_SgSubtractOp :
		case V_SgAssignOp :
		case V_SgAndOp :
		case V_SgOrOp :
		case V_SgGreaterOrEqualOp :
		case V_SgLessOrEqualOp :
		case V_SgGreaterThanOp :
		case V_SgLessThanOp :
			if ( isAffineExpression( isSgBinaryOp(exp)->get_lhs_operand(), legalVars ) && isAffineExpression( isSgBinaryOp(exp)->get_rhs_operand(), legalVars ) )
				return true;
			else
				return false;
		default:
			return false;
	}
}

bool RosePollyCustom::isIntExpression( SgExpression * exp ) const
{
	VariantT varT = exp->variantT();
	
	switch (varT)
	{
		case V_SgIntVal:
			return true;
		case V_SgAddOp:
		case V_SgSubtractOp:
		case V_SgMultiplyOp:
		case V_SgIntegerDivideOp:
		case V_SgDivideOp:
			if ( isIntExpression( isSgBinaryOp(exp)->get_lhs_operand() ) && isIntExpression( isSgBinaryOp(exp)->get_rhs_operand() ) )
				return true;
			else
				return false;
		default:
			return false;
	}
}

// DEFAULT CUSTOMIZING CLASS

bool defaultRosePollyCustom::evaluate_loop( ForLoop * loop )
{
	if ( loop->is_head() ) {
		if ( !isAffineExpression(loop->get_start(),legalVars) ||
			!isAffineExpression(loop->get_end(),legalVars) ) {
				return false;
		}
		legalVars.push_back(loop->get_symbol());
		return true;
	} else {
		legalVars.pop_back();
	}
}

pollyDomain * defaultRosePollyCustom::add_loop( pollyDomain * d, ForLoop * loop ) const
{	
	d->add_loop(loop);	
	return d;
}

bool defaultRosePollyCustom::evaluate_conditional( Conditional * cond ) const
{
	if ( !isAffineExpression(cond->get_exp(),legalVars) )
		return false;
	
	return true;
}

pollyDomain * defaultRosePollyCustom::add_conditional( pollyDomain * d, Conditional * cond ) const
{	
	d->add_conditional(cond->get_exp(),cond->done());
	return d;
}

bool defaultRosePollyCustom::evaluate_access( AccessPattern * ap ) const
{
	for ( int i = 0 ; i < ap->get_dim() ; i++ )
		if ( !isAffineExpression(ap->get_subscript(i),legalVars) )
			return false;
	
	return true;
}

pollyMap * defaultRosePollyCustom::add_pattern( pollyDomain * d, AccessPattern * ap ) const
{
	vector<SgExpression*> subs;
	for ( int i = 0 ; i < ap->get_dim() ; i++ )
		subs.push_back(ap->get_subscript(i));
	
	return new pollyMap(d,subs);
}

void defaultRosePollyCustom::set_params( vector<string> p )
{
	legalVars = p;
}









