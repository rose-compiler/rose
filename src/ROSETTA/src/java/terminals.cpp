#if USE_JAVA_IR_NODES

  //============================================================================
  // SgJavaThrowStatement
  //============================================================================
  // Added Java specific "throw" statement since it is a proper part of the Java
  // Grammar (not true for C++ where it is only an expression).
  //============================================================================
  NEW_TERMINAL_MACRO (JavaThrowStatement, "JavaThrowStatement", "JAVE_THROW_STMT" );
  {
      // DQ (8/17/2011): Added Java "throw" statement support (constructor
      //                 required SgExpression pointer).
      JavaThrowStatement.setFunctionPrototype(
          "HEADER_JAVA_THROW_STATEMENT",
          "../Grammar/Statement.code");
      JavaThrowStatement.setFunctionSource(
          "SOURCE_JAVA_THROW_STATEMENT",
          "../Grammar/Statement.code");

      JavaThrowStatement.setDataPrototype(
          "SgThrowOp*", "throwOp", "= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  }

  //============================================================================
  // SgJavaForEachStatement
  //============================================================================
  NEW_TERMINAL_MACRO (JavaForEachStatement, "JavaForEachStatement", "JAVA_FOREACH_STMT");
  {
      // DQ (8/17/2011): Added Java "foreach" statement support (constructor
      //                 required SgExpression pointer).
      JavaForEachStatement.setFunctionPrototype(
          "HEADER_JAVA_FOREACH_STATEMENT",
          "../Grammar/Statement.code");
      JavaForEachStatement.setFunctionSource(
          "SOURCE_JAVA_FOREACH_STATEMENT",
          "../Grammar/Statement.code");

      JavaForEachStatement.setDataPrototype(
          "SgVariableDeclaration*", "element", "= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

      JavaForEachStatement.setDataPrototype(
          "SgExpression*", "collection", "= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

      JavaForEachStatement.setDataPrototype(
          "SgStatement*", "loop_body", "= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

      // DQ (9/3/2011): Fixing up the new IR node to better match the grammar.
      // JavaForEachStatement.setDataPrototype(
      //     "SgInitializedName*", "element", "= NULL",
      //      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  }

  //============================================================================
  // SgJavaSynchronizedStatement
  //============================================================================
  NEW_TERMINAL_MACRO (JavaSynchronizedStatement, "JavaSynchronizedStatement", "JAVA_SYNC_STMT");
  {
      // DQ (8/17/2011):  Added Java "synchronized" statement support (constructor required SgExpression pointer).
      JavaSynchronizedStatement.setFunctionPrototype(
          "HEADER_JAVA_SYNCHRONIZED_STATEMENT",
          "../Grammar/Statement.code");

      JavaSynchronizedStatement.setFunctionSource(
          "SOURCE_JAVA_SYNCHRONIZED_STATEMENT",
          "../Grammar/Statement.code" );

      JavaSynchronizedStatement.setDataPrototype(
          "SgExpression*", "expression", "= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

      JavaSynchronizedStatement.setDataPrototype(
          "SgStatement*", "body", "= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  }

  //============================================================================
  // SgJavaLabelStatement
  //============================================================================
  // DQ (8/26/2011): Added Java statement as a scope (different from the C/C++
  //                 label statement.
  //============================================================================
  NEW_TERMINAL_MACRO (JavaLabelStatement, "JavaLabelStatement", "JAVA_LABEL_STMT" );
  {
      JavaLabelStatement.setFunctionSource(
          "SOURCE_JAVA_LABEL_STATEMENT",
          "../Grammar/Statement.code" );

      JavaLabelStatement.setFunctionPrototype(
          "HEADER_JAVA_LABEL_STATEMENT",
          "../Grammar/Statement.code");

      JavaLabelStatement.setDataPrototype(
          "SgName", "label", "= \"\"",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

      JavaLabelStatement.setDataPrototype(
          "SgStatement*", "statement", "= NULL",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  }

  //============================================================================
  // SgJavaImportStatement
  //============================================================================
  // DQ (4/12/2011): Added Java support for "import" keyword.
  // DQ (4/16/2011): This is the Java specific SgJavaImportStatement (which is
  //                 a declaration), there is also a Fortran specific import
  //                 statment IR node.
  //============================================================================
  NEW_TERMINAL_MACRO (JavaImportStatement, "JavaImportStatement", "TEMP_JavaImportStatement" );
  {
      JavaImportStatement.setFunctionPrototype(
          "HEADER_JAVA_IMPORT_STATEMENT",
          "../Grammar/Statement.code");

      JavaImportStatement.setFunctionSource(
          "SOURCE_JAVA_IMPORT_STATEMENT",
          "../Grammar/Statement.code");

      JavaImportStatement.setDataPrototype(
          "SgName", "path", "= \"\"",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

      JavaImportStatement.setDataPrototype(
          "bool", "containsWildCard", "= false",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  }

  //============================================================================
  // SgJavaPackageDeclaration
  //============================================================================
  NEW_TERMINAL_MACRO (JavaPackageDeclaration, "JavaPackageDeclaration", "TEMP_JavaPackageDeclaration" );
  {
      JavaPackageDeclaration.setFunctionPrototype(
          "HEADER_JAVA_PACKAGE_DECLARATION",
          "../Grammar/Statement.code" );

      JavaPackageDeclaration.setFunctionSource(
          "SOURCE_JAVA_PACKAGE_DECLARATION",
          "../Grammar/Statement.code");

      // SgJavaPackageDeclaration Attributes, format:
      //
      //     JavaPackageDeclaration.setDataPrototype(
      //         "<data type>", "<name>", "= <default initialization>",
      //         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
      //
  }//JavaPackageDeclaration

  //============================================================================
  // SgJavaPackageStatement
  //============================================================================
  NEW_TERMINAL_MACRO (JavaPackageStatement, "JavaPackageStatement", "TEMP_JavaPackageStatement" );
  {
      JavaPackageStatement.setFunctionPrototype(
          "HEADER_JAVA_PACKAGE_STATEMENT",
          "../Grammar/Statement.code");

      JavaPackageStatement.setFunctionSource(
          "SOURCE_JAVA_PACKAGE_STATEMENT",
          "../Grammar/Statement.code");

      JavaPackageStatement.setDataPrototype(
          "SgName", "name", "= \"\"",
          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  }
#endif

