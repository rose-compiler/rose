with Ada.Command_Line;
with Ada.Text_IO;
with Asis.Extensions;
with GNAT.Command_Line;
with GNAT.OS_Lib;
with GNAT.Strings;

with Asis_Tool_2.Tool;

procedure Run_Asis_Tool_2 is
   package ACL renames Ada.Command_Line;
   package ATI renames Ada.Text_IO;
   package GCL renames GNAT.Command_Line;

   Usage_Error : Exception;

   type Options_Record is record -- Initialized
      Config    : GCL.Command_Line_Configuration; -- Initialized
      Debug     : aliased Boolean := False;
      File_Name : aliased GNAT.Strings.String_Access; -- Initialized
   end record;

   Options : aliased Options_Record; -- Initialized
   Tool    : Asis_Tool_2.Tool.Class; -- Initialized

   procedure Get_Options is
   begin
      GCL.Define_Switch (Options.Config, Options.Debug'Access,
                         "-d", Long_Switch => "--debug",
                         Help => "Output debug information");
      GCL.Define_Switch (Options.Config, Options.File_Name'Access,
                         "-f:", Long_Switch => "--file=",
                         Help => "File to process");
      GCL.Getopt (Options.Config);
      if Options.File_Name.all = "" then
         raise Usage_Error with "File name must be provided, was empty";
      end if;
   exception
      when X : GNAT.Command_Line.Exit_From_Command_Line =>
         ATI.Put_Line ("GCL raised Exit_From_Command_Line.  Program will exit now.");
         raise;
   end Get_Options;

   Null_Compile_Args : GNAT.OS_Lib.Argument_List (1 .. 0);

   Compile_Succeeded : Boolean := False;
begin
   Get_Options;
   Asis.Extensions.Compile
     (Source_File  => Options.File_Name,
      Args         => Null_Compile_Args,
      Success      => Compile_Succeeded,
      Display_Call => True);
   if Compile_Succeeded then
      Tool.Process
        (File_Name => Options.File_Name.all,
         Debug     => Options.Debug);
   end if;
end Run_Asis_Tool_2;
