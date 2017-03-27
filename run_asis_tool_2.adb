with Asis_Tool_2.Tool;

procedure Run_Asis_Tool_2
is
   This_Tool : Asis_Tool_2.Tool.Class;
begin
   Asis_Tool_2.Trace_On := True;
   This_Tool.Process;
end Run_Asis_Tool_2;
