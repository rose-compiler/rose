-- At the beginning of each line, puts out white space relative to the
-- current indent.  Emits Wide text:
package Indented_Text is

   --------------------------------------------------------
   -- Trace routines emit no output when Trace_On is False.  At the moment,
   -- Indented_Text.Class output routines call these:

   procedure Trace_Put (Message : in Wide_String);

   procedure Trace_Put_Line (Message : in Wide_String);

   -- Controls behavior of Trace_ routines::
   Trace_On : Boolean := False;

   --------------------------------------------------------

   type Class is tagged private;
   type Access_Class is access Class;

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
