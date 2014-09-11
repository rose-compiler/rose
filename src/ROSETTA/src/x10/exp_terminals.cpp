#if USE_X10_IR_NODES
        NEW_TERMINAL_MACRO (HereExp,    "HereExp",    "HERE_NODE" );
#if 0
        NEW_TERMINAL_MACRO (DotDotExp,    "DotDotExp",    "DOTDOT_NODE" );
#endif
        
        HereExp.setFunctionPrototype            ( "HEADER_HERE_EXPRESSION", "../Grammar/Expression.code");
        HereExp.setFunctionSource                       ( "SOURCE_HERE_EXPRESSION", "../Grammar/Expression.code" );
        HereExp.setDataPrototype                ( "SgType*", "expression_type", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    HereExp.setDataPrototype                    ( "SgExpression*", "expression", "= NULL",
                                                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#if 0
    DotDotExp.editSubstitute        ( "PRECEDENCE_VALUE", "16" );
        DotDotExp.setFunctionPrototype          ( "HEADER_DOTDOT_EXPRESSION", "../Grammar/Expression.code");
        DotDotExp.setFunctionSource                     ( "SOURCE_DOTDOT_EXPRESSION", "../Grammar/Expression.code" );
        DotDotExp.setDataPrototype              ( "SgType*", "expression_type", "= NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    DotDotExp.setDataPrototype                  ( "SgExpression*", "expression", "= NULL",
                                                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif
#endif
