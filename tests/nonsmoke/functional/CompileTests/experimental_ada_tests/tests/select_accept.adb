-----------------------------------------------------------
--  This program asks the user to enter a character,
--  preferably a 'c' or a 't'. The resultant behavior
--  (conditional or timed) depends on the character entered
--  and the time it is entered.
-- Taken from http://www.cs.uni.edu//~mccormic/AdaEssentials/select_statement.htm
-----------------------------------------------------------
with Ada.Text_IO; use Ada.Text_IO;
procedure Select_Accept is

  Ch : Character;

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
        Put_Line("Do_This accepted and service provided");
      end Do_This;
    or
      accept Do_That do
        Put_Line("Do_That accepted and service provided");
      end Do_That;
    or
      delay 5.0;
      Put_Line("Callee no longer waiting");
    end select;

    accept Do_Nothing;
    Put_Line("Both tasks shutting down");
  end Callee;

begin                                -- Caller executable part

  Put_Line("Enter c or t (Conditional or Timed)");
  Get(Ch);

  if Ch = 'c' then                   -- Conditional Entry Call
    select
      Callee.Do_This;
    else
      Put_Line("Conditional call not accepted");
    end select;

  elsif Ch = 't' then                -- Timed Entry Call
    select
      Callee.Do_That;
    or
      delay 3.0;
      Put_Line("Timed call withdrawn");
    end select;

  else
    Put_Line("Invalid Input");
  end if;

  Callee.Do_Nothing;                 -- Both tasks can quit

end Select_Accept;
