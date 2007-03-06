// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: main.C,v 1.3 2008-01-30 17:07:09 markus Exp $

#include <rose.h>

#include "CFGTraversal.h"
#include "ProcTraversal.h"

#include "cfg_support.h"
#include "analysis_info.h"

#include <config.h>
#include "CFGTraversal.h"
#include "ProcTraversal.h"

#undef NDEBUG
#include "testcfg.h"
#include "CommandLineParser.h"

extern "C" int yylex() { return 0; }
extern "C" int yywrap() { return 0; }

int main(int argc, char **argv)
{
    SgProject *root = frontend(argc, argv);
    std::cout << "collecting functions... ";
    ProcTraversal s;
    s.traverse(root, preorder);
    std::cout << "done" << std::endl;

    std::cout << "generating cfg..." << std::endl;
    CFGTraversal t(s.get_procedures());
    t.traverseInputFiles(root, preorder);
    std::cout << "done" << std::endl;

    std::cout << "testing cfg" << std::endl;
    if (kfg_testit(t.getCFG(), 0, "cfg_dump.gdl") == 0)
        std::cerr << "oops" << std::endl;
    std::cout << "done" << std::endl;

    return 0;
}
