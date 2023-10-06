procedure overload2 is
  package P is
    procedure get(z: out integer);
  end P;

  package body P is
    z : integer;

    procedure get(z : out integer) is
    begin
      z := P.z;
    end;
  end;
begin
  null;
end;
