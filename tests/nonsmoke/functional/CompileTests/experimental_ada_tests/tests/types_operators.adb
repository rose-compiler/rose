
procedure types_operators is
  package P is
    subtype S0 is integer range 2..9;
    type D is delta 2.0**(-8) range -1.0 .. 1.0;
    type S1 is private;
    type X is private;
    type Y is private;
  private
    type S1 is range 5..7;
    type X is new integer range 3..8;

    type Z is null record;
    type Y is access all Z;
  end P;

  function eq(l, r: in P.Y) return boolean renames P."=";

  d   : P.D;
  s0  : P.S0;
  s1  : P.S1;
  x  : P.X;
  y  : P.Y;
  ok : boolean;
begin
  d  := d*d;
  d  := Standard."*"(d, d);
  ok := P."/="(d, d);
  ok := P."="(d, d);
  
  s0 := s0*s0;
  s0 := Standard."+"(s0, s0);
  ok := Standard."="(s0, s0);
  
  ok := P."="(s1, s1);
  ok := P."="(x, x);
  ok := eq(y, y);
end;

