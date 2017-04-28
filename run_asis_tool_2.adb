with Ada.Command_Line;
with Ada.Text_IO;
with Asis_Tool_2.Tool;

procedure Run_Asis_Tool_2 is
   package ACL renames Ada.Command_Line;
   package ATI renames Ada.Text_IO;
   This_Tool : Asis_Tool_2.Tool.Class;
begin
   Asis_Tool_2.Trace_On := False;
   -- This should work, but always returns zero!
   ATI.Put_Line ("Argument_Count =>" & ACL.Argument_Count'Img);
   if ACL.Argument_Count > 0 then
      if ACL.Argument (1) = "-d" then
         ATI.Put_Line ("Found ""-d""");
         Asis_Tool_2.Trace_On := True;
      end if;
   end if;
   This_Tool.Process;
end Run_Asis_Tool_2;
