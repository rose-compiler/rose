
package DBase is

   type DB_Record is tagged record
      Key : Natural range 1 .. 9999;
      Data : String (1..10);
   end record;

   function "=" (L, R : in DB_Record) return Boolean;

end DBase;
