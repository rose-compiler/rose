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
      Node_Stmt_1 : Dot.Node_Stmt.Class; -- Initialized
      Node_Stmt_2 : Dot.Node_Stmt.Class; -- Initialized
      Assignment_List_1 : Dot.Assignment.List; -- Initialized
      Assignment_List_2 : Dot.Assignment.List; -- Initialized
   begin
      Graph.Digraph := False;
      Graph.Strict := False;

      Node_Stmt_1.Node_ID.ID := Dot.To_ID_Type ("minimal_node");
      Node_Stmt_1.Append_To(Graph.Stmt_List);
--        Graph.Stmt_List.Append (new Dot.Node_Stmt.Class'(Node_Stmt_1));

      Node_Stmt_2.Node_ID.ID := Dot.To_ID_Type ("node_with_attrs");
      Assignment_List_1.Append ((L => Dot.To_ID_Type ("Today"),
                                 R => Dot.To_ID_Type ("Thursday")));
      Assignment_List_1.Append ((L => Dot.To_ID_Type ("Tomorrow"),
                                 R => Dot.To_ID_Type ("Friday")));
      Node_Stmt_2.Attrs.Append (Assignment_List_1);
      Assignment_List_2.Append ((L => Dot.To_ID_Type ("Now"),
                                 R => Dot.To_ID_Type ("Day")));
      Assignment_List_2.Append ((L => Dot.To_ID_Type ("Later"),
                                 R => Dot.To_ID_Type ("Night")));
      Node_Stmt_2.Attrs.Append (Assignment_List_2);
      Graph.Stmt_List.Append (new Dot.Node_Stmt.Class'(Node_Stmt_2));
      Print (Graph, "Graph with " & Graph.Stmt_List.Length'Image & " nodes");
   end;

begin
   Demo_Graph;
   Demo_Digraph;
   Demo_Strict_Graph;
   Demo_Strict_Digraph;
   Demo_ID;

   Demo_Nodes;
end Demo_Dot;
