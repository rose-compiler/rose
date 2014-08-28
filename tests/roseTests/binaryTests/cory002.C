#include "rose.h"
#include "BinaryControlFlow.h"

using namespace rose::BinaryAnalysis;

void foo(SgAsmFunction* func) {
  ControlFlow cfg_analyzer;
  ControlFlow::Graph cfg = cfg_analyzer.build_block_cfg_from_ast<ControlFlow::Graph>(func);
}

void bar(SgAsmFunction* func) {
  ControlFlow cfg_analyzer;
  ControlFlow::Graph cfg = cfg_analyzer.build_block_cfg_from_ast<ControlFlow::Graph>(func);
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
