procedure ifexpr is
  x : integer := 0;
--  y : integer := (if x > 0 then 1 else (if x = 0 then 0 else -1));

  procedure consumer(a, b : in integer := 0) is
  begin
    null;
  end consumer;
begin
  consumer(if x > 0 then 1 else 0);
--  consumer(x, (if x > 0 then 1 else 0));
--  consumer(b => (if x > 0 then -1 else 1));
end ifexpr;
