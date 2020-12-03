-- This code is for compiler front end testing only. It does nothing at run time.
procedure Requeue_Statement_2 is

   protected Protected_1 is
      entry Entry_1;
      entry Entry_2;
   private
      Barrier_Open : Boolean := False;
   end Protected_1;

   protected body Protected_1 is
      entry Entry_1
        when Barrier_Open is
      begin
         requeue Entry_1;
      end Entry_1;

      entry Entry_2
        when Barrier_Open is
      begin
         requeue Entry_2 with abort;
      end Entry_2;
   end Protected_1;
begin
   null;
end Requeue_Statement_2;
--------------------------------------------------------------
