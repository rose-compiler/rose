-- This is the main Asis_Tool_2 class.

private with Asis_Tool_2.Context;
private with Dot;
with A_Nodes;
with a_nodes_h;

package Asis_Tool_2.Tool is

   type Class is tagged limited private;

   -- Runs in the current directory.
   -- Uses project file "default.gpr" in containing directory of File_Name.
   -- Creates .adt file in project file Object_Dir.
   -- Creates .dot file in Output_Dir.  If Output_Dir = "", uses current directory.
   --
   -- LEAKS. Only intended to be called once per program execution:
   procedure Process
     (This       : in out Class;
      File_Name  : in     String;
      Output_Dir : in     String := "";
      GNAT_Home  : in     String;
      Debug      : in     Boolean);

   -- Call Process first:
   function Get_Nodes
     (This      : in out Class)
      return a_nodes_h.Nodes_Struct;

private

   type Class is tagged limited -- Initialized
      record
         My_Context : Asis_Tool_2.Context.Class; -- Initialized
         Outputs    : Outputs_Record; -- Initialized
      end record;

end Asis_Tool_2.Tool;
