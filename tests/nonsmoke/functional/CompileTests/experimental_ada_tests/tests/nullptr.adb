

procedure Nullptr is
  type PInt is access Integer;

  procedure SetZero(pi : PInt) is
  begin
    if pi = null then 
      return;
    end if;

    pi.all := 0;
  end SetZero;

  x : PInt := null;

begin
  x := new Integer'(1);
  SetZero(x);
end Nullptr;
