with Ada.Directories;
with Asis.Implementation;

-- GNAT specific:
with Asis.Extensions;
with Gnat.OS_Lib;

with Asis_Tool_2.Unit;

package body Asis_Tool_2.Tool is

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This                         : in out Class;
      File_Name                    : in     String;
      Output_Dir                   : in     String := "";
      GNAT_Home                    : in     String;
      Process_Predefined_Units     : in     Boolean;
      Process_Implementation_Units : in     Boolean;
      Debug                        : in     Boolean)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process";
      package Logging is new Generic_Logging (Module_Name); use Logging;

      package AD renames Ada.Directories;
      Full_File_Name   : constant String := AD.Full_Name (File_Name);
      Source_File_Dir  : constant String :=
        Ada.Directories.Containing_Directory (Full_File_Name);
      Simple_File_Name : aliased String := AD.Simple_Name (Full_File_Name);
      Base_File_Name   : constant String := AD.Base_Name (Simple_File_Name);

      Tree_File_Dir    : constant String := AD.Compose (AD.Current_Directory, "obj");
      Tree_File_Name   : constant String :=
        AD.Compose (Tree_File_Dir, Base_File_Name, "adt");
      Real_Output_Dir  : constant String :=
        (if Output_Dir = "" then AD.Current_Directory else Output_Dir);

      -- LEAKS (only intended to be called once per program execution):
      procedure Init_And_Process_Context is
         Unit_Options : Unit.Options_Record; -- Initialized
      begin
         -- -dall - All the ASIS-for-GNAT debug flags are set ON
         -- Asis.Implementation.Initialize (Parameters => "-dall");
         Asis.Implementation.Initialize;
         Unit_Options.Process_If_Origin_Is (Asis.A_Predefined_Unit) :=
           Process_Predefined_Units;
         Unit_Options.Process_If_Origin_Is (Asis.An_Implementation_Unit) :=
           Process_Implementation_Units;
         This.Outputs.Output_Dir := ASU.To_Unbounded_String (Real_Output_Dir);
         This.Outputs.Text := new Indented_Text.Class;
         This.Outputs.Graph := Dot.Graphs.Create (Is_Digraph => True,
                                                  Is_Strict  => False);
         This.Outputs.A_Nodes := new A_Nodes.Class;
         -- TODO: use File_Name:
         This.My_Context.Process (Tree_File_Name => Tree_File_Name,
                                  Unit_Options   => Unit_Options,
                                  Outputs        => This.Outputs);
         This.Outputs.Graph.Write_File
           (ASU.To_String (This.Outputs.Output_Dir) & '/' & Simple_File_Name);
         This.Outputs.A_Nodes.Print_Stats;
         Asis.Implementation.Finalize;
      end Init_And_Process_Context;

      Compile_Succeeded : Boolean := False;

      -- The aliased string for the Asis.Extensions.Compile GCC parm.  It
      -- should end in "/gcc" or "/gprbuild". When using gprbuild, there must be
      -- 1) A .gpr file in the current directory that covers Source_File, or
      --    -P<proj file> in Args
      -- 2) Use_GPRBUILD => True
      -- Using GNAT_Home to avoid calling the wrong (non-GNAT) gcc
      Compile_Command     : aliased String := GNAT_Home & "/bin/gprbuild";
      Create_Missing_Dirs : aliased String := "-p";
      Project_File        : aliased String :=
        "-P" & Source_File_Dir & "/default.gpr";
      Relocate_build_tree : aliased String := "--relocate-build-tree";
      -- From GNAT docs:
      -- "Args contains only needed -I, -gnatA, -gnatec options, and project
      -- file in case of the GPRBUILD call."
      -- Unchecked_Access is safe because Asis.Extensions.Compile does not use
      -- the pointed-to strings out of scope:
      GPRBUILD_Args       : Gnat.OS_Lib.Argument_List :=
        (1 => Create_Missing_Dirs'Unchecked_Access,
         2 => Project_File'Unchecked_Access,
         3 => Relocate_build_tree'Unchecked_Access);
   begin
      Asis_Tool_2.Trace_On := Debug;
      Log ("BEGIN");
      Log ("File_Name  => """ & File_Name & """");
      Log ("Output_Dir => """ & Output_Dir & """");
      Log ("GNAT_Home  => """ & GNAT_Home & """");
      if File_Name = "" then
         raise Usage_Error with "File_Name must be provided, was empty.";
      end if;
      if GNAT_Home = "" then
         raise Usage_Error with "GNAT_Home must be provided, was empty.";
      end if;
      Log ("Current directory is: """ & Ada.Directories.Current_Directory & """");
      Log ("Full_File_Name  => """ & Full_File_Name & """");
      Log ("Real_Output_Dir => """ & Real_Output_Dir & """");
      Log ("Calling Asis.Extensions.Compile");
      -- Unchecked_Access is safe because Asis.Extensions.Compile does not use
      -- the pointed-to strings out of scope:
      Asis.Extensions.Compile
        (Source_File           => Simple_File_Name'Unchecked_Access,
         Args                  => GPRBUILD_Args,
         Success               => Compile_Succeeded,
         GCC                   => Compile_Command'Unchecked_Access,
         Use_GPRBUILD          => True,
         -- Only effective when Use_GPRBUILD is True:
         Result_In_Current_Dir => False,
         -- Send compiler output here instead of stderr:
--           Compiler_Out          => "",
--           All_Warnings_Off      => True,
         Display_Call          => True);
      if Compile_Succeeded then
         Init_And_Process_Context;
      else
         raise External_Error with "*** Asis.Extensions.Compile FAILED. Exiting.";
      end if;
      Log ("END");
   exception
      when X : External_Error =>
         Log_Exception (X);
         Log ("Reraising");
         raise;
      when X: others =>
         Log_Exception (X);
         Log ("Raising Internal_Error");
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
      when X : others =>
         Log_Exception (X);
         raise Internal_Error;
   end Get_Nodes;

end Asis_Tool_2.Tool;
