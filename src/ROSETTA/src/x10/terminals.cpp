#if USE_X10_IR_NODES

        NEW_TERMINAL_MACRO (AsyncStmt,  "AsyncStmt",  "ASYNC_STMT" );
        NEW_TERMINAL_MACRO (FinishStmt, "FinishStmt", "FINISH_STMT" );
        NEW_TERMINAL_MACRO (AtStmt,     "AtStmt",     "AT_STMT" );
#if 0
        NEW_TERMINAL_MACRO (ClassPropertyList,     "ClassPropertyList",     "CLASS_PROPERTY_LIST" );
#endif
        NEW_TERMINAL_MACRO (AtomicStmt, "AtomicStmt", "ATOMIC_STMT" );
        NEW_TERMINAL_MACRO (WhenStmt, "WhenStmt", "WHEN_STMT" );

#if 1
        AsyncStmt.setFunctionPrototype          ( "HEADER_ASYNC_STATEMENT", "../Grammar/Statement.code" );
        AsyncStmt.setFunctionSource             ( "SOURCE_ASYNC_STATEMENT", "../Grammar/Statement.code" );
        AsyncStmt.editSubstitute                ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        AsyncStmt.editSubstitute                ( "LIST_NAME", "statements" );
        AsyncStmt.editSubstitute                ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        AsyncStmt.editSubstitute                ( "LIST_FUNCTION_NAME", "statement" );
        AsyncStmt.editSubstitute                ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        AsyncStmt.setDataPrototype              ( "SgStatement*", "body", "=NULL",
                                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#else
        AsyncStmt.setFunctionPrototype          ( "HEADER_ASYNC_STATEMENT", "../Grammar/Statement.code" );
        AsyncStmt.setFunctionSource             ( "SOURCE_ASYNC_STATEMENT", "../Grammar/Statement.code" );
        AsyncStmt.editSubstitute                ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        AsyncStmt.editSubstitute                ( "LIST_NAME", "statements" );
        AsyncStmt.editSubstitute                ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        AsyncStmt.editSubstitute                ( "LIST_FUNCTION_NAME", "statement" );
        AsyncStmt.editSubstitute                ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        AsyncStmt.setDataPrototype              ( "SgStatement*", "body", "=NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        AsyncStmt.setDataPrototype              ( "bool", "isClocked", "=false",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
        

#if 1
        FinishStmt.setFunctionPrototype         ( "HEADER_FINISH_STATEMENT", "../Grammar/Statement.code" );
        FinishStmt.setFunctionSource            ( "SOURCE_FINISH_STATEMENT", "../Grammar/Statement.code" );
        FinishStmt.editSubstitute               ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        FinishStmt.editSubstitute               ( "LIST_NAME", "statements" );
        FinishStmt.editSubstitute               ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        FinishStmt.editSubstitute               ( "LIST_FUNCTION_NAME", "statement" );
        FinishStmt.editSubstitute               ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        FinishStmt.setDataPrototype             ( "SgStatement*", "body", "=NULL",
                                                CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#else
        FinishStmt.setFunctionPrototype         ( "HEADER_FINISH_STATEMENT", "../Grammar/Statement.code" );
        FinishStmt.setFunctionSource            ( "SOURCE_FINISH_STATEMENT", "../Grammar/Statement.code" );
        FinishStmt.editSubstitute               ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        FinishStmt.editSubstitute               ( "LIST_NAME", "statements" );
        FinishStmt.editSubstitute               ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        FinishStmt.editSubstitute               ( "LIST_FUNCTION_NAME", "statement" );
        FinishStmt.editSubstitute               ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        FinishStmt.setDataPrototype             ( "SgStatement*", "body", "=NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        FinishStmt.setDataPrototype             ( "bool", "isClocked", "=false",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
        
        AtStmt.setFunctionPrototype             ( "HEADER_AT_STATEMENT", "../Grammar/Statement.code");
        AtStmt.setFunctionSource                ( "SOURCE_AT_STATEMENT", "../Grammar/Statement.code" );
        AtStmt.editSubstitute                   ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        AtStmt.editSubstitute                   ( "LIST_NAME", "statements" );
        AtStmt.editSubstitute                   ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        AtStmt.editSubstitute                   ( "LIST_FUNCTION_NAME", "statement" );
        AtStmt.editSubstitute                   ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        AtStmt.setDataPrototype                 ( "SgExpression*", "expression", "= NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        AtStmt.setDataPrototype                 ( "SgStatement*", "body", "= NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

#if 0
        ClassPropertyList.setFunctionPrototype  ( "HEADER_CLASS_PROPERTY_LIST", "../Grammar/Statement.code" );
        ClassPropertyList.setFunctionSource     ( "SOURCE_CLASS_PROPERTY_LIST", "../Grammar/Statement.code" );
        ClassPropertyList.editSubstitute        ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_DECLARATIONS", "../Grammar/Statement.code" );
        ClassPropertyList.editSubstitute        ( "LIST_DATA_TYPE", "SgInitializedNamePtrList" );
        ClassPropertyList.editSubstitute        ( "LIST_NAME", "args" );
        ClassPropertyList.editSubstitute        ( "LIST_FUNCTION_RETURN_TYPE", "SgInitializedNamePtrList::iterator" );
        ClassPropertyList.editSubstitute        ( "LIST_FUNCTION_NAME", "arg" );
        ClassPropertyList.editSubstitute        ( "LIST_ELEMENT_DATA_TYPE", "SgInitializedName*" );
        ClassPropertyList.setDataPrototype      ( "SgInitializedNamePtrList", "args", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif


        AtomicStmt.setFunctionPrototype         ( "HEADER_ATOMIC_STATEMENT", "../Grammar/Statement.code" );
        AtomicStmt.setFunctionSource            ( "SOURCE_ATOMIC_STATEMENT", "../Grammar/Statement.code" );
        AtomicStmt.editSubstitute               ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        AtomicStmt.editSubstitute               ( "LIST_NAME", "statements" );
        AtomicStmt.editSubstitute               ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        AtomicStmt.editSubstitute               ( "LIST_FUNCTION_NAME", "statement" );
        AtomicStmt.editSubstitute               ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        AtomicStmt.setDataPrototype             ( "SgStatement*", "body", "=NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


        WhenStmt.setFunctionPrototype         ( "HEADER_WHEN_STATEMENT", "../Grammar/Statement.code" );
        WhenStmt.setFunctionSource            ( "SOURCE_WHEN_STATEMENT", "../Grammar/Statement.code" );
        WhenStmt.editSubstitute               ( "LIST_DATA_TYPE", "SgStatementPtrList" );
        WhenStmt.editSubstitute               ( "LIST_NAME", "statements" );
        WhenStmt.editSubstitute               ( "LIST_FUNCTION_RETURN_TYPE", "void" );
        WhenStmt.editSubstitute               ( "LIST_FUNCTION_NAME", "statement" );
        WhenStmt.editSubstitute               ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
        WhenStmt.setDataPrototype             ( "SgExpression*", "expression", "= NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        WhenStmt.setDataPrototype             ( "SgStatement*", "body", "=NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

#endif
