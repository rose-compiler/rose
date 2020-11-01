with Ada.Real_Time;

procedure Delay_Until is
use Ada.Real_Time;

Start_Time :          Time      := Clock;
Period     : constant Time_Span := Milliseconds(5000);
Poll_Time  :          Time;
begin
Poll_Time  := Start_Time;
delay until Poll_Time;
end Delay_Until;
