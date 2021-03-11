-- Demonstrates:
--      Declaring arrays
--      Indexted Component
--      Slice


procedure simple_array_expressions  is
   type Arr_Type is array (Integer range <>) of Integer;
   Arr1 : Arr_Type ( 1 .. 26);

begin
   Arr1(1) := 1;

   if Arr1(1) = 1 then
     null;
   end if;

   Arr1(7 .. 9) := (7, 8, 9);
   if Arr1(7) = 7 then
     null;
   end if;

   if 5 in  4 .. 6  then
      null;
   end if;

   if Arr1(2) not in 4 .. 6 | 0  then
      null;
   end if;

end simple_array_expressions;
