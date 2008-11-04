// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: Rule.c++,v 1.3 2008-11-04 10:04:27 gergo Exp $

#include <string>
#include <sstream>
#include <iostream>
#include <cstring>

#include "spec.h"
#include "Rule.h"

Rule::Rule(std::string rulename, std::string nodename, isn type,
        isn constr, isn consti, isn field, isn fieldi, isn ftype)
{
    this->macro = false;
    this->rulename = rulename;
    this->nodename = nodename;
    this->type = type;
    this->constr = constr;
    this->consti = consti;
    this->field = field;
    this->fieldi = fieldi;
    this->ftype = ftype;
}

bool Rule::matches(std::string &type, std::string &constr,
        unsigned long consti, std::string &field, unsigned long fieldi,
        std::string &ftype)
{
    if (field == "$matchall")
        return true;
#define SMATCH(S) (this->S.type == T_ID || this->S.str == S)
#define IMATCH(I) (this->I.type == T_ID || this->I.num == I)
    
    return SMATCH(type) && SMATCH(constr) && IMATCH(consti) &&
        SMATCH(field) && IMATCH(fieldi) && SMATCH(ftype);

#undef SMATCH
#undef IMATCH
}

bool Rule::is_catchall() const
{
    return type.type == T_ID && constr.type == T_ID &&
        consti.type == T_ID && field.type == T_ID &&
        fieldi.type == T_ID && ftype.type == T_ID;
}

bool Rule::clashes_with(const Rule &r)
{
    /* TODO: implement field-fieldi clashes */

#define VAR(M) (M.type == T_ID || r.M.type == T_ID)
#define STRCMP(M, E) (strcmp(M.str, r.M.str) E 0)
#define ICMP(M, E) (M.num E r.M.num)
    
    return
        (is_catchall() && r.is_catchall()) ||
        (!is_catchall() && !r.is_catchall() &&
        /* clash if unifiable: both variables or equal constants or
         * one variable, one constant, i.e. first is var or second
         * is var or equal constants */
        (VAR(type) || STRCMP(type, ==)) &&
        (VAR(constr) || STRCMP(constr, ==)) &&
        (VAR(consti) || ICMP(consti, ==)) &&
        (VAR(field) || STRCMP(field,==)) &&
        (VAR(fieldi) || ICMP(fieldi, ==)) &&
        (VAR(ftype) || STRCMP(ftype, ==)));

#undef VAR
#undef STRCMP
#undef ICMP
}

std::string Rule::stringize()
{
    std::stringstream s;
    /* string representation of the arguments */
    s << rulename << "(_, "; /* first arg node is always a var */
    s << (type.type == T_ID ? "_" : type.str) << ", ";
    s << (constr.type == T_ID ? "_" : constr.str) << ", ";
    if (consti.type == T_ID)
        s << "_, ";
    else
        s << consti.num << ", ";
    s << (field.type == T_ID ? "_" : field.str) << ", ";
    if (fieldi.type == T_ID)
        s << "_, ";
    else
        s << fieldi.num << ", ";
    s << (ftype.type == T_ID ? "_" : ftype.str) << ")";

    return s.str();
}

std::string Rule::get_rulename()
{
    return rulename;
}

std::string Rule::get_nodename()
{
    return nodename;
}

std::string Rule::macrohead(unsigned int i)
{
    std::stringstream s;
    
    s << "#define " << macroname(rulename, i) << '(';
    s << nodename << ',';

#define PRINT(F) \
    if (F.type == T_ID && strcmp(F.id, "_") != 0) \
        s << F.id; \
    else \
        s << "PIG_VAR_" << varno++;
    
    PRINT(type);
    s << ',';
    PRINT(constr);
    s << ',';
    PRINT(consti);
    s << ',';
    PRINT(field);
    s << ',';
    PRINT(fieldi);
    s << ',';
    PRINT(ftype);
    s << ')';

#undef PRINT

    return s.str();
}

std::string Rule::get_field() const
{
    if (field.type == T_STR)
        return std::string(field.str);
    else
        return std::string("_");
}

std::string Rule::get_ftype() const
{
    if (field.type == T_STR)
        return std::string(ftype.str);
    else
        return std::string("_");
}
