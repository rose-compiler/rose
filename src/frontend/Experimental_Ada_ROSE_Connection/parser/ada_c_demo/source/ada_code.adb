with Ada.Text_IO;
with c_code_h;

package body Ada_Code is
   package ATI renames Ada.Text_Io;

   procedure Ada_Proc is
   begin
        ATI.Put_Line ("Ada_Proc: Begin");
        ATI.Put_Line ("Ada_Proc: End");
   end Ada_Proc;

   procedure Ada_C_Caller is
   begin
      ATI.Put_Line ("Ada_C_Caller: Calling c_func");
      c_code_h.c_func;
      ATI.Put_Line ("Ada_C_Caller: Returned from c_func");
   end Ada_C_Caller;

end Ada_Code;
