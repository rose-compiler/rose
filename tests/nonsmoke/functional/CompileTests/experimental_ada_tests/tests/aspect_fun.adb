procedure Aspect_Fun is

  function errno_location return Integer
           with Import => True,
                Convention => CPP,
                External_Name => "__errno_location";   
begin
  null;
end;

