with Identity;

procedure Renamings is
  function id(local_val: in integer) return integer renames Identity.id_int;
  function id(local_val: in string) return string renames Identity.id_str;

  x : integer := id(0);

begin 
  null;
end Renamings;
