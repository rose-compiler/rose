/*
   POET : Parameterized Optimizations for Empirical Tuning
   Copyright (c)  2008,  Qing Yi.  All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 3. Neither the name of UTSA nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific
    prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISEDOF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ERROR_CONFIG_H
#define ERROR_CONFIG_H

#include "poetAST.h"

class Error {};

#define SHORT(str,len)  ((str.size() > len)? str.substr(0,len)+"......" : str)

class ParseError : public Error {
  POETCode* r1, *r2;
  int lineno;
 public:
  ParseError(POETCode* _r1, POETCode *_r2, int _lineno) : r1(_r1), r2(_r2), lineno(_lineno) {}
  std::string message() const 
   { std:: stringstream msg; msg << "Parsing type mismatch at line " << lineno << ": " << SHORT(r1->toString(),100) << "\n=> " << r2->toString() << "\n";  
     return msg.str(); }
};
#define PARSE_MISMATCH(r1,r2,ln)  { throw ParseError(r1,r2,ln); }

#define EXIT(a)  {  std::cerr << (a) << "\n"; throw Error(); }
#define USER_EXIT() { throw Error(); }
#define TRACE_RECURSIVE(v) { std:: stringstream msg; msg << "Error: empty or recursively defined tracing handle: " << v->toString() ;  EXIT(msg.str()); }
#define SYM_ALREADY_DEFINED(n) { std:: stringstream msg; msg << "Error: symbol already defined: " << n ;  EXIT(msg.str()); }
#define SYNTAX_ERROR(f, i) { std:: stringstream msg; msg << "Syntax error at Line " << i << " of file " << f   EXIT(msg.str()); }
#define CODE_OR_XFORM_UNDEFINED(n) { std:: stringstream msg;  msg << "Error: undefined code template or xform routine: " << n ;   EXIT(msg.str()); }
#define XFORM_UNDEFINED(n) { std:: stringstream msg;  msg << "Error: undefined xform routine: " << n ;   EXIT(msg.str()); }
#define CODE_SYNTAX_UNDEFINED(n) { std:: stringstream msg;  msg << "Error: syntax is undefined for code template: " << n ;   EXIT(msg.str()); }
#define CODE_UNDEFINED(n) { std:: stringstream msg;  msg << "Error: undefined code template: " << n ;   EXIT(msg.str()); }
#define CODE_ALREADY_DEFINED(n) { std:: stringstream msg; msg << "Error: code template already defined: " << n ;  EXIT(msg.str()); }
#define SYM_UNDEFINED(n) { std:: stringstream msg;  msg << "Error: undefined symbol: " << n ;   EXIT(msg.str()); }
#define SYM_INCORRECT_VAL(n,v, c) { std:: stringstream msg; msg << "Error: the value " << v << " assigned to symbol " << n << " does not satisfy constraint: " << c ;EXIT(msg.str()); }
#define VAR_ASSIGN_ERROR(c) { std:: stringstream msg; msg << "Error: variables cannot be assigned: " << c;EXIT(msg.str()); }
#define SYM_DIFF_DEFINED(n, m) { std::stringstream msg; msg << "Error: symbol already defined differently: " << n << "**vs.**" << "Original definition: " << m; EXIT(msg.str()); }
#define SYM_MULTI_DEFINED(n, m) { std::stringstream msg; msg << "Error: symbol already defined: " << n << "**vs.**" << "Original definition: " << m; EXIT(msg.str()); }
#define INHERIT_MULTI_DEFINED(n,m1,m2) { std::stringstream msg; msg << "Error: INHERIT already defined for code template: " << n->toString() << "; new definition:" << m1->toString() << "**vs.**" << "Original definition: " << m2->toString(); EXIT(msg.str()); }
#define XFORM_CONFIG_INCORRECT(op, n) { std:: stringstream msg; msg << "Incorrect configuration for " << op << " : "  << n ;  EXIT(msg.str()); }
#define XFORM_PARAM_INCORRECT(op, n) { std:: stringstream msg; msg << "Incorrect parameter for " << op << " : "  << n ;  EXIT(msg.str()); }
#define ASSIGN_INCORRECT(n1,n2) { std:: stringstream msg; msg << "Invalid assignment from " << SHORT(n2->toString(),100) << " to " << SHORT(n1->toString(),100) ; EXIT(msg.str()); } 
#define PARSE_MISMATCH_LEFT(r3,ln) { std:: stringstream msg; msg << "Error: type match has leftover at line " << ln << ": " << SHORT((r3)->toString(),100) ;  EXIT(msg.str()); } 
#define CVAR_MISMATCH(f,p,a) { std:: stringstream msg; msg << "Code template parameter mismatch. \nCode template is: " << (f)->toString() << ";\n Parameter is: " << ((p==0)?"NULL":p->toString()) << ";\nArgument is: " + ((a==0)?"NULL": (a)->toString()) ;   EXIT(msg.str()); } 
#define CVAR_ACC_MISMATCH(t,a) { std:: stringstream msg; msg << "code template access mismatch: " << "\n   code template is: " << (t==0? "NULL" : (t)->get_className() +":" + (t)->toString()) << ";\n  Accessor is: [" << ((a==0)?"NULL": (a)->toString()) << "]"; EXIT(msg.str()); } 
#define TUPLE_ACC_MISMATCH(ta, t,a) { std:: stringstream msg; msg << "tuple-accessor mismatch: " << ta->toString() << "\n   Tuple is: " << (t==0? "NULL" : (t)->get_className() +":" + (t)->toString()) << ";\n  Accessor is: [" << ((a==0)?"NULL": (a)->toString()) << "]"; EXIT(msg.str()); } 
#define FUNC_MISMATCH(f,a) { std:: stringstream msg; msg << "Parameter-argument mismatch in function call: " << (f) << "\nIncorrect argument: " << (a)->get_className() << ":" << SHORT((a)->toString(),300) ; EXIT(msg.str()); } 
#define INCORRECT_FNAME(n) { std:: stringstream msg; msg << "Error"; msg << ": cannot open file name \"" << n << "\"";  EXIT(msg.str()); }
#define INCORRECT_STRING(n) { std:: stringstream msg; msg << "Error: expecting an string value, getting " << n ;  EXIT(msg.str()); }
#define INCORRECT_ICONST(n) { std:: stringstream msg; msg << "Error: expecting an integer value, getting " << n ;  EXIT(msg.str()); }
#define INCORRECT_TRACE_HANDLE(n) { std:: stringstream msg; msg << "Error: expecting a tracing handle, getting " << n ;  EXIT(msg.str()); }
#define INCORRECT_VAR(n) { std:: stringstream msg; msg << "Error: expecting a variable, getting " << n ;  EXIT(msg.str()); }
#define INCORRECT_XFORM_DEF(n,m) { std:: stringstream msg; msg << "Error: expecting xform routine interface: " << n << ", getting " << m ;  EXIT(msg.str()); }
#define INCORRECT_XFORM_CODE(m) { std:: stringstream msg; msg << "Error: expecting a code template or xform routine, getting " << m ;  EXIT(msg.str()); }
#define INCORRECT_CVAR(m) { std:: stringstream msg; msg << "Error: expecting a code template, getting " << m->toString(OUTPUT_VAR_VAL) ;  EXIT(msg.str()); }
#define INCORRECT_XVAR(m) { std:: stringstream msg; msg << "Error: expecting a xform routine, getting " << m->toString(OUTPUT_VAR_VAL) ;  EXIT(msg.str()); }
#define CODEDEF_INCORRECT(code1,code2,file,line2) { std:: stringstream msg; msg << "Code definition error: start with " << (code1)->toString() << " but end with " << (code2)->toString() <<  " at " << line2 << " of file " << file ; EXIT(msg.str()); }
#define PARSE_INCORRECT(arg) { std:: stringstream msg; msg << "Parse definition error: " << "expecting three specifications for binary, unary and assignments, getting" << (arg)->toString() ; EXIT(msg.str()); }
#define LEX_INCORRECT(arg) { std::stringstream msg; msg << "Tokenizer error: unrecognized token specification: " << arg->toString(); EXIT(msg.str()); }
#define CASE_NOTFOUND(n) { std:: stringstream msg; msg << "Error: no matching case found for: " << (n)->toString() ; EXIT(msg.str()); }
#define INCORRECT_TUPLE_SIZE(r1,size) { std:: stringstream msg; msg << "Error: wrong tuple size. Expecting " << size << " elements but getting" << (r1)->toString() ; EXIT(msg.str()); }
#define LOOKAHEAD_AMBIGUOUS(r1) { std:: stringstream msg; msg << "Warning: ambiguity in determining the lookahead information for " << r1->toString(); /*EXIT(msg.str());*/ }
#define LOOKAHEAD_EMPTY(r1) { std:: stringstream msg; msg << "Error: empty string encountered when computing lookahead information for " << r1->toString(); EXIT(msg.str()); }
#define REPL_INCOMPLETE(left) { std:: stringstream msg; msg << "Error: incomplete replacement. Did not find a place to apply: " << left->toString(); EXIT(msg.str()); }
#define TRACE_INCOMPLETE(left) { std:: stringstream msg; msg << "Error: incomplete tracing. Did not find a place to insert trace handle: " << left->toString(); EXIT(msg.str()); }
#endif

