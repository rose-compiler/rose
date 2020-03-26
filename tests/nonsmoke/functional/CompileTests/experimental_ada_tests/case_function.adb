function Case_Function(X : in Integer) return Integer is
  Result : Integer; 
begin
   case X is
     when 0        => Result := 0;
     when 1 | 3    => Result := 1;
     when 2 | 4..8 => Result := 2;
     when others   => Result :=3;
  end case;
  return Result;
end Case_Function;
