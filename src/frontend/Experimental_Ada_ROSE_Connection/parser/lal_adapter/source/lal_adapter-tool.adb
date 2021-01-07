with Ada.Directories;

--  with Lal_Adapter.Unit;

with GNATCOLL.Projects;
with GNATCOLL.VFS;
with Langkit_Support.Slocs;
with Libadalang.Analysis;
with Libadalang.Common;
with Libadalang.Project_Provider;

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

      package AD renames Ada.Directories;

      Full_Input_File_Name : constant String := AD.Full_Name (Input_File_Name);
      Source_File_Dir  : constant String := AD.Containing_Directory (Full_Input_File_Name);
      Simple_File_Name : aliased String := AD.Simple_Name (Full_Input_File_Name);

      Real_Output_Dir  : constant String :=
        (if Output_Dir_Name = Use_Current_Dir then AD.Current_Directory else Output_Dir_Name);

      procedure Init_And_Process_Context is
         --  Unit_Options : Unit.Options_Record; -- Initialized
      begin
         --  Unit_Options.Process_If_Origin_Is (Lal.A_Predefined_Unit) :=
         --    Process_Predefined_Units;
         --  Unit_Options.Process_If_Origin_Is (Lal.An_Implementation_Unit) :=
         --    Process_Implementation_Units;
         This.Outputs.Output_Dir := ASU.To_Unbounded_String (Real_Output_Dir);
         This.Outputs.Text := new Indented_Text.Class;
         This.Outputs.Graph := Dot.Graphs.Create (Is_Digraph => True,
                                                  Is_Strict  => False);
         This.Outputs.A_Nodes := new A_Nodes.Class;
      --     -- TODO: use File_Name:
      --     This.My_Context.Process (Tree_File_Name => Tree_File_Name,
      --                              Unit_Options   => Unit_Options,
      --                              Outputs        => This.Outputs);
      --     This.Outputs.Graph.Write_File
      --       (ASU.To_String (This.Outputs.Output_Dir) & '/' & Simple_File_Name);
         --  This.Outputs.A_Nodes.Print_Stats;
      end Init_And_Process_Context;
      
      procedure Do_Example is
         Parent_Name : constant String := Module_Name;
         Module_Name : constant String := Parent_Name & ".Do_Example";
         package Logging is new Generic_Logging (Module_Name); use Logging;
         
         package LAL renames Libadalang.Analysis;
         package LALCO renames Libadalang.Common;
         package Slocs renames Langkit_Support.Slocs;

         --  If Node is an object declaration, print its text. Always continue the
         --  traversal.
         function Process_Node
           (Node : in LAL.Ada_Node'Class)
            return LALCO.Visit_Status
         is
            use type LALCO.Ada_Node_Kind_Type;
         begin
            --  if Node.Kind = LALCO.Ada_Object_Decl then
            Logging.Log ("Line" & Slocs.Line_Number'Image (Node.Sloc_Range.Start_Line) & ": " &
                   LALCO.Ada_Node_Kind_Type'Image (Node.kind) & ": " &
                   Node.Debug_Text);
            --  end if;
            -- Continue traversal:
            return LALCO.Into;
         end Process_Node;

         --  Load the project file
         function Load_Project 
           (Project_File_Name : in String)
            return LAL.Unit_Provider_Reference 
         is
            package GPR renames GNATCOLL.Projects;
            package LAL_GPR renames Libadalang.Project_Provider;
            use type GNATCOLL.VFS.Filesystem_String;

            Project_File : constant GNATCOLL.VFS.Virtual_File :=
              GNATCOLL.VFS.Create (+Project_File_Name);

            Env     : GPR.Project_Environment_Access;
            Project : constant GPR.Project_Tree_Access := new GPR.Project_Tree;
         begin
            GPR.Initialize (Env);
            --  Use procedures in GNATCOLL.Projects to set scenario
            --  variables (Change_Environment), to set the target
            --  and the runtime (Set_Target_And_Runtime), etc.
            Project.Load (Project_File, Env);
            return LAL_GPR.Create_Project_Unit_Provider
              (Tree    => Project,
               Project => GPR.No_Project,
               Env     => Env);
         end Load_Project;

         Unit_Provider : LAL.Unit_Provider_Reference;
         Context       : LAL.Analysis_Context;
         Unit          : LAL.Analysis_Unit;

      begin -- Do_Example
         Logging.Log ("Processing: " & Input_File_Name);
         Unit_Provider := Load_Project (Project_File_Name);
         Context := LAL.Create_Context (Unit_Provider => Unit_Provider);
         -- Parse unit:
         Unit := Context.Get_From_File (Input_File_Name);

         --  Report parsing errors, if any
         if Unit.Has_Diagnostics then
            Logging.Log ((1..50 => '*'));
            Logging.Log ("Unit.Has_Diagnostics returned True.");
            Logging.Log ("Diagnostics:");
            for D of Unit.Diagnostics loop
               Logging.Log (Unit.Format_GNU_Diagnostic (D));
            end loop;
            Logging.Log ("Raising Internal_Error");
            Logging.Log ((1..50 => '*'));
            raise Internal_Error;

            --  Otherwise, look for object declarations
         else
            Unit.Root.Traverse (Process_Node'Access);
         end if;
         This.Outputs.A_Nodes.Print_Stats;
         Logging.Log ("Done.");
      exception
         when X : External_Error | Internal_Error =>
            raise;
         when X: others =>
            Logging.Log_Exception (X);
            Logging.Log ("Raising Internal_Error");
            raise Internal_Error;
      end Do_Example;
      

   begin -- Process
      Lal_Adapter.Trace_On := Debug;
      Log ("BEGIN");
      Log ("Input_File_Name  => """ & Input_File_Name & """");
      Log ("Output_Dir => """ & Output_Dir_Name & """");
      if Input_File_Name = "" then
         raise Usage_Error with "Input_File_Name must be provided, was empty.";
      end if;
      Log ("Current directory is: """ & Ada.Directories.Current_Directory & """");
      Log ("Full_Input_File_Name  => """ & Full_Input_File_Name & """");
      Log ("Real_Output_Dir => """ & Real_Output_Dir & """");
      
      Init_And_Process_Context;
      Do_Example;
      Log ("END");
   exception
      when X : External_Error | Internal_Error =>
         raise;
      when X: others =>
         Logging.Log_Exception (X);
         Logging.Log ("Raising Internal_Error");
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
      when X : External_Error | Internal_Error =>
         raise;
      when X: others =>
         Logging.Log_Exception (X);
         Logging.Log ("Raising Internal_Error");
         raise Internal_Error;
   end Get_Nodes;

end Lal_Adapter.Tool;
