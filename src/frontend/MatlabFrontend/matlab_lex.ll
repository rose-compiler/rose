/*

Copyright (C) 1993-2011 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.
*/


/*%option prefix = "matlab"     */

%top {

}

%s COMMAND_START
%s MATRIX_START

%x SCRIPT_FILE_BEGIN
%x FUNCTION_FILE_BEGIN
%x TYPE_BEGIN

%{

#include <cctype>
#include <cstring>
#include <cassert>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <stack>

#include <sys/types.h>
#include <unistd.h>

#include "sage3basic.h"
#include "SymbolFinder.h"

class StatementList;
class MatlabFunctionBuilder;
  
#include "comment-list.h"
#include "error.h"
#include "input.h"
#include "lex.h"
#include "ourtoken.h"
#include "matlab_parse.h"
#include "octave.gperf.h"

// These would be alphabetical, but y.tab.h must be included before
// oct-gperf.h and y.tab.h must be included after token.h and the tree
// class declarations.  We can't include y.tab.h in oct-gperf.h
// because it may not be protected to allow it to be included multiple
// times.



#if defined (GNULIB_NAMESPACE)
// Calls to the following functions appear in the generated output from
// flex without the namespace tag.  Redefine them so we will use them
// via the gnulib namespace.
#define fprintf GNULIB_NAMESPACE::fprintf
#define fwrite GNULIB_NAMESPACE::fwrite
#define malloc GNULIB_NAMESPACE::malloc
#define realloc GNULIB_NAMESPACE::realloc
#endif

// The current input line number.
int input_line_number = 1;

// The column of the current token.
int current_input_column = 1;

// Buffer for help text snagged from function files.
std::stack<std::string> help_buf;

#if ! (defined (FLEX_SCANNER) \
       && defined (YY_FLEX_MAJOR_VERSION) && YY_FLEX_MAJOR_VERSION >= 2 \
       && defined (YY_FLEX_MINOR_VERSION) && YY_FLEX_MINOR_VERSION >= 5)
#error lex.l requires flex version 2.5.4 or later
#endif

/*#define yylval octave_lval*/

// Arrange to get input via readline.
/*
#ifdef YY_INPUT
#undef YY_INPUT
#endif
#define YY_INPUT(buf, result, max_size) \
  if ((result = octave_read (buf, max_size)) < 0) \
    YY_FATAL_ERROR ("octave_read () in flex scanner failed");*/

// Try to avoid crashing out completely on fatal scanner errors.
// The call to yy_fatal_error should never happen, but it avoids a
// `static function defined but not used' warning from gcc.
/*OCTAVE_QUIT;*/
#ifdef YY_FATAL_ERROR
#undef YY_FATAL_ERROR
#endif
#define YY_FATAL_ERROR(msg) \
  do \
    { \
      error (msg); \
      yy_fatal_error (msg); \
    } \
  while (0)

#define DISPLAY_TOK_AND_RETURN(tok) \
  do \
    { \
      int tok_val = tok; \
      if (Vdisplay_tokens) \
        display_token (tok_val); \
      if (lexer_debug_flag) \
        { \
          std::cerr << "R: "; \
          display_token (tok_val); \
          std::cerr << std::endl;  \
        } \
      return tok_val; \
    } \
  while (0)

#define COUNT_TOK_AND_RETURN(tok) \
  do \
    { \
      Vtoken_count++; \
      DISPLAY_TOK_AND_RETURN (tok); \
    } \
  while (0)

#define TOK_RETURN(tok) \
  do \
    { \
      current_input_column += yyleng; \
      lexer_flags.quote_is_transpose = false; \
      lexer_flags.convert_spaces_to_comma = true; \
      COUNT_TOK_AND_RETURN (tok); \
    } \
  while (0)

#define TOK_PUSH_AND_RETURN(name, tok) \
  do \
    { \
      yylval.tok_val = new token (name, input_line_number, \
                                  current_input_column); \
      token_stack.push (yylval.tok_val); \
      TOK_RETURN (tok); \
    } \
  while (0)

#define BIN_OP_RETURN(tok, convert, bos) \
  do \
    { \
      yylval.tok_val = new token (input_line_number, current_input_column); \
      token_stack.push (yylval.tok_val); \
      current_input_column += yyleng; \
      lexer_flags.quote_is_transpose = false; \
      lexer_flags.convert_spaces_to_comma = convert; \
      lexer_flags.looking_for_object_index = false; \
      lexer_flags.at_beginning_of_statement = bos; \
      COUNT_TOK_AND_RETURN (tok); \
    } \
  while (0)

#define XBIN_OP_RETURN(tok, convert, bos) \
  do \
    { \
      gripe_matlab_incompatible_operator (yytext); \
      BIN_OP_RETURN (tok, convert, bos); \
    } \
  while (0)

#define LEXER_DEBUG(pattern) \
  do \
    { \
      if (lexer_debug_flag) \
        lexer_debug (pattern, yytext); \
    } \
  while (0)

// TRUE means that we have encountered EOF on the input stream.
bool parser_end_of_input = false;

// Flags that need to be shared between the lexer and parser.
lexical_feedback lexer_flags;

// Stack to hold tokens so that we can delete them when the parser is
// reset and avoid growing forever just because we are stashing some
// information.  This has to appear before lex.h is included, because
// one of the macros defined there uses token_stack.
//
// FIXME -- this should really be static, but that causes
// problems on some systems.
std::stack <token*> token_stack;

// Did eat_whitespace() eat a space or tab, or a newline, or both?

typedef int yum_yum;

const yum_yum ATE_NOTHING = 0;
const yum_yum ATE_SPACE_OR_TAB = 1;
const yum_yum ATE_NEWLINE = 2;

// Is the closest nesting level a square bracket, squiggly brace or a paren?

class bracket_brace_paren_nesting_level
{
public:

  bracket_brace_paren_nesting_level (void) : context () { }

  ~bracket_brace_paren_nesting_level (void) { }

  void bracket (void) { context.push (BRACKET); }
  bool is_bracket (void)
    { return ! context.empty () && context.top () == BRACKET; }

  void brace (void) {  context.push (BRACE); }
  bool is_brace (void)
    { return ! context.empty () && context.top () == BRACE; }

  void paren (void) {  context.push (PAREN); }
  bool is_paren (void)
    { return ! context.empty () && context.top () == PAREN; }

  bool is_bracket_or_brace (void)
    { return (! context.empty ()
              && (context.top () == BRACKET || context.top () == BRACE)); }

  bool none (void) { return context.empty (); }

  void remove (void) { if (! context.empty ()) context.pop (); }

  void clear (void) { while (! context.empty ()) context.pop (); }

private:

  std::stack<int> context;

  static const int BRACKET;
  static const int BRACE;
  static const int PAREN;

  bracket_brace_paren_nesting_level (const bracket_brace_paren_nesting_level&);

  bracket_brace_paren_nesting_level&
  operator = (const bracket_brace_paren_nesting_level&);
};

const int bracket_brace_paren_nesting_level::BRACKET = 1;
const int bracket_brace_paren_nesting_level::BRACE = 2;
const int bracket_brace_paren_nesting_level::PAREN = 3;

static bracket_brace_paren_nesting_level nesting_level;

static bool Vdisplay_tokens = false;

static unsigned int Vtoken_count = 0;

// The start state that was in effect when the beginning of a block
// comment was noticed.
static int block_comment_nesting_level = 0;

// Internal variable for lexer debugging state.
static bool lexer_debug_flag = false;

// Forward declarations for functions defined at the bottom of this
// file.

static int text_yyinput (void);
static void xunput (char c, char *buf);
static void fixup_column_count (char *s);
static void do_comma_insert_check (void);
static int is_keyword_token (const std::string& s);
static int process_comment (bool start_in_block, bool& eof);
static bool match_any (char c, const char *s);
static bool next_token_is_sep_op (void);
static bool next_token_is_bin_op (bool spc_prev);
static bool next_token_is_postfix_unary_op (bool spc_prev);
static std::string strip_trailing_whitespace (char *s);
static void handle_number (void);
static int handle_string (char delim);
static int handle_close_bracket (bool spc_gobbled, int bracket_type);
static int handle_superclass_identifier (void);
static int handle_meta_identifier (void);
static int handle_identifier (void);
static bool have_continuation (bool trailing_comments_ok = true);
static bool have_ellipsis_continuation (bool trailing_comments_ok = true);
static void scan_for_comments (const char *);
static yum_yum eat_whitespace (void);
static yum_yum eat_continuation (void);
static void maybe_warn_separator_insert (char sep);
static void gripe_single_quote_string (void);
static void gripe_matlab_incompatible (const std::string& msg);
static void maybe_gripe_matlab_incompatible_comment (char c);
static void gripe_matlab_incompatible_continuation (void);
static void gripe_matlab_incompatible_operator (const std::string& op);
static void display_token (int tok);
static void lexer_debug (const char *pattern, const char *text);

%}

D       [0-9]
S       [ \t]
NL      ((\n)|(\r)|(\r\n))
SNL     ({S}|{NL})
EL      (\.\.\.)
BS      (\\)
CONT    ({EL}|{BS})
Im      [iIjJ]
CCHAR   [#%]
COMMENT ({CCHAR}.*{NL})
SNLCMT  ({SNL}|{COMMENT})
NOT     ((\~)|(\!))
POW     ((\*\*)|(\^))
EPOW    (\.{POW})
IDENT   ([_$a-zA-Z][_$a-zA-Z0-9]*)
EXPON   ([DdEe][+-]?{D}+)
NUMBER  (({D}+\.?{D}*{EXPON}?)|(\.{D}+{EXPON}?)|(0[xX][0-9a-fA-F]+))
%%

%{
// Make script and function files start with a bogus token. This makes
// the parser go down a special path.
%}

<SCRIPT_FILE_BEGIN>. {
    LEXER_DEBUG ("<SCRIPT_FILE_BEGIN>.");

    BEGIN (INITIAL);
    xunput (yytext[0], yytext);
    COUNT_TOK_AND_RETURN (SCRIPT_FILE);
  }

<FUNCTION_FILE_BEGIN>. {
    LEXER_DEBUG ("<FUNCTION_FILE_BEGIN>.");

    BEGIN (INITIAL);
    xunput (yytext[0], yytext);
    COUNT_TOK_AND_RETURN (FUNCTION_FILE);
  }

%{
// Help and other command-style functions.
%}

<COMMAND_START>{NL} {
    LEXER_DEBUG ("<COMMAND_START>{NL}");

    BEGIN (INITIAL);
    input_line_number++;
    current_input_column = 1;

    lexer_flags.quote_is_transpose = false;
    lexer_flags.convert_spaces_to_comma = true;
    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = true;

    COUNT_TOK_AND_RETURN ('\n');
  }

<COMMAND_START>[\;\,] {
    LEXER_DEBUG ("<COMMAND_START>[\\;\\,]");

    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = true;

    BEGIN (INITIAL);

    if (strcmp (yytext, ",") == 0)
      TOK_RETURN (',');
    else
      TOK_RETURN (';');
  }

<COMMAND_START>[\"\'] {
    LEXER_DEBUG ("<COMMAND_START>[\\\"\\']");

    lexer_flags.at_beginning_of_statement = false;

    current_input_column++;
    int tok = handle_string (yytext[0]);

    COUNT_TOK_AND_RETURN (tok);
  }

<COMMAND_START>[^#% \t\r\n\;\,\"\'][^ \t\r\n\;\,]*{S}* {
    LEXER_DEBUG ("<COMMAND_START>[^#% \\t\\r\\n\\;\\,\\\"\\'][^ \\t\\r\\n\\;\\,]*{S}*");

    std::string tok = strip_trailing_whitespace (yytext);

    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = false;

    TOK_PUSH_AND_RETURN (tok, SQ_STRING);
  }

%{
// For this and the next two rules, we're looking at ']', and we
// need to know if the next token is `=' or `=='.
//
// It would have been so much easier if the delimiters were simply
// different for the expression on the left hand side of the equals
// operator.
//
// It's also a pain in the ass to decide whether to insert a comma
// after seeing a ']' character...

// FIXME -- we need to handle block comments here.
%}

<MATRIX_START>{SNLCMT}*\]{S}* {
    LEXER_DEBUG ("<MATRIX_START>{SNLCMT}*\\]{S}*");

    scan_for_comments (yytext);
    fixup_column_count (yytext);

    lexer_flags.looking_at_object_index.pop_front ();

    lexer_flags.looking_for_object_index = true;
    lexer_flags.at_beginning_of_statement = false;

    int c = yytext[yyleng-1];
    int cont_is_spc = eat_continuation ();
    bool spc_gobbled = (cont_is_spc || c == ' ' || c == '\t');
    int tok_to_return = handle_close_bracket (spc_gobbled, ']');

    if (spc_gobbled)
      xunput (' ', yytext);

    COUNT_TOK_AND_RETURN (tok_to_return);
  }

%{
// FIXME -- we need to handle block comments here.
%}

<MATRIX_START>{SNLCMT}*\}{S}* {
    LEXER_DEBUG ("<MATRIX_START>{SNLCMT}*\\}{S}*");

    scan_for_comments (yytext);
    fixup_column_count (yytext);

    lexer_flags.looking_at_object_index.pop_front ();

    lexer_flags.looking_for_object_index = true;
    lexer_flags.at_beginning_of_statement = false;

    int c = yytext[yyleng-1];
    int cont_is_spc = eat_continuation ();
    bool spc_gobbled = (cont_is_spc || c == ' ' || c == '\t');
    int tok_to_return = handle_close_bracket (spc_gobbled, '}');

    if (spc_gobbled)
      xunput (' ', yytext);

    COUNT_TOK_AND_RETURN (tok_to_return);
  }

%{
// Commas are element separators in matrix constants.  If we don't
// check for continuations here we can end up inserting too many
// commas.
%}

<MATRIX_START>{S}*\,{S}* {
    LEXER_DEBUG ("<MATRIX_START>{S}*\\,{S}*");

    current_input_column += yyleng;

    int tmp = eat_continuation ();

    lexer_flags.quote_is_transpose = false;
    lexer_flags.convert_spaces_to_comma = true;
    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = false;

    if (! lexer_flags.looking_at_object_index.front ())
      {
        if ((tmp & ATE_NEWLINE) == ATE_NEWLINE)
          {
            maybe_warn_separator_insert (';');

            xunput (';', yytext);
          }
      }

    COUNT_TOK_AND_RETURN (',');
  }

%{
// In some cases, spaces in matrix constants can turn into commas.
// If commas are required, spaces are not important in matrix
// constants so we just eat them.  If we don't check for continuations
// here we can end up inserting too many commas.
%}

<MATRIX_START>{S}+ {
    LEXER_DEBUG ("<MATRIX_START>{S}+");

    current_input_column += yyleng;

    lexer_flags.at_beginning_of_statement = false;

    int tmp = eat_continuation ();

    if (! lexer_flags.looking_at_object_index.front ())
      {
        bool bin_op = next_token_is_bin_op (true);
        bool postfix_un_op = next_token_is_postfix_unary_op (true);
        bool sep_op = next_token_is_sep_op ();

        if (! (postfix_un_op || bin_op || sep_op)
            && nesting_level.is_bracket_or_brace ()
            && lexer_flags.convert_spaces_to_comma)
          {
            if ((tmp & ATE_NEWLINE) == ATE_NEWLINE)
              {
                maybe_warn_separator_insert (';');

                xunput (';', yytext);
              }

            lexer_flags.quote_is_transpose = false;
            lexer_flags.convert_spaces_to_comma = true;

            maybe_warn_separator_insert (',');

            COUNT_TOK_AND_RETURN (',');
          }
      }
  }

%{
// Semicolons are handled as row seprators in matrix constants.  If we
// don't eat whitespace here we can end up inserting too many
// semicolons.

// FIXME -- we need to handle block comments here.
%}

<MATRIX_START>{SNLCMT}*;{SNLCMT}* {
    LEXER_DEBUG ("<MATRIX_START>{SNLCMT}*;{SNLCMT}*");

    scan_for_comments (yytext);
    fixup_column_count (yytext);
    eat_whitespace ();

    lexer_flags.quote_is_transpose = false;
    lexer_flags.convert_spaces_to_comma = true;
    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = false;

    COUNT_TOK_AND_RETURN (';');
  }

%{
// In some cases, new lines can also become row separators.  If we
// don't eat whitespace here we can end up inserting too many
// semicolons.

// FIXME -- we need to handle block comments here.
%}

<MATRIX_START>{S}*{COMMENT}{SNLCMT}* |
<MATRIX_START>{S}*{NL}{SNLCMT}* {
    LEXER_DEBUG ("<MATRIX_START>{S}*{COMMENT}{SNLCMT}*|<MATRIX_START>{S}*{NL}{SNLCMT}*");

    scan_for_comments (yytext);
    fixup_column_count (yytext);
    eat_whitespace ();

    lexer_flags.quote_is_transpose = false;
    lexer_flags.convert_spaces_to_comma = true;
    lexer_flags.at_beginning_of_statement = false;

    if (nesting_level.none ())
      return LEXICAL_ERROR;

    if (! lexer_flags.looking_at_object_index.front ()
        && nesting_level.is_bracket_or_brace ())
      {
        maybe_warn_separator_insert (';');

        COUNT_TOK_AND_RETURN (';');
      }
  }

\[{S}* {
    LEXER_DEBUG ("\\[{S}*");

    nesting_level.bracket ();

    lexer_flags.looking_at_object_index.push_front (false);

    current_input_column += yyleng;
    lexer_flags.quote_is_transpose = false;
    lexer_flags.convert_spaces_to_comma = true;
    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = false;

    if (lexer_flags.defining_func
        && ! lexer_flags.parsed_function_name.top ())
      lexer_flags.looking_at_return_list = true;
    else
      lexer_flags.looking_at_matrix_or_assign_lhs = true;

    ////promptflag--;
    eat_whitespace ();

    lexer_flags.bracketflag++;
    BEGIN (MATRIX_START);
    COUNT_TOK_AND_RETURN ('[');
  }

\] {
    LEXER_DEBUG ("\\]");

    nesting_level.remove ();

    lexer_flags.looking_at_object_index.pop_front ();

    lexer_flags.looking_for_object_index = true;
    lexer_flags.at_beginning_of_statement = false;

    TOK_RETURN (']');
  }

%{
// Imaginary numbers.
%}

{NUMBER}{Im} {
    LEXER_DEBUG ("{NUMBER}{Im}");

    handle_number ();
    COUNT_TOK_AND_RETURN (IMAG_NUM);
  }

%{
// Real numbers.  Don't grab the `.' part of a dot operator as part of
// the constant.
%}

{D}+/\.[\*/\\^\'] |
{NUMBER} {
    LEXER_DEBUG ("{D}+/\\.[\\*/\\^\\']|{NUMBER}");
    handle_number ();
    COUNT_TOK_AND_RETURN (NUM);
  }

%{
// Eat whitespace.  Whitespace inside matrix constants is handled by
// the <MATRIX_START> start state code above.
%}

{S}* {
    current_input_column += yyleng;
  }

%{
// Continuation lines.  Allow comments after continuations.
%}

{CONT}{S}*{NL} |
{CONT}{S}*{COMMENT} {
    LEXER_DEBUG ("{CONT}{S}*{NL}|{CONT}{S}*{COMMENT}");

    if (yytext[0] == '\\')
      gripe_matlab_incompatible_continuation ();
    scan_for_comments (yytext);
    ////promptflag--;
    input_line_number++;
    current_input_column = 1;
  }

%{
// End of file.
%}

<<EOF>> {
    LEXER_DEBUG ("<<EOF>>");

    if (block_comment_nesting_level != 0)
      {
        warning ("block comment open at end of input");

        if ((reading_fcn_file || reading_script_file || reading_classdef_file)
            && ! curr_fcn_file_name.empty ())
          warning ("near line %d of file `%s.m'",
                   input_line_number, curr_fcn_file_name.c_str ());
      }

    TOK_RETURN (END_OF_INPUT);
  }

%{
//Types
%}

"%TYPE"{S}* {
  BEGIN(TYPE_BEGIN);
}

%{
//Types
%}

<TYPE_BEGIN>[a-zA-Z\<\>0-9]+{S}*({IDENT}{S}*)*{IDENT}{S}*{NL} {
  
    lexer_flags.addAnnotatedType(strdup(yytext));

    BEGIN(INITIAL);
  }

%{
// Identifiers.  Truncate the token at the first space or tab but
// don't write directly on yytext.
%}

{IDENT}{S}* {
    LEXER_DEBUG ("{IDENT}{S}*");

    int id_tok = handle_identifier ();

    if (id_tok >= 0)
      COUNT_TOK_AND_RETURN (id_tok);
  }

%{
// Superclass method identifiers.
%}

{IDENT}@{IDENT}{S}* |
{IDENT}@{IDENT}.{IDENT}{S}* {
    LEXER_DEBUG ("{IDENT}@{IDENT}{S}*|{IDENT}@{IDENT}.{IDENT}{S}*");

    int id_tok = handle_superclass_identifier ();

    if (id_tok >= 0)
      {
        lexer_flags.looking_for_object_index = true;

        COUNT_TOK_AND_RETURN (SUPERCLASSREF);
      }
  }

%{
// Metaclass query
%}

\?{IDENT}{S}* |
\?{IDENT}.{IDENT}{S}* {
    LEXER_DEBUG ("\?{IDENT}{S}* | \?{IDENT}.{IDENT}{S}*");

    int id_tok = handle_meta_identifier ();

    if (id_tok >= 0)
      {
        lexer_flags.looking_for_object_index = true;

        COUNT_TOK_AND_RETURN (METAQUERY);
      }
  }

%{
// Function handles and superclass references
%}

"@" {
    LEXER_DEBUG ("@");

    current_input_column++;

    lexer_flags.quote_is_transpose = false;
    lexer_flags.convert_spaces_to_comma = false;
    lexer_flags.looking_at_function_handle++;
    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = false;

    COUNT_TOK_AND_RETURN ('@');

  }
%{
/*
%{
// Explicit type parameter
%}

"##" {
    current_input_column++;

    lexer_flags.quote_is_transpose = false;
    lexer_flags.convert_spaces_to_comma = false;
    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = false;
  COUNT_TOK_AND_RETURN('#');
}
*/
%}



%{
// A new line character.  New line characters inside matrix constants
// are handled by the <MATRIX_START> start state code above.  If closest
// nesting is inside parentheses, don't return a row separator.
%}

{NL} {
    LEXER_DEBUG ("{NL}");

    input_line_number++;
    current_input_column = 1;

    lexer_flags.quote_is_transpose = false;
    lexer_flags.convert_spaces_to_comma = true;

    if (nesting_level.none ())
      {
        lexer_flags.at_beginning_of_statement = true;
        COUNT_TOK_AND_RETURN ('\n');
      }
    else if (nesting_level.is_paren ())
      {
        lexer_flags.at_beginning_of_statement = false;
        gripe_matlab_incompatible ("bare newline inside parentheses");
      }
    else if (nesting_level.is_bracket_or_brace ())
      return LEXICAL_ERROR;
  }

%{
// Single quote can either be the beginning of a string or a transpose
// operator.
%}

"'" {
    LEXER_DEBUG ("'");

    current_input_column++;
    lexer_flags.convert_spaces_to_comma = true;

    if (lexer_flags.quote_is_transpose)
      {
        do_comma_insert_check ();
        COUNT_TOK_AND_RETURN (QUOTE);
      }
    else
      {
        int tok = handle_string ('\'');
        COUNT_TOK_AND_RETURN (tok);
      }
  }

%{
// Double quotes always begin strings.
%}

\" {
    LEXER_DEBUG ("\"");

    current_input_column++;
    int tok = handle_string ('"');

    COUNT_TOK_AND_RETURN (tok);
}

%{
// Gobble comments.
%}

{CCHAR} {
    LEXER_DEBUG ("{CCHAR}");

    lexer_flags.looking_for_object_index = false;

    xunput (yytext[0], yytext);

    bool eof = false;
    int tok = process_comment (false, eof);

    if (eof)
      TOK_RETURN (END_OF_INPUT);
    else if (tok > 0)
      COUNT_TOK_AND_RETURN (tok);
  }

%{
// Block comments.
%}

^{S}*{CCHAR}\{{S}*{NL} {
    LEXER_DEBUG ("^{S}*{CCHAR}\\{{S}*{NL}");

    lexer_flags.looking_for_object_index = false;

    input_line_number++;
    current_input_column = 1;
    block_comment_nesting_level++;
    ////promptflag--;

    bool eof = false;
    process_comment (true, eof);
  }

%{
// Other operators.
%}

":"     { LEXER_DEBUG (":"); BIN_OP_RETURN (':', false, false); }

".+"    { LEXER_DEBUG (".+"); XBIN_OP_RETURN (EPLUS, false, false); }
".-"    { LEXER_DEBUG (".-"); XBIN_OP_RETURN (EMINUS, false, false); }
".*"    { LEXER_DEBUG (".*"); BIN_OP_RETURN (EMUL, false, false); }
"./"    { LEXER_DEBUG ("./"); BIN_OP_RETURN (EDIV, false, false); }
".\\"   { LEXER_DEBUG (".\\"); BIN_OP_RETURN (ELEFTDIV, false, false); }
".^"    { LEXER_DEBUG (".^"); BIN_OP_RETURN (EPOW, false, false); }
".**"   { LEXER_DEBUG (".**"); XBIN_OP_RETURN (EPOW, false, false); }
".'"    { LEXER_DEBUG (".'"); do_comma_insert_check (); BIN_OP_RETURN (TRANSPOSE, true, false); }
"++"    { LEXER_DEBUG ("++"); do_comma_insert_check (); XBIN_OP_RETURN (PLUS_PLUS, true, false); }
"--"    { LEXER_DEBUG ("--"); do_comma_insert_check (); XBIN_OP_RETURN (MINUS_MINUS, true, false); }
"<="    { LEXER_DEBUG ("<="); BIN_OP_RETURN (EXPR_LE, false, false); }
"=="    { LEXER_DEBUG ("=="); BIN_OP_RETURN (EXPR_EQ, false, false); }
"~="    { LEXER_DEBUG ("~="); BIN_OP_RETURN (EXPR_NE, false, false); }
"!="    { LEXER_DEBUG ("!="); XBIN_OP_RETURN (EXPR_NE, false, false); }
">="    { LEXER_DEBUG (">="); BIN_OP_RETURN (EXPR_GE, false, false); }
"&"     { LEXER_DEBUG ("&"); BIN_OP_RETURN (EXPR_AND, false, false); }
"|"     { LEXER_DEBUG ("|"); BIN_OP_RETURN (EXPR_OR, false, false); }
"<"     { LEXER_DEBUG ("<"); BIN_OP_RETURN (EXPR_LT, false, false); }
">"     { LEXER_DEBUG (">"); BIN_OP_RETURN (EXPR_GT, false, false); }
"+"     { LEXER_DEBUG ("+"); BIN_OP_RETURN ('+', false, false); }
"-"     { LEXER_DEBUG ("-"); BIN_OP_RETURN ('-', false, false); }
"*"     { LEXER_DEBUG ("*"); BIN_OP_RETURN ('*', false, false); }
"/"     { LEXER_DEBUG ("/"); BIN_OP_RETURN ('/', false, false); }
"\\"    { LEXER_DEBUG ("\\"); BIN_OP_RETURN (LEFTDIV, false, false); }
";"     { LEXER_DEBUG (";"); BIN_OP_RETURN (';', true, true); }
","     { LEXER_DEBUG (","); BIN_OP_RETURN (',', true, ! lexer_flags.looking_at_object_index.front ()); }
"^"     { LEXER_DEBUG ("^"); BIN_OP_RETURN (POW, false, false); }
"**"    { LEXER_DEBUG ("**"); XBIN_OP_RETURN (POW, false, false); }
"="     { LEXER_DEBUG ("="); BIN_OP_RETURN ('=', true, false); }
"&&"    { LEXER_DEBUG ("&&"); BIN_OP_RETURN (EXPR_AND_AND, false, false); }
"||"    { LEXER_DEBUG ("||"); BIN_OP_RETURN (EXPR_OR_OR, false, false); }
"<<"    { LEXER_DEBUG ("<<"); XBIN_OP_RETURN (LSHIFT, false, false); }
">>"    { LEXER_DEBUG (">>"); XBIN_OP_RETURN (RSHIFT, false, false); }

{NOT} {
    LEXER_DEBUG ("{NOT}");

    if (yytext[0] == '~')
      BIN_OP_RETURN (EXPR_NOT, false, false);
    else
      XBIN_OP_RETURN (EXPR_NOT, false, false);
  }

"(" {
    LEXER_DEBUG ("(");

    // If we are looking for an object index, then push TRUE for
    // looking_at_object_index.  Otherwise, just push whatever state
    // is current (so that we can pop it off the stack when we find
    // the matching close paren).

    lexer_flags.looking_at_object_index.push_front
      (lexer_flags.looking_for_object_index);

    lexer_flags.looking_at_indirect_ref = false;
    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = false;

    nesting_level.paren ();
    ////promptflag--;

    TOK_RETURN ('(');
  }

")" {
    LEXER_DEBUG (")");

    nesting_level.remove ();
    current_input_column++;

    lexer_flags.looking_at_object_index.pop_front ();

    lexer_flags.quote_is_transpose = true;
    lexer_flags.convert_spaces_to_comma = nesting_level.is_bracket_or_brace ();
    lexer_flags.looking_for_object_index = true;
    lexer_flags.at_beginning_of_statement = false;

    do_comma_insert_check ();

    COUNT_TOK_AND_RETURN (')');
  }

"." {
    LEXER_DEBUG (".");

    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = false;

    TOK_RETURN ('.');
  }

"+="    { LEXER_DEBUG ("+="); XBIN_OP_RETURN (ADD_EQ, false, false); }
"-="    { LEXER_DEBUG ("-="); XBIN_OP_RETURN (SUB_EQ, false, false); }
"*="    { LEXER_DEBUG ("*="); XBIN_OP_RETURN (MUL_EQ, false, false); }
"/="    { LEXER_DEBUG ("/="); XBIN_OP_RETURN (DIV_EQ, false, false); }
"\\="   { LEXER_DEBUG ("\\="); XBIN_OP_RETURN (LEFTDIV_EQ, false, false); }
".+="   { LEXER_DEBUG (".+="); XBIN_OP_RETURN (ADD_EQ, false, false); }
".-="   { LEXER_DEBUG (".-="); XBIN_OP_RETURN (SUB_EQ, false, false); }
".*="   { LEXER_DEBUG (".*="); XBIN_OP_RETURN (EMUL_EQ, false, false); }
"./="   { LEXER_DEBUG ("./="); XBIN_OP_RETURN (EDIV_EQ, false, false); }
".\\="  { LEXER_DEBUG (".\\="); XBIN_OP_RETURN (ELEFTDIV_EQ, false, false); }
{POW}=  { LEXER_DEBUG ("{POW}="); XBIN_OP_RETURN (POW_EQ, false, false); }
{EPOW}= { LEXER_DEBUG ("{EPOW}="); XBIN_OP_RETURN (EPOW_EQ, false, false); }
"&="    { LEXER_DEBUG ("&="); XBIN_OP_RETURN (AND_EQ, false, false); }
"|="    { LEXER_DEBUG ("|="); XBIN_OP_RETURN (OR_EQ, false, false); }
"<<="   { LEXER_DEBUG ("<<="); XBIN_OP_RETURN (LSHIFT_EQ, false, false); }
">>="   { LEXER_DEBUG (">>="); XBIN_OP_RETURN (RSHIFT_EQ, false, false); }

\{{S}* {
    LEXER_DEBUG ("\\{{S}*");

    nesting_level.brace ();

    lexer_flags.looking_at_object_index.push_front
      (lexer_flags.looking_for_object_index);

    current_input_column += yyleng;
    lexer_flags.quote_is_transpose = false;
    lexer_flags.convert_spaces_to_comma = true;
    lexer_flags.looking_for_object_index = false;
    lexer_flags.at_beginning_of_statement = false;

    //promptflag--;
    eat_whitespace ();

    lexer_flags.braceflag++;
    BEGIN (MATRIX_START);
    COUNT_TOK_AND_RETURN ('{');
  }

"}" {
    LEXER_DEBUG ("}");

    lexer_flags.looking_at_object_index.pop_front ();

    lexer_flags.looking_for_object_index = true;
    lexer_flags.at_beginning_of_statement = false;

    nesting_level.remove ();

    TOK_RETURN ('}');
  }

%{
// Unrecognized input is a lexical error.
%}

. {
    LEXER_DEBUG (".");

    xunput (yytext[0], yytext);

    int c = text_yyinput ();

    if (c != EOF)
      {
        current_input_column++;

        error ("invalid character `%s' (ASCII %d) near line %d, column %d",
               "undo string escape", c,
               input_line_number, current_input_column);

        return LEXICAL_ERROR;
      }
    else
      TOK_RETURN (END_OF_INPUT);
  }

%%

// GAG.
//
// If we're reading a matrix and the next character is '[', make sure
// that we insert a comma ahead of it.

void
do_comma_insert_check (void)
{
  int spc_gobbled = eat_continuation ();

  int c = text_yyinput ();

  xunput (c, yytext);

  if (spc_gobbled)
    xunput (' ', yytext);

  lexer_flags.do_comma_insert = (! lexer_flags.looking_at_object_index.front ()
                                 && lexer_flags.bracketflag && c == '[');
}

// Fix things up for errors or interrupts.  The parser is never called
// recursively, so it is always safe to reinitialize its state before
// doing any parsing.

/*void
reset_parser (void)
{
  // Start off on the right foot.
  BEGIN (INITIAL);

  parser_end_of_input = false;

  while (! symtab_context.empty ())
    symtab_context.pop ();

  // We do want a prompt by default.
  //promptflag = 1;

  // We are not in a block comment.
  block_comment_nesting_level = 0;

  // Error may have occurred inside some brackets, braces, or parentheses.
  nesting_level.clear ();

  // Clear out the stack of token info used to track line and column
  // numbers.
  while (! token_stack.empty ())
    {
      delete token_stack.top ();
      token_stack.pop ();
    }

  // Can be reset by defining a function.
  if (! (reading_script_file || reading_fcn_file || reading_classdef_file))
    {
      current_input_column = 1;
      input_line_number = command_editor::current_command_number ();
    }

  // Only ask for input from stdin if we are expecting interactive
  // input.
  if ((interactive || forced_interactive)
      && ! (reading_fcn_file
        || reading_classdef_file
            || reading_script_file
            || get_input_from_eval_string
            || input_from_startup_file))
    yyrestart (stdin);

  // Clear the buffer for help text.
  while (! help_buf.empty ())
    help_buf.pop ();

  // Reset other flags.
  lexer_flags.init ();
}
*/

static void
display_character (char c)
{
  if (isgraph (c))
    std::cerr << c;
  else
    switch (c)
      {
      case 0:
        std::cerr << "NUL";
        break;

      case 1:
        std::cerr << "SOH";
        break;

      case 2:
        std::cerr << "STX";
        break;

      case 3:
        std::cerr << "ETX";
        break;

      case 4:
        std::cerr << "EOT";
        break;

      case 5:
        std::cerr << "ENQ";
        break;

      case 6:
        std::cerr << "ACK";
        break;

      case 7:
        std::cerr << "\\a";
        break;

      case 8:
        std::cerr << "\\b";
        break;

      case 9:
        std::cerr << "\\t";
        break;

      case 10:
        std::cerr << "\\n";
        break;

      case 11:
        std::cerr << "\\v";
        break;

      case 12:
        std::cerr << "\\f";
        break;

      case 13:
        std::cerr << "\\r";
        break;

      case 14:
        std::cerr << "SO";
        break;

      case 15:
        std::cerr << "SI";
        break;

      case 16:
        std::cerr << "DLE";
        break;

      case 17:
        std::cerr << "DC1";
        break;

      case 18:
        std::cerr << "DC2";
        break;

      case 19:
        std::cerr << "DC3";
        break;

      case 20:
        std::cerr << "DC4";
        break;

      case 21:
        std::cerr << "NAK";
        break;

      case 22:
        std::cerr << "SYN";
        break;

      case 23:
        std::cerr << "ETB";
        break;

      case 24:
        std::cerr << "CAN";
        break;

      case 25:
        std::cerr << "EM";
        break;

      case 26:
        std::cerr << "SUB";
        break;

      case 27:
        std::cerr << "ESC";
        break;

      case 28:
        std::cerr << "FS";
        break;

      case 29:
        std::cerr << "GS";
        break;

      case 30:
        std::cerr << "RS";
        break;

      case 31:
        std::cerr << "US";
        break;

      case 32:
        std::cerr << "SPACE";
        break;

      case 127:
        std::cerr << "DEL";
        break;
      }
}

static int
text_yyinput (void)
{
  int c = yyinput ();

  if (lexer_debug_flag)
    {
      std::cerr << "I: ";
      display_character (c);
      std::cerr << std::endl;
    }

  // Convert CRLF into just LF and single CR into LF.

  if (c == '\r')
    {
      c = yyinput ();

      if (lexer_debug_flag)
        {
          std::cerr << "I: ";
          display_character (c);
          std::cerr << std::endl;
        }

      if (c != '\n')
        {
          xunput (c, yytext);
          c = '\n';
        }
    }

  if (c == '\n')
    input_line_number++;

  return c;
}

static void
xunput (char c, char *buf)
{
  if (lexer_debug_flag)
    {
      std::cerr << "U: ";
      display_character (c);
      std::cerr << std::endl;
    }

  if (c == '\n')
    input_line_number--;

  yyunput (c, buf);
}

// If we read some newlines, we need figure out what column we're
// really looking at.

static void
fixup_column_count (char *s)
{
  char c;
  while ((c = *s++) != '\0')
    {
      if (c == '\n')
        {
          input_line_number++;
          current_input_column = 1;
        }
      else
        current_input_column++;
    }
}

// Include these so that we don't have to link to libfl.a.

int
yywrap (void)
{
  return 1;
}

// Tell us all what the current buffer is.

YY_BUFFER_STATE
current_buffer (void)
{
  return YY_CURRENT_BUFFER;
}

// Create a new buffer.

YY_BUFFER_STATE
create_buffer (FILE *f)
{
  return yy_create_buffer (f, YY_BUF_SIZE);
}

// Start reading a new buffer.

void
switch_to_buffer (YY_BUFFER_STATE buf)
{
  yy_switch_to_buffer (buf);
}

// Delete a buffer.

void
delete_buffer (YY_BUFFER_STATE buf)
{
  yy_delete_buffer (buf);
}

// Restore a buffer (for unwind-prot).

void
restore_input_buffer (void *buf)
{
  switch_to_buffer (static_cast<YY_BUFFER_STATE> (buf));
}

// Delete a buffer (for unwind-prot).

void
delete_input_buffer (void *buf)
{
  delete_buffer (static_cast<YY_BUFFER_STATE> (buf));
}

static bool
inside_any_object_index (void)
{
  bool retval = false;

  for (std::list<bool>::const_iterator i = lexer_flags.looking_at_object_index.begin ();
       i != lexer_flags.looking_at_object_index.end (); i++)
    {
      if (*i)
        {
          retval = true;
          break;
        }
    }

  return retval;
}

// Handle keywords.  Return -1 if the keyword should be ignored.

static int
is_keyword_token (const std::string& s)
{
  int l = input_line_number;
  int c = current_input_column;

  int len = s.length ();

  const octave_kw *kw = octave_kw_hash::in_word_set (s.c_str (), len);
  //const octave_kw *kw = in_word_set (s.c_str (), len);
  if (kw)
    {
      yylval.tok_val = 0;

      switch (kw->kw_id)
        {
        case break_kw:
        case catch_kw:
        case continue_kw:
        case else_kw:
        case otherwise_kw:
        case return_kw:
        case unwind_protect_cleanup_kw:
          lexer_flags.at_beginning_of_statement = true;
          break;

        case case_kw:
        case elseif_kw:
        case global_kw:
        case static_kw:
        case until_kw:
          break;

        case end_kw:
          if (! reading_classdef_file
              && (inside_any_object_index ()
                  || (lexer_flags.defining_func
                      && ! (lexer_flags.looking_at_return_list
                            || lexer_flags.parsed_function_name.top ()))))
            return 0;

          yylval.tok_val = new token (token::simple_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case end_try_catch_kw:
          yylval.tok_val = new token (token::try_catch_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case end_unwind_protect_kw:
          yylval.tok_val = new token (token::unwind_protect_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case endfor_kw:
          yylval.tok_val = new token (token::for_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case endfunction_kw:
          yylval.tok_val = new token (token::function_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case endif_kw:
          yylval.tok_val = new token (token::if_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case endswitch_kw:
          yylval.tok_val = new token (token::switch_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case endwhile_kw:
          yylval.tok_val = new token (token::while_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case endclassdef_kw:
          yylval.tok_val = new token (token::classdef_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case endevents_kw:
          yylval.tok_val = new token (token::events_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case endmethods_kw:
          yylval.tok_val = new token (token::methods_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case endproperties_kw:
          yylval.tok_val = new token (token::properties_end, l, c);
          lexer_flags.at_beginning_of_statement = true;
          break;

        case for_kw:
        case while_kw:
          //promptflag--;
          lexer_flags.looping++;
          break;

        case do_kw:
          lexer_flags.at_beginning_of_statement = true;
          //promptflag--;
          lexer_flags.looping++;
          break;

        case try_kw:
        case unwind_protect_kw:
          lexer_flags.at_beginning_of_statement = true;
          //promptflag--;
          break;

        case if_kw:
        case switch_kw:
          //promptflag--;
          break;

        case get_kw:
        case set_kw:
          // 'get' and 'set' are keywords in classdef method
          // declarations.
          if (! lexer_flags.maybe_classdef_get_set_method)
            return 0;
          break;

        case properties_kw:
        case methods_kw:
        case events_kw:
          // 'properties', 'methods' and 'events' are keywords for
          // classdef blocks.
          if (! lexer_flags.parsing_classdef)
            return 0;
          // fall through ...

        case classdef_kw:
          // 'classdef' is always a keyword.
          //promptflag--;
          break;

        case function_kw:
          //promptflag--;

          lexer_flags.defining_func++;
          lexer_flags.parsed_function_name.push (false);

          if (! (reading_fcn_file || reading_script_file
                 || reading_classdef_file))
            input_line_number = 1;
          break;

        case magic_file_kw:
          {
            if ((reading_fcn_file || reading_script_file
                 || reading_classdef_file)
                && ! curr_fcn_file_full_name.empty ())
              yylval.tok_val = new token (curr_fcn_file_full_name, l, c);
            else
              yylval.tok_val = new token ("stdin", l, c);
          }
          break;

        case magic_line_kw:
          yylval.tok_val = new token (static_cast<double> (l), "", l, c);
          break;

        default:
          panic_impossible ();
        }

      if (! yylval.tok_val)
        yylval.tok_val = new token (l, c);

      token_stack.push (yylval.tok_val);

      return kw->tok;
    }

  return 0;
}

static bool
is_variable (const std::string& name)
{
  //return true;
  return (SymbolFinder::symbol_exists (name, currentScope) || (lexer_flags.pending_local_variables.find (name) != lexer_flags.pending_local_variables.end ()));
}

static std::string
grab_block_comment (stream_reader& reader, bool& eof)
{
  std::string buf;

  bool at_bol = true;
  bool look_for_marker = false;

  bool warned_incompatible = false;

  int c = 0;

  while ((c = reader.getc ()) != EOF)
    {
      current_input_column++;

      if (look_for_marker)
        {
          at_bol = false;
          look_for_marker = false;

          if (c == '{' || c == '}')
            {
              std::string tmp_buf (1, static_cast<char> (c));

              int type = c;

              bool done = false;

              while ((c = reader.getc ()) != EOF && ! done)
                {
                  current_input_column++;

                  switch (c)
                    {
                    case ' ':
                    case '\t':
                      tmp_buf += static_cast<char> (c);
                      break;

                    case '\n':
                      {
                        current_input_column = 0;
                        at_bol = true;
                        done = true;

                        if (type == '{')
                          {
                            block_comment_nesting_level++;
                            //promptflag--;
                          }
                        else
                          {
                            block_comment_nesting_level--;
                            //promptflag++;

                            if (block_comment_nesting_level == 0)
                              {
                                buf += grab_comment_block (reader, true, eof);

                                return buf;
                              }
                          }
                      }
                      break;

                    default:
                      at_bol = false;
                      tmp_buf += static_cast<char> (c);
                      buf += tmp_buf;
                      done = true;
                      break;
                    }
                }
            }
        }

      if (at_bol && (c == '%' || c == '#'))
        {
          if (c == '#' && ! warned_incompatible)
            {
              warned_incompatible = true;
              maybe_gripe_matlab_incompatible_comment (c);
            }

          at_bol = false;
          look_for_marker = true;
        }
      else
        {
          buf += static_cast<char> (c);

          if (c == '\n')
            {
              current_input_column = 0;
              at_bol = true;
            }
        }
    }

  if (c == EOF)
    eof = true;

  return buf;
}

std::string
grab_comment_block (stream_reader& reader, bool at_bol,
                    bool& eof)
{
  std::string buf;

  // TRUE means we are at the beginning of a comment block.
  bool begin_comment = false;

  // TRUE means we are currently reading a comment block.
  bool in_comment = false;

  bool warned_incompatible = false;

  int c = 0;

  while ((c = reader.getc ()) != EOF)
    {
      current_input_column++;

      if (begin_comment)
        {
          if (c == '%' || c == '#')
            {
              at_bol = false;
              continue;
            }
          else if (at_bol && c == '{')
            {
              std::string tmp_buf (1, static_cast<char> (c));

              bool done = false;

              while ((c = reader.getc ()) != EOF && ! done)
                {
                  current_input_column++;

                  switch (c)
                    {
                    case ' ':
                    case '\t':
                      tmp_buf += static_cast<char> (c);
                      break;

                    case '\n':
                      {
                        current_input_column = 0;
                        at_bol = true;
                        done = true;

                        block_comment_nesting_level++;
                        //promptflag--;

                        buf += grab_block_comment (reader, eof);

                        in_comment = false;

                        if (eof)
                          goto done;
                      }
                      break;

                    default:
                      at_bol = false;
                      tmp_buf += static_cast<char> (c);
                      buf += tmp_buf;
                      done = true;
                      break;
                    }
                }
            }
          else
            {
              at_bol = false;
              begin_comment = false;
            }
        }

      if (in_comment)
        {
          buf += static_cast<char> (c);

          if (c == '\n')
            {
              at_bol = true;
              current_input_column = 0;
              in_comment = false;

              // FIXME -- bailing out here prevents things like
              //
              //    octave> # comment
              //    octave> x = 1
              //
              // from failing at the command line, while still
              // allowing blocks of comments to be grabbed properly
              // for function doc strings.  But only the first line of
              // a mult-line doc string will be picked up for
              // functions defined on the command line.  We need a
              // better way of collecting these comments...
              if (! (reading_fcn_file || reading_script_file))
                goto done;
            }
        }
      else
        {
          switch (c)
            {
            case ' ':
            case '\t':
              break;

            case '#':
              if (! warned_incompatible)
                {
                  warned_incompatible = true;
                  maybe_gripe_matlab_incompatible_comment (c);
                }
              // fall through...

            case '%':
              in_comment = true;
              begin_comment = true;
              break;

            default:
              current_input_column--;
              reader.ungetc (c);
              goto done;
            }
        }
    }

 done:

  if (c == EOF)
    eof = true;

  return buf;
}

class
flex_stream_reader : public stream_reader
{
public:
  flex_stream_reader (char *buf_arg) : stream_reader (), buf (buf_arg) { }

  int getc (void) { return ::text_yyinput (); }
  int ungetc (int c) { ::xunput (c, buf); return 0; }

private:

  // No copying!

  flex_stream_reader (const flex_stream_reader&);

  flex_stream_reader& operator = (const flex_stream_reader&);

  char *buf;
};

static int
process_comment (bool start_in_block, bool& eof)
{
  eof = false;

  std::string help_txt;

  if (! help_buf.empty ())
    help_txt = help_buf.top ();

  flex_stream_reader flex_reader (yytext);

  // process_comment is only supposed to be called when we are not
  // initially looking at a block comment.

  std::string txt = start_in_block
    ? grab_block_comment (flex_reader, eof)
    : grab_comment_block (flex_reader, false, eof);

  if (lexer_debug_flag)
    std::cerr << "C: " << txt << std::endl;

  if (help_txt.empty () && nesting_level.none ())
    {
      if (! help_buf.empty ())
        help_buf.pop ();

      help_buf.push (txt);
    }

  octave_comment_buffer::append (txt);

  current_input_column = 1;
  lexer_flags.quote_is_transpose = false;
  lexer_flags.convert_spaces_to_comma = true;
  lexer_flags.at_beginning_of_statement = true;

  if (YY_START == COMMAND_START)
    BEGIN (INITIAL);

  if (nesting_level.none ())
    return '\n';
  else if (nesting_level.is_bracket_or_brace ())
    return ';';
  else
    return 0;
}

// Return 1 if the given character matches any character in the given
// string.

static bool
match_any (char c, const char *s)
{
  char tmp;
  while ((tmp = *s++) != '\0')
    {
      if (c == tmp)
        return true;
    }
  return false;
}

// Given information about the spacing surrounding an operator,
// return 1 if it looks like it should be treated as a binary
// operator.  For example,
//
//   [ 1 + 2 ]  or  [ 1+ 2]  or  [ 1+2 ]  ==>  binary
//
//   [ 1 +2 ]  ==>  unary

static bool
looks_like_bin_op (bool spc_prev, int next_char)
{
  bool spc_next = (next_char == ' ' || next_char == '\t');

  return ((spc_prev && spc_next) || ! spc_prev);
}

// Recognize separators.  If the separator is a CRLF pair, it is
// replaced by a single LF.

static bool
next_token_is_sep_op (void)
{
  bool retval = false;

  int c = text_yyinput ();

  retval = match_any (c, ",;\n]");

  xunput (c, yytext);

  return retval;
}

// Try to determine if the next token should be treated as a postfix
// unary operator.  This is ugly, but it seems to do the right thing.

static bool
next_token_is_postfix_unary_op (bool spc_prev)
{
  bool un_op = false;

  int c0 = text_yyinput ();

  if (c0 == '\'' && ! spc_prev)
    {
      un_op = true;
    }
  else if (c0 == '.')
    {
      int c1 = text_yyinput ();
      un_op = (c1 == '\'');
      xunput (c1, yytext);
    }
  else if (c0 == '+')
    {
      int c1 = text_yyinput ();
      un_op = (c1 == '+');
      xunput (c1, yytext);
    }
  else if (c0 == '-')
    {
      int c1 = text_yyinput ();
      un_op = (c1 == '-');
      xunput (c1, yytext);
    }

  xunput (c0, yytext);

  return un_op;
}

// Try to determine if the next token should be treated as a binary
// operator.
//
// This kluge exists because whitespace is not always ignored inside
// the square brackets that are used to create matrix objects (though
// spacing only really matters in the cases that can be interpreted
// either as binary ops or prefix unary ops: currently just +, -).
//
// Note that a line continuation directly following a + or - operator
// (e.g., the characters '[' 'a' ' ' '+' '\' LFD 'b' ']') will be
// parsed as a binary operator.

static bool
next_token_is_bin_op (bool spc_prev)
{
  bool bin_op = false;

  int c0 = text_yyinput ();

  switch (c0)
    {
    case '+':
    case '-':
      {
        int c1 = text_yyinput ();

        switch (c1)
          {
          case '+':
          case '-':
            // Unary ops, spacing doesn't matter.
            break;

          case '=':
            // Binary ops, spacing doesn't matter.
            bin_op = true;
            break;

          default:
            // Could be either, spacing matters.
            bin_op = looks_like_bin_op (spc_prev, c1);
            break;
          }

        xunput (c1, yytext);
      }
      break;

    case ':':
    case '/':
    case '\\':
    case '^':
      // Always a binary op (may also include /=, \=, and ^=).
      bin_op = true;
      break;

    // .+ .- ./ .\ .^ .* .**
    case '.':
      {
        int c1 = text_yyinput ();

        if (match_any (c1, "+-/\\^*"))
          // Always a binary op (may also include .+=, .-=, ./=, ...).
          bin_op = true;
        else if (! isdigit (c1) && c1 != ' ' && c1 != '\t' && c1 != '.')
          // A structure element reference is a binary op.
          bin_op = true;

        xunput (c1, yytext);
      }
      break;

    // = == & && | || * **
    case '=':
    case '&':
    case '|':
    case '*':
      // Always a binary op (may also include ==, &&, ||, **).
      bin_op = true;
      break;

    // < <= <> > >=
    case '<':
    case '>':
      // Always a binary op (may also include <=, <>, >=).
      bin_op = true;
      break;

    // ~= !=
    case '~':
    case '!':
      {
        int c1 = text_yyinput ();

        // ~ and ! can be unary ops, so require following =.
        if (c1 == '=')
          bin_op = true;

        xunput (c1, yytext);
      }
      break;

    default:
      break;
    }

  xunput (c0, yytext);

  return bin_op;
}

// Used to delete trailing white space from tokens.

static std::string
strip_trailing_whitespace (char *s)
{
  std::string retval = s;

  size_t pos = retval.find_first_of (" \t");

  if (pos != std::string::npos)
    retval.resize (pos);

  return retval;
}

// FIXME -- we need to handle block comments here.

static void
scan_for_comments (const char *text)
{
  std::string comment_buf;

  bool in_comment = false;
  bool beginning_of_comment = false;

  int len = strlen (text);
  int i = 0;

  while (i < len)
    {
      char c = text[i++];

      switch (c)
        {
        case '%':
        case '#':
          if (in_comment)
            {
              if (! beginning_of_comment)
                comment_buf += static_cast<char> (c);
            }
          else
            {
              maybe_gripe_matlab_incompatible_comment (c);
              in_comment = true;
              beginning_of_comment = true;
            }
          break;

        case '\n':
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              octave_comment_buffer::append (comment_buf);
              comment_buf.resize (0);
              in_comment = false;
              beginning_of_comment = false;
            }
          break;

        default:
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              beginning_of_comment = false;
            }
          break;
        }
    }

  if (! comment_buf.empty ())
    octave_comment_buffer::append (comment_buf);
}

// Discard whitespace, including comments and continuations.
//
// Return value is logical OR of the following values:
//
//  ATE_NOTHING      : no spaces to eat
//  ATE_SPACE_OR_TAB : space or tab in input
//  ATE_NEWLINE      : bare new line in input

// FIXME -- we need to handle block comments here.

static yum_yum
eat_whitespace (void)
{
  yum_yum retval = ATE_NOTHING;

  std::string comment_buf;

  bool in_comment = false;
  bool beginning_of_comment = false;

  int c = 0;

  while ((c = text_yyinput ()) != EOF)
    {
      current_input_column++;

      switch (c)
        {
        case ' ':
        case '\t':
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              beginning_of_comment = false;
            }
          retval |= ATE_SPACE_OR_TAB;
          break;

        case '\n':
          retval |= ATE_NEWLINE;
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              octave_comment_buffer::append (comment_buf);
              comment_buf.resize (0);
              in_comment = false;
              beginning_of_comment = false;
            }
          current_input_column = 0;
          break;

        case '#':
        case '%':
          if (in_comment)
            {
              if (! beginning_of_comment)
                comment_buf += static_cast<char> (c);
            }
          else
            {
              maybe_gripe_matlab_incompatible_comment (c);
              in_comment = true;
              beginning_of_comment = true;
            }
          break;

        case '.':
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              beginning_of_comment = false;
              break;
            }
          else
            {
              if (have_ellipsis_continuation ())
                break;
              else
                goto done;
            }

        case '\\':
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              beginning_of_comment = false;
              break;
            }
          else
            {
              if (have_continuation ())
                break;
              else
                goto done;
            }

        default:
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              beginning_of_comment = false;
              break;
            }
          else
            goto done;
        }
    }

  if (! comment_buf.empty ())
    octave_comment_buffer::append (comment_buf);

 done:
  xunput (c, yytext);
  current_input_column--;
  return retval;
}

static inline bool
looks_like_hex (const char *s, int len)
{
  return (len > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'));
}

static void
handle_number (void)
{
  double value = 0.0;
  int nread = 0;

  if (looks_like_hex (yytext, strlen (yytext)))
    {
      unsigned long ival;

      nread = sscanf (yytext, "%lx", &ival);

      value = static_cast<double> (ival);
    }
  else
    {
      char *tmp = strdup(yytext);//strsave (yytext);

      char *idx = strpbrk (tmp, "Dd");

      if (idx)
        *idx = 'e';

      nread = sscanf (tmp, "%lf", &value);

      delete [] tmp;
    }

  // If yytext doesn't contain a valid number, we are in deep doo doo.

  assert (nread == 1);

  lexer_flags.quote_is_transpose = true;
  lexer_flags.convert_spaces_to_comma = true;
  lexer_flags.looking_for_object_index = false;
  lexer_flags.at_beginning_of_statement = false;

  yylval.tok_val = new token (value, yytext, input_line_number,
                              current_input_column);

  token_stack.push (yylval.tok_val);

  current_input_column += yyleng;

  do_comma_insert_check ();
}

// We have seen a backslash and need to find out if it should be
// treated as a continuation character.  If so, this eats it, up to
// and including the new line character.
//
// Match whitespace only, followed by a comment character or newline.
// Once a comment character is found, discard all input until newline.
// If non-whitespace characters are found before comment
// characters, return 0.  Otherwise, return 1.

// FIXME -- we need to handle block comments here.

static bool
have_continuation (bool trailing_comments_ok)
{
  std::ostringstream buf;

  std::string comment_buf;

  bool in_comment = false;
  bool beginning_of_comment = false;

  int c = 0;

  while ((c = text_yyinput ()) != EOF)
    {
      buf << static_cast<char> (c);

      switch (c)
        {
        case ' ':
        case '\t':
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              beginning_of_comment = false;
            }
          break;

        case '%':
        case '#':
          if (trailing_comments_ok)
            {
              if (in_comment)
                {
                  if (! beginning_of_comment)
                    comment_buf += static_cast<char> (c);
                }
              else
                {
                  maybe_gripe_matlab_incompatible_comment (c);
                  in_comment = true;
                  beginning_of_comment = true;
                }
            }
          else
            goto cleanup;
          break;

        case '\n':
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              octave_comment_buffer::append (comment_buf);
            }
          current_input_column = 0;
          //promptflag--;
          gripe_matlab_incompatible_continuation ();
          return true;

        default:
          if (in_comment)
            {
              comment_buf += static_cast<char> (c);
              beginning_of_comment = false;
            }
          else
            goto cleanup;
          break;
        }
    }

  xunput (c, yytext);
  return false;

cleanup:

  std::string s = buf.str ();

  int len = s.length ();
  while (len--)
    xunput (s[len], yytext);

  return false;
}

// We have seen a `.' and need to see if it is the start of a
// continuation.  If so, this eats it, up to and including the new
// line character.

static bool
have_ellipsis_continuation (bool trailing_comments_ok)
{
  char c1 = text_yyinput ();
  if (c1 == '.')
    {
      char c2 = text_yyinput ();
      if (c2 == '.' && have_continuation (trailing_comments_ok))
        return true;
      else
        {
          xunput (c2, yytext);
          xunput (c1, yytext);
        }
    }
  else
    xunput (c1, yytext);

  return false;
}

// See if we have a continuation line.  If so, eat it and the leading
// whitespace on the next line.
//
// Return value is the same as described for eat_whitespace().

static yum_yum
eat_continuation (void)
{
  int retval = ATE_NOTHING;

  int c = text_yyinput ();

  if ((c == '.' && have_ellipsis_continuation ())
      || (c == '\\' && have_continuation ()))
    retval = eat_whitespace ();
  else
    xunput (c, yytext);

  return retval;
}

// Replace backslash escapes in a string with the real values.
//Method copied from src/utils.cc

std::string
do_string_escapes (const std::string& s)
{
  std::string retval;

  size_t i = 0;
  size_t j = 0;
  size_t len = s.length ();

  retval.resize (len);

  while (j < len)
    {
      if (s[j] == '\\' && j+1 < len)
        {
          switch (s[++j])
            {
            case '0':
              retval[i] = '\0';
              break;

            case 'a':
              retval[i] = '\a';
              break;

            case 'b': // backspace
              retval[i] = '\b';
              break;

            case 'f': // formfeed
              retval[i] = '\f';
              break;

            case 'n': // newline
              retval[i] = '\n';
              break;

            case 'r': // carriage return
              retval[i] = '\r';
              break;

            case 't': // horizontal tab
              retval[i] = '\t';
              break;

            case 'v': // vertical tab
              retval[i] = '\v';
              break;

            case '\\': // backslash
              retval[i] = '\\';
              break;

            case '\'': // quote
              retval[i] = '\'';
              break;

            case '"': // double quote
              retval[i] = '"';
              break;

            default:
              warning ("unrecognized escape sequence `\\%c' --\
 converting to `%c'", s[j], s[j]);
              retval[i] = s[j];
              break;
            }
        }
      else
        {
          retval[i] = s[j];
        }

      i++;
      j++;
    }

  retval.resize (i);

  return retval;
}

static int
handle_string (char delim)
{
  std::ostringstream buf;

  int bos_line = input_line_number;
  int bos_col = current_input_column;

  int c;
  int escape_pending = 0;

  while ((c = text_yyinput ()) != EOF)
    {
      current_input_column++;

      if (c == '\\')
        {
          if (delim == '\'' || escape_pending)
            {
              buf << static_cast<char> (c);
              escape_pending = 0;
            }
          else
            {
              if (have_continuation (false))
                escape_pending = 0;
              else
                {
                  buf << static_cast<char> (c);
                  escape_pending = 1;
                }
            }
          continue;
        }
      else if (c == '.')
        {
          if (delim == '\'' || ! have_ellipsis_continuation (false))
            buf << static_cast<char> (c);
        }
      else if (c == '\n')
        {
          error ("unterminated string constant");
          break;
        }
      else if (c == delim)
        {
          if (escape_pending)
            buf << static_cast<char> (c);
          else
            {
              c = text_yyinput ();
              if (c == delim)
                {
                  buf << static_cast<char> (c);
                }
              else
                {
                  std::string s;
                  xunput (c, yytext);

                  if (delim == '\'')
                    s = buf.str ();
                  else
                    s = do_string_escapes (buf.str ());

                  lexer_flags.quote_is_transpose = true;
                  lexer_flags.convert_spaces_to_comma = true;

                  yylval.tok_val = new token (s, bos_line, bos_col);
                  token_stack.push (yylval.tok_val);

                  if (delim == '"')
                    gripe_matlab_incompatible ("\" used as string delimiter");
                  else if (delim == '\'')
                    gripe_single_quote_string ();

                  lexer_flags.looking_for_object_index = true;
                  lexer_flags.at_beginning_of_statement = false;

                  return delim == '"' ? DQ_STRING : SQ_STRING;
                }
            }
        }
      else
        {
          buf << static_cast<char> (c);
        }

      escape_pending = 0;
    }

  return LEXICAL_ERROR;
}

static bool
next_token_is_assign_op (void)
{
  bool retval = false;

  int c0 = text_yyinput ();

  switch (c0)
    {
    case '=':
      {
        int c1 = text_yyinput ();
        xunput (c1, yytext);
        if (c1 != '=')
          retval = true;
      }
      break;

    case '+':
    case '-':
    case '*':
    case '/':
    case '\\':
    case '&':
    case '|':
      {
        int c1 = text_yyinput ();
        xunput (c1, yytext);
        if (c1 == '=')
          retval = true;
      }
      break;

    case '.':
      {
        int c1 = text_yyinput ();
        if (match_any (c1, "+-*/\\"))
          {
            int c2 = text_yyinput ();
            xunput (c2, yytext);
            if (c2 == '=')
              retval = true;
          }
        xunput (c1, yytext);
      }
      break;

    case '>':
      {
        int c1 = text_yyinput ();
        if (c1 == '>')
          {
            int c2 = text_yyinput ();
            xunput (c2, yytext);
            if (c2 == '=')
              retval = true;
          }
        xunput (c1, yytext);
      }
      break;

    case '<':
      {
        int c1 = text_yyinput ();
        if (c1 == '<')
          {
            int c2 = text_yyinput ();
            xunput (c2, yytext);
            if (c2 == '=')
              retval = true;
          }
        xunput (c1, yytext);
      }
      break;

    default:
      break;
    }

  xunput (c0, yytext);

  return retval;
}

static bool
next_token_is_index_op (void)
{
  int c = text_yyinput ();
  xunput (c, yytext);
  return c == '(' || c == '{';
}

static int
handle_close_bracket (bool spc_gobbled, int bracket_type)
{
  int retval = bracket_type;

  if (! nesting_level.none ())
    {
      nesting_level.remove ();

      if (bracket_type == ']')
        lexer_flags.bracketflag--;
      else if (bracket_type == '}')
        lexer_flags.braceflag--;
      else
        panic_impossible ();
    }

  if (lexer_flags.bracketflag == 0 && lexer_flags.braceflag == 0)
    BEGIN (INITIAL);

  if (bracket_type == ']'
      && next_token_is_assign_op ()
      && ! lexer_flags.looking_at_return_list)
    {
      retval = CLOSE_BRACE;
    }
  else if ((lexer_flags.bracketflag || lexer_flags.braceflag)
           && lexer_flags.convert_spaces_to_comma
           && (nesting_level.is_bracket ()
               || (nesting_level.is_brace ()
                   && ! lexer_flags.looking_at_object_index.front ())))
    {
      bool index_op = next_token_is_index_op ();

      // Don't insert comma if we are looking at something like
      //
      //   [x{i}{j}] or [x{i}(j)]
      //
      // but do if we are looking at
      //
      //   [x{i} {j}] or [x{i} (j)]

      if (spc_gobbled || ! (bracket_type == '}' && index_op))
        {
          bool bin_op = next_token_is_bin_op (spc_gobbled);

          bool postfix_un_op = next_token_is_postfix_unary_op (spc_gobbled);

          bool sep_op = next_token_is_sep_op ();

          if (! (postfix_un_op || bin_op || sep_op))
            {
              maybe_warn_separator_insert (',');

              xunput (',', yytext);
              return retval;
            }
        }
    }

  lexer_flags.quote_is_transpose = true;
  lexer_flags.convert_spaces_to_comma = true;

  return retval;
}

static void
maybe_unput_comma (int spc_gobbled)
{
  if (nesting_level.is_bracket ()
      || (nesting_level.is_brace ()
          && ! lexer_flags.looking_at_object_index.front ()))
    {
      int bin_op = next_token_is_bin_op (spc_gobbled);

      int postfix_un_op = next_token_is_postfix_unary_op (spc_gobbled);

      int c1 = text_yyinput ();
      int c2 = text_yyinput ();

      xunput (c2, yytext);
      xunput (c1, yytext);

      int sep_op = next_token_is_sep_op ();

      int dot_op = (c1 == '.'
                    && (isalpha (c2) || isspace (c2) || c2 == '_'));

      if (postfix_un_op || bin_op || sep_op || dot_op)
        return;

      int index_op = (c1 == '(' || c1 == '{');

      // If there is no space before the indexing op, we don't insert
      // a comma.

      if (index_op && ! spc_gobbled)
        return;

      maybe_warn_separator_insert (',');

      xunput (',', yytext);
    }
}

static bool
next_token_can_follow_bin_op (void)
{
  std::stack<char> buf;

  int c = EOF;

  // Skip whitespace in current statement on current line
  while (true)
    {
      c = text_yyinput ();

      buf.push (c);

      if (match_any (c, ",;\n") || (c != ' ' && c != '\t'))
        break;
    }

  // Restore input.
  while (! buf.empty ())
    {
      xunput (buf.top (), yytext);

      buf.pop ();
    }

  return (isalnum (c) || match_any (c, "!\"'(-[_{~"));
}

static bool
can_be_command (const std::string& tok)
{
  // Don't allow these names to be treated as commands to avoid
  // surprises when parsing things like "NaN ^2".

  return ! (tok == "e"
            || tok == "I" || tok == "i"
            || tok == "J" || tok == "j"
            || tok == "Inf" || tok == "inf"
            || tok == "NaN" || tok == "nan");
}

static bool
looks_like_command_arg (void)
{
  bool retval = true;

  int c0 = text_yyinput ();

  switch (c0)
    {
    // = ==
    case '=':
      {
        int c1 = text_yyinput ();

        if (c1 == '=')
          {
            int c2 = text_yyinput ();

            if (! match_any (c2, ",;\n") && (c2 == ' ' || c2 == '\t')
                && next_token_can_follow_bin_op ())
              retval = false;

            xunput (c2, yytext);
          }
        else
          retval = false;

        xunput (c1, yytext);
      }
      break;

    case '(':
    case '{':
      // Indexing.
      retval = false;
      break;

    case '\n':
      // EOL.
      break;

    case '\'':
    case '"':
      // Beginning of a character string.
      break;

    // + - ++ -- += -=
    case '+':
    case '-':
      {
        int c1 = text_yyinput ();

        switch (c1)
          {
          case '\n':
            // EOL.
          case '+':
          case '-':
            // Unary ops, spacing doesn't matter.
            break;

          case '\t':
          case ' ':
            {
              if (next_token_can_follow_bin_op ())
                retval = false;
            }
            break;

          case '=':
            {
              int c2 = text_yyinput ();

              if (! match_any (c2, ",;\n") && (c2 == ' ' || c2 == '\t')
                  && next_token_can_follow_bin_op ())
                retval = false;

              xunput (c2, yytext);
            }
            break;
          }

        xunput (c1, yytext);
      }
      break;

    case ':':
    case '/':
    case '\\':
    case '^':
      {
        int c1 = text_yyinput ();

        if (! match_any (c1, ",;\n") && (c1 == ' ' || c1 == '\t')
            && next_token_can_follow_bin_op ())
          retval = false;

        xunput (c1, yytext);
      }
      break;

    // .+ .- ./ .\ .^ .* .**
    case '.':
      {
        int c1 = text_yyinput ();

        if (match_any (c1, "+-/\\^*"))
          {
            int c2 = text_yyinput ();

            if (c2 == '=')
              {
                int c3 = text_yyinput ();

                if (! match_any (c3, ",;\n") && (c3 == ' ' || c3 == '\t')
                    && next_token_can_follow_bin_op ())
                  retval = false;

                xunput (c3, yytext);
              }
            else if (! match_any (c2, ",;\n") && (c2 == ' ' || c2 == '\t')
                     && next_token_can_follow_bin_op ())
              retval = false;

            xunput (c2, yytext);
          }
        else if (! match_any (c1, ",;\n")
                 && (! isdigit (c1) && c1 != ' ' && c1 != '\t'
                     && c1 != '.'))
          {
            // Structure reference.  FIXME -- is this a complete check?

            retval = false;
          }

        xunput (c1, yytext);
      }
      break;

    // & && | || * **
    case '&':
    case '|':
    case '*':
      {
        int c1 = text_yyinput ();

        if (c1 == c0)
          {
            int c2 = text_yyinput ();

            if (! match_any (c2, ",;\n") && (c2 == ' ' || c2 == '\t')
                && next_token_can_follow_bin_op ())
              retval = false;

            xunput (c2, yytext);
          }
        else if (! match_any (c1, ",;\n") && (c1 == ' ' || c1 == '\t')
                 && next_token_can_follow_bin_op ())
          retval = false;

        xunput (c1, yytext);
      }
      break;

    // < <= > >=
    case '<':
    case '>':
      {
        int c1 = text_yyinput ();

        if (c1 == '=')
          {
            int c2 = text_yyinput ();

            if (! match_any (c2, ",;\n") && (c2 == ' ' || c2 == '\t')
                && next_token_can_follow_bin_op ())
              retval = false;

            xunput (c2, yytext);
          }
        else if (! match_any (c1, ",;\n") && (c1 == ' ' || c1 == '\t')
                 && next_token_can_follow_bin_op ())
          retval = false;

        xunput (c1, yytext);
      }
      break;

    // ~= !=
    case '~':
    case '!':
      {
        int c1 = text_yyinput ();

        // ~ and ! can be unary ops, so require following =.
        if (c1 == '=')
          {
            int c2 = text_yyinput ();

            if (! match_any (c2, ",;\n") && (c2 == ' ' || c2 == '\t')
                && next_token_can_follow_bin_op ())
              retval = false;

            xunput (c2, yytext);
          }
        else if (! match_any (c1, ",;\n") && (c1 == ' ' || c1 == '\t')
                 && next_token_can_follow_bin_op ())
          retval = false;

        xunput (c1, yytext);
      }
      break;

    default:
      break;
    }

  xunput (c0, yytext);

  return retval;
}

static int
handle_superclass_identifier (void)
{
  eat_continuation ();

  std::string pkg;
  std::string meth = strip_trailing_whitespace (yytext);
  size_t pos = meth.find ("@");
  std::string cls = meth.substr (pos).substr (1);
  meth = meth.substr (0, pos - 1);

  pos = cls.find (".");
  if (pos != std::string::npos)
    {
      pkg = cls.substr (pos).substr (1);
      cls = cls.substr (0, pos - 1);
    }

  int kw_token = (is_keyword_token (meth) || is_keyword_token (cls)
                  || is_keyword_token (pkg));
  if (kw_token)
    {
      error ("method, class and package names may not be keywords");
      return LEXICAL_ERROR;
    }

  yylval.tok_val
    = new token (0);
  token_stack.push (yylval.tok_val);

  lexer_flags.convert_spaces_to_comma = true;
  current_input_column += yyleng;

  return SUPERCLASSREF;
}

static int
handle_meta_identifier (void)
{
  eat_continuation ();

  std::string pkg;
  std::string cls = strip_trailing_whitespace (yytext).substr (1);
  size_t pos = cls.find (".");

  if (pos != std::string::npos)
    {
      pkg = cls.substr (pos).substr (1);
      cls = cls.substr (0, pos - 1);
    }

  int kw_token = is_keyword_token (cls) || is_keyword_token (pkg);
  if (kw_token)
    {
       error ("class and package names may not be keywords");
      return LEXICAL_ERROR;
    }

  yylval.tok_val
    = new token (0);
  token_stack.push (yylval.tok_val);

  lexer_flags.convert_spaces_to_comma = true;
  current_input_column += yyleng;

  return METAQUERY;
}

// Figure out exactly what kind of token to return when we have seen
// an identifier.  Handles keywords.  Return -1 if the identifier
// should be ignored.

static int
handle_identifier (void)
{
  bool at_bos = lexer_flags.at_beginning_of_statement;

  std::string tok = strip_trailing_whitespace (yytext);

  int c = yytext[yyleng-1];

  int cont_is_spc = eat_continuation ();

  int spc_gobbled = (cont_is_spc || c == ' ' || c == '\t');

  // If we are expecting a structure element, avoid recognizing
  // keywords and other special names and return STRUCT_ELT, which is
  // a string that is also a valid identifier.  But first, we have to
  // decide whether to insert a comma.

  if (lexer_flags.looking_at_indirect_ref)
    {
      do_comma_insert_check ();

      maybe_unput_comma (spc_gobbled);

      yylval.tok_val = new token (tok, input_line_number,
                                  current_input_column);
      token_stack.push (yylval.tok_val);

      lexer_flags.quote_is_transpose = true;
      lexer_flags.convert_spaces_to_comma = true;
      lexer_flags.looking_for_object_index = true;

      current_input_column += yyleng;

      return STRUCT_ELT;
    }

  lexer_flags.at_beginning_of_statement = false;

  // The is_keyword_token may reset
  // lexer_flags.at_beginning_of_statement.  For example, if it sees
  // an else token, then the next token is at the beginning of a
  // statement.

  int kw_token = is_keyword_token (tok);

  // If we found a keyword token, then the beginning_of_statement flag
  // is already set.  Otherwise, we won't be at the beginning of a
  // statement.

  if (lexer_flags.looking_at_function_handle)
    {
      if (kw_token)
        {
          error ("function handles may not refer to keywords");

          return LEXICAL_ERROR;
        }
      else
        {
          yylval.tok_val = new token (tok, input_line_number,
                                      current_input_column);
          
          token_stack.push (yylval.tok_val);

          current_input_column += yyleng;
          lexer_flags.quote_is_transpose = false;
          lexer_flags.convert_spaces_to_comma = true;
          lexer_flags.looking_for_object_index = true;

          return FCN_HANDLE;
        }
    }

  // If we have a regular keyword, return it.
  // Keywords can be followed by identifiers.

  if (kw_token)
    {
      if (kw_token >= 0)
        {
          current_input_column += yyleng;
          lexer_flags.quote_is_transpose = false;
          lexer_flags.convert_spaces_to_comma = true;
          lexer_flags.looking_for_object_index = false;
        }

      return kw_token;
    }

  // See if we have a plot keyword (title, using, with, or clear).

  int c1 = text_yyinput ();

  bool next_tok_is_eq = false;
  if (c1 == '=')
    {
      int c2 = text_yyinput ();
      xunput (c2, yytext);

      if (c2 != '=')
        next_tok_is_eq = true;
    }

  xunput (c1, yytext);

  // Kluge alert.
  //
  // If we are looking at a text style function, set up to gobble its
  // arguments.
  //
  // If the following token is `=', or if we are parsing a function
  // return list or function parameter list, or if we are looking at
  // something like [ab,cd] = foo (), force the symbol to be inserted
  // as a variable in the current symbol table.

  if (! is_variable (tok))
    {
      if (at_bos && spc_gobbled && can_be_command (tok)
          && looks_like_command_arg ())
        {
          BEGIN (COMMAND_START);
        }
      else if (next_tok_is_eq
               || lexer_flags.looking_at_decl_list
               || lexer_flags.looking_at_return_list
               || (lexer_flags.looking_at_parameter_list
                   && ! lexer_flags.looking_at_initializer_expression))
        {
          //tok = tok;
          //FIXME...couldn't figure out what force_variable does
          //symbol_table::force_variable (tok);
        }
      else if (lexer_flags.looking_at_matrix_or_assign_lhs)
        {
          lexer_flags.pending_local_variables.insert (tok);
        }
    }

  // Find the token in the symbol table.  Beware the magic
  // transformation of the end keyword...

  if (tok == "end")
    tok = "__end__";

//TODO Deal with symbol tables
  yylval.tok_val = new token (tok, input_line_number,
                                      current_input_column);
  token_stack.push (yylval.tok_val);

  // After seeing an identifer, it is ok to convert spaces to a comma
  // (if needed).

  lexer_flags.convert_spaces_to_comma = true;

  if (! (next_tok_is_eq || YY_START == COMMAND_START))
    {
      lexer_flags.quote_is_transpose = true;

      do_comma_insert_check ();

      maybe_unput_comma (spc_gobbled);
    }

  current_input_column += yyleng;

  if (tok != "__end__")
    lexer_flags.looking_for_object_index = true;

  return NAME;
}

void
lexical_feedback::init (void)
{
  // Not initially defining a matrix list.
  bracketflag = 0;

  // Not initially defining a cell array list.
  braceflag = 0;

  // Not initially inside a loop or if statement.
  looping = 0;

  // Not initially defining a function.
  defining_func = 0;

  // Not parsing an object index.
  while (! parsed_function_name.empty ())
    parsed_function_name.pop ();

  parsing_class_method = false;

  // Not initially defining a class with classdef.
  maybe_classdef_get_set_method = false;
  parsing_classdef = false;

  // Not initiallly looking at a function handle.
  looking_at_function_handle = 0;

  // Not parsing a function return, parameter, or declaration list.
  looking_at_return_list = false;
  looking_at_parameter_list = false;
  looking_at_decl_list = false;

  // Not looking at an argument list initializer expression.
  looking_at_initializer_expression = false;

  // Not parsing a matrix or the left hand side of multi-value
  // assignment statement.
  looking_at_matrix_or_assign_lhs = false;

  // Not parsing an object index.
  while (! looking_at_object_index.empty ())
    looking_at_object_index.pop_front ();

  looking_at_object_index.push_front (false);

  // Object index not possible until we've seen something.
  looking_for_object_index = false;

  // Yes, we are at the beginning of a statement.
  at_beginning_of_statement = true;

  // No need to do comma insert or convert spaces to comma at
  // beginning of input.
  convert_spaces_to_comma = true;
  do_comma_insert = false;

  // Not initially looking at indirect references.
  looking_at_indirect_ref = false;

  // Quote marks strings intially.
  quote_is_transpose = false;

  // Set of identifiers that might be local variable names is empty.
  pending_local_variables.clear ();
}

bool
is_keyword (const std::string& s)
{
  return octave_kw_hash::in_word_set (s.c_str (), s.length ()) != 0;
  //return in_word_set (s.c_str (), s.length ()) != 0;
}

/*DEFUN (iskeyword, args, ,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} iskeyword ()\n\
@deftypefnx {Built-in Function} {} iskeyword (@var{name})\n\
Return true if @var{name} is an Octave keyword.  If @var{name}\n\
is omitted, return a list of keywords.\n\
@seealso{isvarname, exist}\n\
@end deftypefn")
{
  octave_value retval;

  int argc = args.length () + 1;

  string_vector argv = args.make_argv ("iskeyword");

  if (error_state)
    return retval;

  if (argc == 1)
    {
      string_vector lst (TOTAL_KEYWORDS);

      for (int i = 0; i < TOTAL_KEYWORDS; i++)
        lst[i] = wordlist[i].name;

      retval = Cell (lst.sort ());
    }
  else if (argc == 2)
    {
      retval = is_keyword (argv[1]);
    }
  else
    print_usage ();

  return retval;
}*/

void
prep_lexer_for_script_file (void)
{
  BEGIN (SCRIPT_FILE_BEGIN);
}

void
prep_lexer_for_function_file (void)
{
  BEGIN (FUNCTION_FILE_BEGIN);
}

static void
maybe_warn_separator_insert (char sep)
{
  std::string nm = curr_fcn_file_full_name;

  if (nm.empty ())
    warning_with_id ("Octave:separator-insert",
                     "potential auto-insertion of `%c' near line %d",
                     sep, input_line_number);
  else
    warning_with_id ("Octave:separator-insert",
                     "potential auto-insertion of `%c' near line %d of file %s",
                     sep, input_line_number, nm.c_str ());
}

static void
gripe_single_quote_string (void)
{
  std::string nm = curr_fcn_file_full_name;

  if (nm.empty ())
    warning_with_id ("Octave:single-quote-string",
                     "single quote delimited string near line %d",
                     input_line_number);
  else
    warning_with_id ("Octave:single-quote-string",
                     "single quote delimited string near line %d of file %s",
                     input_line_number, nm.c_str ());
}

static void
gripe_matlab_incompatible (const std::string& msg)
{
  std::string nm = curr_fcn_file_full_name;

  if (nm.empty ())
    warning_with_id ("Octave:matlab-incompatible",
                     "potential Matlab compatibility problem: %s",
                     msg.c_str ());
  else
    warning_with_id ("Octave:matlab-incompatible",
                     "potential Matlab compatibility problem: %s near line %d offile %s",
                     msg.c_str (), input_line_number, nm.c_str ());
}

static void
maybe_gripe_matlab_incompatible_comment (char c)
{
  if (c == '#')
    gripe_matlab_incompatible ("# used as comment character");
}

static void
gripe_matlab_incompatible_continuation (void)
{
  gripe_matlab_incompatible ("\\ used as line continuation marker");
}

static void
gripe_matlab_incompatible_operator (const std::string& op)
{
  std::string t = op;
  int n = t.length ();
  if (t[n-1] == '\n')
    t.resize (n-1);
  gripe_matlab_incompatible (t + " used as operator");
}

static void
display_token (int tok)
{
  switch (tok)
    {
    case '=': std::cerr << "'='\n"; break;
    case ':': std::cerr << "':'\n"; break;
    case '-': std::cerr << "'-'\n"; break;
    case '+': std::cerr << "'+'\n"; break;
    case '*': std::cerr << "'*'\n"; break;
    case '/': std::cerr << "'/'\n"; break;
    case ADD_EQ: std::cerr << "ADD_EQ\n"; break;
    case SUB_EQ: std::cerr << "SUB_EQ\n"; break;
    case MUL_EQ: std::cerr << "MUL_EQ\n"; break;
    case DIV_EQ: std::cerr << "DIV_EQ\n"; break;
    case LEFTDIV_EQ: std::cerr << "LEFTDIV_EQ\n"; break;
    case POW_EQ: std::cerr << "POW_EQ\n"; break;
    case EMUL_EQ: std::cerr << "EMUL_EQ\n"; break;
    case EDIV_EQ: std::cerr << "EDIV_EQ\n"; break;
    case ELEFTDIV_EQ: std::cerr << "ELEFTDIV_EQ\n"; break;
    case EPOW_EQ: std::cerr << "EPOW_EQ\n"; break;
    case AND_EQ: std::cerr << "AND_EQ\n"; break;
    case OR_EQ: std::cerr << "OR_EQ\n"; break;
    case LSHIFT_EQ: std::cerr << "LSHIFT_EQ\n"; break;
    case RSHIFT_EQ: std::cerr << "RSHIFT_EQ\n"; break;
    case LSHIFT: std::cerr << "LSHIFT\n"; break;
    case RSHIFT: std::cerr << "RSHIFT\n"; break;
    case EXPR_AND_AND: std::cerr << "EXPR_AND_AND\n"; break;
    case EXPR_OR_OR: std::cerr << "EXPR_OR_OR\n"; break;
    case EXPR_AND: std::cerr << "EXPR_AND\n"; break;
    case EXPR_OR: std::cerr << "EXPR_OR\n"; break;
    case EXPR_NOT: std::cerr << "EXPR_NOT\n"; break;
    case EXPR_LT: std::cerr << "EXPR_LT\n"; break;
    case EXPR_LE: std::cerr << "EXPR_LE\n"; break;
    case EXPR_EQ: std::cerr << "EXPR_EQ\n"; break;
    case EXPR_NE: std::cerr << "EXPR_NE\n"; break;
    case EXPR_GE: std::cerr << "EXPR_GE\n"; break;
    case EXPR_GT: std::cerr << "EXPR_GT\n"; break;
    case LEFTDIV: std::cerr << "LEFTDIV\n"; break;
    case EMUL: std::cerr << "EMUL\n"; break;
    case EDIV: std::cerr << "EDIV\n"; break;
    case ELEFTDIV: std::cerr << "ELEFTDIV\n"; break;
    case EPLUS: std::cerr << "EPLUS\n"; break;
    case EMINUS: std::cerr << "EMINUS\n"; break;
    case QUOTE: std::cerr << "QUOTE\n"; break;
    case TRANSPOSE: std::cerr << "TRANSPOSE\n"; break;
    case PLUS_PLUS: std::cerr << "PLUS_PLUS\n"; break;
    case MINUS_MINUS: std::cerr << "MINUS_MINUS\n"; break;
    case POW: std::cerr << "POW\n"; break;
    case EPOW: std::cerr << "EPOW\n"; break;

    case NUM:
    case IMAG_NUM:
      std::cerr << (tok == NUM ? "NUM" : "IMAG_NUM")
                << " [" << "yylval.tok_val->number ()" << "]\n"; //tok_val has all its methods stripped. so just use it as a dummy string
      break;

    case STRUCT_ELT:
      std::cerr << "STRUCT_ELT [" << "yylval.tok_val->text ()" << "]\n"; break;

    case NAME:
      {
        /*symbol_table::symbol_record *sr = yylval.tok_val->sym_rec ();
        std::cerr << "NAME";
        if (sr)
          std::cerr << " [" << sr->name () << "]";
        std::cerr << "\n";*/

        std::cerr << "NAME";
      }
      break;

    case END: std::cerr << "END\n"; break;

    case DQ_STRING:
    case SQ_STRING:
      std::cerr << (tok == DQ_STRING ? "DQ_STRING" : "SQ_STRING")
                << " [" << "yylval.tok_val->text ()" << "]\n";
      break;

    case FOR: std::cerr << "FOR\n"; break;
    case WHILE: std::cerr << "WHILE\n"; break;
    case DO: std::cerr << "DO\n"; break;
    case UNTIL: std::cerr << "UNTIL\n"; break;
    case IF: std::cerr << "IF\n"; break;
    case ELSEIF: std::cerr << "ELSEIF\n"; break;
    case ELSE: std::cerr << "ELSE\n"; break;
    case SWITCH: std::cerr << "SWITCH\n"; break;
    case CASE: std::cerr << "CASE\n"; break;
    case OTHERWISE: std::cerr << "OTHERWISE\n"; break;
    case BREAK: std::cerr << "BREAK\n"; break;
    case CONTINUE: std::cerr << "CONTINUE\n"; break;
    case FUNC_RET: std::cerr << "FUNC_RET\n"; break;
    case UNWIND: std::cerr << "UNWIND\n"; break;
    case CLEANUP: std::cerr << "CLEANUP\n"; break;
    case TRY: std::cerr << "TRY\n"; break;
    case CATCH: std::cerr << "CATCH\n"; break;
    case GLOBAL: std::cerr << "GLOBAL\n"; break;
    case STATIC: std::cerr << "STATIC\n"; break;
    case FCN_HANDLE: std::cerr << "FCN_HANDLE\n"; break;
    case END_OF_INPUT: std::cerr << "END_OF_INPUT\n\n"; break;
    case LEXICAL_ERROR: std::cerr << "LEXICAL_ERROR\n\n"; break;
    case FCN: std::cerr << "FCN\n"; break;
    case CLOSE_BRACE: std::cerr << "CLOSE_BRACE\n"; break;
    case SCRIPT_FILE: std::cerr << "SCRIPT_FILE\n"; break;
    case FUNCTION_FILE: std::cerr << "FUNCTION_FILE\n"; break;
    case SUPERCLASSREF: std::cerr << "SUPERCLASSREF\n"; break;
    case METAQUERY: std::cerr << "METAQUERY\n"; break;
    case GET: std::cerr << "GET\n"; break;
    case SET: std::cerr << "SET\n"; break;
    case PROPERTIES: std::cerr << "PROPERTIES\n"; break;
    case METHODS: std::cerr << "METHODS\n"; break;
    case EVENTS: std::cerr << "EVENTS\n"; break;
    case CLASSDEF: std::cerr << "CLASSDEF\n"; break;
    case '\n': std::cerr << "\\n\n"; break;
    case '\r': std::cerr << "\\r\n"; break;
    case '\t': std::cerr << "TAB\n"; break;
    default:
      {
        if (tok < 256)
          std::cerr << static_cast<char> (tok) << "\n";
        else
          std::cerr << "UNKNOWN(" << tok << ")\n";
      }
      break;
    }
}

static void
display_state (void)
{
  std::cerr << "S: ";

  switch (YY_START)
    {
    case INITIAL:
      std::cerr << "INITIAL" << std::endl;
      break;

    case COMMAND_START:
      std::cerr << "COMMAND_START" << std::endl;
      break;

    case MATRIX_START:
      std::cerr << "MATRIX_START" << std::endl;
      break;

    case SCRIPT_FILE_BEGIN:
      std::cerr << "SCRIPT_FILE_BEGIN" << std::endl;
      break;

    case FUNCTION_FILE_BEGIN:
      std::cerr << "FUNCTION_FILE_BEGIN" << std::endl;
      break;

    default:
      std::cerr << "UNKNOWN START STATE!" << std::endl;
      break;
    }
}

static void
lexer_debug (const char *pattern, const char *text)
{
  std::cerr << std::endl;

  display_state ();

  std::cerr << "P: " << pattern << std::endl;
  std::cerr << "T: " << text << std::endl;
}

/*
DEFUN (__display_tokens__, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} __display_tokens__ ()\n\
Query or set the internal variable that determines whether Octave's\n\
lexer displays tokens as they are read.\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (display_tokens);
}

DEFUN (__token_count__, , ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} __token_count__ ()\n\
Number of language tokens processed since Octave startup.\n\
@end deftypefn")
{
  return octave_value (Vtoken_count);
}

DEFUN (__lexer_debug_flag__, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {@var{old_val} =} __lexer_debug_flag__ (@var{new_val}))\n\
Undocumented internal function.\n\
@end deftypefn")
{
  octave_value retval;

  retval = set_internal_variable (lexer_debug_flag, args, nargout,
                                  "__lexer_debug_flag__");

  return retval;
}
*/
