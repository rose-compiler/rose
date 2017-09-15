with Asis;

with A_Nodes;
with Dot;

package Asis_Tool_2.Context is

   type Class is tagged limited private;

   procedure Process
     (This           : in out Class;
      Tree_File_Name : in     String;
      Outputs        : in     Outputs_Record);

private

   type Class is tagged limited -- Initialized
      record
         Asis_Context : Asis.Context; -- Initialized
      end record;

end Asis_Tool_2.Context;
