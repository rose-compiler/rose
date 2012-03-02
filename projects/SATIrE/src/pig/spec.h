// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: spec.h,v 1.4 2008-08-23 13:47:04 gergo Exp $

#ifndef H_SPEC
#define H_SPEC

#include <string>

#include "GrammarIr.h"

enum types { T_ID, T_STR, T_NUM };

struct isn
{
    enum types type;
    unsigned long line, col;
    const char *id;
    char *str;
    unsigned long num;
};

std::string splice(std::string s);
std::string macroname(std::string rulename, unsigned long ruleno);

GGrammar *synparse(void);

#endif
