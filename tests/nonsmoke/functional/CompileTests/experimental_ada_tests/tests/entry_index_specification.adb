procedure entry_index_specification is

   protected Obj is
     entry Start(0..5);
   private
      Is_Set : Boolean := False;
   end Obj;

   protected body Obj is
     entry Start(for i in 0..5) when Is_Set is
     begin
       null;
     end;
   end Obj;
begin
  null;
end entry_index_specification;
