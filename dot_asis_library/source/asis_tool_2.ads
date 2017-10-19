with Ada.Strings.Unbounded;

with A_Nodes;
with Dot;
with Indented_Text;

private with Ada.Exceptions;
private with Ada.Text_IO;
private with Ada.Wide_Text_IO;
private with Asis;
private with Interfaces.C.Extensions;
private with Interfaces.C.Strings;

-- GNAT-specific:
private with A4G.A_Types;
private with Types;

private with a_nodes_h.Support;

-- Contains supporting declarations for child packages
package Asis_Tool_2 is

   -- Controls behavior of Trace_ routines.  Renamed here so clients have to
   -- with fewer packages:
   Trace_On : Boolean renames Indented_Text.Trace_On;

   type Outputs_Record is record -- Initialized
      Output_Dir : Ada.Strings.Unbounded.Unbounded_String; -- Initialized
      A_Nodes    : Standard.A_Nodes.Access_Class; -- Initialized
      Graph      : Dot.Graphs.Access_Class; -- Initialized
      Text       : Indented_Text.Access_Class; -- Initialized
   end record;

   -- Raised when a subprogram is called incorrectly:
   Usage_Error : Exception;

   -- Raised when an external routine fails and the subprogram cannot continue:
   External_Error : Exception;

private

   package AEX renames Ada.Exceptions;
   package ASU renames Ada.Strings.Unbounded;
   package ATI renames Ada.Text_IO;
   package AWTI renames Ada.Wide_Text_IO;
   package IC renames Interfaces.C;
   package ICE renames Interfaces.C.Extensions;
   package ICS renames Interfaces.C.Strings;
   package anhS renames a_nodes_h.Support;

   function To_String (Item : in Wide_String) return String;
   function To_Quoted_String (Item : in Wide_String) return String;
   function "+"(Item : in Wide_String) return String renames To_String;

   function To_Wide_String (Item : in String) return Wide_String;
   function "+"(Item : in String) return Wide_String renames To_Wide_String;

   function To_Chars_Ptr (Item : in Wide_String)
                          return Interfaces.C.Strings.chars_ptr;
   function To_Chars_Ptr (Item : in String)
                          return Interfaces.C.Strings.chars_ptr
     renames Interfaces.C.Strings.New_String;

   procedure Put      (Item : in String) renames ATI.Put;
   procedure Put_Line (Item : in String) renames ATI.Put_Line;

   procedure Trace_Put      (Message : in Wide_String) renames
     Indented_Text.Trace_Put;
   procedure Trace_Put_Line (Message : in Wide_String) renames
     Indented_Text.Trace_Put_Line;

   procedure Print_Exception_Info (X : in Aex.Exception_Occurrence);
   procedure Log_Exception (X : in Aex.Exception_Occurrence)
     renames Print_Exception_Info;

   -- Returns the image minus the leading space:
   function Spaceless_Image (Item : in Natural) return String;
   function NLB_Image (Item : in Natural) return String renames Spaceless_Image;

   type ID_Kind is (Unit_ID_Kind, Element_ID_Kind);

   function To_String
     (Id   : in IC.int;
      Kind : in ID_Kind) return String;

   function To_Dot_ID_Type
     (Id   : in IC.int;
      Kind : in ID_Kind)
      return Dot.ID_Type;

   -- String:
   -- Add <Name> => <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (Dot_Label : in out Dot.HTML_Like_Labels.Class;
      Outputs   : in out Outputs_Record;
      Name      : in     String;
      Value     : in     String);

   -- Boolean:
   -- Add <Name> => <Value> to the label, and print it if trace is on:
   -- ONLY acts if Value = True:
   procedure Add_To_Dot_Label
     (Dot_Label : in out Dot.HTML_Like_Labels.Class;
      Outputs   : in out Outputs_Record;
      Name      : in     String;
      Value     : in     Boolean);

   -- String:
   -- Add <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (Dot_Label : in out Dot.HTML_Like_Labels.Class;
      Outputs   : in out Outputs_Record;
      Value     : in     String);

   -- Unit_ID or Element_ID:
   -- Add an edge node to the the dot graph:
   -- Use for both Unit_ID and Element_ID:
   procedure Add_Dot_Edge
     (Outputs   : in out Outputs_Record;
      From      : in     IC.int;
      From_Kind : in     ID_Kind;
      To        : in     IC.int;
      To_Kind   : in     ID_Kind;
      Label     : in     String);

end Asis_Tool_2;
