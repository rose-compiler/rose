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

#ifndef AST_EVAL_H
#define AST_EVAL_H

#include <ASTfactory.h>

/******************************Utility functions *********************/
#define IS_SPACE(c)  c == " " || c == "\t" || c == "\n" 
#define ANY ASTFactory::inst()->make_any()

#define PAIR(v1,v2) ASTFactory::inst()->new_pair(v1,v2)
#define TUPLE3(v1,v2,v3) ASTFactory::inst()->append_tuple(PAIR(v1,v2), v3)
#define CODE_REF(v1,v2) ASTFactory::inst()->build_codeRef(v1,v2,false)

extern POETProgram* curfile;


inline POETCode* get_head(POETCode* c)
{
   POETList* l = dynamic_cast<POETList*>(c);
   if (l != 0) { c = l->get_first(); }  
   return c;
}

inline POETList* get_tail(POETCode* c)
{
   POETList* l = dynamic_cast<POETList*>(c);
   if (l == 0) return 0;
   return l->get_rest();
}

inline POETCode* Vector2List(const std::vector<POETCode*>& v)
{
   if (v.size() == 0) return EMPTY;
   if (v.size() == 1) return v[0];
   POETCode* result = 0;
   for (int i = v.size()-1; i >= 0; --i) 
          result = ASTFactory::inst()->new_list(v[i], result);
   return result;
}

inline void List2Vector(POETList* l, std::vector<POETCode*>& v) {
   while (l != 0) {
     v.push_back(l->get_first());
     l = dynamic_cast<POETList*>(l->get_rest());
   }
}

inline void check_localVar(POETCode* v)
{
  if (v->get_enum() == SRC_LVAR) {
     POETCode* code = static_cast<LocalVar*>(v)->get_entry().get_code();
     if (code == 0 || code == v)
        SYM_UNDEFINED(v->toString());
  }
}
inline POETString* AST2String(POETCode* r)
{
  switch (r->get_enum()) {
  case SRC_STRING:  return static_cast<POETString*>(r);
  case SRC_ICONST: return ASTFactory::inst()->new_string(r->toString());
  case SRC_LIST: 
      {
      for (POETList* cur = static_cast<POETList*>(r);
           cur != 0; cur = cur->get_rest())
      {
         POETString* cur_string =  AST2String(cur->get_first());
         if (cur_string == 0) return 0;
      }
      return ASTFactory::inst()->new_string(r->toString(OUTPUT_NO_DEBUG));
     }
  default: return 0;
  }
}

inline POETIconst* AST2Iconst(POETCode* code)
{
   POETEnum t = code->get_enum();
   if (t == SRC_LIST) {
       POETList* l = static_cast<POETList*>(code);
       if (l->get_rest() != 0) return 0;
       code = l->get_first(); t = code->get_enum();
   }
   switch (t) {
   case SRC_ICONST: return static_cast<POETIconst*>(code);
   case SRC_STRING:  {
          std::string content = static_cast<POETString*>(code)->get_content();
          return string2Iconst(content.c_str(), content.size());
          }
   default: return 0;
   }
}
int inline AST2Int(POETCode* _r)
{
  _r = EvalTrace(_r);
  POETCode* r = AST2Iconst(_r);
  if (r != 0) {
     int v = static_cast<POETIconst*>(r)->get_val();
     return v;
  }
  INCORRECT_ICONST(_r->toString());
}
typedef enum {MATCH_PAR_NOMOD =0, MATCH_PAR_MOD_CODE=1, MATCH_PAR_MOD_RESTR=2}
                MatchParameterConfig;
bool match_parameters(POETCode* _pars, POETCode* _args, MatchParameterConfig modpar);


/************************** building various AST ************************/
inline CodeVar* build_codeRef(POETCode* r1, POETCode* r2, bool overwrite)
{
         CodeVar* r1Code = dynamic_cast<CodeVar*>(r1);
         if (r1Code == 0) CODE_UNDEFINED(r1->get_className()+":"+r1->toString());
         POETCode* pars = r1Code->get_entry().get_param();
         if (pars != 0 && !match_parameters(pars, r2, MATCH_PAR_NOMOD))
                 CVAR_MISMATCH(r1Code, pars, r2);
         return ASTFactory::inst()->build_codeRef(r1Code,r2, overwrite);
}

inline POETCode* MakeXformList( POETCode* car, POETCode *cdr, bool mod=true)
{ 
  if (car->get_enum() == SRC_XVAR) {
     XformVar*  carOp = static_cast<XformVar*>(car); 
     if (carOp->get_args() == 0) {
       car = get_head(cdr); cdr = get_tail(cdr);
       //Get_ListContent(cdr, &car, &cdr);
       XformVarInvoke* invoke = dynamic_cast<XformVarInvoke*>(carOp);
       if (mod && invoke != 0) { invoke->set_args(car); }
       else { carOp = new XformVarInvoke(carOp->get_entry(),0, car); } 
       if (cdr == 0) return carOp;
       return ASTFactory::inst()->new_list(carOp, cdr);    
     }
  }
  return 0;
}

/*************************interface functions ********************************/
/* initialization of POET interpreter */
int initialize(int argc, char** argv);

/* read an input program ( must be done after initialization) */
POETProgram* process_file(const char* fname);

/* interprete the input code and returns the evaluation result. */
POETCode* eval_AST(POETCode* code);
void eval_foreach(POETCode* input, POETCode* pattern, 
                  POETCode* found, POETCode* body, bool backward);
/* duplicate sel by dup in input */
POETCode* eval_duplicate(POETCode* sel, POETCode* dup, POETCode* input);
POETCode* eval_copy(POETCode* input); /* no tracing handles in the copy*/
POETCode* eval_rebuild(POETCode* input); /* rebuild input */
POETCode* eval_replace(POETCode* config, POETCode *input);
POETCode* eval_replace(POETCode* opd1, POETCode * opd2, POETCode* opd3);
/*apply binary "op" to integer operands "op1" and "op2"*/
int apply_Bop(POETOperatorType op, POETCode* op1, POETCode* op2);
/*apply a unary operation "op" to integer operand "op1"*/
int apply_Uop(POETOperatorType op, POETCode* op1);

/* split the strings in "code" immediately after the first "prefix" 
   characters. Return the first part, modify $left$ with the rest of code*/
POETCode* split_prefix(POETCode* code, unsigned& prefix, POETCode*& left);
/* split the strings in input "code" using separator "split"*/
POETCode* split_string(const std::string& split, POETCode* code);

/****************pattern matching  ********************/
/* match "input" according against the structure of "pattern". 
   if successful, return the matched pattern, save leftOver fragment.
   if matching fails: return NULL(0).  */
typedef enum {MATCH_AST_EQ, MATCH_AST_PATTERN} MatchOption;
POETCode* match_AST(POETCode* input, POETCode* pattern, MatchOption config);

/* assign rhs to lhs via pattern matching; return modified lhs; 
   Report error if matching is impossible */
POETCode* assign_AST(POETCode* rhs, POETCode* lhs);
/* determine whether "code" fall within the "range" declared for it
   if "range" is a type, does the type conversion */ 
POETCode* check_range(POETCode* code, POETCode* range); 


/****************parsing and unparsing ********************/
/* QY: return the code-template-structured result from parsing 
 if fails, or if leftOver==0 but match has leftover,throw exception*/
POETCode* parse_AST(POETCode* input,POETCode* target,POETCode** leftOver=0);

/* unparse $input$ to $out$; 
  if $unparseInvoke$!=0, invoke it using input before unparsing result*/
void code_gen(std::ostream& out, POETCode* input, POETCode* unparseInvoke=0);

/* print the given AST tree to $out$ */
void print_AST(std::ostream& out, POETCode* code);

/********************* Utillity classes *********************/
class EvaluatePOET
{
 protected:
   static POETCode*  lp, *rp, *lb, *rb, *space, *linebreak, *tab, *comma;
   static ASTFactory* fac;
   static LocalVar* exp_item, *tokens, *funcall, *arrref, *parseBop, *parseUop;
   static LocalVar *buildUop, *buildBop, *exp_bop, *exp_uop, *keywords;
   static LocalVar* prep, *parseTarget, *unparseTarget;
 public: 
   EvaluatePOET() {
     if (fac == 0) { /*QY: none have been initialized*/
        fac = ASTFactory::inst();
        lp = fac->new_string("("); rp = fac->new_string(")");
        lb = fac->new_string("["); rb = fac->new_string("]");
        space = fac->new_string(" ");
        linebreak = fac->new_string("\n");
        tab = fac->new_string("\t");
        comma =  fac->new_string(",");
        exp_item = curfile->make_macroVar(fac->new_string("EXP_BASE"));
        tokens = curfile->make_macroVar(fac->new_string("TOKEN")); 
        arrref= curfile->make_macroVar(fac->new_string("PARSE_ARRAY"));
        funcall = curfile->make_macroVar(fac->new_string("PARSE_CALL"));
        parseBop=curfile->make_macroVar(fac->new_string("PARSE_BOP"));
        parseUop=curfile->make_macroVar(fac->new_string("PARSE_UOP"));
        prep=curfile->make_macroVar(fac->new_string("PREP"));
        keywords=curfile->make_macroVar(fac->new_string("KEYWORDS"));
        parseTarget=curfile->make_macroVar(fac->new_string("PARSE"));
        unparseTarget=curfile->make_macroVar(fac->new_string("UNPARSE"));
        buildBop=curfile->make_macroVar(fac->new_string("BUILD_BOP"));
        buildUop=curfile->make_macroVar(fac->new_string("BUILD_UOP"));
        exp_uop=curfile->make_macroVar(fac->new_string("EXP_UOP"));
        exp_bop=curfile->make_macroVar(fac->new_string("EXP_BOP"));
     }
   }
   POETCode* SkipEmpty(POETCode* input, int *lineno=0) 
   {
     for (POETCode* p_input=input; p_input != 0; p_input=get_tail(p_input)) 
      {
         POETCode* cur = get_head(p_input);
         if (lineno != 0 && cur == linebreak) ++(*lineno); 
         if (cur!=EMPTY && cur!=space && cur != linebreak && cur != tab)
             return p_input;
      }
      return EMPTY;
   }

   POETCode* NextToken(POETCode* input) 
   { return SkipEmpty(get_tail(input)); }

   POETCode* FirstToken(POETCode* c)
   {
     POETList* l = dynamic_cast<POETList*>(c);
     while (l != 0) {
      c = l->get_first();
      l = dynamic_cast<POETList*>(c);
     }  
     return c;
   }

   /* apply unary arithematic/comparison "op" to "op1" 
     build a code template object if parse=true and op1 has unknown value*/
   POETCode* build_Uop(POETOperatorType op, POETCode* op1, bool parse);

   /* apply binary arithematic/comparison operation "op" to "op1" and "op2".  
     build and return a code template object if parse=true*/
   POETCode* build_Bop(POETOperatorType op, POETCode* op1, POETCode* op2, bool parse);

   /*QY: return the min length of tokens placed in the lookehead res*/
   unsigned compute_exp_lookahead(std::vector<POETCode*>& res);

   /*QY: return the min length of tokens placed in the lookehead res*/
   unsigned compute_lookaheadInfo(POETCode* cur, std::vector<POETCode*>& res, 
                           unsigned numOfFilterTokens=1);
   /*QY: whether r1 can be parsed using cur_filter as lookahead */
   bool match_lookahead(POETCode* r1, POETCode* cur_filter);

   /*QY: whether input matches the given type; return 0 if fails */
   POETCode* match_Type(POETCode* input, POETType* pattern, bool convertType);

   /*QY apply a given token pattern to the head of input */
   POETCode* apply_tokenFilter(POETCode* pattern, POETList* input, 
                               POETCode*& leftOver, bool make_string=true);
   /* apply the token filter function to input */
   POETCode* apply_tokens(POETCode* token, POETList* input);
   POETCode* apply_tokens(POETList* input) 
      { return apply_tokens(tokens->get_entry().get_code(), input); }

    /* QY: apply prep, tokenizer and parse target before invoking parse_AST*/
   POETCode* parse_input(POETCode* input, POETCode* pattern);

   /*QY: evaluate the readInput command */
   POETCode* eval_readInput(POETCode* readinput);

   /*QY: evaluate the writeOutput command */
   POETCode* eval_writeOutput(POETCode* output);

   /*QY: evaluate a given POET program */
   POETCode* eval_program(POETProgram* prog);
};

#endif
