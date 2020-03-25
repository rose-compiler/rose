function Fib(x: in Integer) return Integer is
   Result : Integer;
begin
  if x <= 1 then
    Result := x;
  else
    Result := Fib(x-2) + Fib(x-1);
  end if;

  return Result;
end Fib;
