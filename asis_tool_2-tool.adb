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
      This.My_Context.Process (Graph => This.Graph);
      Awti.New_Line;
      This.Graph.Write_File (File_Name);
      Asis.Implementation.Finalize;
   end Process;

end Asis_Tool_2.Tool;
