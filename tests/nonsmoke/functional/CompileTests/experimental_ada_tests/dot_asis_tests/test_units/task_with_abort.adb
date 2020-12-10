-- Note: This test does not yet work due to problems with
--  declaring tasks.  We can't abort without a task

--with Ada.Text_IO;

procedure Task_With_Abort is
  task AbortMe is
    entry Go;
  end AbortMe;

  task body AbortMe is
  begin
    accept Go;
    loop
       delay 1.0;
       --Ada.Text_IO.Put_Line("I'm not dead yet!");
    end loop;
  end AbortMe;

begin
  AbortMe.Go;
  delay 10.0;
  abort AbortMe;
  --Ada.Text_IO.Put_Line("Aborted AbortMe");
  delay 2.0;
end Task_With_Abort;

