with Ada.Exceptions;

-- Contains supporting declarations for child packages

package Asis_Tool_2 is

   -- Controls behavior of Trace_Put:
   Trace_On : Boolean := False;

private

   procedure Trace_Put (Message : in Wide_String);

   procedure Print_Exception_Info (X : in Ada.Exceptions.Exception_Occurrence);

end Asis_Tool_2;
