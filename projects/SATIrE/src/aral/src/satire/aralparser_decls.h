#ifndef _parser_decls_h
#define _parser_decls_h

#include <string>

// These are global declarations needed for the lexer and parser files.

// ARAL classes
#include "aralir.h"
using namespace Aral;

// Front End specific classes
class CCompiler;
void InitializeParser(const std::string& inString, CCompiler* compilerAlias);
void FinishParser();

//void InitializeLexer(const std::string& tokenizeString);
//void FinishLexer();

extern char* araltext;
extern int arallineno;
extern bool araldeprecated;
int arallex();
int aralparse();
void aralerror(const char*);

#endif
