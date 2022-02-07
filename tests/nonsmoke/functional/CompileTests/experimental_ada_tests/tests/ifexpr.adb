procedure ifexpr is
  x : integer := 0;
--  y : integer := (if x > 1 then 2 else (if x = -1 then -2 else -3));

  procedure consumer(a, b : in integer := 3) is
  begin
    null;
  end consumer;
begin
  consumer(if x > 4 then 5 else 6);
--  consumer(x, (if x > 7 then 8 else 9));
--  consumer(b => (if x > -4 then -5 elsif x > -6 then -7 else -8));
end ifexpr;
