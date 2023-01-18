procedure opergen8 is

  package Y is
     type History_Type is array (integer range <>) of Integer;

     arr : History_Type (1..2) := (others => 1);
  end Y;

  arr :  Y.History_Type (4..5) := (others => 2);
begin
  if (Y."="(Y.arr, arr)) then
    null;
  end if;
end opergen8;
