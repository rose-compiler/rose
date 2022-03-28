with GNAT.Directory_Operations;

with GNATCOLL.Projects;
with GNATCOLL.VFS;
with Libadalang.Project_Provider;

with Generic_Logging;
with Lal_Adapter.Unit;

package body Lal_Adapter.Context is

   -- Raises Internal_Error:
   procedure Parse_Input_File
     (This : in out Class;
      Input_File_Name   : in     String)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Parse_Input_File";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      Auto : Logging.Auto_Logger; -- Logs BEGIN and END
   begin
      -- Parse the source file into an Analysis_Unit (tree):
      This.Top_Unit := This.Lal_Context.Get_From_File (Input_File_Name);
      --  Report parsing errors, if any
      if This.Top_Unit.Has_Diagnostics then
         Log ((1..50 => '*'));
         Log ("Unit.Has_Diagnostics returned True.");
         Log ("Diagnostics:");
         for D of This.Top_Unit.Diagnostics loop
            Log (This.Top_Unit.Format_GNU_Diagnostic (D));
         end loop;
         Log ("Raising External_Error");
         Log ((1..50 => '*'));
         raise External_Error with
           "Parsing """ & Input_File_Name & """ failed.";
      end if;
   exception
      when X : External_Error | Internal_Error | Usage_Error =>
         raise;
      when X: others =>
         Log_Exception (X);
         Log ("No handler for this exception.  Raising Internal_Error");
         raise Internal_Error;
   end Parse_Input_File;


   procedure Process_Top_Unit
     (This : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Top_Unit";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      Auto : Logging.Auto_Logger; -- Logs BEGIN and END
   begin
      This.Top_Compilation_Unit :=
        LAL.As_Compilation_Unit (This.Top_Unit.Root);
      declare
         Unit_Processor : Lal_Adapter.Unit.Class; -- Initialized
      begin
         Unit_Processor.Process (This.Top_Compilation_Unit, This.Outputs);
      end;
   end Process_Top_Unit;


   procedure Process_Dependencies
     (This    : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Dependencies";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      Auto : Logging.Auto_Logger; -- Logs BEGIN and END
   begin
      declare
         Dependencies : constant LAL.Compilation_Unit_Array :=
           LAL.P_Unit_Dependencies (This.Top_Compilation_Unit);
      begin
         for Dependency of Dependencies Loop
            declare
               Unit_Processor : Lal_Adapter.Unit.Class; -- Initialized
            begin
               Unit_Processor.Process (Dependency, This.Outputs);
            end;
         end loop;
      end;
   end Process_Dependencies;


   --  Load the project file:
   procedure Load_Project_And_Setup_Context
     (This              : in out Class;
      Project_File_Name : in String)
   is
      package GPR renames GNATCOLL.Projects;
      package LAL_GPR renames Libadalang.Project_Provider;
      use type GNATCOLL.VFS.Filesystem_String; -- For "+"

      Env           : GPR.Project_Environment_Access; -- Initialized
      Project_File  : constant GNATCOLL.VFS.Virtual_File :=
        GNATCOLL.VFS.Create (+Project_File_Name);
      Project       : constant GPR.Project_Tree_Access := new GPR.Project_Tree;
      Unit_Provider : LAL.Unit_Provider_Reference := LAL.No_Unit_Provider_Reference;
   begin -- Load_Project_And_Setup_Context
      GPR.Initialize (Env);
      --  Use procedures in GNATCOLL.Projects to set scenario
      --  variables (Change_Environment), to set the target
      --  and the runtime (Set_Target_And_Runtime), etc.
      Project.Load (Project_File, Env);
      Unit_Provider := LAL_GPR.Create_Project_Unit_Provider
        (Tree    => Project,
         Project => GPR.No_Project,
         Env     => Env);
      This.Lal_Context := LAL.Create_Context
        (Unit_Provider => Unit_Provider);
   end Load_Project_And_Setup_Context;


   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This              : in out Class;
      Input_File_Name   : in     String;
      Project_File_Name : in     String;
      --  Unit_Options   : in     Unit.Options_Record;
      Outputs           : in     Output_Accesses_Record)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      Auto : Logging.Auto_Logger; -- Logs BEGIN and END
   begin -- Process
      Log ("Project_File_Name    => """ & Project_File_Name & """");
      Log ("Input_File_Name      => """ & Input_File_Name & """");
      This.Outputs := Outputs;

      -- TODO: Move up to .Tool?
      This.Outputs.Graph.Set_ID ("""" & Input_File_Name & """");
      -- TODO: Move up to .Tool?
      This.Outputs.A_Nodes.Set
        (Context =>
           (Name        => To_Chars_Ptr (Project_File_Name),
            Parameters  => To_Chars_Ptr (String'("")),
            Debug_Image => To_Chars_Ptr (String'(""))));

      This.Load_Project_And_Setup_Context (Project_File_Name);
      This.Parse_Input_File (Input_File_Name);
      -- TODO: Process dependencies first, so that declarations come before 
      -- references?  Actually, we don't know if the dependencies will be 
      -- traversed in dependency order themselves!
      This.Process_Top_Unit;
      This.Process_Dependencies;
   exception
      when X : External_Error | Internal_Error | Usage_Error =>
         raise;
      when X: others =>
         Log_Exception (X);
         Log ("No handler for this exception.  Raising Internal_Error");
         raise Internal_Error;
   end Process;

end Lal_Adapter.Context;
