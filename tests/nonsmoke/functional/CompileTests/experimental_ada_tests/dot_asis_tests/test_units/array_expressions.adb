-- Demonstrates:
--      Declaring arrays
--      Indexted Component
--      Slice


with ada.text_io;
procedure array_expressions  is
   type Arr_Type is array (Integer range <>) of Integer;
   Arr1 : Arr_Type ( 1 .. 26);

begin
   Arr1(1) := 1;

   if Arr1(1) = 1 then
     ada.Text_Io.Put_Line("Indexed Component OK");
   end if;

   Arr1(7 .. 9) := (7, 8, 9);
   if Arr1(7) = 7 then
     ada.Text_Io.Put_Line("Slice OK");
   end if;

   if 5 in  4 .. 6  then
      ada.Text_Io.Put_Line("In Membership test OK");
   end if;

   if 8 not in  4 .. 6  then
      ada.Text_Io.Put_Line("Not in Membership test OK");
   end if;

end array_expressions;
