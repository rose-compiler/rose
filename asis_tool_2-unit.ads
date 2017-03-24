with Asis;

package Asis_Tool_2.Unit is

   type Class (Trace : Boolean := False) is tagged limited private;

   procedure Set_Up
     (This      : in out Class;
      Asis_Unit : in     Asis.Compilation_Unit);

   procedure Process
     (This : in out Class);

private

   type Class (Trace : Boolean := False) is tagged limited
      record
         My_Unit : Asis.Compilation_Unit := Asis.Nil_Compilation_Unit;
      end record;

end Asis_Tool_2.Unit;
