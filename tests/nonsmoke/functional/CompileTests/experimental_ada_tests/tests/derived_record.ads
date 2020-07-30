package Derived_Record is

   type Tagged_Record is tagged record
      Component_1 : Integer;
   end record;

   type Derived_Record is new Tagged_Record with record
      Component_2 : Integer;
   end record;

end Derived_Record;
