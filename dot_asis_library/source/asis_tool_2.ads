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

   -- Returns the image minus the leading space:
   function Spaceless_Image (Item : in Natural) return String;
   function NLB_Image (Item : in Natural) return String renames Spaceless_Image;

   function To_String (Unit_Id : in A4G.A_Types.Unit_Id) return String;
   function To_String (Element_Id : in a_nodes_h.Element_ID) return String;
   function To_Dot_ID_Type (Unit_Id : in A4G.A_Types.Unit_Id) return Dot.ID_Type;
   function To_Dot_ID_Type (Element_Id : in a_nodes_h.Element_ID) return Dot.ID_Type;

end Asis_Tool_2;
