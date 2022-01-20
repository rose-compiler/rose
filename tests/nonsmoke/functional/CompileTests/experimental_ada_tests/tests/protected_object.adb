with Ada.Text_IO; use Ada.Text_IO;

-- from https://learn.adacore.com/courses/intro-to-ada/chapters/tasking.html
procedure Protected_Object is

   protected type Obj_Type is
      procedure Set (V : Integer);
      function Get return Integer;
   private
      Local : Integer := 0;
   end Obj_Type;

   protected body Obj_Type is
      procedure Set (V : Integer) is
      begin
         Local := V;
      end Set;

      function Get return Integer is
      begin
         return Local;
      end Get;
   end Obj_Type;

   Obj : Obj_Type;
   type PObj is access Obj_Type;
begin
   Obj.Set (5);
   Put_Line ("Number is: " & Integer'Image (Obj.Get));
end Protected_Object;
