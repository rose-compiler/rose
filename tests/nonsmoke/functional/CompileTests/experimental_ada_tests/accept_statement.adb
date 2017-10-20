procedure Accept_Statement is

   task type The_Task_Type is
      entry The_Entry;
   end The_Task_Type;

   task body The_Task_Type is
   begin
      accept The_Entry;
      accept The_Entry  do
         null;
      end The_Entry;
   end The_Task_Type;

begin
   null;
end Accept_Statement;
