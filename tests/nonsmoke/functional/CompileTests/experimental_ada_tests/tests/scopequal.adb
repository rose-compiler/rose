with System, Ada.Text_IO;
with Interfaces;

procedure ScopeQual is
  x : Interfaces.Unsigned_64 := 1;
begin
  x := Interfaces.Shift_Left(x, 2);
  Ada.Text_IO.Put_Line (System.Address'Size'Img);
end ScopeQual;
