package discriminant_specification is
type Buffer(Size : Integer := 100)  is        
   record
      Pos   : Integer := 0;
      Value : String(1 .. Size);
   end record;
end discriminant_specification;
