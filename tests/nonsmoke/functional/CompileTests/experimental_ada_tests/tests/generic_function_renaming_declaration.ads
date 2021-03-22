package generic_function_renaming_declaration is
  generic
    function test(X : Integer) return Integer ;

  generic function newtest renames test;

end generic_function_renaming_declaration;
