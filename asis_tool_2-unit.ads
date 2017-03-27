with Asis;

with Dot;

package Asis_Tool_2.Unit is

   type Class (Trace : Boolean := False) is tagged limited private;

   procedure Process
     (This      : in out Class;
      Asis_Unit : in     Asis.Compilation_Unit;
      Graph     : in     Dot.Graphs.Access_Class);

private

   type Class (Trace : Boolean := False) is tagged limited
      record
         Asis_Unit : Asis.Compilation_Unit := Asis.Nil_Compilation_Unit;
         Graph   : Dot.Graphs.Access_Class; -- Initialized
      end record;

   procedure Process_Context_Clauses
     (This            : in out Class;
      Asis_Unit       : in Asis.Compilation_Unit;
      Include_Pragmas : in Boolean := True);

end Asis_Tool_2.Unit;
