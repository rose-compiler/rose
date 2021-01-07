with Ada.Exceptions;
-- Provides routines that prefix the output with the name of the current
-- module.
--
-- Note:
-- If this is instantiated multiple times inside nested declarative regions
-- (e.g. nested subprograms) and the resulting package is "use"d, then calls on
-- Log, etc. in the inner statements will be ambiguious and will not compile.
-- Solution: Prefix the call with the instantiated package name.
generic
   Module_Name : in string;
package Generic_Logging is
   procedure Log (Message : in String);
   procedure Log_Wide (Message : in Wide_String);
   procedure Log_Exception (X : in Ada.Exceptions.Exception_Occurrence);
end Generic_Logging;
