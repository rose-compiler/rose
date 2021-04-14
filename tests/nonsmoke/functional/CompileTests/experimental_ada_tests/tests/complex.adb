package body Complex is

function "+" (lhs, rhs : in Complex) return Complex is
begin
  return Complex'(lhs.Re + rhs.Re, lhs.Im + rhs.Im);
end "+";

function Imag(val : in Complex) return Float is
begin
  return val.Im;
end Imag;

function Real(val : in Complex) return Float is
begin
  return val.Re;
end Real;


end Complex;
