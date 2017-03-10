with Asis.Elements;

with Element_Processing;

package body Unit_Processing is

   procedure Process_Unit
     (The_Unit : Asis.Compilation_Unit;
      Do_Context_Clauses : Boolean := True)
   is
      Top_Element : Asis.Element := Asis.Elements.Unit_Declaration (The_Unit);
   begin
      if Do_Context_Clauses then
         declare
            Context_Clauses : constant Asis.Element_List :=
              Asis.Elements.Context_Clause_Elements
                (Compilation_Unit => The_Unit,
                 Include_Pragmas  => True);
         begin
            for Context_Clause of Context_Clauses loop

               -- Do actual work:
               Element_Processing.Process_Construct (Context_Clause);

            end loop;
         end;
      end if;

      -- Do actual work:
      Element_Processing.Process_Construct (Top_Element);

      --  This procedure does not contain any exception handler because it
      --  supposes that Element_Processing.Process_Construct should handle
      --  all the exceptions which can be raised when processing the element
      --  hierarchy
   end Process_Unit;

end Unit_Processing;
