

#include <rose.h>

using namespace std;

#include <rosepoly/data.h>
#include <rosepoly/Utilities.h>

namespace utilities {

Type extract_scalar_type( SgType * t )
{
	Type final = NOTYPE;
	
	if ( isSgPointerType(t) != NULL )
		final = extract_scalar_type( isSgPointerType(t)->get_base_type() );
	else if ( isSgArrayType(t) != NULL )
		final = extract_scalar_type( isSgArrayType(t)->dereference() );
	else if ( isSgModifierType(t) != NULL )
		final = extract_scalar_type( isSgModifierType(t)->get_base_type() );
	else if ( isSgTypedefType(t) )
		final = extract_scalar_type( isSgTypedefType(t)->get_base_type() );
	else
		final = check_scalar_type(t);
	
	return final;
}

Type check_scalar_type( SgType * t )
{
	Type scalar;
	
	if ( isSgTypeInt(t) != NULL )
		return INT;
	else if ( isSgTypeFloat(t) != NULL )
		return FLOAT;
	else if ( isSgTypeDouble(t) != NULL )
		return DOUBLE;
	else
		return NOTYPE;
}
	
bool isAffineExpression( SgExpression * exp, vector<string>& vars )
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
			for ( int i = 0 ; i < vars.size() ; i++ ) {
				if ( name == vars[i] )
					return true;
			}}
			return false;
		case V_SgMultiplyOp :
			if ( ( isAffineExpression( isSgBinaryOp(exp)->get_lhs_operand(), vars ) && isIntExpression(isSgBinaryOp(exp)->get_rhs_operand()) ) ||
				( isIntExpression(isSgBinaryOp(exp)->get_lhs_operand()) && isAffineExpression( isSgBinaryOp(exp)->get_rhs_operand(), vars ) )
				)
				return true;
			else
				return false;
		case V_SgDivideOp:
		case V_SgIntegerDivideOp:
			if ( isAffineExpression( isSgBinaryOp(exp)->get_lhs_operand(), vars ) && isIntExpression(isSgBinaryOp(exp)->get_rhs_operand()) )
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
			if ( isAffineExpression( isSgBinaryOp(exp)->get_lhs_operand(), vars ) && isAffineExpression( isSgBinaryOp(exp)->get_rhs_operand(), vars ) )
				return true;
			else
				return false;
		default:
			return false;
	}
}
	
bool isIntExpression( SgExpression * exp )
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
	
}

