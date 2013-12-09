#include "snippetTests.h"

using namespace rose;


// Holds information about where to insert something
struct InsertionPoint {
    SgFunctionDefinition *function;
    SgStatement *last_stmt, *insert_here;
    std::vector<SgInitializedName*> localvars;

    InsertionPoint(SgProject *project, const std::string &function_name)
        : function(NULL), last_stmt(NULL), insert_here(NULL) {
        if ((function = SnippetTests::findFunctionDefinition(project, function_name))) {
            last_stmt = SnippetTests::findLastStatement(function);
            localvars = SnippetTests::findFunctionVariables(function);

            struct T1: AstSimpleProcessing {
                void visit(SgNode *node) {
                    if (SgVarRefExp *vref = isSgVarRefExp(node)) {
                        if (vref->get_symbol()->get_name() == "INSERT_HERE")
                            throw SageInterface::getEnclosingNode<SgStatement>(vref);
                    }
                }
            } t1;
            try {
                t1.traverse(function, preorder);
            } catch (SgStatement *stmt) {
                insert_here = stmt;
            }
        }
    }

    bool isValid(size_t nvars=0) const {
        return function!=NULL && last_stmt!=NULL && localvars.size() > nvars;
    }
};

static void
list_function_definitions(SgProject *project)
{
    std::set<std::string> names;
    std::vector<SgFunctionDefinition*> fdefs = SageInterface::querySubTree<SgFunctionDefinition>(project);
    for (size_t i=0; i<fdefs.size(); ++i)
        names.insert(fdefs[i]->get_declaration()->get_qualified_name().getString());
    for (std::set<std::string>::iterator ni=names.begin(); ni!=names.end(); ++ni)
        std::cerr <<"  " <<*ni <<"\n";
}

int
main(int argc, char *argv[])
{
    // Parse our own command-line switches
    std::string snippet_file_name="snippets", snippet_name, ipoint_function_name;
    std::vector<std::string> frontend_args;
    frontend_args.push_back(argv[0]);
    int argno = 1;
    for (/*void*/; argno<argc; ++argno) {
        if (!strncmp(argv[argno], "--test:snippet=", 15)) {
            std::string s = argv[argno]+15;
            size_t dot = s.find_first_of('.');
            if (dot==std::string::npos) {
                snippet_name = s;
            } else {
                snippet_file_name = s.substr(0, dot) + ".c";
                snippet_name = s.substr(dot+1);
            }
        } else if (!strncmp(argv[argno], "--test:ipoint_function=", 23)) {
            ipoint_function_name = argv[argno] + 23;
        } else if (!strncmp(argv[argno], "--test:", 7)) {
            std::cerr <<"unknown switch: " <<argv[argno] <<"\n";
            exit(1);
        } else {
            frontend_args.push_back(argv[argno]);
        }
    }

    if (snippet_name.empty()) {
        std::cerr <<"use --test:snippet=NAME to specify a snippet name\n";
        exit(1);
    }

    // Parse the source code into which the snippet will be inserted and find a place to insert.
    SgProject *project = frontend(frontend_args);
    assert(project!=NULL);
    if (ipoint_function_name.empty()) {
        std::cerr <<"use --test:ipoint_function=NAME to specify a fully qualified insertion point function\n"
                  <<"the following insertion point functions are available:\n";
        list_function_definitions(project);
        exit(1);
    }
    InsertionPoint insertionPoint(project, ipoint_function_name);
    if (!insertionPoint.isValid()) {
        std::cerr <<"could not find insertion point\n";
        return 1;
    }

    // Load the snippet from its file.  This actually loads all the snippets in the file.
    SnippetPtr snippet = Snippet::instanceFromFile(snippet_name, SnippetTests::findSnippetFile(snippet_file_name));
    assert(snippet!=NULL);

    // Insert the snippet. This test just passes the first N local variables as snippet arguments
    size_t nargs = snippet->numberOfArguments();
    std::vector<SgNode*> args(insertionPoint.localvars.begin(), insertionPoint.localvars.begin()+nargs);
    SgStatement *ipoint = insertionPoint.insert_here ? insertionPoint.insert_here : insertionPoint.last_stmt;
    snippet->insert(ipoint, args);

    // Unparse the modified source code
#if 1 /*DEBUGGING [Robb P. Matzke 2013-12-03]*/
    generateDOT(*project);
#endif
    backend(project);
    return 0;
}
