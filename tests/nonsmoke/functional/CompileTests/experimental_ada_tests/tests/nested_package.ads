with Ada.Strings.Unbounded;

package Nested_Package is

   procedure Do_It (This : in Integer);
   
-- This statement is a problem.
   Dont_Like_5 : Exception;

end Nested_Package;
