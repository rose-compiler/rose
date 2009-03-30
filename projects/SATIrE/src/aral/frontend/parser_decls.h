#ifndef _parser_decls_h
#define _parser_decls_h

#include <string>

// These are global declarations needed for the lexer and parser files.

// ARAL classes
#include "../Aral1/aral.h"
using namespace Aral;

// Front End specific classes
class CCompiler;
void InitializeParser(const std::string& inString, CCompiler* compilerAlias);
void FinishParser();

void InitializeLexer(const std::string& tokenizeString);
void FinishLexer();

extern char* yytext;
extern int yylineno;
extern bool yydeprecated;
int yylex();
int yyparse();
void yyerror(const char*);

#endif
