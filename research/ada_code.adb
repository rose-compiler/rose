with Ada.Text_IO;

package body Ada_Code is
   package ATI renames Ada.Text_Io;

   procedure Ada_Proc is
   begin
        ATI.Put_Line ("Ada_Proc: Begin");
        ATI.Put_Line ("Ada_Proc: End");
   end Ada_Proc;

end Ada_Code;
