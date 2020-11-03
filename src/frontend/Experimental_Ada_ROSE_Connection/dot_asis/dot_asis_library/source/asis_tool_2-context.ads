with Asis;

with A_Nodes;
with Dot;

with Asis_Tool_2.Unit;

package Asis_Tool_2.Context is

   type Class is tagged limited private;

   procedure Process
     (This           : in out Class;
      Tree_File_Name : in     String;
      Unit_Options   : in     Unit.Options_Record;
      Outputs        : in     Outputs_Record);

private

   -- For debuggng:
   Parent_Name : constant String := Module_Name;
   Module_Name : constant String := Parent_Name & ".Context";

   type Class is tagged limited -- Initialized
      record
         Asis_Context : Asis.Context; -- Initialized
      end record;

end Asis_Tool_2.Context;
