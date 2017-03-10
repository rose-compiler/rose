with Asis;

package Asis_Tool_2.Context is

   type Class is tagged limited private;

   procedure Process (Object : in out Class);

private

   type Class is tagged limited
      record
         My_Context : Asis.Context;
      end record;

   -- Process all the compilation units (ADT files) in the context:
   procedure Process_Units (Object : in out Class);

end Asis_Tool_2.Context;
