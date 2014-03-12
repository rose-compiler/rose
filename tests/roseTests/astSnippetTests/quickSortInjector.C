// Inject a few things into the quicksort.c specimen
#include "snippetTests.h"

// Define this if you want to do AST fixups after each insertion.
#define PERFORM_AST_FIXUPS

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

int main(int argc, char *argv[])
{
    // Load the target specimen into ROSE. This is the code which we will change by injecting snippets.  If we are performing
    // AST fixups after each snippet insertion, then the target file must include all necessary declarations upon which the
    // inserted snippets depend.
    std::vector<std::string> args(argv, argv+argc);
#ifdef PERFORM_AST_FIXUPS
    args.insert(args.begin()+1, "-DDECLARE_RELATED_THINGS");
#endif
    std::cerr <<"Parsing the target specimen...\n";
    SgProject *project = frontend(args);

    // Load the snippet file that contains all the snippets we wish to inject.  It is permissible to have more than one
    // such file, but we use only one here.  The snippet files are not unparsed -- only the target specimen is eventually
    // unparsed.
    std::cerr <<"Loading the snippet file...\n";
    std::string snippetFileName = SnippetTests::findSnippetFile("QuickSortC/errorSnippets.c");
    SnippetFilePtr snippetFile = SnippetFile::instance(snippetFileName);
    ROSE_ASSERT(snippetFile!=NULL || !"unable to load snippet file");
    snippetFile->setCopyAllSnippetDefinitions(false);

    // If we're performing AST fixups after each insertion then we should not be copying things (global declarations, include
    // directives etc) that are related to the snippets that are inserted.
#ifdef PERFORM_AST_FIXUPS
    snippetFile->setCopyRelatedThings(false);
#else
    snippetFile->setCopyRelatedThings(true);
#endif

    // Find where to insert the readEnvironment snippet. Look for "SNIPPET" comments in the target specimen.  This snippet
    // should be inserted before the first "while" statement of the ::quickSort function.
    std::cerr <<"Looking for the readEnvironment insertion point...\n";
    SgFunctionDefinition *quickSort = SnippetTests::findFunctionDefinition(project, "::quickSort");
    ROSE_ASSERT(quickSort || !"could not find ::quickSort in the target specimen");
    SgWhileStmt *readEnvironmentInsertionPoint = findStatement<SgWhileStmt>(quickSort);
    ROSE_ASSERT(readEnvironmentInsertionPoint || !"could not find insertion point for readEnvironment");

    // Insert the readEnvironment snippet
    std::cerr <<"Inserting the readEnvironment snippet...\n";
    SnippetPtr readEnvironment = snippetFile->findSnippet("::readEnvironment");
    ROSE_ASSERT(readEnvironment!=NULL || !"unable to find ::readEnvironment snippet");
    ROSE_ASSERT(readEnvironment->numberOfArguments()==0);
    readEnvironment->setInsertMechanism(Snippet::INSERT_STMTS);
    readEnvironment->setLocalDeclarationPosition(Snippet::LOCDECLS_AT_END);
    readEnvironment->setInsertRecursively(false);
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
    SnippetPtr saveInteger = snippetFile->findSnippet("::saveInteger");
    ROSE_ASSERT(saveInteger!=NULL || !"unable to find ::saveInteger snippet");
    ROSE_ASSERT(saveInteger->numberOfArguments()==1);
    saveInteger->setInsertMechanism(Snippet::INSERT_STMTS);
    saveInteger->setLocalDeclarationPosition(Snippet::LOCDECLS_AT_END);
    saveInteger->setInsertRecursively(true);
    saveInteger->insert(saveIntegerInsertionPoint, qs_error);

    // Find where to insert the restoreInteger snippet.
    std::cerr <<"Looking for the restoreInteger insertion point...\n";
    SgFunctionDefinition *partition = SnippetTests::findFunctionDefinition(project, "::partition");
    ROSE_ASSERT(partition || !"could not find ::partition in the target specimen");
    SgStatement *restoreIntegerInsertionPoint = findAssignment(partition, "i");
    ROSE_ASSERT(restoreIntegerInsertionPoint || !"could not find assignment to 'i' in partition()");

    // Insert restoreInteger snippet
    std::cerr <<"Inserting the restoreInteger snippet...\n";
    SnippetPtr restoreInteger = snippetFile->findSnippet("::restoreInteger");
    ROSE_ASSERT(restoreInteger!=NULL || !"unable to find ::restoreInteger snippet");
    ROSE_ASSERT(restoreInteger->numberOfArguments()==0);
    restoreInteger->setInsertMechanism(Snippet::INSERT_STMTS);
    restoreInteger->setLocalDeclarationPosition(Snippet::LOCDECLS_AT_END);
    restoreInteger->setInsertRecursively(true);
    restoreInteger->insert(restoreIntegerInsertionPoint);

    // Find where to insert the arrayElementSwap snippet
    std::cerr <<"Looking for the arrayElementSwap insertion point...\n";
    SgStatement *arrayElementSwapInsertionPoint = findLastReturnStatement(partition);
    ROSE_ASSERT(arrayElementSwapInsertionPoint || !"could not find insertion point for ::arrayElementSwap");
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
    SnippetPtr arrayElementSwap = snippetFile->findSnippet("::arrayElementSwap");
    ROSE_ASSERT(arrayElementSwap!=NULL || !"unable to find ::arrayElementSwap snippet");
    ROSE_ASSERT(arrayElementSwap->numberOfArguments()==4);
    arrayElementSwap->setInsertMechanism(Snippet::INSERT_BODY);
    arrayElementSwap->setInsertRecursively(false);
    arrayElementSwap->insert(arrayElementSwapInsertionPoint, var_a, var_j, var_lb, var_restoredValue);
    
    return backend(project);
}
