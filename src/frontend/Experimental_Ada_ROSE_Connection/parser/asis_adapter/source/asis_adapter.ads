with Ada.Strings.Unbounded;

with A_Nodes;
with Dot;
with Indented_Text;

private with Ada.Exceptions;
private with Ada.Text_IO;
private with Ada.Wide_Text_IO;
private with Asis;
private with Asis.Set_Get;
private with Interfaces.C.Extensions;
private with Interfaces.C.Strings;
private with Unchecked_Conversion;

-- GNAT-specific:
private with A4G.A_Types;
private with Types;

private with a_nodes_h.Support;

-- Contains supporting declarations for child packages
package Asis_Adapter is

   -- Controls behavior of Trace_ routines.  Renamed here so clients have to
   -- with fewer packages:
   Trace_On : Boolean renames Indented_Text.Trace_On;
   Log_On : Boolean ;

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

   -- Raised when an external routine raises a usage-error-like exception or
   -- there is an internal logic error:
   Internal_Error : Exception;

private
   Module_Name : constant String := "Asis_Adapter";

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
   procedure Put_Wide      (Item : in Wide_String) renames AWTI.Put;
   procedure Put_Line_Wide (Item : in Wide_String) renames AWTI.Put_Line;

   procedure Trace_Put      (Message : in Wide_String) renames
     Indented_Text.Trace_Put;
   procedure Trace_Put_Line (Message : in Wide_String) renames
     Indented_Text.Trace_Put_Line;

   -- Provides routines that peofix the output with the name of the current
   -- module:
   generic
      Module_Name : in string;
   package Generic_Logging is
      procedure Log (Message : in String);
      procedure Log_Wide (Message : in Wide_String);
      procedure Log_Exception (X : in Aex.Exception_Occurrence);
   end Generic_Logging;


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

   -- Order below is alphabetical:
   function To_Access_Definition_Kinds is new Unchecked_Conversion
     (Source => Asis.Access_Definition_Kinds,
      Target => a_nodes_h.Access_Definition_Kinds);

   function To_Access_Type_Kinds is new Unchecked_Conversion
     (Source => Asis.Access_Type_Kinds,
      Target => a_nodes_h.Access_Type_Kinds);

   function To_Association_Kinds is new Unchecked_Conversion
     (Source => Asis.Association_Kinds,
      Target => a_nodes_h.Association_Kinds);

   function To_Attribute_Kinds is new Unchecked_Conversion
     (Source => Asis.Attribute_Kinds,
      Target => a_nodes_h.Attribute_Kinds);

   function To_Clause_Kinds is new Unchecked_Conversion
     (Source => Asis.Clause_Kinds,
      Target => a_nodes_h.Clause_Kinds);

   function To_Representation_Clause_Kinds is new Unchecked_Conversion
     (Source => Asis.Representation_Clause_Kinds,
      Target => a_nodes_h.Representation_Clause_Kinds);

   function To_Constraint_Kinds is new Unchecked_Conversion
     (Source => Asis.Constraint_Kinds,
      Target => a_nodes_h.Constraint_Kinds);

   function To_Declaration_Kinds is new Unchecked_Conversion
     (Source => Asis.Declaration_Kinds,
      Target => a_nodes_h.Declaration_Kinds);

   function To_Declaration_Origins is new Unchecked_Conversion
     (Source => Asis.Declaration_Origins,
      Target => a_nodes_h.Declaration_Origins);

   function To_Defining_Name_Kinds is new Unchecked_Conversion
     (Source => Asis.Defining_Name_Kinds,
      Target => a_nodes_h.Defining_Name_Kinds);

   function To_Definition_Kinds is new Unchecked_Conversion
     (Source => Asis.Definition_Kinds,
      Target => a_nodes_h.Definition_Kinds);

   function To_Discrete_Range_Kinds is new Unchecked_Conversion
     (Source => Asis.Discrete_Range_Kinds,
      Target => a_nodes_h.Discrete_Range_Kinds);

   function To_Element_Kinds is new Unchecked_Conversion
     (Source => Asis.Element_Kinds,
      Target => a_nodes_h.Element_Kinds);

   function To_Expression_Kinds is new Unchecked_Conversion
     (Source => Asis.Expression_Kinds,
      Target => a_nodes_h.Expression_Kinds);

   function To_Formal_Type_Kinds is new Unchecked_Conversion
     (Source => Asis.Formal_Type_Kinds,
      Target => a_nodes_h.Formal_Type_Kinds);

   function To_Mode_Kinds is new Unchecked_Conversion
     (Source => Asis.Mode_Kinds,
      Target => a_nodes_h.Mode_Kinds);

   function To_Operator_Kinds is new Unchecked_Conversion
     (Source => Asis.Operator_Kinds,
      Target => a_nodes_h.Operator_Kinds);

   function To_Path_Kinds is new Unchecked_Conversion
     (Source => Asis.Path_Kinds,
      Target => a_nodes_h.Path_Kinds);

   function To_Pragma_Kinds is new Unchecked_Conversion
     (Source => Asis.Pragma_Kinds,
      Target => a_nodes_h.Pragma_Kinds);

   function To_Root_Type_Kinds is new Unchecked_Conversion
     (Source => Asis.Root_Type_Kinds,
      Target => a_nodes_h.Root_Type_Kinds);

   function To_Statement_Kinds is new Unchecked_Conversion
     (Source => Asis.Statement_Kinds,
      Target => a_nodes_h.Statement_Kinds);

   function To_Subprogram_Default_Kinds is new Unchecked_Conversion
     (Source => Asis.Subprogram_Default_Kinds,
      Target => a_nodes_h.Subprogram_Default_Kinds);

   function To_Type_Kinds is new Unchecked_Conversion
     (Source => Asis.Type_Kinds,
      Target => a_nodes_h.Type_Kinds);

   function To_Unit_Classes is new Unchecked_Conversion
     (Source => Asis.Unit_Classes,
      Target => a_nodes_h.Unit_Classes);

   function To_Unit_Kinds is new Unchecked_Conversion
     (Source => Asis.Unit_Kinds,
      Target => a_nodes_h.Unit_Kinds);

   function To_Unit_Origins is new Unchecked_Conversion
     (Source => Asis.Unit_Origins,
      Target => a_nodes_h.Unit_Origins);

   -- End alphabetical order

end Asis_Adapter;
