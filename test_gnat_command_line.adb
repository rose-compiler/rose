with Ada.Text_IO;

with Gnat.Command_Line;
with Gnat.Strings;

procedure Test_GNAT_Command_Line is
   package ATI renames Ada.Text_IO;
   package GCL renames Gnat.Command_Line;

   Debug        : aliased Boolean;
   Optimization : aliased Integer;
   File_Name    : aliased GNAT.Strings.String_Access;
   Config       : GCL.Command_Line_Configuration;

begin
   GCL.Define_Switch (Config, Debug'Access,
                  "-d", Long_Switch => "--debug",
                  Help => "Output debug information");
   GCL.Define_Switch (Config, File_Name'Access,
                  "-f:", Long_Switch => "--file=",
                  Help => "File to process");
   GCL.Define_Switch (Config, Optimization'Access,
                  "-O?", Help => "Optimization level");

   begin
      GCL.Getopt (Config);
   exception
      when X : GNAT.Command_Line.Exit_From_Command_Line =>
         ATI.Put_Line ("GCL raised Exit_From_Command_Line.  Program should exit now.");
   end;

   ATI.Put_Line ("Debug        => " & Debug'Img);
   ATI.Put_Line ("Optimization => " & Optimization'Img);
   ATI.Put_Line ("File         => """ & File_Name.all & """");

end Test_GNAT_Command_Line;
