with Ada.Text_IO;
with c_code_h;

procedure Ada_C_Ada_Main is
   package ATI renames Ada.Text_Io;
begin
   ATI.Put_Line ("Ada_C_Ada_Main: Calling c_ada_caller");
   c_code_h.c_ada_caller;
   ATI.Put_Line ("Ada_C_Ada_Main: Returned from c_ada_caller");
end Ada_C_Ada_Main;
