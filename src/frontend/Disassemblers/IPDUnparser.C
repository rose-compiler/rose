#include "sage3basic.h"
#include "Partitioner.h"

namespace rose {
namespace BinaryAnalysis {


//class IpdUnparsser: public AsmUnparser {
//protected:
//    class FunctionDecl: public UnparserCallback {
//    public:
//        virtual bool operator()(bool enabled, const FunctionArgs &args) {
//            args.output <<"function 




void
Partitioner::IPDParser::unparse(std::ostream &output, SgNode *ast)
{
    struct T1: public AstPrePostProcessing {
        std::ostream &output;
        T1(std::ostream &output): output(output) {}

        void preOrderVisit(SgNode *node) {
            if (isSgAsmFunction(node)) {
                SgAsmFunction *func = isSgAsmFunction(node);
                output <<"\n\n"
                       <<"function " <<StringUtility::addrToString(func->get_entry_va())
                       <<" \"" <<escapeString(func->get_name()) <<"\" {\n";
                switch (func->get_may_return()) {
                    case SgAsmFunction::RET_NEVER:
                        output <<"  noreturn;\n";
                        break;
                    case SgAsmFunction::RET_ALWAYS:
                        output <<"  returns;\n";
                        break;
                    default:
                        /*void*/
                        break;
                }
            } else if (isSgAsmBlock(node)) {
                SgAsmBlock *blk = isSgAsmBlock(node);
                const SgAsmStatementPtrList &stmts = blk->get_statementList();
                if (!stmts.empty() && isSgAsmInstruction(stmts[0])) {
                    output <<"  block " <<StringUtility::addrToString(blk->get_address()) <<" " <<stmts.size() <<" {\n";
                    output <<"    successors {\n";
                    const SgAsmIntegerValuePtrList &succs = blk->get_successors();
                    for (size_t i=0; i<succs.size(); ++i) {
                        SgAsmIntegerValueExpression *iv = succs[i];
                        output <<"      " <<StringUtility::addrToString(iv->get_absoluteValue())
                               <<(i+1<succs.size()?",":"");
                        std::string label = iv->get_label();
                        if (!label.empty())
                            output <<" # " <<label;
                        output <<"\n";
                    }
                    if (!blk->get_successors_complete())
                        output <<"      ...\n";
                    output <<"    }\n";
                }
            }
        }

        void postOrderVisit(SgNode *node) {
            if (isSgAsmFunction(node)) {
                output <<"}\n";
            } if (isSgAsmBlock(node)) {
                SgAsmBlock *blk = isSgAsmBlock(node);
                const SgAsmStatementPtrList &stmts = blk->get_statementList();
                if (!stmts.empty() && isSgAsmInstruction(stmts[0])) {
                    output <<"  }\n";
                }
            }
        }
    };

    if (ast)
        T1(output).traverse(ast);
}

} // namespace
} // namespace
