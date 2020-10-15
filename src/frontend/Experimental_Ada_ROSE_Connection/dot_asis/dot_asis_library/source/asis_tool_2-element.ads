with Asis;
-- GNAT-specific:
with A4G.Int_Knds;
with Types;

with a_nodes_h;
with Dot;

private with Ada.Containers.Doubly_Linked_Lists;

package Asis_Tool_2.Element is

   type Class is tagged private; -- Initialized

   -- Process an element and all of its components:
   procedure Process_Element_Tree
     (This    : in out Class;
      Element : in     Asis.Element;
      Outputs : in     Outputs_Record);

   -----------------------------------------------------------------------------
   -- This encapsulates the identity of an Element, since sometimes a
   -- Node_ID gets reused!
   --
   type Element_ID is record
      Node_ID : Types.Node_ID := Types.Error;
      Kind    : A4G.Int_Knds.Internal_Element_Kinds :=
        A4G.Int_Knds.Not_An_Element;
   end record;

   -- To get an a_nodes_h.Element_ID:
   -- Asis.Element -> Get_Element_ID -> To_Element_ID -> a_nodes_h.Element_ID
   -- or
   -- Asis.Element -> Get_Element_ID -> a_nodes_h.Element_ID
   --
   -- To get a string for DOT or text output:
   -- a_nodes_h.Element_ID -> To_String -> String (e.g. Element_12001)
   --
   function Get_Element_ID
     (Element : in Asis.Element)
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
     (Element : in Asis.Element)
      return a_nodes_h.Element_ID;

   function To_String
     (This : in a_nodes_h.Element_ID)
      return String;

   -- END Element_ID support
   -----------------------------------------------------------------------------

   -- Add an array of Element IDs to Dot_Label and maybe Dot_Edge, and return
   -- an Element_ID_List:
   -- LEAKS:
   function To_Element_ID_List
     (Dot_Label       : in out Dot.HTML_Like_Labels.Class;
      Outputs         : in out Outputs_Record;
      This_Element_ID : in     a_nodes_h.Element_ID;
      Elements_In     : in     Asis.Element_List;
      Dot_Label_Name  : in     String;
      Add_Edges       : in     Boolean := False;
      This_Is_Unit    : in     Boolean := False)
     return  a_nodes_h.Element_ID_List;

private

   Module_Name : constant String := "Asis_Tool_2.Element";

   package Element_ID_Lists is new
     Ada.Containers.Doubly_Linked_Lists
       (Element_Type => a_nodes_h.Element_ID,
        "="          => IC."=");
   -- Make type and operations directly visible:
   type Element_ID_List is new Element_ID_Lists.List with null record;

   -- Can't be limited because generic Asis.Iterator.Traverse_Element doesn't
   -- want limited state information:
   type Class is tagged -- Initialized
      record
         -- Current, in-progress intermediate output products.  These need to be
         -- turned into stacks if they are ever used in Post_Operation.  Now
         -- their usage ends at the end of Pre_Operation:
         Dot_Node   : Dot.Node_Stmt.Class; -- Initialized
         Dot_Label  : Dot.HTML_Like_Labels.Class; -- Initialized
         A_Element  : a_nodes_h.Element_Struct := anhS.Default_Element_Struct;
         -- Used when making dot edges to child nodes.  Treated s a stack:
         Element_IDs : Element_ID_List;
--           Element_ID : a_nodes_h.Element_ID := anhS.Invalid_Element_ID;
      -- I would like to just pass Outputs through and not store it in the
      -- object, since it is all pointers and we doesn't need to store their
      -- values between calls to Process_Element_Tree. Outputs has to go into
      -- State_Information in the Traverse_Element instatiation, though, so
      -- we'll put it in the object and pass that:
         Outputs    : Outputs_Record; -- Initialized
      end record;

   -- Helper methods for use by children:

   -- String
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String);

   -- Wide_String
   -- Add <Name> => <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String);

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

   -- String:
   -- Add <Value> to the label, and print it if trace is on:
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Value : in     String);

   -- Add to dot label: ASIS_PROCESSING => "NOT_IMPLEMENTED_COMPLETELY"
   procedure Add_Not_Implemented
     (This : in out Class);

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

   function Add_Operator_Kind
     (State   : in out Class;
      Element : in     Asis.Element)
      return a_nodes_h.Operator_Kinds;

   function To_Element_ID_List
     (This           : in out Class;
      Elements_In    : in     Asis.Element_List;
      Dot_Label_Name : in     String;
      Add_Edges      : in     Boolean := False)
      return  a_nodes_h.Element_ID_List;

   procedure Add_Element_List
     (This           : in out Class;
      Elements_In    : in     Asis.Element_List;
      Dot_Label_Name : in     String;
      List_Out       :    out a_nodes_h.Element_ID_List;
      Add_Edges      : in     Boolean := False);

end Asis_Tool_2.Element;
