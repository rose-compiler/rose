with System;

procedure rep_sys_address(Starting_Address : in System.Address) is
      type Word_Array is array (1..31) of Integer;
      for Word_Array'Size use 992;
      Words : Word_Array;
      for Words use at Starting_Address;
begin
      null;
end; 
