// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: generate.c++,v 1.6 2008-11-04 10:04:27 gergo Exp $

#include <fstream>
#include <vector>
#include <cstdlib>

#include "spec.h"
#include "Rule.h"
#include "generate.h"

void printRuledefs(std::vector<std::vector<RuleDef *> *> &rules,
        std::ofstream &h, std::ofstream &c);
void printMatches(std::string &type, std::string &constructor,
        int constri, std::string &field, int fieldi, std::string &ftype,
        std::vector<std::vector<RuleDef *> *> &rules, std::ofstream &h,
        std::ofstream &c, bool islist, bool ignore_multiples);
void preprocess(std::string opts, std::string infile, std::string outfile);
void prepost(const char *infile, const char *outfile, std::string &pre,
        std::string &post);

void generateEverything(GGrammar *grammar,
        std::vector<std::vector<RuleDef *> *> &rules, bool preprocess_code,
        std::string &hpre, std::string &cpre, std::string &hpost,
        std::string &cpost, bool ignore_multiples)
{
    std::ofstream h("pig_temp.h"), c("pig_temp.c");

    /* print the macros that will later be expanded */
    printRuledefs(rules, h, c);
    h << std::endl;
    c << std::endl;

    GProductionList *pl = grammar->getProductionList();
    for (GProductionList::iterator i = pl->begin(); i != pl->end(); ++i)
    {
        GNonTerminal *lhs = (*i)->getLhs();
        GProductionRhsList *rhs = (*i)->getRhs();
        std::string type = lhs->getName();
        int constri = 0; /* constructor index */
        for (GProductionRhsList::iterator i = rhs->begin();
                i != rhs->end(); ++i, ++constri)
        {
            GSymbol *rhsSymbol = (*i)->getRhs();
            std::string constructor = rhsSymbol->getName();
            int fieldi = 0; /* field index */
            GSymbolList &symList = rhsSymbol->getSymbolList();
            for (GSymbolList::iterator j = symList.begin();
                    j != symList.end(); ++j, ++fieldi)
            {
                std::string field = (*j)->getAccessName();
                std::string ftype = (*j)->getName();
                if ((*j)->getRegexOperator() == REGEX_ONE)
                    printMatches(type, constructor, constri, field,
                            fieldi, ftype, rules, h, c, false,
                            ignore_multiples);
                else if ((*j)->getRegexOperator() == REGEX_STAR)
                {
                    //std::string listtype = "LIST_" + ftype;
                    std::string listtype = "" + ftype;
                    printMatches(type, constructor, constri, field,
                            fieldi, listtype, rules, h, c, true,
                            ignore_multiples);
                }
            }
            /* If the symList is empty, i.e. the constructor has no
             * arguments, we must still call printMatches to invoke
             * per_constructor functions. Specifically, this is
             * needed to implement is_ functions for constructors
             * without arguments! */
            if (symList.empty())
            {
                std::string matchall = "$matchall";
                printMatches(type, constructor, constri, matchall, 0,
                        matchall, rules, h, c, false, ignore_multiples);
            }
        }
    }
    h.close();
    c.close();

    /* preprocess h and possibly c */
    preprocess("-DPIG_define='#define'", "pig_temp.h", "pig_temp2.h");
    if (preprocess_code)
        preprocess("", "pig_temp.c", "pig_temp2.c");

    /* attach pre- and postfixes */
    prepost("pig_temp2.h", "syntree.h", hpre, hpost);
    if (preprocess_code)
        prepost("pig_temp2.c", "syntree.c", cpre, cpost);
    else
        prepost("pig_temp.c", "syntree.c", cpre, cpost);
}

void printRuledefs(std::vector<std::vector<RuleDef *> *> &rules,
        std::ofstream &h, std::ofstream &c)
{
    std::vector<std::vector<RuleDef *> *>::iterator pos = rules.begin();
    for (; pos != rules.end(); ++pos)
    {
        std::vector<RuleDef *>::const_iterator posi = (*pos)->begin();
        RuleDef *head = (*posi);
        ++posi;
        
        int rule = 0;
        for (; posi != (*pos)->end(); ++posi, ++rule)
        {
            if (head->first->macro)
            {
                /* This function is to be implemented as a macro.
                 * Write the macro definition, consisting of the
                 * head and body, to the header file; don't write
                 * anything to the code file. 
                 * Prepend "PIG_define " for preprocessing. */
                h << head->first->macrohead(rule) << '\\' << std::endl
                    << "PIG_define " << head->second << '\\'
                    << std::endl << (*posi)->second << std::endl;
            }
            else
            {
                /* This function is to be implemented as a real
                 * function. Write the prototype (head followed by a
                 * semicolon) into the header file, the function
                 * definition (head and body) into the code file. */
                h << head->first->macrohead(rule) << '\\' << std::endl
                    << head->second << ';' << std::endl;
                c << head->first->macrohead(rule) << '\\'
                    << std::endl << head->second << '\\'
                    << std::endl << "{\\" << std::endl
                    << (*posi)->second << '\\' << std::endl
                    << '}' << std::endl;
            }
        }
    }
}

void printMatches(std::string &type, std::string &constructor,
        int constri, std::string &field, int fieldi, std::string &ftype_,
        std::vector<std::vector<RuleDef *> *> &rules, std::ofstream &h,
        std::ofstream &c, bool islist, bool ignore_multiples)
{
    std::string ftype = ftype_;
    std::string basetype = ftype;
    /* whoa, what's this? comment! */
    // basetype.erase(0, 5);

    /* iterate over all rules defined in the pig file */
    std::vector<std::vector<RuleDef *> *>::iterator pos = rules.begin();
    for (; pos != rules.end(); ++pos)
    {
        long no_of_matches = 0, catchall_matches = 0;
        RuleDef *first_match = NULL;

        std::vector<RuleDef *>::const_iterator posi = (*pos)->begin();
        RuleDef *head = (*posi);
        ++posi;

        /* Don't consider list rules for non-list identifiers. Do
         * consider non-list rules for list identifiers, however. */
#if 0
        if (head->first->islist)
        {
            if (islist)
                std::cout << ftype << std::endl;
            continue;
        }
#else
        if (head->first->islist && !islist)
            continue;
        else if (head->first->islist && islist)
            ftype = basetype;
        else
            ftype = ftype_;
#endif

        /* If this is a "per constructor" rule, consider only the
         * first match per constructor; this is the one with
         * fieldi == 0. Skip the others. */
        if (head->first->per_constructor && fieldi > 0)
            continue;
        
        int rule = 0;
        for (; posi != (*pos)->end(); ++posi, ++rule)
        {
            if ((*posi)->first->matches(type, constructor, constri,
                        field, fieldi, ftype))
            {
                if (field == "$matchall")
                    field = (*posi)->first->get_field();
                if (ftype == "$matchall")
                    ftype = (*posi)->first->get_ftype();
                /* matchall fields are only relevant for
                 * per_constructor rules */
                if ((field == "_" || ftype == "_")
                        && !head->first->per_constructor)
                {
                    catchall_matches = 0;
                    no_of_matches = 1;
                    break;
                }
                if ((*posi)->first->is_catchall())
                    ++catchall_matches;
                else
                    ++no_of_matches;
                if (no_of_matches == 2 && !ignore_multiples)
                {
                    std::cerr << "warning: multiple matches for"
                        << std::endl << '(' << type << ", "
                        << constructor << ", " << constri << ", "
                        << field << ", " << fieldi << ", "
                        << ftype << ")" << std::endl;
                    std::cerr << '\t'
                        << first_match->first->stringize() << std::endl
                        << '\t' << (*posi)->first->stringize()
                        << std::endl;
                }
                else if (no_of_matches > 2 && !ignore_multiples)
                {
                    std::cerr << '\t' << (*posi)->first->stringize()
                        << std::endl;
                }
                if (no_of_matches + catchall_matches <= 1)
                {
                    if (head->first->macro)
                    {
                        /* This function is implemented as a macro.
                         * Write the macro invocation into the header
                         * file where the preprocessor turns it into the
                         * real macro definition. */
                        /* Access functions for the same list type
                         * may be generated multiple times for
                         * multiple occurrences; therefore surround
                         * such definitions with preprocessor
                         * guards. */
                        if (head->first->islist)
                            h << "#ifndef PIG_LIST_" << ftype << "_"
                                << head->first->get_rulename() << std::endl
                                << "#define PIG_LIST_" << ftype << "_"
                                << head->first->get_rulename() << std::endl;
                        h << macroname(head->first->get_rulename(), rule)
                            << '(' << head->first->get_nodename() << ", "
                            << type << ", " << constructor << ", " << constri
                            << ", " << field << ", " << fieldi << ", " << ftype
                            << ')' << std::endl;
                        if (head->first->islist)
                            h << "#endif" << std::endl;
                    }
                    else
                    {
                        /* The function is implemented as a real
                         * function. Write the macro invocation into the
                         * header and code files. In the header, the
                         * prototype is created, while in the code file
                         * the function's implementation is created. */
                        if (head->first->extern_c)
                            h << "PIG_EXTERN_C" << " ";
                        h << macroname(head->first->get_rulename(), rule)
                            << '(' << head->first->get_nodename() << ", "
                            << type << ", " << constructor << ", " << constri
                            << ", " << field << ", " << fieldi << ", " << ftype
                            << ')' << std::endl;
                        /* Access functions for the same list type
                         * may be generated multiple times for
                         * multiple occurrences; therefore surround
                         * such definitions with preprocessor
                         * guards. */
                        if (head->first->islist)
                            c << "#ifndef PIG_LIST_" << ftype << "_"
                                << head->first->get_rulename() << std::endl
                                << "#define PIG_LIST_" << ftype << "_"
                                << head->first->get_rulename() << std::endl;
                        if (head->first->extern_c)
                            c << "PIG_EXTERN_C" << " ";
                        c << macroname(head->first->get_rulename(), rule)
                            << '(' << head->first->get_nodename() << ", "
                            << type << ", " << constructor << ", " << constri
                            << ", " << field << ", " << fieldi << ", " << ftype
                            << ')' << std::endl;
                        if (head->first->islist)
                            c << "#endif" << std::endl;
                    }
                }
                first_match = (*posi);
            }
        }
        if (no_of_matches + catchall_matches == 0 && field.at(0) != '$')
        {
            std::cout << "warning: no match for "
                << head->first->get_rulename()
                << '(' << type << ", " << constructor << ", "
                << constri << ", " << field << ", " << fieldi << ", "
                << ftype << ')' << std::endl;
        }
    }
}

void preprocess(std::string opts, std::string infile, std::string outfile)
{
  std::string cmd = "cpp -P " + opts + ' ' + infile + " -o " + outfile;
  if (std::system(cmd.c_str()) != 0) {
    std::cerr << cmd << std::endl 
	      << "** ERROR Invocation of cpp failed" << std::endl;
    exit(-1);
  }
}

void prepost(const char *infile, const char *outfile, std::string &pre,
        std::string &post)
{
    std::ifstream in(infile);
    std::ofstream out(outfile);
    char ch;

    out << "#ifndef PIG_EXTERN_C" << std::endl
        <<    "#ifdef __cplusplus" << std::endl
        <<        "#define PIG_EXTERN_C extern \"C\"" << std::endl
        <<    "#else" << std::endl
        <<        "#define PIG_EXTERN_C" <<std::endl
        <<    "#endif" << std::endl
        << "#endif" << std::endl;
    out << pre << std::endl << std::endl;
    while (in.get(ch))
        out.put(ch);
    out << std::endl << post << std::endl;
}
