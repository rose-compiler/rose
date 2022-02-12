with Ada.Text_IO; 
use Ada.Text_IO;

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

function Origin return Complex is
begin
  return zero;
end Origin;

procedure Put(v : in Complex) is
begin
  Put("(");
  Put(Float'Image(v.Re));
  Put(" + ");
  Put(Float'Image(v.Im));
  Put("i)");
end Put;

end Complex;
