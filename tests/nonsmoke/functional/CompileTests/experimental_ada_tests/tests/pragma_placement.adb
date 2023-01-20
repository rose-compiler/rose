procedure pragma_placement is

   procedure Imp;
   pragma Import(CPP, Imp, "Imp");

begin
  pragma endless_loop;
  loop
    null;
  end loop;
exception
   when Program_error =>
     null;

   pragma unreachable_code;
   when others =>
     null;
end ;

