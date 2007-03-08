// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: generate.h,v 1.2 2007-03-08 15:36:49 markus Exp $

#ifndef H_GENERATE
#define H_GENERATE

#include <vector>

#include "Rule.h"
#include "../grato/GrammarIr.h"

void generateEverything(GGrammar *grammar,
        std::vector<std::vector<RuleDef *> *> &rules, bool preprocess_code,
        std::string &hpre, std::string &cpre, std::string &hpost,
        std::string &cpost, bool ignore_multiples);

#endif
