procedure ASCII_USER is
  A0 : constant Character := ASCII.NUL;
begin
  for I in Character'('A') .. Ascii.DEL loop
    null;
  end loop;
end;
