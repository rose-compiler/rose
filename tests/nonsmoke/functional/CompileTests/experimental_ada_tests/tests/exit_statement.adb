procedure Exit_Statement is
begin
   Loop_1 : loop
      exit;
      exit when True;
      exit Loop_1;
   end loop Loop_1;
end Exit_Statement;
