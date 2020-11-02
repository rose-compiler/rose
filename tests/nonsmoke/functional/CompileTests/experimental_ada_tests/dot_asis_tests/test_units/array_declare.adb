-- Demonstrates:
--      Declaring arrays
--      Passing arrays as parameters
--      Aggregate assignment
--      Attribute: 'first, 'last, 'range
with ada.integer_text_io; use ada.integer_text_io; 
with ada.text_io; use ada.text_io; 
procedure array_declare  is 
    -- A CONSTRAINED array type.  Bounds can be any values
    type My_C_Array_T is array(-3 .. 3) of Natural;

    -- A named array type as a parameter
    procedure print1(a: My_C_Array_T) is 
    begin
        for i in -3 .. 3 loop
            put(a(i));
        end loop;
    end print1;


    -- An UNCONSTRAINED array type: bounds must be naturals
    type My_U_Array_T is array(Natural range <>) of Integer;

    -- A named array type as a parameter
    procedure print2(a: My_U_Array_T) is 
    begin
        for i in a'range loop
            put(a(i));
        end loop;
    end print2;


    -- Declare some variables - declare them here so they are not global
    -- a1 and a2 have anonymous types and can't be used in assignment or as parameters
    a1: array(-3 .. 3) of Natural := (others => 0);   -- Aggregate assignment
    a2: array(a1'range) of Natural := (others => 0);   -- Aggregate assignment

    -- a3 and a4 have a named array type 
    a3: My_C_Array_T;

    -- Different sizes, same named type
    a4: My_U_Array_T(2 .. 5) := (others => 0);
    a5: My_U_Array_T(12 .. 25) := (others => 0);

begin
    -- Print a1 and a2:
    for i in a1'range loop
        put(a1(i));
        put(a2(i));
        new_line;
    end loop;

    a3 := (-2 | 2 => 1, others => 0);   -- Aggregate assignment

    print1(a3);
    print2(a4);
    print2(a5);
end array_declare;
