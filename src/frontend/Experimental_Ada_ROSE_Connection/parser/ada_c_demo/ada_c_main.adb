with Ada.Text_IO;
with c_code_h;

procedure Ada_C_Main is
   package ATI renames Ada.Text_Io;
begin
   ATI.Put_Line ("Ada_C_Main: Calling c_func");
   c_code_h.c_func;
   ATI.Put_Line ("Ada_C_Main: Returned from c_func");
end Ada_C_Main;
