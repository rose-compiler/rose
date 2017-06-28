with Asis.Implementation;
with Ada.Text_IO;

package body Asis_Tool_2.Tool is

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This      : in out Class;
      File_Name : in     String) is
   begin
      Asis.Implementation.Initialize;
      This.Graph := Dot.Graphs.Create (Is_Digraph => True,
                                       Is_Strict  => False);
      This.A_Node_List := new A_Nodes.Class;
      This.My_Context.Process (Graph       => This.Graph,
                               A_Node_List => This.A_Node_List);
      Awti.New_Line;
      This.Graph.Write_File (File_Name);
      Asis.Implementation.Finalize;
   end Process;

   ------------
   -- EXPORTED:
   ------------
   function Get_Elements
     (This      : in out Class)
      return a_nodes_h.Element_Struct_Ptr is
   begin
      return This.A_Node_List.Get_Head;
   end Get_Elements;

end Asis_Tool_2.Tool;
