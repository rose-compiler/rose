#include "snippetTests.h"
#include "stringify.h"

using namespace rose;

static void
usage(const std::string &arg0)
{
    std::cerr <<"usage: " <<arg0 <<" --test:snippet=[FILE_NAME.]SNIPPET_NAME --test:ipoint-function=NAME [TEST_SWITCHES] [ROSE_SWITCHES] SPECIMEN\n"
              <<"  These two switches are required since they describe what to inject and\n"
              <<"  where to inject it:\n"
              <<"    --test:snippet=[FILE_NAME,]SNIPPET_NAME\n"
              <<"        The name of the snippet, a function name.  The snippet name can\n"
              <<"        optionally be prefixed by the name of the source file which\n"
              <<"        contains the snippet and the test will search various places for that\n"
              <<"        file.  For example, \"--test:snippet=file1.c,snippet1\" will find a file named\n"
              <<"        \"file1.c\" which contains a function named \"snippet1\".\n"
              <<"    --test:ipoint-function=FQNAME\n"
              <<"        The fully qualified name of the specimen function where the snippet is to be\n"
              <<"        inserted. This switch is required.  The snippet is inserted before the first\n"
              <<"        statement in the ipoint-function that references a variable named\n"
              <<"        \"INSERT_HERE\" (normally one would find an insertion point by\n"
              <<"        matching some pattern in the specimen, but this is only a test).\n"
              <<"  The following switches are optional:\n"
              <<"    --test:insert-mechanism=(body|stmts)\n"
              <<"        Indicates how the snippet is inserted into the SPECIMEN.  If the value\n"
              <<"        is \"body\" then the entire snippet body is inserted, scope and all.\n"
              <<"        If the value is \"stmts\" then each snippet statement is copied into\n"
              <<"        the insertion site's scope, placing variable declarations near the\n"
              <<"        top of the scope and other statements at the insertion point. The\n"
              <<"        default is \"stmts\".\n"
              <<"    --test:locdecls-position=(beginning|end)\n"
              <<"        When the insert-mechanism is \"stmts\", this switch controls whether\n"
              <<"        declaration statements in the snippet are copied to the very beginning\n"
              <<"        of the insertion point's scope, or appended to the end of declarations\n"
              <<"        that are already present in the insertion point's scope.  The default\n"
              <<"        is \"end\".\n"
              <<"    --test:recursive=(yes|no)\n"
              <<"        Determines whether snippets are recursively inserted when a snippet\n"
              <<"        calls other snippets that are defined in the same snippet file.  The\n"
              <<"        default is \"yes\".\n"
              <<"    --test:copy-definitions=(yes|no)\n"
              <<"        Determines whether all snippet definitions are copied into the specimen\n"
              <<"        file.  The default is to not copy any definitions since the are normally\n"
              <<"        inserted recursively at insertion sites in the specimen.\n";
    exit(1);
}

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
            insert_here = SnippetTests::findInsertHere(function);
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
    std::ios::sync_with_stdio();
    std::string argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    // Parse our own command-line switches
    std::string snippet_file_name="snippets", snippet_name, ipoint_function_name;
    Snippet::InsertMechanism insert_mechanism = Snippet::INSERT_STMTS;
    Snippet::LocalDeclarationPosition locdecls_position = Snippet::LOCDECLS_AT_END;
    bool insert_recursively = true, copy_definitions = false;
    std::vector<std::string> frontend_args;
    frontend_args.push_back(argv[0]);
    int argno = 1;
    for (/*void*/; argno<argc; ++argno) {
        if (!strcmp(argv[argno], "-h") || !strcmp(argv[argno], "--help")) {
            usage(argv0);
        } else if (!strncmp(argv[argno], "--test:snippet=", 15)) {
            std::string s = argv[argno]+15;
            size_t dot = s.find_first_of(',');
            if (dot==std::string::npos) {
                snippet_name = s;
            } else {
                snippet_file_name = s.substr(0, dot);
                snippet_name = s.substr(dot+1);
            }
        } else if (!strcmp(argv[argno], "--test:insert-mechanism=body")) {
            insert_mechanism = Snippet::INSERT_BODY;
        } else if (!strcmp(argv[argno], "--test:insert-mechanism=stmts")) {
            insert_mechanism = Snippet::INSERT_STMTS;
        } else if (!strcmp(argv[argno], "--test:recursive=yes")) {
            insert_recursively = true;
        } else if (!strcmp(argv[argno], "--test:recursive=no")) {
            insert_recursively = false;
        } else if (!strcmp(argv[argno], "--test:locdecls-position=beginning")) {
            locdecls_position = Snippet::LOCDECLS_AT_BEGINNING;
        } else if (!strcmp(argv[argno], "--test:locdecls-position=end")) {
            locdecls_position = Snippet::LOCDECLS_AT_END;
        } else if (!strcmp(argv[argno], "--test:copy-definitions=yes")) {
            copy_definitions = true;
        } else if (!strcmp(argv[argno], "--test:copy-definitions=no")) {
            copy_definitions = false;
        } else if (!strncmp(argv[argno], "--test:ipoint-function=", 23)) {
            ipoint_function_name = argv[argno] + 23;
        } else if (!strncmp(argv[argno], "--test:", 7)) {
            std::cerr <<"unknown switch: " <<argv[argno] <<"\n"
                      <<"run \"" <<argv[0] <<" --help\" to see usage information.\n";
        } else {
            frontend_args.push_back(argv[argno]);
        }
    }
    if (snippet_name.empty())
        usage(argv0);
    std::cout <<"Configuration:\n"
              <<"    snippet file base name:   " <<snippet_file_name <<"\n"
              <<"    snippet name:             " <<snippet_name <<"\n"
              <<"    insertion point function: " <<ipoint_function_name <<"\n"
              <<"    insert mechanism:         " <<stringifySnippetInsertMechanism(insert_mechanism) <<"\n"
              <<"    local decls position:     " <<stringifySnippetLocalDeclarationPosition(locdecls_position) <<"\n"
              <<"    insert recursively:       " <<(insert_recursively ? "yes" : "no") <<"\n"
              <<"    copy all definitions:     " <<(copy_definitions ? "yes" : "no") <<"\n";

    // Load replacement variable names (optional). If this isn't present then variables will have random names.
    SnippetFile::loadVariableNames("/usr/share/dict/words");

    // Parse the source code into which the snippet will be inserted and find a place to insert.
    SgProject *project = frontend(frontend_args);
    assert(project!=NULL);
    if (ipoint_function_name.empty()) {
        std::cerr <<"use --test:ipoint-function=NAME to specify a fully qualified insertion point function\n"
                  <<"the following insertion point functions are available:\n";
        list_function_definitions(project);
        exit(1);
    }
    InsertionPoint insertionPoint(project, ipoint_function_name);
    if (!insertionPoint.isValid()) {
        std::cerr <<"could not find insertion point\n";
        return 1;
    }

    SnippetPtr snippet;
    if (SageInterface::is_Java_language()) {
        // ROSE's java support doesn't currently let us parse another java file after we've called frontend().  Therefore, the
        // snippet file must have been passed as one of ROSE's command-line arguments so that it's been processed by frontend()
        // already. We just have to find it and create a Snippet object that points to that part of the AST.
        snippet = SnippetTests::findSnippetInAst(project, snippet_file_name, snippet_name);
    } else {
        // Load the snippet from its file.  This actually loads all the snippets in the file.
        snippet = Snippet::instanceFromFile(snippet_name, SnippetTests::findSnippetFile(snippet_file_name));
    }
    assert(snippet!=NULL);

    // Insert the snippet. This test just passes the first N local variables as snippet arguments
    size_t nargs = snippet->numberOfArguments();
    std::vector<SgNode*> args(insertionPoint.localvars.begin(), insertionPoint.localvars.begin()+nargs);
    SgStatement *ipoint = insertionPoint.insert_here ? insertionPoint.insert_here : insertionPoint.last_stmt;
    snippet->setInsertMechanism(insert_mechanism);
    snippet->setLocalDeclarationPosition(locdecls_position);
    snippet->setInsertRecursively(insert_recursively);
    snippet->setCopyAllSnippetDefinitions(copy_definitions);
    snippet->insert(ipoint, args);

    // Unparse the modified source code
#if 1 /*DEBUGGING [Robb P. Matzke 2013-12-03]*/
    generateDOT(*project);
#endif
    backend(project);
    return 0;
}
