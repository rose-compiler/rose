with a_nodes_h.Support;

private with Ada.Containers.Hashed_Sets;
private with Interfaces.C;

-- This class hides and manages some of the details of the liniked
-- list of Element_Struct C interface records.  Like Dot.ads, you only need to
-- pass in Access_Class to the users - they don't need to pass it back.  The top
-- caller declares the actual Class object.

package A_Nodes is

   type Class is tagged private;
   type Access_Class is access Class;

   -- Copies Node to a new node on the heap and links it to the top of the list.
   -- Raises Usage_Error if a node with this ID has already been pushed:
   procedure Push
     (This : access Class;
      Unit : in     a_nodes_h.Unit_Struct);

   -- Copies Node to a new node on the heap and links it to the top of the list.
   -- Raises Usage_Error if a node with this ID has already been pushed:
   procedure Push
     (This    : access Class;
      Element : in     a_nodes_h.Element_Struct);

   -- Returns a pointer to the node at the top of the list:
   function Get_Head_Unit
     (This : access Class)
      return a_nodes_h.Unit_Structs_Ptr;

   -- Returns a pointer to the node at the top of the list:
   function Get_Head_Element
     (This : access Class)
      return a_nodes_h.Element_Structs_Ptr;

   function Is_Empty
     (This : access Class)
      return Boolean;

   procedure Print_Stats
     (This : access Class);

   -- A client has used the package improperly:
   Usage_Error : exception;

private

   package AC renames Ada.Containers;
   package IC renames Interfaces.C;

   function Hash (Element : a_nodes_h.Element_ID) return AC.Hash_Type is
      (AC.Hash_Type (Element.Node));

   package Element_ID_Sets is new AC.Hashed_Sets
     (Element_Type        => a_nodes_h.Element_ID,
      Hash                => Hash,
      Equivalent_Elements => a_nodes_h."=",
      "="                 => a_nodes_h."=");
   -- Make primitive operations directly visible:
   Type Element_ID_Set is new Element_ID_Sets.Set with null record;

   function Hash (Unit : a_nodes_h.Unit_ID) return AC.Hash_Type is
      (AC.Hash_Type (Unit));

   package Unit_ID_Sets is new AC.Hashed_Sets
     (Element_Type        => a_nodes_h.Unit_ID,
      Hash                => Hash,
      Equivalent_Elements => IC."=",
      "="                 => IC."=");
   -- Make primitive operations directly visible:
   Type Unit_ID_Set is new Unit_ID_Sets.Set with null record;

   type Class is tagged record -- Initialized
      Head_Unit          : a_nodes_h.Unit_Structs_Ptr; -- Initialized
      Head_Element       : a_nodes_h.Element_Structs_Ptr; -- Initialized
      Has_Context        : Boolean := False;
      Unit_IDs           : Unit_ID_Set; -- Initialized
      Element_IDs        : Element_ID_Set; -- Initialized
      Highest_Unit_ID    : a_nodes_h.Unit_ID := a_nodes_h.Support.Invalid_Unit_ID;
      Highest_Element_ID : a_nodes_h.Element_ID := a_nodes_h.Support.Invalid_Element_ID;
   end record;

end A_Nodes;
