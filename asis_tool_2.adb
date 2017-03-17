with Ada.Characters.Handling;

package body Asis_Tool_2 is

   package Ach renames Ada.Characters.Handling;

   -----------
   -- PRIVATE:
   -----------
   function To_String (Item : in Wide_String) return String is
   begin
      return Ach.To_String (Item, Substitute => ' ');
   end To_String;

   -----------
   -- PRIVATE:
   -----------
   function To_Wide_String (Item : in String) return Wide_String
     renames Ach.To_Wide_String;

   -----------
   -- PRIVATE:
   -----------
   procedure Trace_Put (Message : in Wide_String) is
   begin
      if Trace_On then
         Awti.Put (Message);
      end if;
   end Trace_Put;

   -----------
   -- PRIVATE:
   -----------
   procedure Trace_Put_Line (Message : in Wide_String) is
   begin
      if Trace_On then
         Awti.Put_Line (Message);
      end if;
   end Trace_Put_Line;

   -----------
   -- PRIVATE:
   -----------
   procedure Print_Exception_Info (X : in Ada.Exceptions.Exception_Occurrence) is
   begin
      Awti.Put_Line ("EXCEPTION: " & To_Wide_String (Ae.Exception_Name (X)) &
                  " (" & To_Wide_String (Ae.Exception_Information (X)) & ")");
      Awti.New_Line;
   end Print_Exception_Info;

end Asis_Tool_2;
