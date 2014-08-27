#include "rose.h"
#include "RoseAst.h"

int main(int argc, char *argv[])
{
    RoseAst ast(frontend(argc, argv));

    size_t maxinsns = 0;
    SgAsmFunction *biggestFunction = NULL;

    for (RoseAst::iterator ai=ast.begin(); ai!=ast.end(); ++ai) {
        if (SgAsmFunction *func = isSgAsmFunction(*ai)) {
            size_t ninsns = SageInterface::querySubTree<SgAsmInstruction>(func).size();
            if (ninsns > maxinsns) {
                maxinsns = ninsns;
                biggestFunction = func;
            }
            ai.skipChildrenOnForward();
        }
    }

    if (biggestFunction) {
        std::cout <<maxinsns <<" " <<StringUtility::addrToString(biggestFunction->get_entry_va()) <<"\n";
    } else {
        std::cout <<"0\n";
    }

    return 0;
}
