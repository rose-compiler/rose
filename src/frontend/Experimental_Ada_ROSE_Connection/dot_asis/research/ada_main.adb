with Ada.Text_IO;

procedure Ada_Main is
   procedure C_Func;
   pragma Import (C, C_Func);

   package ATI renames Ada.Text_Io;
begin
   ATI.Put_Line ("Ada_Main: Calling C_Func");
   C_Func;
   ATI.Put_Line ("Ada_Main: Returned from C_Func");
end Ada_Main;
