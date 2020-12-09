-- Demonstrates:
--      Explicit Dereference


with ada.text_io;
procedure explicit_deref  is
   type A_Int is access Integer;
   Var : A_Int := new Integer;
begin
   Var.all := 12;

   if Var.all = 12 then
     ada.Text_Io.Put_Line("Explicit Dereference OK");
   end if;
end Explicit_Deref;
