package Access_To_Subprogram is

   type Access_To_Procedure_1 is access procedure;
   type Access_To_Procedure_2 is access procedure (Parm_1 : in Integer);
   type Access_To_Procedure_3 is access protected procedure;

   type Access_To_Function_1 is access function return Boolean;
   type Access_To_Function_2 is access function (Parm_2 : in Integer) return Boolean;
   type Access_To_Function_3 is access protected function return Boolean;

end Access_To_Subprogram;
