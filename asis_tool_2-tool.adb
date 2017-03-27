with Asis.Implementation;

package body Asis_Tool_2.Tool is

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This : in out Class)
   is
   begin
      Asis.Implementation.Initialize;
      This.Graph := new Dot.Graphs.Class;
      This.Graph.Set_Is_Digraph (True);
      This.Graph.Set_Is_Strict (False);
      This.My_Context.Process (Graph => This.Graph);
      This.Graph.Print;
      Asis.Implementation.Finalize;
   end Process;

end Asis_Tool_2.Tool;
