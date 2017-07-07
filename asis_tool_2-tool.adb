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
      This.Outputs.Graph := Dot.Graphs.Create (Is_Digraph => True,
                                       Is_Strict  => False);
      This.Outputs.A_Nodes := new A_Nodes.Class;
      -- TODO: use File_Name:
      This.My_Context.Process (This.Outputs);
      Awti.New_Line;
      This.Outputs.Graph.Write_File (File_Name);
      Asis.Implementation.Finalize;
   end Process;

   ------------
   -- EXPORTED:
   ------------
   function Get_Nodes
     (This      : in out Class)
      return a_nodes_h.Node_List_Ptr is
   begin
      return This.Outputs.A_Nodes.Get_Head;
   end Get_Nodes;

end Asis_Tool_2.Tool;
