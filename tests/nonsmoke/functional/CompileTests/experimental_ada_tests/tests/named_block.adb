
procedure Named_Block is
begin
  TheBlock: declare
    type IntArray is array(1..2) of Integer;
    ia : IntArray := (others => 0);
  begin
    null;
  end TheBlock;
end Named_Block;
