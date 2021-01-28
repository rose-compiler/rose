procedure  Entry_Body_Declaration is

   protected Obj is
     entry Start;
   private
      Is_Set : Boolean := False;
   end Obj;

   protected body Obj is
     entry Start when Is_Set is
     begin
       null;
     end;
   end Obj;
begin
  null;
end Entry_Body_Declaration;
