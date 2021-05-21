with Ada.Text_IO;
with Ada_Code;

procedure Ada_Main is
   package ATI renames Ada.Text_Io;
begin
   ATI.Put_Line ("Ada_Main: Calling Ada_Proc");
   Ada_Code.Ada_Proc;
   ATI.Put_Line ("Ada_Main: Returned from Ada_Proc");
end Ada_Main;
