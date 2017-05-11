package body Unit_2 is

   task Boring_Task is
      entry Drop_Off_Work (Work_In : in Range_Type);
   end Boring_Task;

   task body Boring_Task is
      Work : Range_Type := 5;
      Result : Integer := 0;
      Factor : constant Positive := 2;
   begin
      accept Drop_Off_Work (Work_In : in Range_Type) do
         Work := Work_In;
      end Drop_Off_Work;
      Result := Integer (Work) * Factor;
   end Boring_Task;

   procedure You_Do_It (Using : in Range_Type) is
   begin
      if Using = 5 then
         raise Dont_Like_5;
      else
         You_Do_It (Using => Using);
      end if;
   end You_Do_It;

   procedure Do_It (This : in Range_Type) is
   begin
      You_Do_It (Using => This);
   exception
      when X : Dont_Like_5 =>
         null;
   end Do_It;


end Unit_2;
