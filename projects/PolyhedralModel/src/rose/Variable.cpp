/**
 * \file src/rose/Variable.cpp
 * \brief Implementation of variables representation for structure handling.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "rose/Variable.hpp"

RoseVariable::RoseVariable(SgInitializedName * var, RoseVariable * base, bool is_arrow) :
	p_var(var),
	p_base(base),
	p_is_arrow(is_arrow)
{}

RoseVariable::RoseVariable(const RoseVariable & v) :
	p_var(v.p_var),
	p_base(v.p_base != NULL ? new RoseVariable(*(v.p_base)) : NULL),
	p_is_arrow(v.p_is_arrow)
{}

RoseVariable::~RoseVariable() {
	if (p_base != NULL)
		delete p_base;
}
	
RoseVariable & RoseVariable::operator = (const RoseVariable & v) {
	if (this != &v) {
		p_var = v.p_var;
		p_base = v.p_base != NULL ? new RoseVariable(*(v.p_base)) : NULL;
		p_is_arrow = v.p_is_arrow;
	}
	return *this;
}

std::string RoseVariable::getString() const {
	std::string res;
	if (p_base != NULL) {
		res += p_base->getString();
		if (p_is_arrow)
			res += "->";
		else
			res += ".";
	}
	if (p_var != NULL)
		res += p_var->get_name().getString();
	else
		res += "NULL";
	return res;
}

bool RoseVariable::is(SgInitializedName * var) const {
	return p_base == NULL && p_var == var;
}

SgExpression * RoseVariable::generate(SgScopeStatement * stmt) const {
	if (p_base == NULL)
		return SageBuilder::buildVarRefExp(p_var, stmt);
	else
		if (p_is_arrow)
			return SageBuilder::buildArrowExp(
				p_base->generate(stmt),
				SageBuilder::buildVarRefExp(p_var, stmt)
			);
		else
			return SageBuilder::buildDotExp(
				p_base->generate(stmt),
				SageBuilder::buildVarRefExp(p_var, stmt)
			);

}

bool operator == (const RoseVariable & v1, const RoseVariable & v2) {
	return v1.p_var == v2.p_var && ((v1.p_base == NULL && v2.p_base == NULL) || *(v1.p_base) == *(v2.p_base));
}

bool operator != (const RoseVariable & v1, const RoseVariable & v2) { // i'm too lazy to simplify this XD
	if (v1.p_var != v2.p_var)
		return true;
	if (v1.p_base == NULL && v2.p_base == NULL)
		return false;
	else if (v1.p_base == NULL || v2.p_base == NULL)
		return true;
	else
		return *(v1.p_base) != *(v2.p_base);
}

bool operator < (const RoseVariable & v1, const RoseVariable & v2) {
	if (v1.p_base != NULL && v2.p_base != NULL) {
		if (*(v1.p_base) != *(v2.p_base))
			return *(v1.p_base) < *(v2.p_base);
		else
			return v1.p_var < v2.p_var;
	}
	else if (v1.p_base == NULL && v2.p_base == NULL)
		return v1.p_var < v2.p_var;
	else
		return v1.p_base == NULL; // I consider NULL (no upper structure) inferior.
}

bool isConstant(const RoseVariable & v) {
	return v.is(NULL);
}

std::string toString(const RoseVariable & v) {
	return v.getString();
}

RoseVariable constantLBL() {
	return RoseVariable();
}

