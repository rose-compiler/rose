#ifndef _parser_decls_h
#define _parser_decls_h

#include <string>

// These are global declarations needed for the lexer and parser files.

// Match operations classes
#include "MatchOperation.h"

// Front End specific classes
class CCompiler;
void InitializeParser(const std::string& inString);
void FinishParser();

void InitializeLexer(const std::string& tokenizeString);
void FinishLexer();

extern char* matchertext;
extern int matcherlineno;
extern bool matcherdeprecated;
int matcherlex();
int matcherparse();
void matchererror(const char*);

#endif
