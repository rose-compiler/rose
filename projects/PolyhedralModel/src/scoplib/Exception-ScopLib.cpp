/**
 * \file src/scoplib/Exception-ScopLib.tpp
 * \brief Implementation of exception specific to all ScopLib interface (ScopLib, Clang, Cloog).
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "scoplib/Exception-ScopLib.hpp"

namespace Exception {

ExceptionScopLib::ExceptionScopLib(std::string str) :
	ExceptionBase(str)
{}

ExceptionScopLib::~ExceptionScopLib() {}
		
void ExceptionScopLib::print(std::ostream & out) {
	out << p_str << std::endl;
}

OutOfScopLibScope::OutOfScopLibScope(std::string str) :
	ExceptionScopLib(str)
{}

OutOfScopLibScope::~OutOfScopLibScope() {}
		
void OutOfScopLibScope::print(std::ostream & out) {
	out << p_str << std::endl;
}

WrongDependencyType::WrongDependencyType() :
	ExceptionScopLib("")
{}

WrongDependencyType::~WrongDependencyType() {}
		
void WrongDependencyType::print(std::ostream & out) {
	out << "A dependency with a wrong type have been found." << std::endl;
}

}

