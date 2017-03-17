private with Ada.Exceptions;
private with Ada.Wide_Text_IO;

with Dot;

-- Contains supporting declarations for child packages

package Asis_Tool_2 is

   -- Controls behavior of Trace_Put:
   Trace_On : Boolean := False;

   -- Global DOT graph:
   Graph    : Dot.Graph.Class;

private

   package Ae renames Ada.Exceptions;
   package Awti renames Ada.Wide_Text_IO;

   function To_String (Item : in Wide_String) return String;
   function "+"(Item : in Wide_String) return String renames To_String;

   function To_Wide_String (Item : in String) return Wide_String;
   function "+"(Item : in String) return Wide_String renames To_Wide_String;

   procedure Trace_Put (Message : in Wide_String);

   procedure Trace_Put_Line (Message : in Wide_String);

   procedure Print_Exception_Info (X : in Ae.Exception_Occurrence);

end Asis_Tool_2;
