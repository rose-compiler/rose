with Ada.Text_IO;
with Dot;

procedure Demo_Dot is

   package ATI renames Ada.Text_IO;

   -- Support subprograms:

   procedure Print (Graph : in Dot.Graphs.Access_Class;
                   Title  : in String) is
   begin
      ATI.Put_Line ("// " & Title & ":");
      ATI.Put_Line ("//");
      Graph.Put;
      ATI.New_Line;
      ATI.New_Line;
   end;

   function Create_Graph return Dot.Graphs.Access_Class is
   begin
      return Dot.Graphs.Create (Is_Digraph => True,
                                Is_Strict  => False);
   end;

   -- Demo subprograms:

   procedure Demo_Default_Graph is
      Graph : Dot.Graphs.Access_Class := new Dot.Graphs.Class; -- Initialized
   begin
      Print (Graph, "Default Graph");
   end;

   procedure Demo_Graph is
      Graph : Dot.Graphs.Access_Class := Create_Graph;
   begin
      Graph.Set_Is_Digraph (False);
      Graph.Set_Is_Strict (False);
      Print (Graph, "Graph");
   end;

   procedure Demo_Digraph is
      Graph : Dot.Graphs.Access_Class := Create_Graph;
   begin
      Graph.Set_Is_Digraph (True);
      Graph.Set_Is_Strict (False);
      Print (Graph, "Digraph");
   end;

   procedure Demo_Strict_Graph is
      Graph : Dot.Graphs.Access_Class := Create_Graph;
   begin
      Graph.Set_Is_Digraph (False);
      Graph.Set_Is_Strict (True);
      Print (Graph, "Strict Graph");
   end;

   procedure Demo_Strict_Digraph is
      Graph : Dot.Graphs.Access_Class := Create_Graph;
   begin
      Graph.Set_Is_Digraph (True);
      Graph.Set_Is_Strict (True);
      Print (Graph, "Strict Digraph");
   end;

   procedure Demo_ID is
      Graph : Dot.Graphs.Access_Class := Create_Graph;
   begin
      Graph.Set_ID ("Some_ID");
      Print (Graph, "Graph with ID");
   end;

   procedure Demo_Reserved_ID is
      Graph : Dot.Graphs.Access_Class := Create_Graph;
   begin
      Graph.Set_ID ("graph");
      Print (Graph, "Graph with reserved word as ID");
   end;

   procedure Demo_Minimal_Node is
      Graph       : Dot.Graphs.Access_Class := Create_Graph;
      Node_Stmt_1 : Dot.Node_Stmt.Class; -- Initialized
   begin
      Node_Stmt_1.Node_ID.ID := Dot.To_ID_Type ("minimal_node");
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_1));
      Print (Graph, "One minimal node");
   end;


   procedure Demo_Two_Nodes is
      Graph       : Dot.Graphs.Access_Class := Create_Graph;
      Node_Stmt_1 : Dot.Node_Stmt.Class; -- Initialized
      Node_Stmt_2 : Dot.Node_Stmt.Class; -- Initialized
   begin
      Node_Stmt_1.Node_ID.ID := Dot.To_ID_Type ("minimal_node_1");
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_1));
      Node_Stmt_2.Node_ID.ID := Dot.To_ID_Type ("minimal_node_2");
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_2));
      Print (Graph, "Two minimal nodes");
   end;

   procedure Demo_One_Attr_Two_Assigns is
      Graph       : Dot.Graphs.Access_Class := Create_Graph;
      Node_Stmt_2 : Dot.Node_Stmt.Class; -- Initialized
      Attr_1      : Dot.Attr.Class; -- Initialized
   begin
      Node_Stmt_2.Node_ID.ID := Dot.To_ID_Type ("node_with_attr");
      Attr_1.A_List.Append ("Today", "Thursday");
      Attr_1.A_List.Append ("Tomorrow", "Friday");
      Node_Stmt_2.Attr_List.Append (Attr_1);
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_2));
      Print (Graph, "One node, one attr, two assigns");
   end;

   procedure Demo_Two_Attrs_One_Assign_Each is
      Graph       : Dot.Graphs.Access_Class := Create_Graph;
      Node_Stmt_2 : Dot.Node_Stmt.Class; -- Initialized
      Attr_1      : Dot.Attr.Class; -- Initialized
      Attr_2      : Dot.Attr.Class; -- Initialized
   begin
      Node_Stmt_2.Node_ID.ID := Dot.To_ID_Type ("node_with_attrs");
      Attr_1.A_List.Append ("Today", "Thursday");
      Node_Stmt_2.Attr_List.Append (Attr_1);
      Attr_2.A_List.Append ("Now", "Day");
      Node_Stmt_2.Attr_List.Append (Attr_2);
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_2));
      Print (Graph, "One node, two attrs, one assign each");
   end;

   procedure Demo_Two_Attrs_No_Assigns is
      Graph         : Dot.Graphs.Access_Class := Create_Graph;
      Node_Stmt_2   : Dot.Node_Stmt.Class; -- Initialized
      Attr_1 : Dot.Attr.Class; -- Initialized
      Attr_2 : Dot.Attr.Class; -- Initialized
   begin
      Node_Stmt_2.Node_ID.ID := Dot.To_ID_Type ("node_with_attrs");
      Node_Stmt_2.Attr_List.Append (Attr_1);
      Node_Stmt_2.Attr_List.Append (Attr_2);
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_2));
      Print (Graph, "One node, two attrs, no assigns");
   end;

   procedure Demo_Two_Nodes_Two_Attrs_Two_Assigns is
      Graph         : Dot.Graphs.Access_Class := Create_Graph;
      Node_Stmt_1   : Dot.Node_Stmt.Class; -- Initialized
      Node_Stmt_2   : Dot.Node_Stmt.Class; -- Initialized
      Attr_1 : Dot.Attr.Class; -- Initialized
      Attr_2 : Dot.Attr.Class; -- Initialized
      Attr_3 : Dot.Attr.Class; -- Initialized
      Attr_4 : Dot.Attr.Class; -- Initialized
   begin
      Node_Stmt_1.Node_ID.ID := Dot.To_ID_Type ("node_1");
      Attr_1.A_List.Append ("Today", "Thursday");
      Attr_1.A_List.Append ("Tomorrow", "Friday");
      Node_Stmt_1.Attr_List.Append (Attr_1);
      Attr_2.A_List.Append ("Now", "Day");
      Attr_2.A_List.Append ("Later", "Night");
      Node_Stmt_1.Attr_List.Append (Attr_2);
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_1));

      Node_Stmt_2.Node_ID.ID := Dot.To_ID_Type ("node_2");
      Attr_3.A_List.Append ("Today", "Thursday");
      Attr_3.A_List.Append ("Yesterday", "Wednesday");
      Node_Stmt_2.Attr_List.Append (Attr_3);
      Attr_4.A_List.Append ("Now", "Day");
      Attr_4.A_List.Append ("Before", "Morning");
      Node_Stmt_2.Attr_List.Append (Attr_4);
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_2));
      Print (Graph, "Two nodes, two attrs, two assigns");
   end;

   procedure Demo_Add_Assign_To_First_Attr is
      Graph       : Dot.Graphs.Access_Class := Create_Graph;
      Node_Stmt_2 : Dot.Node_Stmt.Class; -- Initialized
   begin
      Node_Stmt_2.Node_ID.ID := Dot.To_ID_Type ("node_with_attr");
      Node_Stmt_2.Attr_List.Add_Assign_To_First_Attr ("Today", "Thursday");
      Node_Stmt_2.Attr_List.Add_Assign_To_First_Attr  ("Tomorrow", "Friday");
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node_Stmt_2));
      Print (Graph, "One node, one attr, two assigns, using Add_Assign_To_First_Attr");
   end;

   procedure Demo_Default_Edge is
      Graph     : Dot.Graphs.Access_Class := Create_Graph;
      Edge_Stmt : Dot.Edges.Stmts.Class; -- Initialized
   begin
      Graph.Append_Stmt (new Dot.Edges.Stmts.Class'(Edge_Stmt));
      Print (Graph, "One default edge");
   end;

   procedure Demo_One_Edge is
      Graph     : Dot.Graphs.Access_Class := Create_Graph;
      Edge_Stmt : Dot.Edges.Stmts.Class; -- Initialized
   begin
      Edge_Stmt.LHS.Node_Id.ID := Dot.To_ID_Type ("A");
      Edge_Stmt.RHS.Node_Id.ID := Dot.To_ID_Type ("B");
      Graph.Append_Stmt (new Dot.Edges.Stmts.Class'(Edge_Stmt));
      Print (Graph, "One edge stmt");
   end;

   procedure Demo_Two_Edges is
      Graph      : Dot.Graphs.Access_Class := Create_Graph;
      Edge_Stmts : array (1..2) of Dot.Edges.Stmts.Class; -- Initialized
   begin
      Edge_Stmts (1).LHS.Node_Id.ID := Dot.To_ID_Type ("A");
      Edge_Stmts (1).RHS.Node_Id.ID := Dot.To_ID_Type ("B");
      Graph.Append_Stmt (new Dot.Edges.Stmts.Class'(Edge_Stmts (1)));
      Edge_Stmts (2).LHS.Node_Id.ID := Dot.To_ID_Type ("C");
      Edge_Stmts (2).RHS.Node_Id.ID := Dot.To_ID_Type ("D");
      Graph.Append_Stmt (new Dot.Edges.Stmts.Class'(Edge_Stmts (2)));
      Print (Graph, "Two edge stmts");
   end;


begin
   Demo_Default_Graph;
   Demo_Graph;
   Demo_Digraph;
   Demo_Strict_Graph;
   Demo_Strict_Digraph;
   Demo_ID;
   Demo_Reserved_ID;

   Demo_Minimal_Node;
   Demo_Two_Nodes;
   Demo_One_Attr_Two_Assigns;
   Demo_Two_Attrs_One_Assign_Each;
   Demo_Two_Attrs_No_Assigns;
   Demo_Two_Nodes_Two_Attrs_Two_Assigns;
   Demo_Add_Assign_To_First_Attr;

   Demo_Default_Edge;
   Demo_One_Edge;
   Demo_Two_Edges;
end Demo_Dot;
