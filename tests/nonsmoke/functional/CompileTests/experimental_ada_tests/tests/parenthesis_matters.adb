
procedure Parenthesis_Matter is
  subtype SmallIntType is Integer range 0..12; 

  function SmallIntVal return SmallIntType is
  begin
    return 0;
  end SmallIntVal;

begin
  case SmallIntVal is
    when 0 => null;
    -- when 13 => null; -- error
    when others => null;
  end case;

  case (SmallIntVal) is
    when 0 => null;
    when 13 => null; -- no error
    when others => null;
  end case;
end Parenthesis_Matter;
