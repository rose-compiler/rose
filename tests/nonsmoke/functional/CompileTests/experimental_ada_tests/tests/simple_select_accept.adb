-----------------------------------------------------------
--  This program asks the user to enter a character,
--  preferably a 'c' or a 't'. The resultant behavior
--  (conditional or timed) depends on the character entered
--  and the time it is entered.
-- Taken from http://www.cs.uni.edu//~mccormic/AdaEssentials/select_statement.htm
-----------------------------------------------------------

procedure Simple_Select_Accept(Ch : Character) is

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
      accept Do_That do
        null;
      end Do_That;
    or
      delay 5.0;
      null;
    end select;

    select                           -- Selective accept with terminate
      accept Do_this do
	null;
      end Do_This;
    or
      terminate;
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

end Simple_Select_Accept;
