--------------------------------------------------------------
--------  Load_Control  --------------------------------------
--  This package provides a protected object that controls
--  the servicing of calling tasks, such that they are kept
--  waiting until the system Load is less than some threshold
--  value (different for each task type).
-- Lifted from: http://www.cs.uni.edu//~mccormic/AdaEssentials/requeue_statement.htm
--------------------------------------------------------------
--------------------------------------------------------------
--------  Test_Requeue  --------------------------------------
--  This procedure creates a task that reduces the Load to 3
--  and then to 1, and two arrays of tasks of type Urgent_Task
--  and Casual_Task having threshold values of 4 and 2,
--  respectively
--------------------------------------------------------------
with Ada.Text_IO; use Ada.Text_IO;
procedure Requeue_statements is
   package Load_Control is
      subtype Load_Type is Integer range 0..5;

      protected Load_Manager is
         entry Wait_Until_Load_Less_Than(Threshold : in Load_Type);
         procedure Set_Load(Load : in Load_Type);
      private
         Current_Load : Load_Type := 5;
         Retrying     : Boolean := False;
         entry Wait_To_Retry(Load : in Load_Type);
      end Load_Manager;
   end Load_Control;
   --------------------------------------------------------------
   package body Load_Control is

      protected body Load_Manager is
         ----------------------------------
         entry Wait_Until_Load_Less_Than(Threshold : in Load_Type)
         when not Retrying is
         begin
            if Current_Load > Threshold then
               requeue Wait_To_Retry;                -- requeue statement
            end if;
         end Wait_Until_Load_Less_Than;
         ----------------------------------
         procedure Set_Load(Load : in Load_Type) is
         begin
            Current_Load := Load;
            Retrying := True;
         end Set_Load;
         ----------------------------------
         entry Wait_To_Retry(Load : in Load_Type)
         when Retrying is
         begin
            if Wait_To_Retry'Count = 0 then         -- Count attribute
               Retrying := False;
            end if;
            requeue Wait_Until_Load_Less_Than;      -- requeue statement
         end Wait_To_Retry;
         ----------------------------------
      end Load_Manager;
   end Load_Control;


   --------------------------------------------
  task Load_Monitor;
  task body Load_Monitor is
  begin
    delay 4.0;
    Load_Control.Load_Manager.Set_Load(3);
    delay 2.0;
    Load_Control.Load_Manager.Set_Load(1);
  end Load_Monitor;
  --------------------------------------------
  task type Urgent_Task;
  task body Urgent_Task is                    -- Threshold = 4
  begin
    delay 2.0;
    Put_Line("Urgent_Task will get on queue");
    Load_Control.Load_Manager.Wait_Until_Load_Less_Than(4);
    Put_Line("Urgent task completed");
  end Urgent_Task;
  --------------------------------------------
  task type Casual_Task;
  task body Casual_Task is                    -- Threshold = 2
  begin
    Put_Line("Casual task will get on queue");
    Load_Control.Load_Manager.Wait_Until_Load_Less_Than(2);
    Put_Line("Casual task completed");
  end Casual_Task;
  --------------------------------------------
  UT : array(1..2) of Urgent_Task;            -- declare 2 tasks
  CT : array(1..2) of Casual_Task;            -- declare 2 tasks
  --------------------------------------------
begin
  null;
end Requeue_statements;
--------------------------------------------------------------
