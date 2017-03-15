with Ada.Text_IO;
with Dot;

procedure Demo_Dot is

   package ATI renames Ada.Text_IO;

   procedure Print (Graph : in Dot.Graph.Class) is
   begin
      ATI.New_Line;
      Graph.Print;
      ATI.New_Line;
   end;

   procedure Demo_Default_Graph is
      Graph : Dot.Graph.Class;
   begin
      ATI.Put_Line ("Default Graph:");
      Print (Graph);
   end;

   procedure Demo_Graph is
      Graph : Dot.Graph.Class (Digraph => False,
                               Strict => False);
   begin
      ATI.Put_Line ("Graph:");
      Print (Graph);
   end;

   procedure Demo_Digraph is
      Graph : Dot.Graph.Class (Digraph => True,
                               Strict => False);
   begin
      ATI.Put_Line ("Digraph:");
      Print (Graph);
   end;

   procedure Demo_Strict_Graph is
      Graph : Dot.Graph.Class (Digraph => False,
                               Strict => True);
   begin
      ATI.Put_Line ("Strict Graph:");
      Print (Graph);
   end;

   procedure Demo_Strict_Digraph is
      Graph : Dot.Graph.Class (Digraph => True,
                               Strict => True);
   begin
      ATI.Put_Line ("Strict Digraph:");
      Print (Graph);
   end;

   procedure Demo_ID is
      Graph : Dot.Graph.Class (Digraph => True,
                               Strict => False);
   begin
      ATI.Put_Line ("Graph with ID:");
      Graph.ID := Dot.To_ID_Type ("Some_ID");
      Print (Graph);
   end;

   procedure Demo_Nodes is
      Graph : Dot.Graph.Class (Digraph => False,
                               Strict => False);
   begin
      ATI.Put_Line ("Graph with nodes:");
--        Graph.Add_Node (Node => Node_Info);
      Print (Graph);
   end;

begin
   Demo_Graph;
   Demo_Digraph;
   Demo_Strict_Graph;
   Demo_Strict_Digraph;
   Demo_ID;

   Demo_Nodes;
end Demo_Dot;
