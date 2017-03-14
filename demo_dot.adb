with Ada.Text_IO;
with Dot.Graph;

procedure Demo_Dot is

   package ATI renames Ada.Text_IO;

   procedure Print (Graph : in Dot.Graph.Class) is
   begin
      ATI.New_Line;
      Graph.Print;
      ATI.New_Line;
   end;

   procedure Demo_Graph is
      Graph : Dot.Graph.Class (Digraph => False);
   begin
      ATI.Put_Line ("Graph:");
      Graph.Strict := False;
      Print (Graph);
   end;

   procedure Demo_Digraph is
      Graph : Dot.Graph.Class (Digraph => True);
   begin
      ATI.Put_Line ("Digraph:");
      Graph.Strict := False;
      Print (Graph);
   end;

   procedure Demo_Strict_Graph is
      Graph : Dot.Graph.Class (Digraph => False);
   begin
      ATI.Put_Line ("Strict Graph:");
      Graph.Strict := True;
      Print (Graph);
   end;

   procedure Demo_Strict_Digraph is
      Graph : Dot.Graph.Class (Digraph => True);
   begin
      ATI.Put_Line ("Strict Digraph:");
      Graph.Strict := True;
      Print (Graph);
   end;

   procedure Demo_ID is
      Graph : Dot.Graph.Class (Digraph => False);
   begin
      ATI.Put_Line ("Graph with ID:");
      Graph.Strict := False;
      Graph.ID := Dot.To_ID_Type ("Some_ID");
      Print (Graph);
   end;

   Graph : Dot.Graph.Class (Digraph => True);
begin
   Demo_Graph;
   Demo_Digraph;
   Demo_Strict_Graph;
   Demo_Strict_Digraph;
   Demo_ID;
end Demo_Dot;
