with System;

procedure Enumeration is
  type MyColor is (Blue, Red, Green, Yellow);
  for MyColor use ( Blue    => 11,
                    Red     => 22,
                    Green   => 33,
                    Yellow  => 44);

  type YourColor is (Blue, White, Red);
  for YourColor use (- 1,    0,   + 1);

  type MyBounds is (Negative_Large, Positive_Large);
  for MyBounds use (Negative_Large => System.Min_Int, Positive_Large => System.Max_Int);

begin
  null;
end Enumeration;
