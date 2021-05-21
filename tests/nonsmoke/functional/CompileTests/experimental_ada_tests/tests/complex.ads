package Complex is

  type Complex is private;
  
  i, zero : constant Complex; 

  -- arithmetic plus operator on complex values 
  function "+" ( lhs,                -- left hand side
                 rhs : in Complex    -- right hand side
               ) return Complex;     -- returns a Compelx

  -- extracts imaginary component
  function Imag(val : in Complex) return Float;

  -- extracts real component
  function Real(val : in Complex) return Float;

private

  type Complex is record
         Re : Float := 0.0;
         Im : Float := 0.0;
       end record;

  i : constant Complex := (0.0, 1.0);
  zero: constant Complex := (0.0, 0.0);

end Complex;
