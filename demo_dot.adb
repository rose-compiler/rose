with Dot.Graph;

procedure Demo_Dot is
   Graph : Dot.Graph.Class (Digraph => True);
begin
   Graph.Print;

   Graph.Strict := False;
   Graph.Print;

   Graph.ID := Dot.To_ID_Type ("Some_ID");
   Graph.Print;

   declare
      Graph : Dot.Graph.Class (Digraph => False);
   begin
      Graph.print;
   end;

end Demo_Dot;
