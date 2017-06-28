with a_nodes_h;

-- This class hides and manages some of the details of the liniked
-- list of Element_Struct C interface records. Like Dot.ads, you only need to
-- pass in Access_Class to the users - they don't need to pass it back.  The top
-- user declares the actual object.

package A_Nodes is

   type Class is tagged private;
   type Access_Class is access Class;

   procedure Push
     (This : access Class;
      Node : in a_nodes_h.Element_Struct);

   function Get_Head
     (This : access Class)
      return a_nodes_h.Element_Struct_Ptr;

   function Is_Empty
     (This : access Class)
     return Boolean;

private

   type Class is tagged record
      Element : a_nodes_h.Element_Struct_Ptr; -- Initialized
   end record;

end A_Nodes;
