package body Protected_Body_Declaration is

   protected body T is
    function func return Integer is
     begin
      return 0;
     end func;
   end T;
end Protected_Body_Declaration;
