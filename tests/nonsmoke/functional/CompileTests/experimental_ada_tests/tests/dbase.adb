package body Dbase is
  function "="(L, R:  in DB_Record) return Boolean is
  begin
    return L.Data = R.Data;
  end "=";
end Dbase;
