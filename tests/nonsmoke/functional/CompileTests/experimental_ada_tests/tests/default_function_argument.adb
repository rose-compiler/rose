procedure default_function_argument is

  function plus_defaulted(x, y: Integer := 0) return Integer is
  begin
    return x + y;
  end plus_defaulted;

  z : Integer := plus_defaulted;

begin
  null; 
end default_function_argument; 
