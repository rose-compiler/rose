with Ada.Text_IO;
procedure Goto_Loop is
begin
<<Start>>
   Ada.Text_IO.Put_Line ("Goto Loop!");
goto Start;
end Goto_Loop;
