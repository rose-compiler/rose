// This example demonstrates where an extra SgBasicBlock 
// is generated which is then a problem for the AST merge.
void foo()
   {
 // There used to be a redundant SgBasicBlock generated if the else_stmt was NULL in EDG.
    if (true){};
   }

