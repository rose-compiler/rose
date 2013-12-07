// Tests the various store and load snippet examples

#include "snippetTests.h"

using namespace rose;


int
main(int argc, char *argv[])
{
    std::string snippetFileName = SnippetTests::findSnippetFile("snippets4");
    SgProject *project = frontend(argc, argv);

#if 1
    SnippetPtr storeString = Snippet::instanceFromFile("storeHeapString", snippetFileName);
    SnippetPtr loadString  = Snippet::instanceFromFile("loadHeapString",  snippetFileName);
#else
    SnippetPtr storeString = Snippet::instanceFromFile("storeStructString", snippetFileName);
    SnippetPtr loadString  = Snippet::instanceFromFile("loadStructString",  snippetFileName);
#endif

    // Find the insertion point in the big file.
    SgFunctionDefinition *func_ipoint1 = SnippetTests::findFunctionDefinition(project, "::ipoint1");
    SgStatement *insertBefore = SnippetTests::findLastStatement(func_ipoint1);

    // Insert the storeString snippet, storing variable "s" from the specimen file.
    SgInitializedName *var_s = SnippetTests::findVariableDeclaration(func_ipoint1, "s");
    storeString->insert(insertBefore, var_s);

    // Insert the loadString snippet, loading from "storage" (defined by the previous snippet) into "s".
    SgInitializedName *var_storage = SnippetTests::findVariableDeclaration(func_ipoint1, "storage"); // we just inserted it
    loadString->insert(insertBefore, var_s, var_storage);

    backend(project);
}

