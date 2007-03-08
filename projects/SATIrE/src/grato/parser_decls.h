// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: parser_decls.h,v 1.2 2007-03-08 15:36:49 markus Exp $

// Author: Markus Schordan

#ifndef _parser_decls_h
#define _parser_decls_h

#include <string>
#include <list>

// These are global declarations needed for the lexer and parser files.

class CCompiler;
void InitializeParser(const std::string& inString, CCompiler* compilerAlias);
void FinishParser();

void InitializeLexer(const std::string& tokenizeString);
void FinishLexer();

extern char* syntext;
extern int synlineno;
//extern bool yydeprecated;

int synlex();
int synparse();
void synerror(const char*);

class GGrammar;
GGrammar* synParse(void);

#include "GrammarIr.h"
#include "GBackEnd.h"
#include "GTextBackEnd.h"
#include "GDotBackEnd.h"

#endif
