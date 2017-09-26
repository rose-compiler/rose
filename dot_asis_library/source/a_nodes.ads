with a_nodes_h;

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
      Node : in     a_nodes_h.Node_Struct);

   -- Returns a pointer to the node at the top of the list:
   function Get_Head
     (This : access Class)
      return a_nodes_h.Node_List_Ptr;

   function Is_Empty
     (This : access Class)
      return Boolean;

   -- A client has used the package improperly:
   Usage_Error : exception;

private

   package AC renames Ada.Containers;
   package IC renames Interfaces.C;

   function Hash (Element : a_nodes_h.Node_ID) return AC.Hash_Type is
      (AC.Hash_Type (Element));

   package Node_ID_Sets is new AC.Hashed_Sets
     (Element_Type        => a_nodes_h.Node_ID,
      Hash                => Hash,
      Equivalent_Elements => IC."=",
      "="                 => IC."=");
   -- Make primitive operations directly visible:
   Type Node_ID_Set is new Node_ID_Sets.Set with null record;

   type Class is tagged record -- Initialized
      Head        : a_nodes_h.Node_List_Ptr; -- Initialized
      Has_Context : Boolean := False;
      Unit_IDs    : Node_ID_Set; -- Initialized
      Element_IDs : Node_ID_Set; -- Initialized
   end record;

end A_Nodes;
