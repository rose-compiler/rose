/**
 * \file include/rose/Variable.hpp
 * \brief Contains the representation of variable handling structures' references.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _VARIABLE_HPP_
#define _VARIABLE_HPP_

#include "rose.h"

#include <string>

class RoseVariable {
	protected:
		SgInitializedName * p_var;
		RoseVariable * p_base;
		bool p_is_arrow;
	
	public:
		RoseVariable(SgInitializedName * var = NULL, RoseVariable * base = NULL, bool is_arrow = false);
		RoseVariable(const RoseVariable & v);
		~RoseVariable();
	
		std::string getString() const;
		
		bool is(SgInitializedName * var) const;
		
		SgExpression * generate(SgScopeStatement * stmt) const;
	
		RoseVariable & operator = (const RoseVariable & v);
		
	friend bool operator == (const RoseVariable & v1, const RoseVariable & v2);
	friend bool operator != (const RoseVariable & v1, const RoseVariable & v2);
	friend bool operator < (const RoseVariable & v1, const RoseVariable & v2); // for std::map
};

bool operator == (const RoseVariable & v1, const RoseVariable & v2);
bool operator != (const RoseVariable & v1, const RoseVariable & v2);
bool operator < (const RoseVariable & v1, const RoseVariable & v2);

bool isConstant(const RoseVariable & v);

std::string toString(const RoseVariable & v);

RoseVariable constantLBL();

#endif /* _VARIABLE_HPP_ */
