with A_Nodes;
with Dot;
private with Lal_Adapter.Node;

--  with Lal_Adapter.Unit;

package Lal_Adapter.Context is

   type Class is tagged limited private;

   procedure Process
     (This              : in out Class;
      Input_File_Name   : in     String;
      Project_File_Name : in     String;
      --  Unit_Options   : in     Unit.Options_Record;
      Outputs        : in     Output_Accesses_Record);

private

   -- For debuggng:
   Parent_Name : constant String := Module_Name;
   Module_Name : constant String := Parent_Name & ".Context";

   type Class is new Lal_Adapter.Class with -- Initialized
      record
         Lal_Context          : LAL.Analysis_Context := LAL.No_Analysis_Context;
         Top_Unit             : LAL.Analysis_Unit    := LAL.No_Analysis_Unit;
         Top_Compilation_Unit : LAL.Compilation_Unit := LAL.No_Compilation_Unit;
         Nodes                : Lal_Adapter.Node.Class; -- Initialized
      end record;

end Lal_Adapter.Context;
