package Opaque is

  -- declare opaque elements
  type Handle is private;
 
  NIX : constant Handle; -- hide handle value

private
   -- define opaque elements
   type Handle is new Integer;

   NIX : constant Handle := 0;
end Opaque;
