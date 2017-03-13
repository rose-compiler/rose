with Asis_Tool_2.Context;

procedure Run_Asis_Tool_2
is
   This_Context : Asis_Tool_2.Context.Class;
begin
   Asis_Tool_2.Trace_On := False;
   This_Context.Process;
end Run_Asis_Tool_2;
