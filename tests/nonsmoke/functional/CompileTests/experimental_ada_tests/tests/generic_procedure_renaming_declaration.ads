package generic_procedure_renaming_declaration is
  generic
    procedure test(X : Integer);

  generic procedure newtest renames test;

end generic_procedure_renaming_declaration;
