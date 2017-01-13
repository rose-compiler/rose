// Tests the various store and load snippet examples

#include "snippetTests.h"

using namespace rose;


int
main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr <<"usage: " <<argv[0] <<" HeapString|StructString [ROSE_SWITCHES] SPECIMEN\n";
        exit(1);
    }
    std::vector<std::string> args(argv+0, argv+argc);
    std::string what = args[1];
    args.erase(args.begin()+1);
    SgProject *project = frontend(args);
    

    std::string snippetFileName = SnippetTests::findSnippetFile("snippets4.c");
    SnippetPtr storeString = Snippet::instanceFromFile("::store"+what, snippetFileName);
    SnippetPtr loadString  = Snippet::instanceFromFile("::load"+what,  snippetFileName);

    SnippetFilePtr snippetFile = storeString->getFile();
    assert(snippetFile == loadString->getFile());

    // Find the insertion point in the big file.
    SgFunctionDefinition *func_ipoint1 = SnippetTests::findFunctionDefinition(project, "::main");
    SgStatement *insertBefore = SnippetTests::findInsertHere(func_ipoint1);
    assert(insertBefore!=NULL);

    // Insert the storeString snippet, storing variable "s" from the specimen file.
    SgInitializedName *var_s = SnippetTests::findVariableDeclaration(func_ipoint1, "s");
    storeString->insert(insertBefore, var_s);

    // Insert the loadString snippet, loading from "storage" (defined by the previous snippet) into "s".
    SgInitializedName *var_storage = SnippetTests::findVariableDeclaration(func_ipoint1, "storage"); // we just inserted it
    loadString->insert(insertBefore, var_s, var_storage);

    // Remove the "INSERT_HERE" statement.
    SageInterface::removeStatement(insertBefore);
    backend(project);
}

