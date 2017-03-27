with Ada.Text_IO;
with Dot;

procedure Demo_Dot is

   package ATI renames Ada.Text_IO;

   procedure Print (Graph : in Dot.Graphs.Access_Class;
                   Title  : in String) is
   begin
      ATI.Put_Line ("// " & Title & ":");
      ATI.Put_Line ("//");
      Graph.Print;
      ATI.New_Line;
   end;

   procedure Demo_Default_Graph is
      Graph : Dot.Graphs.Access_Class := new Dot.Graphs.Class;
   begin
      Print (Graph, "Default Graph");
   end;

   procedure Demo_Graph is
      Graph : Dot.Graphs.Access_Class := new Dot.Graphs.Class;
   begin
      Graph.Set_Is_Digraph (False);
      Graph.Set_Is_Strict (False);
      Print (Graph, "Graph");
   end;

   procedure Demo_Digraph is
      Graph : Dot.Graphs.Access_Class := new Dot.Graphs.Class;
   begin
      Graph.Set_Is_Digraph (True);
      Graph.Set_Is_Strict (False);
      Print (Graph, "Digraph");
   end;

   procedure Demo_Strict_Graph is
      Graph : Dot.Graphs.Access_Class := new Dot.Graphs.Class;
   begin
      Graph.Set_Is_Digraph (False);
      Graph.Set_Is_Strict (True);
      Print (Graph, "Strict Graph");
   end;

   procedure Demo_Strict_Digraph is
      Graph : Dot.Graphs.Access_Class := new Dot.Graphs.Class;
   begin
      Graph.Set_Is_Digraph (True);
      Graph.Set_Is_Strict (True);
      Print (Graph, "Strict Digraph");
   end;

   procedure Demo_ID is
      Graph : Dot.Graphs.Access_Class := new Dot.Graphs.Class;
   begin
      Graph.Set_Is_Digraph (False);
      Graph.Set_Is_Strict (False);
      Graph.Set_ID ("Some_ID");
      Print (Graph, "Graph with ID");
   end;

   procedure Demo_Reserved_ID is
      Graph : Dot.Graphs.Access_Class := new Dot.Graphs.Class;
   begin
      Graph.Set_Is_Digraph (False);
      Graph.Set_Is_Strict (False);
      Graph.Set_ID ("graph");
      Print (Graph, "Graph with reserved word as ID");
   end;

   procedure Demo_Nodes is
      Graph : Dot.Graphs.Access_Class := new Dot.Graphs.Class;
      Node_Stmt_1 : Dot.Node_Stmt.Class; -- Initialized
      Node_Stmt_2 : Dot.Node_Stmt.Class; -- Initialized
      Assignment_List_1 : Dot.Assignment.List; -- Initialized
      Assignment_List_2 : Dot.Assignment.List; -- Initialized
   begin
      Graph.Set_Is_Digraph (False);
      Graph.Set_Is_Strict (False);

      Node_Stmt_1.Node_ID.ID := Dot.To_ID_Type ("minimal_node");
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_1));

      Node_Stmt_2.Node_ID.ID := Dot.To_ID_Type ("node_with_attrs");
      Assignment_List_1.Append ("Today", "Thursday");
      Assignment_List_1.Append ("Tomorrow", "Friday");
      Node_Stmt_2.Attrs.Append (Assignment_List_1);
      Assignment_List_2.Append ("Now", "Day");
      Assignment_List_2.Append ("Later", "Night");
      Node_Stmt_2.Attrs.Append (Assignment_List_2);
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_2));
      Print (Graph, "Graph with " & Graph.Stmt_Count'Image & " nodes");
   end;

begin
   Demo_Default_Graph;
   Demo_Graph;
   Demo_Digraph;
   Demo_Strict_Graph;
   Demo_Strict_Digraph;
   Demo_ID;
   Demo_Reserved_ID;

   Demo_Nodes;
end Demo_Dot;
