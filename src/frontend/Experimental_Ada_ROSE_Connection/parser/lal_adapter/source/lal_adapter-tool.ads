-- This is the main Lal_Adapter class.

private with Lal_Adapter.Context;
with a_nodes_h;

package Lal_Adapter.Tool is

   type Class is tagged limited private; -- Initialized

   Use_Current_Dir : constant String := "";

   -- Runs in the current directory.
   -- Creates .dot file in Output_Dir_Name.  If Output_Dir_Name = "", uses
   -- current directory.
   --
   -- LEAKS. Only intended to be called once per program execution:
   procedure Process
     (This                         : in out Class;
      Project_File_Name            : in     String;
      Input_File_Name              : in     String;
      Output_Dir_Name              : in     String := Use_Current_Dir;
      Process_Predefined_Units     : in     Boolean;
      Process_Implementation_Units : in     Boolean;
      Debug                        : in     Boolean);

   -- Call Process first:
   function Get_Nodes
     (This      : in out Class)
      return a_nodes_h.Nodes_Struct;

private
   -- For debuggng:
   Parent_Name : constant String := Module_Name;
   Module_Name : constant String := Parent_Name & ".Tool";

   type Class is new Lal_Adapter.Class with -- Initialized
      record
         My_Context : Lal_Adapter.Context.Class; -- Initialized
      end record;

end Lal_Adapter.Tool;
