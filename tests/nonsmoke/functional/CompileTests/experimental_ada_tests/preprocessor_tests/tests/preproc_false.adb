
#if RENAMED then
with Text_IO;
use Text_IO;
#else 
with Ada.Text_IO;
use Ada.Text_IO;
#end if;

procedure preproc is
begin
  Put_Line("Hello World");
end preproc;
