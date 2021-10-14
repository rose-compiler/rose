with Ada.Characters.Handling;
with Ada.Text_IO;
with Ada.Wide_Text_IO;
with GNAT.Traceback.Symbolic;

package body Generic_Logging is
   
   Wide_Module_Name : constant Wide_String := 
     Ada.Characters.Handling.To_Wide_String (Module_Name);
   
   ------------
   -- EXPORTED:
   ------------
   procedure Log (Message : in String) is
   begin
      Ada.Text_IO.Put_Line (Module_Name & ":  " & Message);
   end Log;
   
   ------------
   -- EXPORTED:
   ------------
   procedure Log_Wide (Message : in Wide_String) is
   begin
      Ada.Wide_Text_IO.Put_Line (Wide_Module_Name & ":  " & Message);
   end Log_Wide;

   ------------
   -- EXPORTED:
   ------------
   procedure Log_Exception (X : in Ada.Exceptions.Exception_Occurrence) is
   begin
      Log ("EXCEPTION: " & Ada.Exceptions.Exception_Name (X));
      Log (Ada.Exceptions.Exception_Information (X));
      Log ("TRACEBACK: ");
      Log (GNAT.Traceback.Symbolic.Symbolic_Traceback (X));
   end Log_Exception;

   ----------------------
   -- EXPORTED (private):
   ----------------------
   procedure Initialize (Self : in out Auto_Logger) is
   begin
      Log ("BEGIN");
   exception
      when X: others =>
         Log_Exception (X);
         Log ("Reraising exception in Initialize procedure.  Should cause Program_Error");
         raise;
   end Initialize;

   ----------------------
   -- EXPORTED (private):
   ----------------------
   procedure Finalize (Self : in out Auto_Logger) is
   begin
      Log ("END");
   exception
      when X: others =>
         Log_Exception (X);
         Log ("Reraising exception in Initialize procedure.  Should cause Program_Error");
         raise;
   end Finalize;
   
end Generic_Logging;
