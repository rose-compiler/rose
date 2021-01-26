package REPRESENTATION_CLAUSE_2 is

   type BITS is record
      Lower  : INTEGER range 0..3;
      Middle : INTEGER range 0..1;
      High   : INTEGER range 0..3;
   end record;

   for BITS use record
      at mod 8;
      Lower  at 0 range 0..1;
      Middle at 0 range 2..2;
      High   at 0 range 3..4;
   end record;

   for BITS'SIZE use 16;
end REPRESENTATION_CLAUSE_2;
