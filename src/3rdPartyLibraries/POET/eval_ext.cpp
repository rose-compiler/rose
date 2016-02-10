
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

/********************************************************************
  Routines for implementing evaluation of POET AST which can be substituted
with alternive implementations to connect POET with existing compilers.
These routines include:
  EvaluatePOET::eval_readInput_nosyntax
  POETAstInterface::Ast2String
  POETAstInterface::unparseToString
  POETAstInterface::MatchAstTypeName
  POETAstInterface::Ast2POET
  POETAstInterface::ReplaceChildren
********************************************************************/
#include <poet_ASTeval.h>
#include <poet_ASTinterface.h>

POETCode* EvaluatePOET::
eval_readInput_nosyntax(POETCode* inputFiles, POETCode* codeType, POETCode* inputInline)
{ 
  return eval_readInput(inputFiles, codeType,inputInline); 
}

bool POETAstInterface::MatchAstTypeName(const Ast& n, const std::string& name)
{ std::cerr << "Need to be implemented by external compilers!"; assert(0); }

std::string POETAstInterface::Ast2String(const Ast & n)
{ std::cerr << "Need to be implemented by external compilers!"; assert(0); }

void POETAstInterface::unparse(POETCode_ext *e, std::ostream& out, int align)
{ std::cerr << "Need to be implemented by external compilers!"; assert(0); }

POETCode* POETAstInterface::Ast2POET(const Ast& n) 
{ std::cerr << "Need to be implemented by external compilers!"; assert(0); }

POETCode* POETAstInterface::ReplaceChildren(POETCode_ext* ext, POETCode* new_children)
{ std::cerr << "Need to be implemented by external compilers!"; assert(0); }
