package body Rep_E_Queue is
   pkg_bool : boolean := False;
   task type Queue1 is
      pragma Priority (50);
      pragma Storage_Size (5000);
   end Queue1;
   task body Queue1    is separate;
    procedure proc1(in_bool : in out boolean) is
      my_bool : boolean := False;
    begin
      my_bool := in_bool;
    end proc1;

   procedure LogRate(rate : in integer);
   procedure LogRate(rate : in integer) is separate;
begin
   pkg_bool := True;
end Rep_E_Queue;
