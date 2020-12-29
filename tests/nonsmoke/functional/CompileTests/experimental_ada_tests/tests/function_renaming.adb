procedure Function_Renaming is
  function Increment (I : Integer) return Integer is
  begin
    return I + 1;
  end Increment;

  function RenamedIncrement (NewI : Integer) return Integer renames Increment;
begin
  null;
end Function_Renaming;
