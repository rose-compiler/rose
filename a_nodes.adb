-- For int."+":
with Interfaces.C;

package body A_Nodes is

   ------------
   -- EXPORTED:
   ------------
   procedure Push
     (This : access Class;
      Node : in a_nodes_h.Element_Struct)
   is
      Local_Node : a_nodes_h.Element_Struct := Node;
      use type Interfaces.C.int;
   begin
      if This.Is_Empty then
         Local_Node.next := null;
         Local_Node.next_count := 0;
      else
         Local_Node.next := This.Element;
         Local_Node.next_count := This.Element.next_count + 1;
      end if;
      This.Element := new a_nodes_h.Element_Struct'(Local_Node);
   end Push;

   ------------
   -- EXPORTED:
   ------------
   function Get_Head
     (This : access Class)
      return a_nodes_h.Element_Struct_Ptr is
   begin
      return This.Element;
   end Get_Head;

   ------------
   -- EXPORTED:
   ------------
   function Is_Empty
     (This : access Class)
      return Boolean
   is
      use type a_nodes_h.Element_Struct_Ptr;
   begin
      return This.Element = null;
   end Is_Empty;

end A_Nodes;
