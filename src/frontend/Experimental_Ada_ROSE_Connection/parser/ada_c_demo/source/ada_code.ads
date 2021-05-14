package Ada_Code is

   procedure Ada_Proc;
   pragma Export (C, Ada_Proc, "Ada_Proc");

   procedure Ada_C_Caller;
   pragma Export (C, Ada_C_Caller, "Ada_C_Caller");

end Ada_Code;
