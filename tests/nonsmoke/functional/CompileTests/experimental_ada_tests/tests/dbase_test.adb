
with DBase;

procedure Dbase_Test is
  DB_Rec1:  DBase.DB_Record := (Key => 1, Data => "aaaaaaaaaa");
  DB_Rec2:  DBase.DB_Record := (Key => 55, Data => "aaaaaaaaaa");

  use type Dbase.DB_Record;
begin
  if not (DB_Rec1 = DB_Rec2) then
    null;
  end if;
  if DB_Rec1 /= DB_Rec2 then
    null;
  end if;
end Dbase_Test;
