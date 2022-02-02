-- with Ada.Text_IO; 
--
with generic_package_declaration;

procedure generic_package_renaming_declaration is 
   generic package gpd renames generic_package_declaration;

   PACKAGE gint IS NEW gpd(INTEGER);

   use gint; 

   data : File_Name; 

begin
  null;
end generic_package_renaming_declaration;
