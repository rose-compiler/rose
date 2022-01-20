with a_nodes_h.Support;
with Libadalang.Analysis;
with Libadalang.Common;
with Ada.Containers.Indefinite_Hashed_Maps;
with Ada.Strings.Hash;

use Ada.Containers;

private with Ada.Containers.Doubly_Linked_Lists;

package Lal_Adapter.Node is

   package anhS renames a_nodes_h.Support;
   package LAL renames Libadalang.Analysis;
   package LALCO renames Libadalang.Common;

   type Class (Trace : Boolean := False) is tagged limited private; -- Initialized

   -- Process a node and all of its components:
   -- Raises Internal_Error for unhandled internal exceptions.
   procedure Process
     (This    : in out Class;
      Node    : in     LAL.Ada_Node'Class;
      --  Options : in     Options_Record;
      Outputs : in     Output_Accesses_Record);

   -- NOTE: There appears to be no unique ID for a LAL.Ada_Node!
   -- Retaining this until we figure out what TODO:

   -----------------------------------------------------------------------------
   -- This encapsulates the identity of an Element, since sometimes a
   -- Node_ID gets reused!
   --
   type Element_ID is record -- initialized
      Node_ID : Integer := anhS.Invalid_ID;
      Kind    : LALCO.Ada_Node_Kind_Type := LALCO.Ada_Abort_Absent;
   end record;

   No_Element_ID : constant Element_ID := (Node_ID => anhS.Empty_ID,
                                           Kind    => LALCO.Ada_Abort_Absent);

   -- To get an a_nodes_h.Element_ID:
   -- Asis.Element -> Get_Element_ID -> To_Element_ID -> a_nodes_h.Element_ID
   -- or
   -- Asis.Element -> Get_Element_ID -> a_nodes_h.Element_ID
   --
   -- To get a string for DOT or text output:
   -- a_nodes_h.Element_ID -> To_String -> String (e.g. Element_12001)
   --
   function Get_Element_ID
     (Node : in LAL.Ada_Node'Class)
      return Element_ID;

   -- Turns Node_ID and Kind into one number.  Currently (GNAT GPL 2017 ASIS)
   -- there are about 800 values in A4G.Int_Knds.Internal_Element_Kinds, so
   -- we multiply Node_ID by 1000 and add Kind.  Assuming a 32-bit int for
   -- a_nodes_h.Element_ID, this means we cannot process Elements with a Node_ID
   -- over 1,000,000.
   --
   -- TODO: Move to anhS
   function To_Element_ID
     (This : in Element_ID)
      return a_nodes_h.Element_ID;

   function Get_Element_ID
     (Element : in LAL.Ada_Node'Class)
      return a_nodes_h.Element_ID;

   function To_String
     (This : in a_nodes_h.Element_ID)
      return String;

   -- END Element_ID support
   -----------------------------------------------------------------------------

private

   -- For debuggng:
   Parent_Name : constant String := Module_Name;
   Module_Name : constant String := Parent_Name & ".Node";

-- This provides a map for the address of node to look up for its ID
   package Node_ID_Map is new Indefinite_Hashed_Maps (Key_Type => LAL.Ada_Node,
                                               Element_Type => Integer,
                                               Hash => LAL.Hash,
                                               Equivalent_Keys => LAL."=");

   package Element_ID_Lists is new
     Ada.Containers.Doubly_Linked_Lists
       (Element_Type => a_nodes_h.Element_ID,
        "="          => IC."=");
   -- Make type and operations directly visible:
   type Element_ID_List is new Element_ID_Lists.List with null record;

   type Class (Trace : Boolean := False) is tagged limited -- Initialized
      record
         -- Current, in-progress intermediate output products:
         -- Used when making dot edges to child nodes:
         Dot_Node  : Dot.Node_Stmt.Class; -- Initialized
         Dot_Label : Dot.HTML_Like_Labels.Class; -- Initialized
         A_Element : a_nodes_h.Element_Struct := anhS.Default_Element_Struct;
         -- Since there are libadalang nodes, and dot graph nodes, to
         -- avoid confusion for a while, let's use Element_ID for the libadalang
         -- node ID (especially since we don't know how to get a libadalang node
         -- ID yet!)

         -- Used when making dot edges to child nodes.  Treated s a stack:
         Element_IDs : Element_ID_List;
         -- I would like to just pass Outputs through and not store it in the
         -- object, since it is all pointers and we don't need to store their
         -- values between calls to Traverse. Outputs has to go into
         -- Add_To_Dot_Label, though, so we'll put it in the object and pass
         -- the object to Add_To_Dot_Label:
         Outputs : Output_Accesses_Record; -- Initialized
      end record;

   -- Helper methods for use by children:

   -- String:
   -- Add <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Value : in     String);

   -- String
   -- Add <Name> => <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String);

   -- Element_ID
   -- Add <Name> => <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     a_nodes_h.Element_ID);

   -- Boolean
   -- Add <Name> => <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Boolean);

   -- Add an edge:
   procedure Add_Dot_Edge
     (This  : in out Class;
      From  : in     a_nodes_h.Element_ID;
      To    : in     a_nodes_h.Element_ID;
      Label : in     String);

   -- Add an edge and a dot label:
   procedure Add_To_Dot_Label_And_Edge
     (This  : in out Class;
      Label : in     String;
      To    : in     a_nodes_h.Element_ID);

   type Ada_Versions is
     (Ada_83,
      Ada_95,
      Ada_2005,
      Ada_2012,
      Ada_2020);
   pragma Ordered (Ada_Versions);

   Supported_Ada_Version : constant Ada_Versions := Ada_95;

   -- If Ada_Version <= Supported_Ada_Version then:
   --    Add to dot label: ASIS_PROCESSING =>
   --      "NOT_IMPLEMENTED_COMPLETELY"
   --    and increment the Not_Implemented count
   -- Otherwise:
   --    Add to dot label: ASIS_PROCESSING =>
   --      Ada_Version & "_FEATURE_NOT_IMPLEMENTED_IN_" & Supported_Ada_Version
   procedure Add_Not_Implemented
     (This        : in out Class;
      Ada_Version : in     Ada_Versions := Supported_Ada_Version);

end Lal_Adapter.Node;
