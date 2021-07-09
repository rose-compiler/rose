-- from ACATS 2.6 tests
package Switches is  -- Switches

  type Toggle is tagged private;    ---------------------------------- Toggle

  procedure Flip ( It : in out Toggle );

private

  type Toggle is tagged record
    On : Boolean := False;
  end record;

end Switches;
