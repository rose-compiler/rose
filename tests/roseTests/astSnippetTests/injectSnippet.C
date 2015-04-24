#include "snippetTests.h"
#include "stringify.h"

#include <boost/algorithm/string/erase.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

using namespace rose;

static void
usage(const std::string &arg0)
{
    std::cerr <<"usage: " <<arg0 <<" --test:snippet=FILE_NAME,SNIPPET_NAME --test:ipoint-function=NAME [TEST_SWITCHES]"
              <<" [ROSE_SWITCHES] SPECIMEN\n"
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
              <<"        If the FQNAME is the word \"none\" then no snippets are inserted.\n"
              <<"  The following switches are optional:\n"
              <<"    --test:insert-mechanism=(body|stmts)\n"
              <<"        Indicates how the snippet is inserted into the SPECIMEN.  If the value\n"
              <<"        is \"body\" then the entire snippet body is inserted, scope and all.\n"
              <<"        If the value is \"stmts\" then each snippet statement is copied into\n"
              <<"        the insertion site's scope, placing variable declarations near the\n"
              <<"        top of the scope and other statements at the insertion point. The\n"
              <<"        default is \"stmts\".\n"
              <<"    --test:locdecls-position=(beginning|end|cursor)\n"
              <<"        When the insert-mechanism is \"stmts\", this switch controls whether\n"
              <<"        declaration statements in the snippet are copied to the very beginning\n"
              <<"        of the insertion point's scope, appended to the end of declarations\n"
              <<"        that are already present in the insertion point's scope, or simply inserted\n"
              <<"        along with the non-declaration statements without moving them.  The default\n"
              <<"        is \"end\" for C and \"cursor\" for Java. The \"cursor\" setting only works\n"
              <<"        for languages like Java and C++ which allow declarations and non-declarations\n"
              <<"        to be mixed within a single scope.\n"
              <<"    --test:recursive=(yes|no)\n"
              <<"        Determines whether snippets are recursively inserted when a snippet\n"
              <<"        calls other snippets that are defined in the same snippet file.  The\n"
              <<"        default is \"yes\".\n"
              <<"    --test:copy-definitions=(yes|no)\n"
              <<"        Determines whether all snippet definitions are copied into the specimen\n"
              <<"        file.  The default is to not copy any definitions since the are normally\n"
              <<"        inserted recursively at insertion sites in the specimen.\n"
              <<"    --test:skip-ast-fixup\n"
              <<"        Skip all AST fixup steps that normally occur after each injection.  This\n"
              <<"        is mostly for debugging, so we can check that the injection mechanism works\n"
              <<"        when the AST fixup mechanism is failing.\n";
    exit(1);
}

// Holds information about where to insert something
struct InsertionPoint {
    SgFunctionDefinition *function;
    SgStatement *last_stmt, *insert_here;
    std::vector<SgInitializedName*> localvars;

    InsertionPoint()
        : function(NULL), last_stmt(NULL), insert_here(NULL) {}

    InsertionPoint(SgProject *project, const std::string &function_name)
        : function(NULL), last_stmt(NULL), insert_here(NULL) {
        if ((function = SnippetTests::findFunctionDefinition(project, function_name))) {
            last_stmt = SnippetTests::findLastStatement(function);
            localvars = SnippetTests::findFunctionVariables(function);
            insert_here = SnippetTests::findInsertHere(function);
        }
    }

    bool isValid(size_t nvars=0) const {
        return function!=NULL && last_stmt!=NULL && localvars.size() >= nvars;
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
    boost::optional<Snippet::LocalDeclarationPosition> locdecls_position;
    bool insert_recursively = true, copy_definitions = false, shouldFixupAst = true;
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
        } else if (!strcmp(argv[argno], "--test:locdecls-position=cursor")) {
            locdecls_position = Snippet::LOCDECLS_AT_CURSOR;
        } else if (!strcmp(argv[argno], "--test:copy-definitions=yes")) {
            copy_definitions = true;
        } else if (!strcmp(argv[argno], "--test:copy-definitions=no")) {
            copy_definitions = false;
        } else if (!strncmp(argv[argno], "--test:ipoint-function=", 23)) {
            ipoint_function_name = argv[argno] + 23;
        } else if (!strcmp(argv[argno], "--test:skip-ast-fixup")) {
            shouldFixupAst = false;
        } else if (!strncmp(argv[argno], "--test:", 7)) {
            std::cerr <<"unknown switch: " <<argv[argno] <<"\n"
                      <<"run \"" <<argv[0] <<" --help\" to see usage information.\n";
        } else {
            frontend_args.push_back(argv[argno]);
        }
    }
    if (snippet_name.empty())
        usage(argv0);

    // One of the SS_JAVA_CWES tests uses an upper-case name for the snippet function rather than the usual lower-case name.
    if (snippet_name == "Injection.CWE_564.CWE_564_3.cwe_564_3")
        snippet_name =  "Injection.CWE_564.CWE_564_3.CWE_564_3";

    std::cout <<"Configuration:\n"
              <<"    snippet file base name:   " <<snippet_file_name <<"\n"
              <<"    snippet name:             " <<snippet_name <<"\n"
              <<"    insertion point function: " <<ipoint_function_name <<"\n"
              <<"    insert mechanism:         " <<stringifySnippetInsertMechanism(insert_mechanism) <<"\n";
    if (locdecls_position) {
        std::cout <<"    local decls position:     " <<stringifySnippetLocalDeclarationPosition(*locdecls_position) <<"\n";
    } else {
        std::cout <<"    local decls position:     depends on language (C=LOCDECLS_AT_END; Java=LOCDECLS_AT_CURSOR)\n";
    }
    std::cout <<"    insert recursively:       " <<(insert_recursively ? "yes" : "no") <<"\n"
              <<"    copy all definitions:     " <<(copy_definitions ? "yes" : "no") <<"\n"
              <<"    fixup AST after inject:   " <<(shouldFixupAst ? "yes" : "no") <<"\n";

#if 0
    // DQ (2/28/2014): This code is causing memory problems.
    // Valgrind shows no problems here on my machine, although there are 10 prior "Conditional jump or move depends on
    // uninialize value" errors reported before this point, all of which are in SgTreeTraversal. [Robb P. Matzke 2014-03-07]
    // Load replacement variable names (optional). If this isn't present then variables will have random names.
    SnippetFile::loadVariableNames("/usr/share/dict/words");
#endif

 // DQ (4/16/2015): Adding -c option to avoid linking in the backend compiler (required 
 // due to recent bug fixes in the command line option generation from the backend).
    frontend_args.push_back("-c");

    // Parse the source code into which the snippet will be inserted and find a place to insert.
    SgProject *project = frontend(frontend_args);
    assert(project!=NULL);
    if (ipoint_function_name.empty()) {
        std::cerr <<"use --test:ipoint-function=NAME to specify a fully qualified insertion point function\n"
                  <<"the following insertion point functions are available:\n";
        list_function_definitions(project);
        exit(1);
    }
    InsertionPoint insertionPoint;
    if (0!=ipoint_function_name.compare("none")) {
        insertionPoint = InsertionPoint(project, ipoint_function_name);
        if (!insertionPoint.isValid()) {
            std::cerr <<"could not find insertion point\n";
            return 1;
        }
    }

    // Load the snippet from its file.  This actually loads all the snippets in the file.
    SnippetPtr snippet = Snippet::instanceFromFile(snippet_name, SnippetTests::findSnippetFile(snippet_file_name));


    assert(snippet!=NULL);
    SnippetFilePtr snippetFile = snippet->getFile();
    snippetFile->setCopyAllSnippetDefinitions(copy_definitions);

    // Test that we can prevent some things from being copied in C
    snippetFile->doNotInsert("::shouldNotBeInserted");  // a function
    snippetFile->doNotInsert("::SomeOtherInteger");     // a typedef
    snippetFile->doNotInsert("::someOtherInteger");     // a global variable
    snippetFile->doNotInsert("::SomeOtherStruct");      // a struct

    // Test that we can prevent some things from being copied in Java
    snippetFile->doNotInsert("Snippets6.shouldNotBeInserted");
    snippetFile->doNotInsert("Snippets6.someOtherInteger");
    snippetFile->doNotInsert("Snippets6.SomeOtherStruct");

    // Never copy the main() function from the snippet file to the target file
    if (SageInterface::is_C_language()) {
        snippetFile->doNotInsert("::main");
    } else {
        assert(SageInterface::is_Java_language());
        size_t slashIdx = snippet_file_name.rfind('/');
        std::string className = boost::erase_last_copy((slashIdx==std::string::npos ?
                                                        snippet_file_name :
                                                        snippet_file_name.substr(slashIdx+1)),
                                                       ".java");
        snippetFile->doNotInsert(className + ".main");
    }
    
    // Test that we can black list functions based on their signature, not just their name.
    std::vector<SgFunctionDefinition*> someFunctions = SnippetTests::findFunctionDefinitions(snippetFile->getAst(),
                                                                                             "Snippets6.someFunction");
    BOOST_FOREACH (SgFunctionDefinition *someFunction, someFunctions) {
        SgFunctionDeclaration *fdecl = someFunction->get_declaration();
        if (0==fdecl->get_args().size())
            snippetFile->doNotInsert("Snippets6.someFunction", fdecl->get_type());
    }

#if 0 // DEBUGGING [DQ 2014-03-07]
    SgFile* tmp_snippetSourceFile = snippet->getFile()->getAst();

    ROSE_ASSERT(project->get_fileList_ptr() != NULL);
    SgFilePtrList & vectorFile = project->get_fileList_ptr()->get_listOfFiles();
    printf ("project files (size = %zu): \n",vectorFile.size());
    for (size_t i = 0; i < vectorFile.size(); i++)
       {
         printf ("--- filename = %s \n",vectorFile[i]->getFileName().c_str());
       }

    ROSE_ASSERT(vectorFile.size() == 2);
#endif

#if 0 // DEBUGGING [DQ 2014-03-07]
    printf ("Test the use of the SageInterface::deleteAST() function (appears to fail for snippet file that include stdio.h) \n");
    SageInterface::deleteAST(tmp_snippetSourceFile);
    printf ("Exiting after test of deleteAST on new file \n");
    ROSE_ASSERT(false);
#endif

#if 0 // DEBUGGING [DQ 2014-03-07]
    printf ("After reading snippet file: project->get_fileList().size() = %zu snippetSourceFile = %p = %s = %s \n",
            project->get_fileList().size(),tmp_snippetSourceFile,tmp_snippetSourceFile->class_name().c_str(),
            tmp_snippetSourceFile->getFileName().c_str());
#endif

    // Insert the snippet. This test just passes the first N local variables as snippet arguments
    if (insertionPoint.isValid()) {
        size_t nargs = snippet->numberOfArguments();
        if (nargs > insertionPoint.localvars.size()) {
            throw std::runtime_error("not enough local variables to insert " + snippet_name + " into " + ipoint_function_name +
                                     "; the snippet needs " + StringUtility::plural(nargs, "arguments") +
                                     " but the target function has " +
                                     StringUtility::plural(insertionPoint.localvars.size(), "local variables"));
        }
        std::vector<SgNode*> args(insertionPoint.localvars.begin(), insertionPoint.localvars.begin()+nargs);
        SgStatement *ipoint = insertionPoint.insert_here ? insertionPoint.insert_here : insertionPoint.last_stmt;

        snippet->setInsertMechanism(insert_mechanism);
        if (locdecls_position) {
            snippet->setLocalDeclarationPosition(*locdecls_position);
        } else if (SageInterface::is_C_language()) {
            snippet->setLocalDeclarationPosition(Snippet::LOCDECLS_AT_END);
        } else {
            assert(SageInterface::is_Java_language());
            snippet->setLocalDeclarationPosition(Snippet::LOCDECLS_AT_CURSOR);
        }
        snippet->setInsertRecursively(insert_recursively);
        snippet->setFixupAst(shouldFixupAst);

#if 0 // DEBUGGING [DQ 2014-03-07]
        printf ("Test 1: project->get_fileList().size() = %zu snippetSourceFile = %p = %s = %s \n",
                project->get_fileList().size(),tmp_snippetSourceFile,tmp_snippetSourceFile->class_name().c_str(),
                tmp_snippetSourceFile->getFileName().c_str());
#endif

        snippet->insert(ipoint, args);

#if 0 // DEBUGGING [2014-03-07]
        printf ("Test 2: project->get_fileList().size() = %zu snippetSourceFile = %p = %s = %s \n",
                project->get_fileList().size(),tmp_snippetSourceFile,tmp_snippetSourceFile->class_name().c_str(),
                tmp_snippetSourceFile->getFileName().c_str());
#endif
    }

#if 1
    if (shouldFixupAst) {
        // DQ (2/1/2014): delete the snippet AST as a test of the fixup of the target AST.
        ROSE_ASSERT(snippet->getFile() != NULL);
        SgFile* snippetSourceFile = snippet->getFile()->getAst();
        ROSE_ASSERT(snippetSourceFile != NULL);

        // Disconnect the snippetSourceFile from the SgProject.
        std::vector<SgFilePtrList::iterator> eraseTheseFiles;
        SgFilePtrList::iterator i = project->get_fileList().begin();
        while (i != project->get_fileList().end()) {
            // Find the snippet file.
            if (*i == snippetSourceFile)
                eraseTheseFiles.push_back(i);
            i++;
        }

        snippetSourceFile = NULL;
        for (size_t j = 0; j < eraseTheseFiles.size(); j++)
            project->get_fileList().erase(eraseTheseFiles[j]);
    }
#endif

    // Unparse the modified source code
#if 1 /* [Robb P. Matzke 2014-03-03]: does not yet work for Java -- fails an assertion deep in rose  */
    generateDOT(*project);
#endif
// #elif 0
#if 0
    // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
    printf ("Generate the Whole AST graph! \n");
    const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
    generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
    printf ("DONE: Generate the Whole AST graph! \n");
#endif

    return backend(project);
}
