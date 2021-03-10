with Ada.Text_IO; 

package generic_package_renaming_declaration is
   generic package Enum_IO renames Ada.Text_IO.Enumeration_IO; 
end generic_package_renaming_declaration;
