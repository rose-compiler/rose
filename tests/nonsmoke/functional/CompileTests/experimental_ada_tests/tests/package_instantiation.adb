procedure package_instantiation is
begin
  begin
    declare
      generic package pkg is
         x : integer := 0;
      end pkg;
    begin
      declare
        package p is new pkg;
        begin
         null;
        end; 
    end;   
  end;
end package_instantiation;
