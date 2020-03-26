function IsDivisible(x, y: IN Integer) return Integer is
   Result : Integer;
begin
   Result := 0;

   if y = 0 or else x = 0 then
     Result := 0;
   else
     Result := x / y;
   end if;

   return Result;
end IsDivisible;
