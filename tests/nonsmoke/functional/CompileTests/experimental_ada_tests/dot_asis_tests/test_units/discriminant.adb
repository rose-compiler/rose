with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure Discrim1 is

   type SQUARE is array(INTEGER range <>,
                        INTEGER range <>) of INTEGER;

   type LINEAR_TYPE is array(INTEGER range <>) of POSITIVE;

   type STUFF(List_Size : POSITIVE) is
      record
         Matrix   : SQUARE(1..List_Size, 1..List_Size);
         Elements : INTEGER := List_Size * List_Size;
         Linear   : LINEAR_TYPE(1..List_Size);
         Number   : INTEGER := List_Size;
      end record;

   type ANOTHER_STUFF is new STUFF;

   subtype STUFF_5 is STUFF(5);

   Data_Store  : STUFF(5);
   Big_Store   : STUFF(12);
   Extra_Store : ANOTHER_STUFF(5);
   More_Store  : STUFF(5);
   Five_Store  : STUFF_5;
   Name_Store  : STUFF(List_Size => 5);

begin

   for Index1 in Data_Store.Matrix'RANGE(1) loop
      Data_Store.Linear(Index1) := Index1;
      for Index2 in Data_Store.Matrix'RANGE(2) loop
         Data_Store.Matrix(Index1, Index2) := Index1 * Index2;
      end loop;
   end loop;

   Five_Store := Data_Store;
   More_Store := Five_Store;

   Put("The number of elements in More_Store.Matrix is");
   Put(More_Store.Elements, 5);
   New_Line;

end Discrim1;

