// Simple traversal + postprocessing to fix the ASTs for broken switch
// statements due to case/default statements in nested scopes.

#include <rose.h>
#include <functional>

class DDFix: public std::binary_function<SgNode*, std::set<SgBasicBlock *>* , void* >
{
public:
    void* operator()(first_argument_type node, std::set<SgBasicBlock*>* blocksToFixUp ) const{
            if (!isSgSwitchStatement(node->get_parent()->get_parent()))
            {
                SgNode *p = node->get_parent()->get_parent();
                while (!isSgSwitchStatement(p))
                    p = p->get_parent();
                SgSwitchStatement *sw = isSgSwitchStatement(p);
                ROSE_ASSERT(sw != NULL);

                sw->get_body()->get_statements().remove(isSgStatement(node));
                blocksToFixUp->insert(isSgBasicBlock(node->get_parent()));
            }
     }

};

void fixBlocks(std::set<SgBasicBlock *> &blocks)
{
    std::set<SgBasicBlock *>::iterator block;
    for (block = blocks.begin(); block != blocks.end(); ++block)
    {
        SgBasicBlock *currentBlockToFill = NULL;
        SgStatementPtrList &stmts = (*block)->get_statements();
        SgStatementPtrList::iterator stmt = stmts.begin();
        while (stmt != stmts.end())
        {
            // For each statement: If it is a case or default
            // statement, make its body the "current block" that is
            // filled with all the following statements up to the
            // next case or default. Normal statements are moved
            // into the current block, if there is one.
            if (isSgCaseOptionStmt(*stmt))
            {
                currentBlockToFill = isSgCaseOptionStmt(*stmt)->get_body();
                ++stmt;
            }
            else if (isSgDefaultOptionStmt(*stmt))
            {
                currentBlockToFill = isSgDefaultOptionStmt(*stmt)->get_body();
                ++stmt;
            }
            else if (currentBlockToFill != NULL)
            {
                currentBlockToFill->append_statement(*stmt);
                stmt = stmts.erase(stmt);
            }
            else
            {
                // We should only get here for "normal" statements
                // that come before any case or default; keep these
                // unchanged by ignoring them.
                ++stmt;
            }
        }
        // This should print our shiny new fixed block.
        std::cout << (*block)->unparseToString() << std::endl;
    }
}

int main(int argc, char **argv)
{
    SgProject *project = frontend(argc, argv);

    DDFix ddfix;
    VariantVector vv1 = V_SgCaseOptionStmt + V_SgDefaultOptionStmt;

    std::set<SgBasicBlock*> blocksToFixUp;

    AstQueryNamespace::queryMemoryPool(std::bind2nd( ddfix, &blocksToFixUp ),&vv1);

    std::cout << "will fix " << blocksToFixUp.size()
        << " blocks:" << std::endl;
    std::set<SgBasicBlock *>::iterator b;
    for (b = blocksToFixUp.begin(); b != blocksToFixUp.end(); ++b)
        std::cout << (*b)->unparseToString() << std::endl;
    std::cout << std::endl << "fixing blocks..." << std::endl;
    fixBlocks(blocksToFixUp);

    // Is everything OK?
    AstTests::runAllTests(project);
    return backend(project);
}
