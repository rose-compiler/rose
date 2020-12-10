with Ada.Text_IO;

package body Ada_Code is
   package ATI renames Ada.Text_Io;

   procedure Ada_Proc is
   begin
        ATI.Put_Line ("Ada_Proc: Begin");
        ATI.Put_Line ("Ada_Proc: End");
   end Ada_Proc;

   procedure Ada_C_Caller is
      procedure C_Func;
      pragma Import (C, C_Func);
   begin
      ATI.Put_Line ("Ada_C_Caller: Calling C_Func");
      C_Func;
      ATI.Put_Line ("Ada_C_Caller: Returned from C_Func");
   end Ada_C_Caller;

end Ada_Code;
