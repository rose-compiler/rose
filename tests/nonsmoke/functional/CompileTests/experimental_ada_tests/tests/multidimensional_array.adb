package body Multidimensional_Array is
    procedure test(f: FullTime; p : PartTime; a : Afternoons) is
      x : Fulltime := (others => (others => false));
      y : Afternoons := ((false, false, false, false), (false, false, false, true), (false, false, true, false), (false, true, false, false));
    begin
      x(1,1) := f(2,2);
    end test;
end Multidimensional_Array;
