with Ada.Command_Line;
with Ada.Text_IO;
with GNAT.Command_Line;
with GNAT.OS_Lib;
with GNAT.Strings;

with Asis_Tool_2.Tool;

procedure Run_Asis_Tool_2 is
   package ACL renames Ada.Command_Line;
   package GCL renames GNAT.Command_Line;

   type Options_Record is record -- Initialized
      Config     : GCL.Command_Line_Configuration; -- Initialized
      Debug      : aliased Boolean := False;
      File_Name  : aliased GNAT.Strings.String_Access; -- Initialized
      GNAT_Home  : aliased GNAT.Strings.String_Access; -- Initialized
      Output_Dir : aliased GNAT.Strings.String_Access; -- Initialized
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
      GCL.Define_Switch (Options.Config, Options.Gnat_Home'Access,
                         "-g:", Long_Switch => "--gnat_home=",
                         Help => "GNAT home directory");
      GCL.Define_Switch (Options.Config, Options.Output_Dir'Access,
                         "-o:", Long_Switch => "--output_dir=",
                         Help => "Output directory");
      GCL.Getopt (Options.Config);
   exception
      when X : GNAT.Command_Line.Exit_From_Command_Line =>
         Log ("*** GCL raised Exit_From_Command_Line.  Program will exit now.");
         raise;
   end Get_Options;

   procedure dot_asisinit;
   pragma Import (C, dot_asisinit);

   procedure dot_asisfinal;
   pragma Import (C, dot_asisfinal);

begin
   Get_Options;
   Log ("BEGIN");
   dot_asisinit;
   Tool.Process
     (File_Name  => Options.File_Name.all,
      Output_Dir => Options.Output_Dir.all,
      GNAT_Home  => Options.GNAT_Home.all,
      Debug      => Options.Debug);
   dot_asisfinal;
   Log ("END");
end Run_Asis_Tool_2;
