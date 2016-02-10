
/* POET : Parameterized Optimizations for Empirical Tuning
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

//********************************************************************
//  Routines implementing recognition of additional tokens
//********************************************************************

#include <poet_ASTeval.h>
extern bool debug_lex();
extern "C" POETCode* make_inputlist( POETCode* car, POETCode *cdr);

POETCode* EvaluatePOET:: apply_tokens(POETCode* tokens, POETList* input)
{
   if (tokens != 0)
   {
     if (debug_lex()) 
         std::cerr << "apply token filter: " << ((input==0)? "" : input->toString()) << "\n";
      POETList* p_tokens=dynamic_cast<POETList*>(tokens); 
      POETCode* input1 = 0;
      POETCode* first_token=0;
      /* there is only a single token specifiation*/
      if (p_tokens == 0 || p_tokens->get_first()->get_enum() == SRC_STRING) 
            first_token = apply_tokenFilter(tokens,input, input1); 
      else { /* there is a list of token specs. */
         for ( ; first_token == 0 && p_tokens != 0; p_tokens = p_tokens->get_rest())
            first_token = apply_tokenFilter(p_tokens->get_first(),input,input1); 
      }
      assert(input1==0 || input1->get_enum() == SRC_LIST);
      if (first_token==0) return input;
      if (first_token != EMPTY)
      {
         POETCode* res = make_inputlist(first_token,input1);
         return res;
      }
      return input1;
   }
   return input;
}

POETCode* EvaluatePOET::
apply_tokenFilter(POETCode* pattern, POETList* input, 
                  POETCode*& leftOver, bool make_string)
{
     if (pattern == EMPTY) { leftOver=input; return pattern; }
     leftOver = input;
     switch (pattern->get_enum())
        {
        case SRC_STRING: {
            if (input == 0)   return 0;
            POETCode* cur_input = input->get_first();
            if (cur_input->get_enum() != SRC_STRING) return 0;
            std::string str_pat = static_cast<POETString*>(pattern)->get_content();
            std::string str_input = static_cast<POETString*>(cur_input)->get_content();
            size_t pos = str_input.find(str_pat); 
            if (pos == 0) {
              pos = str_pat.size();
              char nextchar = str_input[pos];
              if (str_input.size() == pos) {
                  leftOver=input->get_rest();
                  return pattern;
              }
              else if (nextchar <= '9' && nextchar >= '0') { /*QY: split identifier*/
                  leftOver=ASTFactory::inst()->new_string(str_input.substr(pos,str_input.size()-pos));
                  leftOver=ASTFactory::inst()->new_list(leftOver,input->get_rest());
                  return pattern;
              }
            }
            return 0;
           }
        case SRC_ICONST:
            if (input != 0 && input->get_first() == pattern) {
                leftOver = input->get_rest();
                return pattern;
            }
            return 0;
        case SRC_TYPE: {
            if (input != 0) { 
              POETCode* cur_token = get_head(input);
              if (match_Type(cur_token, static_cast<POETType*>(pattern), true))
                   leftOver = input->get_rest();
              else cur_token=0;
              return cur_token;
             }
             return 0;
          }
        case SRC_LVAR: {
          LvarSymbolTable::Entry entry = static_cast<LocalVar*>(pattern)->get_entry();
          POETCode* restr = entry.get_restr();
          if (restr == 0) { SYM_UNDEFINED(pattern->toString()); }
          POETCode* cur_token = apply_tokenFilter(restr, input, leftOver);
          if (cur_token != 0) { entry.set_code(cur_token); }
          return cur_token;
          }
        case SRC_LIST: {
            POETList* p_input = input;
            POETList* p_content = static_cast<POETList*>(pattern);
            std::string token_content;
            std::vector<POETCode*> match_res;
            for ( ; p_content != 0; p_content=p_content->get_rest()) { 
                POETCode* cur_token = apply_tokenFilter(p_content->get_first(), p_input, leftOver,make_string);
                if (cur_token == 0) return 0;
                assert(leftOver==0 || leftOver->get_enum() == SRC_LIST);
                p_input=dynamic_cast<POETList*>(leftOver); // leftOver must be a list
                match_res.push_back(cur_token);
                if (cur_token->get_enum() != SRC_STRING)
                    make_string=false;
                else token_content=token_content+static_cast<POETString*>(cur_token)->get_content();
            }
            if (make_string) return fac->new_string(token_content);
            if (match_res.size()==0) return 0;
            return Vector2List(match_res);
         }
         case SRC_CVAR: {  
             POETCode* first = (input==0)? 0 : input->get_first();
              CodeVar* cvar = static_cast<CodeVar*>(pattern);
             if (first != 0 && first->get_enum() == SRC_CVAR && static_cast<CodeVar*>(first)->get_entry() == cvar->get_entry()) { 
                 leftOver=input->get_rest();
                 return first;
             }
              POETCode* pars=cvar->get_entry().get_param();
              POETCode* code=cvar->get_entry().get_code();
              if (code == 0) code=cvar->get_entry().get_parse();
              if (code == 0) CODE_SYNTAX_UNDEFINED(cvar->toString()); 
              try { 
                   if (pars != 0) cvar->get_entry().get_symTable()->push_table(false);
                   POETCode* cur_token = apply_tokenFilter(code, input, leftOver, false); 
                   if (cur_token != 0) {
                       if (debug_lex()) 
std::cerr << "found code template: " << cur_token->toString() << "\n";
                      if (pars != 0) { pars=eval_AST(pars); }
                       cur_token= cvar->invoke_rebuild(pars);
                       if (cur_token == 0) { cur_token = fac->build_codeRef(cvar->get_entry(),pars); }
                    }
                   if (pars != 0)  cvar->get_entry().get_symTable()->pop_table();
                   return cur_token;
              }
              catch (Error err) { std::cerr << "From recognizing token: " << cvar->toString() << "\n"; throw err; }
            } 
         case SRC_OP: {
              POETOperator *op = static_cast<POETOperator*>(pattern);
              switch (op->get_op()) {
                case POET_OP_CONCAT: 
                  if (input != 0) {
                    POETCode* arg1 = op->get_arg(0), *arg2=op->get_arg(1);
                    if (arg1->get_enum() == SRC_STRING)
                        input=dynamic_cast<POETList*>(split_string(static_cast<POETString*>(arg1)->get_content(), input));
                    else if (arg2->get_enum() == SRC_STRING)
                        input=dynamic_cast<POETList*>(split_string(static_cast<POETString*>(arg2)->get_content(), input));
                    else LEX_INCORRECT(op);
                    POETCode* first=apply_tokenFilter(arg1,input,leftOver,make_string);
                    if (first == 0)  { leftOver=input; return 0; }
                    POETCode* second=apply_tokenFilter(arg2,dynamic_cast<POETList*>(leftOver),leftOver,make_string);
                    if (second == 0)  { leftOver=input; return 0; }
                    return fac->new_string(first->toString() + second->toString());
                  } 
                  return 0;
              case POET_OP_RANGE:
                 if (input != 0) {
                    POETCode* cur_token = get_head(input);
                    if (match_AST(cur_token, pattern, MATCH_AST_PATTERN)) 
                         leftOver = input->get_rest();
                    else cur_token=0;
                    return cur_token;
                  }
                  return 0;
              case TYPE_LIST1:
              case TYPE_LIST: {
                  POETCode* arg = op->get_arg(0);
                  std::vector<POETCode*> match_res;
                  do {
                     POETCode* r1_first=apply_tokenFilter(arg,input,leftOver,make_string);
                     if (r1_first == 0)  { leftOver=input; break; }
                     match_res.push_back(r1_first);
                     input = dynamic_cast<POETList*>(leftOver);
                  } while (input != 0);
                  if (op->get_op() == TYPE_LIST1 && match_res.size()==0)
                      return 0;
                  return Vector2List(match_res);
                }
                case TYPE_TOR: {
                   for (unsigned i = 0; i < op->numOfArgs(); ++i)
                   {            
                      POETCode* arg=op->get_arg(i);
                      POETCode* res=apply_tokenFilter(arg,input,leftOver,make_string);
                      if (res != 0) return res; 
                      leftOver=input;
                   }
                 }
                 return 0;
                default: LEX_INCORRECT(pattern);
               }
            }
         default: 
            LEX_INCORRECT(pattern);
      }
}

POETCode* EvaluatePOET::
eval_readInput(POETCode* inputFiles, POETCode* codeType, POETCode* inputInline)
{
   POETCode* res = 0; 
   if (inputFiles != 0) {
      std::list<POETProgram*> inputPrograms;
      ReadFiles(eval_AST(inputFiles), inputPrograms);
      std::list<POETProgram*>::reverse_iterator p = inputPrograms.rbegin(); 
      for ( ; p != inputPrograms.rend(); ++p) {
          POETProgram* prog = *p;
          POETProgram::const_iterator p=prog->begin();
          if (p != prog->end()) {
             POETCode* eval = *p;
             eval = parse_input( eval, codeType);  
             assert(eval != 0);
             if (res == 0) res = eval; 
             else res= make_inputlist(eval, res);
          } 
      }
   }
   if (inputInline != 0) {
      inputInline = parse_input( inputInline, codeType);
      if (!res) res = inputInline;
      else res = ASTFactory::inst()->new_list(res, inputInline);
   }
   return res;
}


