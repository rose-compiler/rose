with Ada.Command_Line;
with Ada.Text_IO;
with GNAT.Command_Line;
with GNAT.OS_Lib;
with GNAT.Strings;

with Asis_Adapter.Tool;

procedure Run_Parser_Adapter is
   package ACL renames Ada.Command_Line;
   package GCL renames GNAT.Command_Line;

   type Options_Record is record -- Initialized
      Config     : GCL.Command_Line_Configuration; -- Initialized
      Debug      : aliased Boolean := False;
      File_Name  : aliased GNAT.Strings.String_Access; -- Initialized
      GNAT_Home  : aliased GNAT.Strings.String_Access; -- Initialized
      Output_Dir : aliased GNAT.Strings.String_Access; -- Initialized
      Process_Predefined_Units
                 : aliased Boolean := False;
      Process_Implementation_Units
                 : aliased Boolean := False;
   end record;

   Options : aliased Options_Record; -- Initialized
   Tool    : Asis_Adapter.Tool.Class; -- Initialized

   procedure Log (Message : in String; Debug : in Boolean) is
   begin
      if Debug then
        Ada.Text_Io.Put_Line ("Run_Asis_Adapter:  " & Message);
      end if;
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
      GCL.Define_Switch (Options.Config, Options.Process_Predefined_Units'Access,
                         "-p", Long_Switch => "--process_predefined_units",
                         Help =>"Process Ada predefined language environment units");
      GCL.Define_Switch (Options.Config, Options.Process_Implementation_Units'Access,
                         "-i", Long_Switch => "--process_implementation_units",
                         Help =>"Process implementation specific library units");
      GCL.Getopt (Options.Config);
   exception
      when X : GNAT.Command_Line.Exit_From_Command_Line =>
         Log ("*** GNAT.Command_Line raised Exit_From_Command_Line.  Program will exit now.",  Options.Debug);
         raise;
   end Get_Options;

   procedure asis_adapterinit;
   pragma Import (C, asis_adapterinit);

   procedure asis_adapterfinal;
   pragma Import (C, asis_adapterfinal);

begin
   Get_Options;
   Log ("BEGIN", Options.Debug);
   asis_adapterinit;
   Tool.Process
     (File_Name                    => Options.File_Name.all,
      Output_Dir                   => Options.Output_Dir.all,
      GNAT_Home                    => Options.GNAT_Home.all,
      Process_Predefined_Units     => Options.Process_Predefined_Units,
      Process_Implementation_Units => Options.Process_Implementation_Units,
      Debug                        => Options.Debug);
   asis_adapterfinal;
   Log ("END", Options.Debug);
end Run_Parser_Adapter;
