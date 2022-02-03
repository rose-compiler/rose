generic
    type Elem is private;
package generic_package_declaration is
    type File_Name is limited private;

    procedure Exchange(U, V : in out Elem);
  private
    type File_Name is limited record
           id : Integer := 0;
         end record;
end generic_package_declaration;
