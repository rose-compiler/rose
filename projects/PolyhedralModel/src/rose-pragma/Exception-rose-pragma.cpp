
#include "rose-pragma/Exception-rose-pragma.hpp"

namespace Exception {

ExceptionPragma::ExceptionPragma(std::string str) :
	ExceptionBase(str)
{}

ExceptionPragma::~ExceptionPragma() {}

NotPolyhedricPragmaException::NotPolyhedricPragmaException(std::string str) :
	ExceptionPragma(str)
{}

NotPolyhedricPragmaException::~NotPolyhedricPragmaException() {}

void NotPolyhedricPragmaException::print(std::ostream & out) {
	out << "Try to parse a polyhedric pragma from: " << p_str << std::endl;
}

InvalidPolyhedricPragmaException::InvalidPolyhedricPragmaException(std::string str) :
	ExceptionPragma(str)
{}

InvalidPolyhedricPragmaException::~InvalidPolyhedricPragmaException() {}

void InvalidPolyhedricPragmaException::print(std::ostream & out) {
	out << p_str << std::endl;
}

InvalidPolyhedricProgramException::InvalidPolyhedricProgramException(std::string str, ExceptionRose & catched_exception) :
	ExceptionPragma(str),
	p_catched_exception(catched_exception)
{}

InvalidPolyhedricProgramException::~InvalidPolyhedricProgramException() {}
		
void InvalidPolyhedricProgramException::print(std::ostream & out) {
	out << "Receive:" << std::endl;
	p_catched_exception.print(out);
	out << "from the polyhedric program parser." << std::endl;
}

}
