/**
 * \file src/rose/rose-utils.cpp
 * \brief Implementation of Output function used from common part when specialized for Rose.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "rose/rose-utils.hpp"

std::ostream & operator << (std::ostream & out, SgExprStatement & arg) {
	out << arg.unparseToString();
	return out;
}

std::ostream & operator << (std::ostream & out, SgInitializedName & arg) {
	out << arg.get_name().getString();
	return out;
}

std::ostream & operator << (std::ostream & out, RoseVariable & arg) {
	out << arg.getString();
	return out;
}

std::ostream & operator << (std::ostream & out, const SgExprStatement & arg) {
	out << arg.unparseToString();
	return out;
}

std::ostream & operator << (std::ostream & out, const SgInitializedName & arg) {
	out << arg.get_name().getString();
	return out;
}

std::ostream & operator << (std::ostream & out, const RoseVariable & arg) {
	out << arg.getString();
	return out;
}

char * varToCstr(const SgInitializedName * var) {
	std::string str = var->get_name().getString();
	char * res = new char [str.size()+1];
	std::strcpy(res, str.c_str());
	return res;
}

char * varToCstr(const RoseVariable var) {
	std::string str = var.getString();
	char * res = new char [str.size()+1];
	std::strcpy(res, str.c_str());
	return res;
}

