package Array_Declare_2  is 
    Array_1   : array (-3 .. 3) of Natural;
    Array_1_1 : array (-3 .. 3) of Natural := (others => 0);
    Array_1_2 : array (-3 .. 3) of Integer := 
      (-3       => -3, 
       -2 .. -1 => -2,
       others   => 0);
    Array_2  : array (Array_1'Range) of Natural := (1, 2, others => 0);

    type Constrained_Array_Type is array(-3 .. 3) of Natural;
    Array_3: Constrained_Array_Type;
    procedure print1(a: Constrained_Array_Type);

    type Unconstrained_Array_Type is array(Natural range <>) of Integer;
    procedure print2(a: Unconstrained_Array_Type);

    Array_4: Unconstrained_Array_Type(2 .. 5);
    Array_5: Unconstrained_Array_Type(12 .. 25);
end Array_Declare_2;
