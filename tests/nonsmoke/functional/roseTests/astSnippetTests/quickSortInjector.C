// Inject a few things into the quicksort.c or QuickSort.java specimen
#include "snippetTests.h"

using namespace rose;

// Find the first statement of the specified type under the specified AST.
template<typename StatementType>
static StatementType *findStatement(SgNode *ast) {
    struct: AstSimpleProcessing {
        void visit(SgNode *node) {
            if (StatementType *stmt = dynamic_cast<StatementType*>(node))
                throw stmt;
        }
    } t1;
    try {
        t1.traverse(ast, preorder);
    } catch (StatementType *found) {
        return found;
    }
    return NULL;
}

// Find first statement that makes an assignment to the specified variable
static SgStatement *findAssignment(SgNode *ast, const std::string &variableName) {
    struct T1: AstSimpleProcessing {
        std::string variableName;
        T1(const std::string &variableName): variableName(variableName) {}
        void visit(SgNode *node) {
            if (SgAssignOp *assn = isSgAssignOp(node)) {
                if (SgVarRefExp *vref = isSgVarRefExp(assn->get_lhs_operand())) {
                    std::string vrefName = vref->get_symbol()->get_name().getString();
                    if (0==vrefName.compare(variableName))
                        throw SageInterface::getEnclosingNode<SgStatement>(node);
                }
            }
        }
    } t1(variableName);
    try {
        t1.traverse(ast, preorder);
        return NULL;
    } catch (SgStatement *stmt) {
        return stmt;
    }
}

// Find the last return statement
static SgReturnStmt *findLastReturnStatement(SgNode *ast) {
    struct T1: AstSimpleProcessing {
        SgReturnStmt *lastReturn;
        T1(): lastReturn(NULL) {}
        void visit(SgNode *node) {
            if (SgReturnStmt *ret = isSgReturnStmt(node))
                lastReturn = ret;
        }
    } t1;
    t1.traverse(ast, preorder);
    return t1.lastReturn;
}

// Find the first statement in a while loop
static SgStatement *firstStatementInLoop(SgScopeStatement *loop) {
    const SgStatementPtrList &stmts = loop->getStatementList();
    assert(!stmts.empty());
    return stmts[0];
}

struct LanguageSpecificInfo {
    std::string snippetFileName;                        // name of file containing snippets
    std::string quickSortFQN;                           // fully qualified name for the quickSort function
    std::string readEnvironmentFQN;                     // fully qualified name for the readEnvironment snippet
    std::string saveIntegerFQN;                         // fully qualified name for the saveInteger snippet
    std::string partitionFQN;                           // fully qualified name for the partition function
    std::string restoreIntegerFQN;                      // fully qualified name for the restoreInteger snippet
    std::string arrayElementSwapFQN;                    // fully qualified name for the arrayElementSwap snippet
    LanguageSpecificInfo() {
        if (SageInterface::is_C_language()) {
            snippetFileName = "QuickSortC/errorSnippets.c";
            quickSortFQN = "::quickSort";
            readEnvironmentFQN = "::readEnvironment";
            saveIntegerFQN = "::saveInteger";
            partitionFQN = "::partition";
            restoreIntegerFQN = "::restoreInteger";
            arrayElementSwapFQN = "::arrayElementSwap";
        } else {
            assert(SageInterface::is_Java_language());
            snippetFileName = "QuickSortJava/ErrorSnippets.java";
            quickSortFQN = "QuickSort.quickSort";
            readEnvironmentFQN = "ErrorSnippets.readEnvironment";
            saveIntegerFQN = "ErrorSnippets.saveInteger";
            partitionFQN = "QuickSort.partition";
            restoreIntegerFQN = "ErrorSnippets.restoreInteger";
            arrayElementSwapFQN = "ErrorSnippets.arrayElementSwap";
        }
    }
};

int main(int argc, char *argv[])
{
    // Load the target specimen into ROSE. This is the code which we will change by injecting snippets.  If we are performing
    // AST fixups after each snippet insertion, then the target file must include all necessary declarations upon which the
    // inserted snippets depend.
    std::cerr <<"Parsing the target specimen...\n";
    SgProject *project = frontend(argc, argv);

    // Language-specific information so this test can run for both java and c specimens
    LanguageSpecificInfo languageSpecificInfo;

    // Load the snippet file that contains all the snippets we wish to inject.  It is permissible to have more than one
    // such file, but we use only one here.  The snippet files are not unparsed -- only the target specimen is eventually
    // unparsed.
    std::cerr <<"Loading the snippet file...\n";
    std::string snippetFileName = SnippetTests::findSnippetFile(languageSpecificInfo.snippetFileName);
    SnippetFilePtr snippetFile = SnippetFile::instance(snippetFileName);
    ROSE_ASSERT(snippetFile!=NULL || !"unable to load snippet file");

 // DQ (3/16/2014): After discussion with Robb, we fixed a bug in Snippet::insertRelatedThingsForC() that
 // was overly permissive in inserting snippet declarations. As a result, I think we need this to be true.
 // snippetFile->setCopyAllSnippetDefinitions(false);
    snippetFile->setCopyAllSnippetDefinitions(true);

#if 1 // DEBUGGING [DQ 2014-03-07]
    {
    ROSE_ASSERT(project->get_fileList_ptr() != NULL);
    SgFilePtrList & vectorFile = project->get_fileList_ptr()->get_listOfFiles();
    printf ("project files (size = %zu): \n",vectorFile.size());
    for (size_t i = 0; i < vectorFile.size(); i++)
       {
         printf ("--- filename = %p = %s \n",vectorFile[i],vectorFile[i]->getFileName().c_str());
       }

    ROSE_ASSERT(vectorFile.size() == 2);
    }
#endif

    static const bool shouldFixupAst = true;
    if (!shouldFixupAst)
        std::cerr <<"NOTE: We are not fixing up the AST after snippet insertions!\n";

    // Find where to insert the readEnvironment snippet. Look for "SNIPPET" comments in the target specimen.  This snippet
    // should be inserted before the first "while" statement of the quickSort function.
    std::cerr <<"Looking for the readEnvironment insertion point...\n";
    SgFunctionDefinition *quickSort = SnippetTests::findFunctionDefinition(project, languageSpecificInfo.quickSortFQN);
    ROSE_ASSERT(quickSort || !"could not find the quickSort function in the target specimen");
    SgWhileStmt *readEnvironmentInsertionPoint = findStatement<SgWhileStmt>(quickSort);
    ROSE_ASSERT(readEnvironmentInsertionPoint || !"could not find insertion point for readEnvironment");

    // Insert the readEnvironment snippet
    std::cerr <<"Inserting the readEnvironment snippet...\n";
    SnippetPtr readEnvironment = snippetFile->findSnippet(languageSpecificInfo.readEnvironmentFQN);
    ROSE_ASSERT(readEnvironment!=NULL || !"unable to find the readEnvironment snippet");
    ROSE_ASSERT(readEnvironment->numberOfArguments()==0);
    readEnvironment->setInsertMechanism(Snippet::INSERT_STMTS);
    readEnvironment->setLocalDeclarationPosition(Snippet::LOCDECLS_AT_END);
    readEnvironment->setInsertRecursively(false);
    readEnvironment->setFixupAst(shouldFixupAst);
    readEnvironment->insert(readEnvironmentInsertionPoint);

    // Find where to insert the saveInteger snippet, and the local variable ("qs_error" from the readEnvironment snippet) that
    // we want to save.
    std::cerr <<"Looking for the saveInteger insertion point...\n";
    SgInitializedName *qs_error = SnippetTests::findVariableDeclaration(quickSort, "qs_error");
    ROSE_ASSERT(qs_error!=NULL || !"unable to locate the 'qs_error' local variable declared by the readEnvironment snippet");
    SgStatement *saveIntegerInsertionPoint = firstStatementInLoop(readEnvironmentInsertionPoint);
    ROSE_ASSERT(saveIntegerInsertionPoint || !"could not find insertion point for saveInteger");

    // Insert the saveInteger snippet
    std::cerr <<"Inserting the saveInteger snippet...\n";
    SnippetPtr saveInteger = snippetFile->findSnippet(languageSpecificInfo.saveIntegerFQN);
    ROSE_ASSERT(saveInteger!=NULL || !"unable to find the saveInteger snippet");
    ROSE_ASSERT(saveInteger->numberOfArguments()==1);
    saveInteger->setInsertMechanism(Snippet::INSERT_STMTS);
    saveInteger->setLocalDeclarationPosition(Snippet::LOCDECLS_AT_END);
    saveInteger->setInsertRecursively(true);
    saveInteger->setFixupAst(shouldFixupAst);
    saveInteger->insert(saveIntegerInsertionPoint, qs_error);

    // Find where to insert the restoreInteger snippet.
    std::cerr <<"Looking for the restoreInteger insertion point...\n";
    SgFunctionDefinition *partition = SnippetTests::findFunctionDefinition(project, languageSpecificInfo.partitionFQN);
    ROSE_ASSERT(partition || !"could not find the partition function in the target specimen");
    SgStatement *restoreIntegerInsertionPoint = findAssignment(partition, "i");
    ROSE_ASSERT(restoreIntegerInsertionPoint || !"could not find assignment to 'i' in partition()");

    // Insert restoreInteger snippet
    std::cerr <<"Inserting the restoreInteger snippet...\n";
    SnippetPtr restoreInteger = snippetFile->findSnippet(languageSpecificInfo.restoreIntegerFQN);
    ROSE_ASSERT(restoreInteger!=NULL || !"unable to find the restoreInteger snippet");
    ROSE_ASSERT(restoreInteger->numberOfArguments()==0);
    restoreInteger->setInsertMechanism(Snippet::INSERT_STMTS);
    restoreInteger->setLocalDeclarationPosition(Snippet::LOCDECLS_AT_END);
    restoreInteger->setInsertRecursively(true);
    restoreInteger->setFixupAst(shouldFixupAst);
    restoreInteger->insert(restoreIntegerInsertionPoint);

    // Find where to insert the arrayElementSwap snippet
    std::cerr <<"Looking for the arrayElementSwap insertion point...\n";
    SgStatement *arrayElementSwapInsertionPoint = findLastReturnStatement(partition);
    ROSE_ASSERT(arrayElementSwapInsertionPoint || !"could not find insertion point for the arrayElementSwap snippet");
    SgInitializedName *var_a = SnippetTests::findArgumentDeclaration(partition, "a");
    ROSE_ASSERT(var_a || !"unable to find local variable 'a' in partition()");
    SgInitializedName *var_j = SnippetTests::findVariableDeclaration(partition, "j");
    ROSE_ASSERT(var_j || !"unable to find local variable 'j' in partition()");
    SgInitializedName *var_lb = SnippetTests::findArgumentDeclaration(partition, "lb");
    ROSE_ASSERT(var_lb || !"unable to find local variable 'lb' in partition()");
    SgInitializedName *var_restoredValue = SnippetTests::findVariableDeclaration(partition, "restoredValue");
    ROSE_ASSERT(var_restoredValue || !"unable to find local variable 'restoredValue' in partition()");

    // Insert the arrayElementSwap snippet
    std::cerr <<"Inserting the arrayElementSwap snippet...\n";
    SnippetPtr arrayElementSwap = snippetFile->findSnippet(languageSpecificInfo.arrayElementSwapFQN);
    ROSE_ASSERT(arrayElementSwap!=NULL || !"unable to find the arrayElementSwap snippet");
    ROSE_ASSERT(arrayElementSwap->numberOfArguments()==4);
    arrayElementSwap->setInsertMechanism(Snippet::INSERT_BODY);
    arrayElementSwap->setInsertRecursively(false);
    arrayElementSwap->setFixupAst(shouldFixupAst);
    arrayElementSwap->insert(arrayElementSwapInsertionPoint, var_a, var_j, var_lb, var_restoredValue);

    SgFile* snippetSourceFile = snippetFile->getAst();
    ROSE_ASSERT(snippetSourceFile != NULL);

 // Disconnect the snippetSourceFile from the SgProject.
    std::vector<SgFilePtrList::iterator> eraseTheseFiles;
    SgFilePtrList::iterator i = project->get_fileList().begin();
    while (i != project->get_fileList().end())
       {
      // Find the snippet file.
         if (*i == snippetSourceFile)
            {
#if 0 // DEBUGGING [DQ 2014-03-07]
              printf ("Removing snippetSourceFile = %p from project \n",snippetSourceFile);
#endif
              eraseTheseFiles.push_back(i);
           // *i = NULL;
            }

         i++;
       }

    snippetSourceFile = NULL;
    for (size_t j = 0; j < eraseTheseFiles.size(); j++)
       {
         project->get_fileList().erase(eraseTheseFiles[j]);
       }

#if 1 // DEBUGGING [DQ 2014-03-07]
    {
    ROSE_ASSERT(project->get_fileList_ptr() != NULL);
    SgFilePtrList & vectorFile = project->get_fileList_ptr()->get_listOfFiles();
    printf ("project files (size = %zu): \n",vectorFile.size());
    for (size_t i = 0; i < vectorFile.size(); i++)
       {
         printf ("--- filename = %p = %s \n",vectorFile[i],vectorFile[i]->getFileName().c_str());
       }

    ROSE_ASSERT(vectorFile.size() == 1);
    }
#endif

    
    return backend(project);
}
