procedure PrefixCalls is
  i : Integer := 0;
begin
  i := "+"(i,i);
  i := Standard."+"(1,i);
  i := Standard."+"(Right => 1, Left => i);
  i := "-"(1);
end PrefixCalls;
