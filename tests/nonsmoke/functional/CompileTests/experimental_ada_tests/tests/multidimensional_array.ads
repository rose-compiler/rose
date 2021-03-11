package Multidimensional_Array is
    subtype WorkWeek is Natural range 1 .. 5;
    subtype WorkHours is Natural range 1 .. 8;

    type FullTime is array(WorkWeek, WorkHours) of Boolean;
    type PartTime is array(WorkWeek range<>, WorkHours range<>) of Boolean;
    type Afternoons is new PartTime(1..5, 5..8);

    procedure test(f: FullTime; p : PartTime; a : Afternoons);
end Multidimensional_Array;
