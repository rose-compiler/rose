private with Ada.Exceptions;
private with Ada.Wide_Text_IO;
private with Asis;

private with Dot;

-- Contains supporting declarations for child packages
package Asis_Tool_2 is

   -- Controls behavior of Trace_Put:
   Trace_On : Boolean := False;

private

   package Aex renames Ada.Exceptions;
   package Awti renames Ada.Wide_Text_IO;

   function To_String (Item : in Wide_String) return String;
   function "+"(Item : in Wide_String) return String renames To_String;

   function To_Wide_String (Item : in String) return Wide_String;
   function "+"(Item : in String) return Wide_String renames To_Wide_String;

   procedure Trace_Put (Message : in Wide_String);

   procedure Trace_Put_Line (Message : in Wide_String);

   procedure Print_Exception_Info (X : in Aex.Exception_Occurrence);

   -- Returns the image minus the leading space:
   function Spaceless_Image (Item : in Natural) return String;

   function Node_Id_Image (Unit : in Asis.Compilation_Unit) return String;
   function To_Dot_ID_Type (Unit : in Asis.Compilation_Unit) return Dot.ID_Type;
   function Node_Id_Image (Element : in Asis.Element) return String;
   function To_Dot_ID_Type (Element : in Asis.Element) return Dot.ID_Type;

      -- At the beginning of each line, puts out white space relative to the
   -- current indent.  Emits Wide text:
   package Indented_Text is

      type Class is tagged private;

      procedure Indent
        (This : in out Class);
      procedure Dedent
        (This : in out Class);

      -- Sets Line_In_Progress:
      procedure New_Line
        (This : in out Class);
      -- Puts a new line only if a line is in progress:
      -- Sets Line_In_Progress:
      procedure End_Line
        (This : in out Class);

      -- Prepends the indent if this is the first put on this line:
      procedure Put
        (This    : in out Class;
         Message : in String);
      procedure Put
        (This    : in out Class;
         Message : in Wide_String);

      -- Prepends the indent if this is the first put on this line:
      procedure Put_Indented_Line
        (This    : in out Class;
         Message : in String);
      procedure Put_Indented_Line
        (This    : in out Class;
         Message : in Wide_String);
   private

   -- Can't be limited because generic Asis.Iterator.Traverse_Element doesn't
   -- want limited state information:
   -- (In Asis_Tool_2.Element)
      type Class is tagged -- Initialized
         record
            Line_In_Progress : Boolean := False;
            Indent_Level     : Natural := 0;
         end record;

      -- Puts an indent only if a line is not in progress (no indent put yet):
      -- Sets Line_In_Progress:
      procedure Put_Indent_If_Needed
        (This : in out Class);

      function White_Space
        (This : in Class)
         return Wide_String;

   end Indented_Text;

end Asis_Tool_2;
