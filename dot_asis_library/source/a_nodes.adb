-- For int."+":
with Interfaces.C;

package body A_Nodes is

   ------------
   -- EXPORTED:
   ------------
   procedure Push
     (This : access Class;
      Node : in a_nodes_h.Node_Struct)
   is
      List_Node : a_nodes_h.List_Node_Struct;
      use type Interfaces.C.int;
   begin
      List_Node.node := Node;
      if This.Is_Empty then
         List_Node.next := null;
         List_Node.next_count := 0;
      else
         List_Node.next := This.Head;
         List_Node.next_count := This.Head.next_count + 1;
      end if;
      This.Head := new a_nodes_h.List_Node_Struct'(List_Node);
   end Push;

   ------------
   -- EXPORTED:
   ------------
   function Get_Head
     (This : access Class)
      return a_nodes_h.Node_List_Ptr is
   begin
      return This.Head;
   end Get_Head;

   ------------
   -- EXPORTED:
   ------------
   function Is_Empty
     (This : access Class)
      return Boolean
   is
      use type a_nodes_h.Node_List_Ptr;
   begin
      return This.Head = null;
   end Is_Empty;

end A_Nodes;
