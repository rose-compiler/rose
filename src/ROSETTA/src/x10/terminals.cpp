#if USE_X10_IR_NODES

        NEW_TERMINAL_MACRO (AsyncStmt,  "AsyncStmt",  "ASYNC_STMT" );
        NEW_TERMINAL_MACRO (FinishStmt, "FinishStmt", "FINISH_STMT" );
        NEW_TERMINAL_MACRO (AtStmt,     "AtStmt",     "AT_STMT" );

        AsyncStmt.setFunctionPrototype          ( "HEADER_ASYNC_STATEMENT", "../Grammar/Statement.code" );
        AsyncStmt.setFunctionSource             ( "SOURCE_ASYNC_STATEMENT", "../Grammar/Statement.code" );
        AsyncStmt.editSubstitute                ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        AsyncStmt.editSubstitute                ( "LIST_NAME", "statements" );
        AsyncStmt.editSubstitute                ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        AsyncStmt.editSubstitute                ( "LIST_FUNCTION_NAME", "statement" );
        AsyncStmt.editSubstitute                ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        AsyncStmt.setDataPrototype              ( "SgStatement*", "body", "=NULL",
                                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        
        FinishStmt.setFunctionPrototype         ( "HEADER_FINISH_STATEMENT", "../Grammar/Statement.code" );
        FinishStmt.setFunctionSource            ( "SOURCE_FINISH_STATEMENT", "../Grammar/Statement.code" );
        FinishStmt.editSubstitute               ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        FinishStmt.editSubstitute               ( "LIST_NAME", "statements" );
        FinishStmt.editSubstitute               ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        FinishStmt.editSubstitute               ( "LIST_FUNCTION_NAME", "statement" );
        FinishStmt.editSubstitute               ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        FinishStmt.setDataPrototype             ( "SgStatement*", "body", "=NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        
        AtStmt.setFunctionPrototype                     ( "HEADER_AT_STATEMENT", "../Grammar/Statement.code");
        AtStmt.setFunctionSource                        ( "SOURCE_AT_STATEMENT", "../Grammar/Statement.code" );
        AtStmt.editSubstitute                   ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        AtStmt.editSubstitute                   ( "LIST_NAME", "statements" );
        AtStmt.editSubstitute                   ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        AtStmt.editSubstitute                   ( "LIST_FUNCTION_NAME", "statement" );
        AtStmt.editSubstitute                   ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        AtStmt.setDataPrototype                         ( "SgExpression*", "expression", "= NULL",
                                                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        AtStmt.setDataPrototype                         ( "SgStatement*", "body", "= NULL",
                                                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif
