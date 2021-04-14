%{
#include <featureTests.h>
#ifdef ROSE_ENABLE_SOURCE_ANALYSIS

/* DQ (12/10/2016): This is a technique to suppress warnings in generated code that we want to be an error elsewhere in ROSE. 
   See https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html for more detail.
 */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include "broadway.h"
#include "language.tab.h"

#include "assert.h"

#include <stdlib.h>
#include <string.h>

// --- Fix the names from the C parser

#define cblval annlval
#define cbtext anntext
#define cbleng annleng

// --- Routines from the C parser for handling constants

void parse_line_directive(char *s);
void save_pragma_directive(char *s);

void EnterC(void);
void ExitC(void);

// --- Line number offset (to deal with preprocessing the code). We
// update this number when we see #line directives.

void adjust_line_number(char * text);

int line_number_offset = 0;

#endif
%}

        /* -- Prefix all symbols with "ann" */

%option prefix="ann"
%option noyywrap
%option yylineno

        /* -- Define the four states: "BDWY" for all Broadway annotations
           language tokens, "C" for C code fragments, and "META" for
           meta-symbols. The meta-symbols always embedded in the C
           patterns. The "BDWYID" state is used for proecdure names, which
           could conflict with reserved words in the annotation language. */

%s BDWY
%s BDWYID
%s C
%s META

        /* -- Define some useful short-hands */

identifier ([a-zA-Z_][0-9a-zA-Z_.]*)
any_white ([ \011\013\014\015])
hwhite ([ \011])

exponent_part ([eE][-+]?[0-9]+)
fractional_constant ([0-9]*"."[0-9]+)|([0-9]+".")
floating_constant ((({fractional_constant}{exponent_part}?)|([0-9]+{exponent_part}))[FfLl]?)

integer_suffix_opt ([uU]?[lL]?[lL]?)|([lL][uU][lL]?)|([lL][lL][uU])
decimal_constant ("0"{integer_suffix_opt}|[1-9][0-9]*{integer_suffix_opt})
octal_constant ("0"[0-7]*{integer_suffix_opt})
hex_constant ("0"[xX][0-9a-fA-F]+{integer_suffix_opt})

simple_escape ([abfnrtv'"?\\])
octal_escape  ([0-7][^'\n]*)
hex_escape ("x"[^'\n]*)
bad_escape ([\\][^abfnrtv'"?\\x0-7\n ])

escape_sequence ([\\]({simple_escape}|{octal_escape}|{hex_escape}))
c_char ([^'\\\n]|{escape_sequence})
s_char ([^"\\\n]|{escape_sequence})

%%

        /* -- General rules for handling line number directives */

<INITIAL,C,BDWY,META>^{hwhite}*"#"{hwhite}*[0-9].*"\n" |
<INITIAL,C,BDWY,META>^{hwhite}*"#"{hwhite}*line.*"\n"
                {
                    adjust_line_number(anntext);
                }

        /* -- State transition (context sensitivity) rules */

<INITIAL,BDWY>"%{"  { BEGIN(C); }
<C>"}%"             { BEGIN(BDWY); }

<C>"${"             { BEGIN(META); }
<META>"}"           { BEGIN(C); }

        /* -- General rules for handling white-space (including regular line number
              adjustment */

{any_white}+     {  }

"\n"             {  }

"//".*"\n"       {  }

<INITIAL>.       {
                 #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                     annlval.t_tok = (annlineno - line_number_offset); return anntext[0];
                 #endif
                 }

        /* -- Broadway annotation tokens ------------------------------------------------------ */

<BDWY>"property"       {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_tok = (annlineno - line_number_offset); return tokMAYPROPERTY;
                        #endif
                       }
<BDWY>"may-property"   {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_tok = (annlineno - line_number_offset); return tokMAYPROPERTY;
                        #endif
                       }
<BDWY>"must-property"  {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_tok = (annlineno - line_number_offset); return tokMUSTPROPERTY;
                        #endif
                       }
<BDWY>"initially"      {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_tok = (annlineno - line_number_offset); return tokINITIALLY;
                        #endif
                       }
<BDWY>"diagnostic"     {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_tok = (annlineno - line_number_offset); return tokDIAGNOSTIC; /* TB new */
                        #endif
                       }

<BDWY>"global"         {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_tok = (annlineno - line_number_offset); return tokGLOBAL;
                        #endif
                       }

<BDWY>"procedure"      {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_tok = (annlineno - line_number_offset);
                         BEGIN(BDWYID);
                         return tokPROCEDURE;
                        #endif
                       }

<BDWY>"enable"         {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_tok = (annlineno - line_number_offset); return tokENABLE;
                        #endif
                       }
<BDWY>"disable"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_tok = (annlineno - line_number_offset); return tokDISABLE;
                        #endif
                       }

<BDWYID>{identifier}   {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                         annlval.t_id = new parserID(anntext, (annlineno - line_number_offset));
                         BEGIN(BDWY);
                         return tokID;
                        #endif
                       }

<BDWY>"access"         {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokACCESS;
                        #endif
                       }
<BDWY>"modify"         {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokMODIFY;
                        #endif
                       }

<BDWY>"analyze"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokANALYZE;
                        #endif
                       }
<BDWY>"default"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokDEFAULT ;
                        #endif
                       }
<BDWY>"if"             {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIF;
                        #endif
                       }

<BDWY>"is-exactly"     {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_EXACTLY;
                        #endif
                       }
<BDWY>"could-be"       {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokCOULD_BE;
                        #endif
                       }

<BDWY>"is-atleast"     {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_ATLEAST;
                        #endif
                       }
<BDWY>"is-atmost"      {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_ATMOST;
                        #endif
                       }
<BDWY>"is-??"          {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_BOTTOM;
                        #endif
                       }

<BDWY>"@before"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokBEFORE;
                        #endif
                       }
<BDWY>"@after"         {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokAFTER;
                        #endif
                       }
<BDWY>"@always"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokALWAYS;
                        #endif
                       }
<BDWY>"@ever"          {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokEVER;
                        #endif
                       }
<BDWY>"@trace"         {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokTRACE;
                        #endif
                       }
<BDWY>"@confidence"    {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokCONFIDENCE;
                        #endif
                       }

<BDWY>"@forward"       {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokFORWARD;
                        #endif
                       }
<BDWY>"@backward"      {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokBACKWARD;
                        #endif
                       }

<BDWY>"is-aliasof"     {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_ALIASOF;
                        #endif
                       }
<BDWY>"is-sameas"      {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_SAMEAS;
                        #endif
                       }
<BDWY>"is-empty"       {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_EMPTY;
                        #endif
                       }

<BDWY>"on_entry"       {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokON_ENTRY;
                        #endif
                       }
<BDWY>"on_exit"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokON_EXIT;
                        #endif
                       }
<BDWY>"new"            {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokNEW;
                        #endif
                       }
<BDWY>"-->"            {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokARROW;
                        #endif
                       }
<BDWY>"delete"         {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokDELETE;
                        #endif
                       }

<BDWY>"pattern"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokPATTERN;
                        #endif
                       }

<BDWY>"&&"             {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokANDAND;
                        #endif
                       }
<BDWY>"||"             {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokOROR;
                        #endif
                       }

<BDWY>"!="             {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokNOT_EQUALS;
                        #endif
                       }
<BDWY>"=="             {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokEQUALS_EQUALS;
                        #endif
                       }
<BDWY>"<="             {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokLESS_EQUALS;
                        #endif
                       }
<BDWY>">="             {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokGREATER_EQUALS;
                        #endif
                       }

<BDWY>"is-#"           {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_CONSTANT;
                        #endif
                       }

<BDWY>"="              {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokEQUALS;
                        #endif
                       }

<BDWY>"<-"             {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokASSIGN;
                        #endif
                       }
<BDWY>"<-+"            {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokWEAKASSIGN;
                        #endif
                       }

<BDWY>"report"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokREPORT;
                        #endif
                       }
<BDWY>"error"         {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokERROR;
                        #endif
                       }
<BDWY>"++"            {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokAPPEND;
                        #endif
                       }

<BDWY>"when"          {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokWHEN;
                        #endif
                       }
<BDWY>"replace-with"  {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokREPLACE;
                        #endif
                       }
<BDWY>"inline"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokINLINE;
                        #endif
                       }

<BDWY>"{union-set}"     {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokUNION_SET;
                        #endif
                       }
<BDWY>"{intersect-set}" {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokINTERSECT_SET;
                        #endif
                       }
<BDWY>"{union-equivalence}" {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokUNION_EQUIV;
                        #endif
                       }
<BDWY>"{intersect-equivalence}" {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokINTERSECT_EQUIV;
                        #endif
                       }

<BDWY>"is-element-of" {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_ELEMENT_OF;
                        #endif
                       }
<BDWY>"is-{}"         {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIS_EMPTYSET;
                        #endif
                       }

<BDWY>"set-add"       {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokADD;
                        #endif
                       }

<BDWY>{identifier}     {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_id = new parserID(anntext, (annlineno - line_number_offset));
                          return tokID;
                        #endif
                       }

<BDWY>"I/O"           {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return tokIO;
                        #endif
                       }

<BDWY>.              {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return anntext[0];
                        #endif
                       }

        /* -- Embedded meta-tokens ------------------------------------------------------------ */

<META>"expr"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return metatokEXPR;
                        #endif
                       }
<META>"stmt"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return metatokSTMT;
                        #endif
                       }
<META>"type"        {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return metatokTYPE;
                        #endif
                       }

<META>{identifier}     {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_id = new parserID(anntext, (annlineno - line_number_offset));
                         return metatokID;
                        #endif
                       }

<META>.                {
                        #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
                          annlval.t_tok = (annlineno - line_number_offset); return anntext[0];
                        #endif
                       }

%%

// --- Adapted from parse_line_directive in lex.l

void adjust_line_number(char * s)
{
#ifdef ROSE_ENABLE_SOURCE_ANALYSIS
 char *p;
  int val;

  // Read until a '#' sign
  for (p = s; *p != '\n' && *p != '#'; p++);
  assert(*p != '\n');

  // Read spaces and tabs
  for (p = p + 1; *p != '\n' && (*p == ' ' || *p == '\t'); p++);
  assert(*p != '\n');

  // Swallow "line" and any trailing whitespace
  if (strncmp(p, "line", 4) == 0) {
    p = p + 4;

    // There must be whitespace
    assert(*p == ' ' || *p == '\t');

    for (     ; *p != '\n' && (*p == ' ' || *p == '\t'); p++);
    assert(*p != '\n');
  }

  // Read an integer from the string
  if (isdigit(*p))
    for (val = 0; *p && isdigit(*p); p++)
      val = val * 10 + (*p - '0');
  else {
    fprintf(stderr, "#line error: expecting line number");
    return;
  }

  line_number_offset = annlineno - val;
#endif
}
