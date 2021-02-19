
procedure Enumeration is
  type MyColor is (Blue, Red, Green, Yellow);
  for MyColor use (Blue    => 11,
                 Red     => 22,
                 Green   => 33,
                 Yellow  => 44);

  type YourColor is (Blue, White, Red);
  for YourColor use (0,    1,     2);

begin
  null;
end Enumeration;
