function Compute(x, y: IN Integer) return Integer is
   Result : Integer;
   I      : Integer;
   J      : Integer;
begin
   Result := 0;
   I := X;
   
   Outer:
   loop
     J := Y;
     loop
       Result := Result + 1;
       exit Outer when I = J;
       J := J-1; 
     end loop;
     I := I+1;
   end loop Outer;

   loop
     Result := Result - 1;
     if Result rem 2 = 0 then 
       exit;
     end if;
   end loop;

   return Result;
end Compute;
