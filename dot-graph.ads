package Dot.Graph is

   type Class (Digraph : Boolean := True;
               Strict  : Boolean := True) is tagged limited
      record
         ID        : ID_Type; -- Initialized
         Stmt_List : Stmt_List_Type;
      end record;

   procedure Print (This : in Class);

end Dot.Graph;
