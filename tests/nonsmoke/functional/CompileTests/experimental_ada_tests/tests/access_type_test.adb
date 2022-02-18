procedure access_type_test is
  procedure Print(pi : access Integer) is
  begin
    null;
  end print;

  type Pool_Pointer     is access          Integer;
  type General_Pointer  is access all      Integer;
  type Constant_Pointer is access constant Integer;

  I1: aliased constant Integer := 1;
  I2: aliased Integer := 2;

  -- P1: General_Pointer  := I1'Access;  -- illegal
  P2: Constant_Pointer := I1'Access;  
  P3: General_Pointer  := I2'Access;  
  P4: Constant_Pointer := I2'Access;  

  P5: constant General_Pointer := I2'Access;  
  
  P6: Pool_Pointer := new Integer'(3);
begin
  -- Print(P2);
  Print(P3);
  Print(P5);
  Print(P6);  
end access_type_test;


