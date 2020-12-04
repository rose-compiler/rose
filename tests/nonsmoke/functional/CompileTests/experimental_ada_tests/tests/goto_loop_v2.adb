with Ada.Text_IO;
procedure goto_loop_Text_IO is
begin
<<Start>>
   Ada.Text_IO.Put_Line ("Goto Loop!");
goto Start;
end goto_loop_Text_IO;
