procedure Renaming_as_body is
  procedure dummy is
  begin
    null;
  end;

  procedure default;

  procedure default renames dummy;

begin
  null;
end;
