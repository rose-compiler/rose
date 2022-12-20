separate (Rep_E_Queue)
task body Queue1 is
   My_Bool : boolean := false;   
begin
      loop
         begin
            My_Bool := True;
<<Stop_One_Queue_Events>>
            My_Bool := True;
         exception
            when Exception_Occurrence: others => My_Bool := False;
         end;
      end loop;
end Queue1; 
