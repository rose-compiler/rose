with a_nodes_h;

-- This class hides and manages some of the details of the liniked
-- list of Element_Struct C interface records.  Like Dot.ads, you only need to
-- pass in Access_Class to the users - they don't need to pass it back.  The top
-- caller declares the actual Class object.

package A_Nodes is

   type Class is tagged private;
   type Access_Class is access Class;

   -- Copies Node to a new node on the heap and links it to the top of the list:
   procedure Push
     (This : access Class;
      Node : in a_nodes_h.Node_Struct);

   -- Returns a pointer to the node at the top of the list:
   function Get_Head
     (This : access Class)
      return a_nodes_h.Node_List_Ptr;

   function Is_Empty
     (This : access Class)
     return Boolean;

private

   type Class is tagged record
      Head : a_nodes_h.Node_List_Ptr; -- Initialized
   end record;

end A_Nodes;
