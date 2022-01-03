package discriminant_specification is
type Buffer(Size : Integer := 100)  is        
   record
      Pos   : Integer := 0;
      Value : String(1 .. Size);
   end record;

   procedure append_ch(b : in out Buffer; ch : in character);
   procedure remove_ch(b : in out Buffer; ch : out character);
   function make_buf(size : in integer) return Buffer;

   mini : Buffer(10);
end discriminant_specification;
