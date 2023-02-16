package Opaque3 is
  -- declare opaque type
  type T is limited private;
 
private
   -- define opaque type
   protected type T is
     entry Set (it : in Integer);
   private
     n : Integer := 0;
   end T;
end Opaque3;
