#if __cplusplus >= 201402L

static const char *purpose = "demonstrate static semantics";
static const char *description =
    "Shows how to attach static semantics to the AST and then "
    "print the AST.";


#include <rose.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/StaticSemantics.h>
#include <stringify.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/TraceSemantics.h>

using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;

static Rose::Diagnostics::Facility mlog;

// Print basic info about an expression tree. There are probably better ways to do this, but I don't use ROSE's ASTs very often
// if I can help it. Also, we're writing to stderr so this output gets interleaved with the semantics tracing diagnostic
// output.
struct ExprPrinter: AstPrePostProcessing {
    size_t indentation = 0;

    void preOrderVisit(SgNode *node) override {
        std::cerr <<std::string(++indentation*2 + 2, ' ') <<node->class_name();
        if (auto op = isSgAsmRiscOperation(node))
            std::cerr <<stringify::SgAsmRiscOperation::RiscOperator(op->get_riscOperator());
        std::cerr <<"\n";
    }

    void postOrderVisit(SgNode *node) override {
        ASSERT_require(indentation > 0);
        --indentation;
    }
};

// Attach semantic information about side effects to each instruction, printing them as we go.
struct SemanticsAttacher: AstSimpleProcessing {
    const P2::Partitioner &partitioner;
    IS::BaseSemantics::RiscOperatorsPtr traceOps;

    // Build a symbolic CPU
    explicit SemanticsAttacher(const P2::Partitioner &partitioner)
        : partitioner(partitioner),
          traceOps(IS::TraceSemantics::RiscOperators::instance(partitioner.newOperators())) {}

    // Visit each instruction and attach the semantic side effects to the SgAsmInstruction node, but run the same instruction
    // through a symbolic tracing dispatcher to show us the steps that occur.
    void visit(SgNode *node) override {
        if (auto insn = isSgAsmInstruction(node)) {
            std::cerr <<partitioner.unparse(insn) <<"\n";

            // Symbolic execution of this single instruction all by itself.
            IS::BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(traceOps);
            try {
                cpu->processInstruction(insn);
            } catch (...) {
                std::cerr <<"  semantic failure\n";
            }

            // Now build the side effect trees for this same single instruciton and print the results
            IS::StaticSemantics::attachInstructionSemantics(insn, partitioner.instructionProvider().disassembler());
            if (!insn->get_semantics() || insn->get_semantics()->get_expressions().empty()) {
                std::cerr <<"  no side effects\n";
            } else {
                for (SgAsmExpression *expr: insn->get_semantics()->get_expressions()) {
                    std::cerr <<"  side effect:\n";
                    ExprPrinter().traverse(expr);
                }
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    mlog[TRACE].enable();

    P2::Engine engine;
    std::vector<std::string> specimen = engine.parseCommandLine(argc, argv, purpose, description).unreachedArgs();
    P2::Partitioner partitioner = engine.partition(specimen);

    SgAsmBlock *ast = P2::Modules::buildAst(partitioner);
    SemanticsAttacher(partitioner).traverse(ast, preorder);
}

#else

#include <iostream>
int main() {
    std::cerr <<"not supported in this ROSE configuration\n";
    return 1;
}

#endif
