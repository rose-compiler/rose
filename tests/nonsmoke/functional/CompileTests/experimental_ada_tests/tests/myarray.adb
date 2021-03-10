package body myarray is

-- Vector addition
  function "+"  
            (Left  : in Vector;  
             Right : in Vector) return Vector is

    Result : Vector (Left'Range) := Left;

  begin

    for I in Result'Range loop
      Result (I) := Result (I) + Right (I);
    end loop;

    return Result;

  end "+";

-- Matrix addition
  function "+"  
             (Left  : in Matrix;  
              Right : in Matrix) return Matrix is
 
     Result : Matrix (Left'Range (1),  
                      Left'Range (2)) := Left;
 
   begin 
 
     for Row in Result'Range (1) loop
       Loop_Column:
         for Column in Result'Range (2) loop
           Result (Row, Column) := Result (Row, Column) + Right (Row, Column);
         end loop Loop_Column;
 
     end loop;
 
     return Result;
 
   end "+";


end myarray; 

