-----------------------------------------------------------
--  This program asks the user to enter a character,
--  preferably a 'c' or a 't'. The resultant behavior
--  (conditional or timed) depends on the character entered
--  and the time it is entered.
-- Taken from http://www.cs.uni.edu//~mccormic/AdaEssentials/select_statement.htm
-----------------------------------------------------------
with Ada.Text_IO; use Ada.Text_IO;

procedure IO_Test is
  Ch : Character;
begin                                -- Caller executable part

  Put_Line("Enter c or t (Conditional or Timed)");
  Get(Ch);

  if Ch = 'c' then                   -- Conditional Entry Call
    null;
  elsif Ch = 't' then                -- Timed Entry Call
    Put_Line("Timed call withdrawn");
  else
    Put_Line("Invalid Input");
  end if;

end IO_Test;
