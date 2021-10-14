with Ada.Text_IO;          use Ada.Text_IO;
with Libadalang.Analysis;  use Libadalang.Analysis;

procedure Parse_Print is
   Ctx  : Analysis_Context := Create_Context;
   Unit : Analysis_Unit := Ctx.Get_From_File ("test.adb");
begin
   Unit.Print;
   Put_Line ("Done.");
end Parse_Print;
