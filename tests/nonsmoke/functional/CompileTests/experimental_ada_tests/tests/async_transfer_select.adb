procedure Async_Transfer_Select is

begin
   
   select
      delay 5.0;
      null;
   then abort
      null;
   end select;
   
   null;

end Async_Transfer_Select;

