package Dot.Graph is

   -- Using the exact spellings from the grammar for the record components
   -- instead of spelling things out:
   type Class (Digraph : Boolean := True) is tagged limited
      record
         Strict : Boolean := True;
         ID     : ID_Type; -- Initialized
--           stmt_list : stmt_list_type;
      end record;

   procedure Print (This : in Class);


end Dot.Graph;
