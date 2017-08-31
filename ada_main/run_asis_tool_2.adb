with Ada.Command_Line;
with Ada.Text_IO;
with GNAT.Command_Line;
with GNAT.OS_Lib;
with GNAT.Strings;

with Asis_Tool_2.Tool;

procedure Run_Asis_Tool_2 is
   package ACL renames Ada.Command_Line;
   package GCL renames GNAT.Command_Line;

   Usage_Error : Exception;

   type Options_Record is record -- Initialized
      Config    : GCL.Command_Line_Configuration; -- Initialized
      Debug     : aliased Boolean := False;
      File_Name : aliased GNAT.Strings.String_Access; -- Initialized
   end record;

   Options : aliased Options_Record; -- Initialized
   Tool    : Asis_Tool_2.Tool.Class; -- Initialized

   procedure Log (Message : in String) is
   begin
      Ada.Text_Io.Put_Line ("Run_Asis_Tool_2:  " & Message);
   end;

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
         Log ("*** GCL raised Exit_From_Command_Line.  Program will exit now.");
         raise;
   end Get_Options;

begin
   Get_Options;
   Log ("BEGIN - target_file_in => """ & Options.File_Name.all & """");
   Tool.Process
     (File_Name => Options.File_Name.all,
--    GNAT_Home => "/usr/workspace/wsb/charles/bin/adacore/gnat-gpl-2017-x86_64-linux",
      GNAT_Home => "/home/quinlan1/ROSE/ADA/x86_64-linux/adagpl-2017/gnatgpl/gnat-gpl-2017-x86_64-linux-bin",
      Debug     => Options.Debug);
   Log ("END.");
end Run_Asis_Tool_2;
