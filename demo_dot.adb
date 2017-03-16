with Ada.Text_IO;
with Dot;

procedure Demo_Dot is

   package ATI renames Ada.Text_IO;

   procedure Print (Graph : in Dot.Graph.Class;
                   Title  : in String) is
   begin
      ATI.Put_Line ("// " & Title & ":");
      ATI.Put_Line ("//");
      Graph.Print;
      ATI.New_Line;
   end;

   procedure Demo_Default_Graph is
      Graph : Dot.Graph.Class;
   begin
      Print (Graph, "Default Graph");
   end;

   procedure Demo_Graph is
      Graph : Dot.Graph.Class;
   begin
      Graph.Digraph := False;
      Graph.Strict := False;
      Print (Graph, "Graph");
   end;

   procedure Demo_Digraph is
      Graph : Dot.Graph.Class;
   begin
      Graph.Digraph := True;
      Graph.Strict := False;
      Print (Graph, "Digraph");
   end;

   procedure Demo_Strict_Graph is
      Graph : Dot.Graph.Class;
   begin
      Graph.Digraph := False;
      Graph.Strict := True;
      Print (Graph, "Strict Graph");
   end;

   procedure Demo_Strict_Digraph is
      Graph : Dot.Graph.Class;
   begin
      Graph.Digraph := True;
      Graph.Strict := True;
      Print (Graph, "Strict Digraph");
   end;

   procedure Demo_ID is
      Graph : Dot.Graph.Class;
   begin
      Graph.Digraph := False;
      Graph.Strict := False;
      Graph.ID := Dot.To_ID_Type ("Some_ID");
      Print (Graph, "Graph with ID");
   end;

   procedure Demo_Nodes is
      Graph : Dot.Graph.Class;
   begin
      Graph.Digraph := False;
      Graph.Strict := False;
--        Graph.Add_Node (Node => Node_Info);
      Print (Graph, "Graph with nodes");
   end;

begin
   Demo_Graph;
   Demo_Digraph;
   Demo_Strict_Graph;
   Demo_Strict_Digraph;
   Demo_ID;

   Demo_Nodes;
end Demo_Dot;
