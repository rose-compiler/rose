// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: Rule.h,v 1.2 2007-03-08 15:36:49 markus Exp $

#ifndef H_RULE
#define H_RULE

#include <string>
#include <sstream>

#include "spec.h"

class Rule
{
private:
    std::string rulename, nodename;
    isn type, constr, consti, field, fieldi, ftype;
    unsigned int varno;

public:
    Rule(std::string rulename, std::string nodename, isn type,
            isn constr, isn consti, isn field, isn fieldi,
            isn ftype);
    bool matches(std::string &type, std::string &constr,
            unsigned long consti, std::string &field, unsigned long fieldi,
            std::string &ftype);
    bool is_catchall() const;
    bool clashes_with(const Rule &r);
    std::string stringize();
    std::string macrohead(unsigned int i);
    std::string get_rulename();
    std::string get_nodename();
    bool extern_c;
    bool per_constructor;
    bool islist;
    bool macro;
    std::string get_field() const;
    std::string get_ftype() const;
};

typedef std::pair<Rule *, std::string> RuleDef;

#endif
