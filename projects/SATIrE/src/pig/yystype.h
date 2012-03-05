// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: yystype.h,v 1.2 2007-03-08 15:36:49 markus Exp $

#ifndef H_YYSTYPE
#define H_YYSTYPE

#include <vector>

#include "spec.h"
#include "Rule.h"

union yystype
{
    struct isn idstrnum;
    std::vector<struct isn> *idstrlist;
    const char *code;
    std::vector<Rule *> *rules;
    bool extern_c;
    bool per_constructor;
    bool islist;
    bool macro;
};

typedef union yystype YYSTYPE;

extern YYSTYPE piglval;

#define YYSTYPE_IS_DECLARED

#endif
