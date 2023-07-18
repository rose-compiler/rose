procedure Label2 is
begin
  <<X>> null;
   
  declare
    x : Integer := 0; -- variable overloads label
  begin
    if x = 1 then
      goto Label2.X; -- label reference needs name qual.
    end if;   
  end;
end Label2;


