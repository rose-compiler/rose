with Asis;

with A_Nodes;
with Dot;

package Asis_Tool_2.Unit is

   type Class (Trace : Boolean := False) is tagged limited private; -- Initialized

   procedure Process
     (This      : in out Class;
      Asis_Unit : in     Asis.Compilation_Unit;
      Outputs   : in     Output_Accesses_Record);

private

   type Class (Trace : Boolean := False) is tagged limited
      record
         Asis_Unit : Asis.Compilation_Unit := Asis.Nil_Compilation_Unit;
      end record;

end Asis_Tool_2.Unit;
