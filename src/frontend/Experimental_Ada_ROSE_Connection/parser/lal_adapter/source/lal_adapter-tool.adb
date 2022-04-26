with Ada.Directories;

with Generic_Logging;

package body Lal_Adapter.Tool is

   ------------
   -- EXPORTED:
   ------------
   -- LEAKS (only intended to be called once per program execution):
   procedure Process
     (This                         : in out Class;
      Project_File_Name            : in     String;
      Input_File_Name              : in     String;
      Output_Dir_Name              : in     String := Use_Current_Dir;
      Process_Predefined_Units     : in     Boolean;
      Process_Implementation_Units : in     Boolean;
      Debug                        : in     Boolean)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      package AD renames Ada.Directories;

      Full_Input_File_Name  : constant String := AD.Full_Name (Input_File_Name);
      Source_File_Dir       : constant String := AD.Containing_Directory (Full_Input_File_Name);
      Simple_File_Name      : aliased  String := AD.Simple_Name (Full_Input_File_Name);
      Full_Output_Dir_Name  : constant String :=
        (if Output_Dir_Name = Use_Current_Dir then 
            AD.Current_Directory 
         else 
            AD.Full_Name (Output_Dir_Name));

      --  Unit_Options : Unit.Options_Record; -- Initialized

   begin -- Process
      Lal_Adapter.Trace_On := Debug;
      Log ("Project_File_Name    => """ & Project_File_Name & """");
      Log ("Input_File_Name      => """ & Input_File_Name & """");
      Log ("Output_Dir_Name      => """ & Output_Dir_Name & """");
      Log ("Current directory    => """ & Ada.Directories.Current_Directory & """");
      Log ("Full_Input_File_Name => """ & Full_Input_File_Name & """");
      Log ("Full_Output_Dir_Name => """ & Full_Output_Dir_Name & """");

      if Input_File_Name = "" then
         raise Usage_Error with "Input_File_Name must be provided, was empty.";
      end if;
     
      --  Unit_Options.Process_If_Origin_Is (Lal.A_Predefined_Unit) :=
      --    Process_Predefined_Units;
      --  Unit_Options.Process_If_Origin_Is (Lal.An_Implementation_Unit) :=
      --    Process_Implementation_Units;
      This.Outputs.Initialize;
      --     This.My_Context.Process (Unit_Options   => Unit_Options,
      --                              Outputs        => This.Outputs);
      This.My_Context.Process (Input_File_Name   => Input_File_Name,
                               Project_File_Name => Project_File_Name,
                               Outputs           => This.Outputs);
      This.Outputs.Graph.Write_File
        (Full_Output_Dir_Name & '/' & Simple_File_Name);
      This.Outputs.A_Nodes.Print_Stats;
   exception
      when X : External_Error | Internal_Error | Usage_Error =>
         raise;
      when X: others =>
         Logging.Log_Exception (X);
         Logging.Log ("No handler for this exception.  Raising Internal_Error");
         raise Internal_Error;
   end Process;

   ------------
   -- EXPORTED:
   ------------
   function Get_Nodes
     (This      : in out Class)
      return a_nodes_h.Nodes_Struct
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Get_Nodes";
      package Logging is new Generic_Logging (Module_Name); use Logging;
   begin
      return This.Outputs.A_Nodes.Get_Nodes;
   exception
      when X : External_Error | Internal_Error | Usage_Error =>
         raise;
      when X: others =>
         Logging.Log_Exception (X);
         Logging.Log ("No handler for this exception.  Raising Internal_Error");
         raise Internal_Error;
   end Get_Nodes;

end Lal_Adapter.Tool;
