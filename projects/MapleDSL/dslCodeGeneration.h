

std::string generateMapleCode(SgExpression* lhs, SgExpression* rhs, OperatorFSM discritizationOperator, bool generateLowlevelCode);

// void callMaple ( const std::string & generatedCode );

void generateStencilCode(StencilOperator stencil, SgExpression* lhs, SgExpression* rhs);
