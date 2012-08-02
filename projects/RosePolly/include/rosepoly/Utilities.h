
#ifndef UTILITIES_H
#define UTILITIES_H

namespace utilities {

Type extract_scalar_type( SgType * t );

Type check_scalar_type( SgType * t );

bool isAffineExpression( SgExpression * exp, vector<string>& vars );
	
bool isIntExpression( SgExpression * exp );
	
}
	
#endif

