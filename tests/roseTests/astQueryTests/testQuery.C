// Tests src/midend/astQuery/{nodeQuery,nameQuery,numberQuery}

#include "rose.h"
#include "stringify.h"

using namespace std;

/** Show an error message about a node. */
static void
emit_node_mesg(SgNode *node, const std::string &mesg="")
{
    Sg_File_Info *loc = isSgLocatedNode(node) ? isSgLocatedNode(node)->get_startOfConstruct() : NULL;
    std::string filename = loc ? loc->get_filenameString() : "__UNKNOWN_FILE__";
    int lno = loc ? loc->get_line() : 0;
    int cno = loc ? loc->get_col() : 0;
    std::cerr <<filename <<":" <<lno <<"." <<cno <<": " <<(mesg.empty() ? "error" : mesg) <<"\n";
}

/** Checks that all nodes in the list are unique. Emit errors about duplicate items. Returns the number of duplicates. */
static size_t
check_unique(const NodeQuerySynthesizedAttributeType &nodes, const std::string &title)
{
    std::set<SgNode*> set;
    std::vector<SgNode*> dups;
    for (NodeQuerySynthesizedAttributeType::const_iterator ni=nodes.begin(); ni!=nodes.end(); ++ni) {
        if (!set.insert(*ni).second)
            dups.push_back(*ni);
    }
    if (!dups.empty()) {
        std::cerr <<"Duplicate nodes returned for the \"" <<title <<"\" test:\n";
        for (std::vector<SgNode*>::const_iterator di=dups.begin(); di!=dups.end(); ++di)
            emit_node_mesg(*di, "appears multiple times in list");
    }
    return dups.size();
}

int
main(int argc, char *argv[])
{
    SgProject* project = frontend(argc,argv);
    AstTests::runAllTests(project); // run internal consistency tests on the AST

    size_t nerrors = 0;
    std::string separator = std::string(80, '-') + "\n";

    std::cerr <<separator <<"Testing NodeQuery::querySubTree for all SgFunctionDeclaration nodes\n";
    NodeQuerySynthesizedAttributeType funcDecls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
    std::cerr <<"found " <<funcDecls.size() <<" function declaration nodes\n";
    nerrors += check_unique(funcDecls, "querySubTree SgFunctionDeclaration");
    for (NodeQuerySynthesizedAttributeType::const_iterator ni=funcDecls.begin(); ni!=funcDecls.end(); ++ni) {
        if (!isSgFunctionDeclaration(*ni)) {
            emit_node_mesg(*ni, "not a function declaration");
            ++nerrors;
        }
    }
    ROSE_ASSERT(0==nerrors); // optional, to exit early

    std::cerr <<separator <<"Testing NodeQuery::queryNodeList for all SgFunctionDeclaration nodes\n";
    NodeQuerySynthesizedAttributeType funcDecls2 = NodeQuery::queryNodeList(funcDecls, V_SgFunctionDeclaration);
    std::cerr <<"found " <<funcDecls2.size() <<" function declaration nodes\n";
    nerrors += check_unique(funcDecls2, "queryNodeList SgFunctionDeclaration");
    for (NodeQuerySynthesizedAttributeType::const_iterator ni=funcDecls2.begin(); ni!=funcDecls2.end(); ++ni) {
        if (!isSgFunctionDeclaration(*ni)) {
            emit_node_mesg(*ni, "not a function declaration");
            ++nerrors;
        }
    }
    ROSE_ASSERT(0==nerrors); // optional, to exit early

    std::cerr <<separator <<"Testing NameQuery::querySubTree for FunctionDeclarationNames\n";
    NameQuerySynthesizedAttributeType funcNames = NameQuery::querySubTree(project, NameQuery::FunctionDeclarationNames);
    std::cerr <<"found " <<funcNames.size() <<" function declaration names\n";
    if (funcNames.size() != funcDecls.size()) {
        std::cerr <<"number of function declaration names (" <<funcNames.size() <<")"
                  <<" does not match number of function declaration nodes (" <<funcDecls.size() <<")\n";
        ++nerrors;
    }
    ROSE_ASSERT(0==nerrors); // optional, to exit early

    std::cerr <<separator <<"Testing NameQuery::queryNodeList for FunctionDeclarationNames\n";
    NameQuerySynthesizedAttributeType funcNames2 = NameQuery::queryNodeList(funcDecls, NameQuery::FunctionDeclarationNames);
    std::cerr <<"found " <<funcNames2.size() <<" function declaration names\n";
    if (funcNames2.size() != funcDecls.size()) {
        std::cerr <<"number of function declaration names (" <<funcNames2.size() <<")"
                  <<" does not match number of function declaration nodes (" <<funcDecls.size() <<")\n";
        ++nerrors;
    }
    ROSE_ASSERT(0==nerrors); // optional, to exit early

    std::cerr <<separator <<"Testing NumberQuery::querySubTree for NumberOfArgsInConstructor\n";
    NumberQuerySynthesizedAttributeType ctorArgCounts = NumberQuery::querySubTree(project,
                                                                                  NumberQuery::NumberOfArgsInConstructor);
    std::cerr <<"found " <<ctorArgCounts.size() <<" results\n";
    if (ctorArgCounts.size() != 1) {
        std::cerr <<"should have found only one result\n";
        ++nerrors;
    }
    ROSE_ASSERT(0==nerrors); // optional, to exit early

    // It is not necessary to call backend for this test; that functionality is tested elsewhere.
    return nerrors ? 1 : 0;
}
