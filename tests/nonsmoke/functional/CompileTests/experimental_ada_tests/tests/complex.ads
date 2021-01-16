package Complex is

  type Complex is private;
  
  i, zero : constant Complex; 

  function "+" (lhs, rhs : in Complex) return Complex;
  function Imag(val : in Complex) return Float;
  function Real(val : in Complex) return Float;

private

  type Complex is record
         Re : Float := 0.0;
         Im : Float := 0.0;
       end record;

  i : constant Complex := (0.0, 1.0);
  zero: constant Complex := (0.0, 0.0);

end Complex;
