with Ada.Command_Line;
with Ada.Text_IO; use Ada.Text_IO;
with GNATCOLL.Projects;
with GNATCOLL.VFS;
with Langkit_Support.Slocs;
with Libadalang.Analysis;
with Libadalang.Common;
with Libadalang.Project_Provider;


procedure Main is
   package LAL renames Libadalang.Analysis;
   package LALCO renames Libadalang.Common;
   package Slocs renames Langkit_Support.Slocs;
   --  Context : constant LAL.Analysis_Context := LAL.Create_Context;
   Context : LAL.Analysis_Context;
   Provider : LAL.Unit_Provider_Reference;

   --  If Node is an object declaration, print its text. Always continue the
   --  traversal.
   function Process_Node 
     (Node : LAL.Ada_Node'Class) 
      return LALCO.Visit_Status
   is
     use type LALCO.Ada_Node_Kind_Type;
   begin
      --  if Node.Kind = LALCO.Ada_Object_Decl then
      Put_Line
        ("Line" & Slocs.Line_Number'Image (Node.Sloc_Range.Start_Line) & ": " &
           LALCO.Ada_Node_Kind_Type'Image (Node.kind) & ": " & 
           Node.Image);
      --  end if;
      return LALCO.Into;
   end Process_Node;
   
   --  Load the project file designated by the first command-line argument
   function Load_Project return LAL.Unit_Provider_Reference is
      package GPR renames GNATCOLL.Projects;
      package LAL_GPR renames Libadalang.Project_Provider;
      use type GNATCOLL.VFS.Filesystem_String;

      Project_Filename : constant String := Ada.Command_Line.Argument (1);
      Project_File     : constant GNATCOLL.VFS.Virtual_File :=
        GNATCOLL.VFS.Create (+Project_Filename);

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
begin
   Context := LAL.Create_Context (Unit_Provider => Load_Project);
   --  Try to parse all source file given as arguments
   for I in 2 .. Ada.Command_Line.Argument_Count loop
      declare
         Filename : constant String := Ada.Command_Line.Argument (I);
         Unit     : constant LAL.Analysis_Unit := Context.Get_From_File (Filename);
      begin
         Put_Line ("== " & Filename & " ==");

         --  Report parsing errors, if any
         if Unit.Has_Diagnostics then
            for D of Unit.Diagnostics loop
               Put_Line (Unit.Format_GNU_Diagnostic (D));
            end loop;

         --  Otherwise, look for object declarations
         else
            Unit.Root.Traverse (Process_Node'Access);
         end if;
         New_Line;
      end;
   end loop;
end Main;
