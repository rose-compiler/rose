with Ada.Characters.Handling;
with Ada.Wide_Text_IO;

package body Asis_Tool_2 is

   -----------
   -- PRIVATE:
   -----------
   procedure Trace_Put (Message : in Wide_String) is
   begin
      if Trace_On then
         Ada.Wide_Text_IO.Put (Message);
      end if;
   end Trace_Put;

   -----------
   -- PRIVATE:
   -----------
   procedure Print_Exception_Info (X : in Ada.Exceptions.Exception_Occurrence) is
      use Ada.Characters.Handling;
      use Ada.Exceptions;
      use Ada.Wide_Text_IO;
   begin
      Put_Line ("EXCEPTION: " & To_Wide_String (Exception_Name (X)) &
                  " (" & To_Wide_String (Exception_Information (X)) & ")");
      New_Line;
   end Print_Exception_Info;

end Asis_Tool_2;
