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

   -----------
   -- PRIVATE:
   -----------
   package body Indented_Text is

      ------------
      -- EXPORTED:
      ------------
      procedure Indent (This : in out Class) is
      begin
         This.Indent_Level := This.Indent_Level + 1;
      end Indent;

      ------------
      -- EXPORTED:
      ------------
      procedure Dedent (This : in out Class) is
      begin
         This.Indent_Level := This.Indent_Level - 1;
      end Dedent;

      ------------
      -- EXPORTED:
      ------------
      procedure Put_Indent
        (This : in Class) is
      begin
         Ada.Wide_Text_IO.Put (This.White_Space);
      end Put_Indent;

      ------------
      -- EXPORTED:
      ------------
      procedure New_Line
        (This : in Class) is
      begin
         Ada.Wide_Text_IO.New_Line;
      end New_Line;

      ------------
      -- EXPORTED:
      ------------
      function White_Space
        (This : in Class)
      return Wide_String is
      begin
         return (1 .. This.Indent_Level * 2 => ' ');
      end White_Space;

      ------------
      -- EXPORTED:
      ------------
      procedure Put_Indented_Line
        (This    : in Class;
         Message : in Wide_String) is
      begin
         Ada.Wide_Text_IO.Put_Line (This.White_Space & Message);
      end Put_Indented_Line;

      ------------
      -- EXPORTED:
      ------------
      procedure Put
        (This    : in Class;
         Message : in Wide_String) is
      begin
         Ada.Wide_Text_IO.Put (Message);
      end Put;

   end Indented_Text;

end Asis_Tool_2;
