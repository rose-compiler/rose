package Complex is

type Complex is
   record
      Re : Float := 0.0;
      Im : Float := 0.0;
   end record;

I : constant Complex := (0.0, 1.0);

function II return Complex;

end Complex;
