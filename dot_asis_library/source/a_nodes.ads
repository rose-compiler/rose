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

   procedure Set
     (This    : access Class;
      Context : in     a_nodes_h.Context_Struct);

   -- Copies Unit to a new unit on the heap and links that to the top of the list.
   -- Raises Usage_Error if a unit with this ID has already been pushed:
   -- LEAKS:
   procedure Push
     (This : access Class;
      Unit : in     a_nodes_h.Unit_Struct);

   -- Copies Element to a new element on the heap and links that to the top of the list.
   -- Raises Usage_Error if a element with this ID has already been pushed:
   -- LEAKS:
   procedure Push
     (This    : access Class;
      Element : in     a_nodes_h.Element_Struct);

   -- Adds one to the count of not-implemented nodes encountered:
   procedure Add_Not_Implemented
     (This : access Class);

   -- Returns pointers to the nodes at the top of the lists:
   function Get_Nodes
     (This : access Class)
      return a_nodes_h.Nodes_Struct;

   procedure Print_Stats
     (This : access Class);

   -- A client has used the package improperly:
   Usage_Error : exception;

private

   package AC renames Ada.Containers;
   package IC renames Interfaces.C;

   function Hash (Item : IC.int) return AC.Hash_Type is
      (AC.Hash_Type (Item));

   package Element_ID_Sets is new AC.Hashed_Sets
     (Element_Type        => a_nodes_h.Element_ID,
      Hash                => Hash,
      Equivalent_Elements => IC."=",
      "="                 => IC."=");
   -- Make primitive operations directly visible:
   Type Element_ID_Set is new Element_ID_Sets.Set with null record;

   package Unit_ID_Sets is new AC.Hashed_Sets
     (Element_Type        => a_nodes_h.Unit_ID,
      Hash                => Hash,
      Equivalent_Elements => IC."=",
      "="                 => IC."=");
   -- Make primitive operations directly visible:
   Type Unit_ID_Set is new Unit_ID_Sets.Set with null record;

   type Class is tagged record -- Initialized
      Nodes              : a_nodes_h.Nodes_Struct; -- Initialized
      Has_Context        : Boolean := False;
      Unit_IDs           : Unit_ID_Set; -- Initialized
      Element_IDs        : Element_ID_Set; -- Initialized
      Highest_Unit_ID    : a_nodes_h.Unit_ID := a_nodes_h.Support.Invalid_Unit_ID;
      Highest_Element_ID : a_nodes_h.Element_ID := a_nodes_h.Support.Invalid_Element_ID;
      Not_Implemented    : Natural := 0;
   end record;

end A_Nodes;
