package Sub_Derived_Types is
  subtype smallSub is Integer range -10 .. 10;
  type smallDerived is new Integer range -10 .. 10;

  subtype bigSub is Integer;
  type bigDerived is new Integer;
end Sub_Derived_Types;
