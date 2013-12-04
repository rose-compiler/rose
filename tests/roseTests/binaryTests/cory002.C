#include "rose.h"
#include "BinaryControlFlow.h"

void foo(SgAsmFunction* func) {
  BinaryAnalysis::ControlFlow cfg_analyzer;
  BinaryAnalysis::ControlFlow::Graph cfg = cfg_analyzer.build_block_cfg_from_ast<BinaryAnalysis::ControlFlow::Graph>(func);
}

void bar(SgAsmFunction* func) {
  BinaryAnalysis::ControlFlow cfg_analyzer;
  BinaryAnalysis::ControlFlow::Graph cfg = cfg_analyzer.build_block_cfg_from_ast<BinaryAnalysis::ControlFlow::Graph>(func);
}

void
test()
{
    SgAsmFunction *func = NULL;
    foo(func);
    bar(func);
}

int main() {
    // Not meant to actually run.  This is only a linking test.
    test();
    return 0;
}
