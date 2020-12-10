function SumRange(x, y: IN Integer) return Integer is
   Result : Integer;
begin
   Result := 0;

   for I in x .. y loop
     Result := Result + I;
   end loop;

   return Result;
end SumRange;
