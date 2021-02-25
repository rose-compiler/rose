package body Overload is
  
  function Minus(x, y : in integer) return integer is
  begin
    return x-y;
  end Minus;
  
  function Minus(y : in integer) return integer is
  begin
    return Minus(0, y);
  end Minus;

end Overload;
