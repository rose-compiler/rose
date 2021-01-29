package myarray is

  type Vector is array (Integer range <>) of Float;
  type Matrix is array (Integer range <>, Integer range <>) of Float; 

  function "+"  
        (Left  : in Vector;  
         Right : in Vector) return Vector;

  function "+"  
            (Left  : in Matrix;  
             Right : in Matrix) return Matrix;

end myarray; 

