
procedure Enumeration is
  type MyColor is (Blue, Red, Green, Yellow);
  for MyColor use (Blue    => 11,
                 Red     => 22,
                 Green   => 33,
                 Yellow  => 44);
begin
  null;
end Enumeration;
