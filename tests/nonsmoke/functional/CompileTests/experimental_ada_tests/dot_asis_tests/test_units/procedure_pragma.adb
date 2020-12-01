procedure Procedure_Pragma is

   --  Declare then export an Integer entity called num_from_Ada
   My_Num : Integer := 10;
   pragma Export (C, My_Num, "num_from_Ada");

   --  Declare an Ada function spec for Get_Num, then use
   --  C function get_num for the implementation.
   function Get_Num return Integer;
   pragma Import (C, Get_Num, "get_num");

   --  Declare an Ada procedure spec for Print_Num, then use
   --  C function print_num for the implementation.
   procedure Print_Num (Num : Integer);
   pragma Import (C, Print_Num, "print_num");

begin
   Print_Num (Get_Num);
end Procedure_Pragma;
