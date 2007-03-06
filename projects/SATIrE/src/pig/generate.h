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
