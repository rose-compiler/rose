procedure procedure_instantiation is
begin
  begin
    declare
      generic procedure proc(pfp:in integer);

       
      procedure proc(pfp:in integer) is
      begin 
        null;
      end proc;
    begin
      declare
        procedure p is new proc;
      begin
       null;
      end; 
    end;   
  end;
end procedure_instantiation;
