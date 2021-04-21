procedure function_instantiation is
begin
  begin
    declare
      generic function func(pfp:in integer) return integer;

       
      function func(pfp:in integer) return integer is
      begin 
        return 0;
      end func;
    begin
      declare
        function p is new func;
      begin
       null;
      end; 
    end;   
  end;
end function_instantiation;
