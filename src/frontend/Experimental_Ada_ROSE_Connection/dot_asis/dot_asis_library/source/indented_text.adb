with Ada.Characters.Handling;
with Ada.Wide_Text_IO;

package body Indented_Text is

   package Ach renames Ada.Characters.Handling;
   package Awti renames Ada.Wide_Text_IO;

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
         Awti.Put_Line ("$$$ " & Message);
      end if;
   end Trace_Put_Line;

   -- Used below to control which routines are used for output:
   procedure Put (Message : in Wide_String) renames
     Trace_Put;
   procedure Put_Line (Message : in Wide_String) renames
     Trace_Put_Line;

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
      if This.Indent_Level = 0 then
         Put_Line ("(Attempted negative indent)");
      else
         This.Indent_Level := This.Indent_Level - 1;
      end if;
   end Dedent;

   ------------
   -- EXPORTED:
   ------------
   procedure New_Line
     (This : in out Class) is
   begin
      Put_Line ("");
      This.Line_In_Progress := False;
   end New_Line;

   ------------
   -- EXPORTED:
   ------------
   procedure End_Line
     (This : in out Class) is
   begin
      if This.Line_In_Progress then
         This.New_Line;
      end if;
   end End_Line;

   ------------
   -- EXPORTED:
   ------------
   procedure Put
     (This    : in out Class;
      Message : in String) is
   begin
      This.Put (ACH.To_Wide_String (Message));
   end Put;

   ------------
   -- EXPORTED:
   ------------
   procedure Put
     (This    : in out Class;
      Message : in Wide_String) is
   begin
      This.Put_Indent_If_Needed;
      Put (Message);
   end Put;

   ------------
   -- EXPORTED:
   ------------
   procedure Put_Indented_Line
     (This    : in out Class;
      Message : in String) is
   begin
      This.Put_Indented_Line (ACH.To_Wide_String (Message));
   end Put_Indented_Line;

   ------------
   -- EXPORTED:
   ------------
   procedure Put_Indented_Line
     (This    : in out Class;
      Message : in Wide_String) is
   begin
      This.Put_Indent_If_Needed;
      Put_Line (Message);
      This.Line_In_Progress := False;
   end Put_Indented_Line;

   ------------
   -- PRIVATE:
   ------------
   procedure Put_Indent_If_Needed
     (This : in out Class) is
   begin
      if not This.Line_In_Progress then
         Put (This.White_Space);
         This.Line_In_Progress := True;
      end if;
   end Put_Indent_If_Needed;

   ------------
   -- PRIVATE:
   ------------
   function White_Space
     (This : in Class)
         return Wide_String is
     ((1 .. This.Indent_Level * 2 => ' '));

end Indented_Text;
