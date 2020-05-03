with Ada.Text_IO;

with Gnat.Command_Line;
with Gnat.Strings;

procedure Test_GNAT_Command_Line is
   package ATI renames Ada.Text_IO;
   package GCL renames Gnat.Command_Line;

   type Options_Record is record -- Initialized
      Config       : GCL.Command_Line_Configuration; -- Initialized
      Debug        : aliased Boolean := False;
      File_Name    : aliased GNAT.Strings.String_Access;
   end record;

   Options : aliased Options_Record; -- Initialized

begin
   GCL.Define_Switch (Options.Config, Options.Debug'Access,
                  "-d", Long_Switch => "--debug",
                  Help => "Output debug information");
   GCL.Define_Switch (Options.Config, Options.File_Name'Access,
                  "-f:", Long_Switch => "--file=",
                  Help => "File to process");

   begin
      GCL.Getopt (Options.Config);
      ATI.Put_Line ("Debug        => " & Options.Debug'Img);
      ATI.Put_Line ("File         => """ & Options.File_Name.all & """");
   exception
      when X : GCL.Exit_From_Command_Line =>
         ATI.Put_Line ("GCL raised Exit_From_Command_Line.  No problem, but program should exit now (e.g. -h).");
      when X : GCL.Invalid_Switch =>
         ATI.Put_Line ("GCL raised Invalid_Switch. ");
   end;

end Test_GNAT_Command_Line;
