private with Ada.Exceptions;
private with Ada.Wide_Text_IO;

-- Contains supporting declarations for child packages
package Asis_Tool_2 is

   -- Controls behavior of Trace_Put:
   Trace_On : Boolean := False;

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

   package Indented_Text is

      type Class is tagged private;

      procedure Indent (This : in out Class);
      procedure Dedent (This : in out Class);
      procedure Put_Indent (This : in Class);
      procedure New_Line (This : in Class);

      function White_Space
        (This : in Class)
         return Wide_String;

      procedure Put_Indented_Line
        (This    : in Class;
         Message : in Wide_String);

      procedure Put
        (This    : in Class;
         Message : in Wide_String);

   private

   -- Can't be limited because generic Asis.Iterator.Traverse_Element doesn't
   -- want limited state information:
   -- (In Asis_Tool_2.Element)
      type Class is tagged -- Initialized
         record
            Indent_Level : Natural := 0;
         end record;

   end Indented_Text;

end Asis_Tool_2;
