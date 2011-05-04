/**
 * \file src/common/Exception.cpp
 * \brief Implementation of Exception (non-template).
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "common/Exception.hpp"

namespace Exception {

ExceptionBase::ExceptionBase(std::string str) :
	p_str(str)
{}

ExceptionBase::~ExceptionBase() {}

std::ostream & operator <<(std::ostream & out, ExceptionBase & e) {
	e.print(out);
	return out;
}

ExceptionFrontend::ExceptionFrontend(std::string str) :
	ExceptionBase(str)
{}
ExceptionFrontend::~ExceptionFrontend() {}

NotImplementedYet::NotImplementedYet(std::string str) :
	ExceptionBase(str)
{}

NotImplementedYet::~NotImplementedYet() {}
		
void NotImplementedYet::print(std::ostream & out) {
	out << p_str << "is not implemented yet." << std::endl;
}

MemoryOverflow::MemoryOverflow() :
	ExceptionBase("")
{}

MemoryOverflow::~MemoryOverflow() {}
		
void MemoryOverflow::print(std::ostream & out) {
	out << "A memory overflow append." << std::endl;
}

/***********************/
/* Container exception */
/***********************/

ContainerException::ContainerException(std::string str) :
	ExceptionBase(str)
{}

ContainerException::~ContainerException() {}

UnknownVariableID::UnknownVariableID() :
	ContainerException("")
{}

UnknownVariableID::~UnknownVariableID() {}
		
void UnknownVariableID::print(std::ostream & out) {
	out << "Looking for an unknown variable ID." << std::endl;
}

UnknownVariable::UnknownVariable() :
	ContainerException("")
{}

UnknownVariable::~UnknownVariable() {}
		
void UnknownVariable::print(std::ostream & out) {
	out << "Looking for an unknown variable (can not be add now)." << std::endl;
}

/***********************/
/* ScopTree exceptions */
/***********************/

ScopTreeException::ScopTreeException(std::string str) : 
	ExceptionBase(str)
{}

ScopTreeException::~ScopTreeException() {}

UnexpectedScopNode::UnexpectedScopNode(std::string str) :
	ScopTreeException(str)
{}

UnexpectedScopNode::~UnexpectedScopNode() {}
		
void UnexpectedScopNode::print(std::ostream & out) {
	out << p_str << std::endl;
}

OutOfScopRestrainedDefinition::OutOfScopRestrainedDefinition(std::string str) :
	ScopTreeException(str)
{}

OutOfScopRestrainedDefinition::~OutOfScopRestrainedDefinition() {}
		
void OutOfScopRestrainedDefinition::print(std::ostream & out) {
	out << p_str << std::endl;
}

}

