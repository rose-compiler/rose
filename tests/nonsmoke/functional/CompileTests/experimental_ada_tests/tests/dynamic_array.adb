procedure dynamic_array is
type OpenArray is array (Natural range <>) of Integer;
subtype ShortArray is OpenArray(1..4);
type ItemArray is access ShortArray;
Items : ItemArray := new ShortArray;
begin
  Items.all := ShortArray'(others => 0);
end dynamic_array;
