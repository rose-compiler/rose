package Sub_Derived_Types is
  subtype smallSub is Integer range -10 .. 10;
  type smallDerived is new Integer range -10 .. 10;
  type smallRange is range -10 .. 10;

  subtype bigSub is Integer;
  type bigDerived is new Integer;
  type bigRange is range -2**10 .. 2**10;
end Sub_Derived_Types;
