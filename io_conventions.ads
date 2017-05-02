-- This is the UNIX line terminator.

with Ada.Characters.Latin_1;

package Io_Conventions is

    Line_Terminator : constant String (1 .. 1) :=
       (1 => Ada.Characters.Latin_1.Lf);

end Io_Conventions;
