-- same as simple_select_accept, with two select cases decorated with
-- when conditions in the task body.
procedure Simple_Select_Accept_Guarded(Ch : Character) is

  task Callee is
    entry Do_This;
    entry Do_That;
    entry Do_Nothing;
  end Callee;

  task body Callee is
  begin

    delay 5.0;

    select                           -- Selective Accept
      accept Do_This do
        null;
      end Do_This;
    or
      when True =>
      accept Do_That do
        null;
      end Do_That;
    or
      when 1 /= 2 =>
      delay 5.0;
      null;
    end select;

    accept Do_Nothing;
  end Callee;

begin                                -- Caller executable part

  if Ch = 'c' then                   -- Conditional Entry Call
    select
      Callee.Do_This;
    else
      null;
    end select;

  elsif Ch = 't' then                -- Timed Entry Call
    select
      Callee.Do_That;
    or
      delay 3.0;
      null;
    end select;

  else
    null;
  end if;

  Callee.Do_Nothing;                 -- Both tasks can quit

end Simple_Select_Accept_Guarded;
