-- from ACATS tests
with Switches;

procedure HardwareStore is -- Hardware_Store

  subtype Switch_Class is Switches.Toggle'Class;

  type Reference is access all Switch_Class;
  type Light_Bank is array(Positive range <>) of Reference;

  A_Switch : aliased Switches.Toggle;
  Lamps    : Light_Bank(1..1) := ( others => new Switches.Toggle );

begin  -- Main test procedure.
  Switches.Flip( A_Switch );
  Switches.Flip( Lamps(1).all );
end HardwareStore;
