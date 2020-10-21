package Complex is

type Complex is
   record
      Re : Float := 0.0;
      Im : Float := 0.0;
   end record;

function II return Complex;

end Complex;
