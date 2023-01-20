procedure pragma_import is
   procedure Imp;
   pragma Import(CPP, Imp, "Imp");
begin
  null;
exception
   when others =>
     null;
end ;

