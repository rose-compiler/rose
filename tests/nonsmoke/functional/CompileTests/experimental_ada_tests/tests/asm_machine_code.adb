--An example of inline assember.  Originally written as a
--test for code-segment, the Ada95 standard way to do
--assembler, but apparently no one has ever used that in
--the history of the world.

with Interfaces; use Interfaces;
with System.Machine_Code; use System.Machine_Code;
with Ada.Text_IO; use Ada.Text_IO;


procedure Asm_Machine_Code is
  function Incr (Value : Unsigned_32) return Unsigned_32 is
     Result : Unsigned_32;
  begin
    Asm("incl %0",
        Inputs  => Unsigned_32'Asm_Input  ("a", Value),
        Outputs => Unsigned_32'Asm_Output ("=a", Result));
    return Result;
  end Incr;


  Value : Unsigned_32;
begin
   Value := 5;
   Put_Line("Value before is " & Value'Image);
   Value := Incr(Value);
   Put_Line("Value after is  " & Value'Image);
end Asm_Machine_Code;
