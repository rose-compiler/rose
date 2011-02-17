%{

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
<INITIAL,C,BDWY,META>^{hwhite}*"#"{hwhite}*line.*"\n" { adjust_line_number(anntext); }

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

<INITIAL>.       { annlval.t_tok = (annlineno - line_number_offset); return anntext[0]; }

        /* -- Broadway annotation tokens ------------------------------------------------------ */

<BDWY>"property"       { annlval.t_tok = (annlineno - line_number_offset); return tokMAYPROPERTY; }
<BDWY>"may-property"   { annlval.t_tok = (annlineno - line_number_offset); return tokMAYPROPERTY; }
<BDWY>"must-property"  { annlval.t_tok = (annlineno - line_number_offset); return tokMUSTPROPERTY; }
<BDWY>"initially"      { annlval.t_tok = (annlineno - line_number_offset); return tokINITIALLY; }
<BDWY>"diagnostic"     { annlval.t_tok = (annlineno - line_number_offset); return tokDIAGNOSTIC; /* TB new */ }

<BDWY>"global"         { annlval.t_tok = (annlineno - line_number_offset); return tokGLOBAL; }

<BDWY>"procedure"      { annlval.t_tok = (annlineno - line_number_offset);
                         BEGIN(BDWYID);
                         return tokPROCEDURE; }

<BDWY>"enable"         { annlval.t_tok = (annlineno - line_number_offset); return tokENABLE; }
<BDWY>"disable"        { annlval.t_tok = (annlineno - line_number_offset); return tokDISABLE; }

<BDWYID>{identifier}   { annlval.t_id = new parserID(anntext, (annlineno - line_number_offset));
                         BEGIN(BDWY);
                         return tokID; }

<BDWY>"access"         { annlval.t_tok = (annlineno - line_number_offset); return tokACCESS; }
<BDWY>"modify"         { annlval.t_tok = (annlineno - line_number_offset); return tokMODIFY; }

<BDWY>"analyze"        { annlval.t_tok = (annlineno - line_number_offset); return tokANALYZE; }
<BDWY>"default"        { annlval.t_tok = (annlineno - line_number_offset); return tokDEFAULT ; }
<BDWY>"if"             { annlval.t_tok = (annlineno - line_number_offset); return tokIF; }

<BDWY>"is-exactly"     { annlval.t_tok = (annlineno - line_number_offset); return tokIS_EXACTLY; }
<BDWY>"could-be"       { annlval.t_tok = (annlineno - line_number_offset); return tokCOULD_BE; }

<BDWY>"is-atleast"     { annlval.t_tok = (annlineno - line_number_offset); return tokIS_ATLEAST; }
<BDWY>"is-atmost"      { annlval.t_tok = (annlineno - line_number_offset); return tokIS_ATMOST; }
<BDWY>"is-??"          { annlval.t_tok = (annlineno - line_number_offset); return tokIS_BOTTOM; }

<BDWY>"@before"        { annlval.t_tok = (annlineno - line_number_offset); return tokBEFORE; }
<BDWY>"@after"         { annlval.t_tok = (annlineno - line_number_offset); return tokAFTER; }
<BDWY>"@always"        { annlval.t_tok = (annlineno - line_number_offset); return tokALWAYS; }
<BDWY>"@ever"          { annlval.t_tok = (annlineno - line_number_offset); return tokEVER; }
<BDWY>"@trace"         { annlval.t_tok = (annlineno - line_number_offset); return tokTRACE; }
<BDWY>"@confidence"    { annlval.t_tok = (annlineno - line_number_offset); return tokCONFIDENCE; }

<BDWY>"@forward"       { annlval.t_tok = (annlineno - line_number_offset); return tokFORWARD; }
<BDWY>"@backward"      { annlval.t_tok = (annlineno - line_number_offset); return tokBACKWARD; }

<BDWY>"is-aliasof"     { annlval.t_tok = (annlineno - line_number_offset); return tokIS_ALIASOF; }
<BDWY>"is-sameas"      { annlval.t_tok = (annlineno - line_number_offset); return tokIS_SAMEAS; }
<BDWY>"is-empty"       { annlval.t_tok = (annlineno - line_number_offset); return tokIS_EMPTY; }

<BDWY>"on_entry"       { annlval.t_tok = (annlineno - line_number_offset); return tokON_ENTRY; }
<BDWY>"on_exit"        { annlval.t_tok = (annlineno - line_number_offset); return tokON_EXIT; }
<BDWY>"new"            { annlval.t_tok = (annlineno - line_number_offset); return tokNEW; }
<BDWY>"-->"            { annlval.t_tok = (annlineno - line_number_offset); return tokARROW; }
<BDWY>"delete"         { annlval.t_tok = (annlineno - line_number_offset); return tokDELETE; }

<BDWY>"pattern"        { annlval.t_tok = (annlineno - line_number_offset); return tokPATTERN; }

<BDWY>"&&"             { annlval.t_tok = (annlineno - line_number_offset); return tokANDAND; }
<BDWY>"||"             { annlval.t_tok = (annlineno - line_number_offset); return tokOROR; }

<BDWY>"!="             { annlval.t_tok = (annlineno - line_number_offset); return tokNOT_EQUALS; }
<BDWY>"=="             { annlval.t_tok = (annlineno - line_number_offset); return tokEQUALS_EQUALS; }
<BDWY>"<="             { annlval.t_tok = (annlineno - line_number_offset); return tokLESS_EQUALS; }
<BDWY>">="             { annlval.t_tok = (annlineno - line_number_offset); return tokGREATER_EQUALS; }

<BDWY>"is-#"           { annlval.t_tok = (annlineno - line_number_offset); return tokIS_CONSTANT; }

<BDWY>"="              { annlval.t_tok = (annlineno - line_number_offset); return tokEQUALS; }

<BDWY>"<-"             { annlval.t_tok = (annlineno - line_number_offset); return tokASSIGN; }
<BDWY>"<-+"            { annlval.t_tok = (annlineno - line_number_offset); return tokWEAKASSIGN; }

<BDWY>"report"        { annlval.t_tok = (annlineno - line_number_offset); return tokREPORT; }
<BDWY>"error"         { annlval.t_tok = (annlineno - line_number_offset); return tokERROR; }
<BDWY>"++"            { annlval.t_tok = (annlineno - line_number_offset); return tokAPPEND; }

<BDWY>"when"          { annlval.t_tok = (annlineno - line_number_offset); return tokWHEN; }
<BDWY>"replace-with"  { annlval.t_tok = (annlineno - line_number_offset); return tokREPLACE; }
<BDWY>"inline"        { annlval.t_tok = (annlineno - line_number_offset); return tokINLINE; }

<BDWY>"{union-set}"     { annlval.t_tok = (annlineno - line_number_offset); return tokUNION_SET; }
<BDWY>"{intersect-set}" { annlval.t_tok = (annlineno - line_number_offset); return tokINTERSECT_SET; }
<BDWY>"{union-equivalence}" { annlval.t_tok = (annlineno - line_number_offset); return tokUNION_EQUIV; }
<BDWY>"{intersect-equivalence}" { annlval.t_tok = (annlineno - line_number_offset); return tokINTERSECT_EQUIV; }

<BDWY>"is-element-of" { annlval.t_tok = (annlineno - line_number_offset); return tokIS_ELEMENT_OF; }
<BDWY>"is-{}"         { annlval.t_tok = (annlineno - line_number_offset); return tokIS_EMPTYSET; }

<BDWY>"set-add"       { annlval.t_tok = (annlineno - line_number_offset); return tokADD; }

<BDWY>{identifier}     { annlval.t_id = new parserID(anntext, (annlineno - line_number_offset));
                         return tokID;
                       }

<BDWY>"I/O"           { annlval.t_tok = (annlineno - line_number_offset); return tokIO; }

<BDWY>.              { annlval.t_tok = (annlineno - line_number_offset); return anntext[0]; }

        /* -- Embedded meta-tokens ------------------------------------------------------------ */

<META>"expr"        { annlval.t_tok = (annlineno - line_number_offset); return metatokEXPR; }
<META>"stmt"        { annlval.t_tok = (annlineno - line_number_offset); return metatokSTMT; }
<META>"type"        { annlval.t_tok = (annlineno - line_number_offset); return metatokTYPE; }

<META>{identifier}     { annlval.t_id = new parserID(anntext, (annlineno - line_number_offset));
                         return metatokID;
                       }

<META>.                { annlval.t_tok = (annlineno - line_number_offset); return anntext[0]; }

%%

// --- Adapted from parse_line_directive in lex.l

void adjust_line_number(char * s)
{
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
}
