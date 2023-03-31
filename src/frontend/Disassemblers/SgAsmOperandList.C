#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

void
SgAsmOperandList::append_operand( SgAsmExpression* operand )
   {
  /* implemented directly until we can fix how the statementList is built */
     p_operands.push_back(operand);
   }

#endif
