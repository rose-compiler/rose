package body GotoTest is
  procedure P(x,y:Integer) is
  begin
    if x = y then  
      goto TheEnd;
    end if;

    <<TheEnd>> null;
  end P;
end GotoTest;
