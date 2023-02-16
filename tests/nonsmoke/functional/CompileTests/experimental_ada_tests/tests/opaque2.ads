package Opaque2 is
  -- declare opaque type
  type T is limited private;
 
private
   -- define opaque type
   task type T is
     entry Get (it : out Integer);
   end T;
end Opaque2;
