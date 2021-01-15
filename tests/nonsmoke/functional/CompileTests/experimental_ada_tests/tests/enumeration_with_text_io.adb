with Ada.Text_IO;

procedure Enumeration_With_Text_IO is
  type MyColor is (Blue, Red, Green, Yellow);
  for MyColor use (Blue    => 11,
                 Red     => 22,
                 Green   => 33,
                 Yellow  => 44);
   Somecolor : MyColor := Green;
begin
   case Somecolor is
      when Blue =>
         Ada.Text_IO.Put_Line ("Blue!");
      when Green =>
         Ada.Text_IO.Put_Line ("Green!");
      when others =>
         Ada.Text_IO.Put_Line ("Oh, man, the colors!!");
   end case;
end Enumeration_With_Text_IO;
