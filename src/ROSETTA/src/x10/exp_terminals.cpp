#if USE_X10_IR_NODES
    NEW_TERMINAL_MACRO (HereExp,    "HereExp",    "HERE_NODE"   );
    NEW_TERMINAL_MACRO (DotDotExp,  "DotDotExp",  "DOTDOT_NODE" );
    NEW_TERMINAL_MACRO (AtExp,      "AtExp",      "ATEXP_NODE" );
    NEW_TERMINAL_MACRO (FinishExp,  "FinishExp",  "FINISHEXP_NODE" );

    HereExp.setFunctionPrototype        ( "HEADER_HERE_EXPRESSION", "../Grammar/Expression.code");
    HereExp.setFunctionSource           ( "SOURCE_HERE_EXPRESSION", "../Grammar/Expression.code" );
    HereExp.setDataPrototype            ( "SgType*", "expression_type", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    HereExp.setDataPrototype            ( "SgExpression*", "expression", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    AtExp.setFunctionPrototype          ( "HEADER_ATEXP_EXPRESSION", "../Grammar/Expression.code");
    AtExp.setFunctionSource             ( "SOURCE_ATEXP_EXPRESSION", "../Grammar/Expression.code" );
    AtExp.setDataPrototype              ( "SgExpression*", "expression", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AtExp.setDataPrototype              ( "SgStatement*", "body", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

    FinishExp.setFunctionPrototype      ( "HEADER_FINISHEXP_EXPRESSION", "../Grammar/Expression.code");
    FinishExp.setFunctionSource         ( "SOURCE_FINISHEXP_EXPRESSION", "../Grammar/Expression.code" );
    FinishExp.setDataPrototype          ( "SgExpression*", "expression", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    FinishExp.setDataPrototype          ( "SgStatement*", "body", "= NULL",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


    DotDotExp.editSubstitute            ( "PRECEDENCE_VALUE", "16" );
    DotDotExp.setFunctionSource         ( "SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code" );
    DotDotExp.setFunctionSource         ( "SOURCE_DOTDOT_EXPRESSION", "../Grammar/Expression.code" );
    DotDotExp.setFunctionPrototype      ( "HEADER_DOTDOT_EXPRESSION", "../Grammar/Expression.code");
#endif
