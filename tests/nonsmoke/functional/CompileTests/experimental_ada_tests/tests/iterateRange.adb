with Ada.Text_IO;

-- source https://en.wikibooks.org/wiki/Ada_Programming/Control

procedure IterateRange is
   type Range_Type is range -5 .. 10;

   package T_IO renames Ada.Text_IO;
   package I_IO is new  Ada.Text_IO.Integer_IO (Range_Type);

begin
   for A in Range_Type loop
      I_IO.Put (Item  => A,
                Width => 3,
                Base  => 10);

      if A < Range_Type'Last then
         T_IO.Put (",");
      else
         T_IO.New_Line;
      end if;
   end loop;
end IterateRange;
