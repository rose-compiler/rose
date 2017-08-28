with Asis.Implementation;

package body Asis_Tool_2.Tool is

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This      : in out Class;
      File_Name : in     String;
      Debug     : in     Boolean)
   is
      procedure Log (Message : in String) is
      begin
         Put_Line ("Asis_Tool_2.Tool.Process:  " & message);
      end;
   begin
      Log ("BEGIN - File_Name => """ & File_Name & """");
      Asis_Tool_2.Trace_On := Debug;
      Asis.Implementation.Initialize;
      This.Outputs.Text := new Indented_Text.Class;
      This.Outputs.Graph := Dot.Graphs.Create (Is_Digraph => True,
                                       Is_Strict  => False);
      This.Outputs.A_Nodes := new A_Nodes.Class;
      -- TODO: use File_Name:
      This.My_Context.Process (This.Outputs);
      This.Outputs.Graph.Write_File (File_Name);
      Asis.Implementation.Finalize;
      Log ("END");
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
