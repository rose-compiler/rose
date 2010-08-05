#include <rose.h>
#include <utilities/Utilities.h>
#include <utilities/CPPDefinesAndNamespaces.h>

using namespace SageInterface;
using namespace SageBuilder;
using namespace backstroke_util;
using namespace std;

int main(int argn, char** argv)
{
    SgProject* project = frontend(argn, argv);

#if 0
    SgBasicBlock* new_block = buildBasicBlock();
    new_block->append_statement(buildVariableDeclaration("i", buildIntType(), NULL, new_block));
    new_block->append_statement(buildExprStatement(buildVarRefExp("i")));
#endif

    vector<SgBasicBlock*> blocks = querySubTree<SgBasicBlock>(project);
    foreach(SgBasicBlock* block, blocks)
        if (isSgBasicBlock(block->get_parent()))
        {
            //SgStatement* stmt = copyStatement(block);

            SgStatement * cpy = copyStatement(
                isSgBasicBlock(block)->get_statements()[0]);

            cout << isSgVariableDeclaration(
                    cpy)->get_variables()[0]->get_scope() << endl;

            insertStatement(block, cpy);
            //isSgVariableDeclaration(cpy)->get_variables()[0]->set_scope(isSgBasicBlock(block->get_parent()));
            //rebuildSymbolTable(isSgBasicBlock(block->get_parent()));

            cout << isSgVariableDeclaration(
                    cpy)->get_variables()[0]->get_scope() << endl;

            //deepDelete(stmt);
            //insertStatement(block, copyStatement(new_block));
            //insertStatement(block, (new_block));
        }


    std::cout << "Start\n";
    //deepDelete(copyStatement(new_block));
    //deepDelete(new_block);
    std::cout << "End\n";

    fixVariableReferences(project);

    generateWholeGraphOfAST("Cong2");

    AstTests::runAllTests(project);
    return backend(project);
}
