procedure Variant2 is

   type POWER is (GAS, STEAM, DIESEL, NONE);

   type VEHICLE (Engine : POWER := NONE) is
      record
         Model_Year : INTEGER range 1888..1992;
         Wheels     : INTEGER range 2..18;
         case Engine is
            when GAS    => Cylinders   : INTEGER range 1..16;
            when STEAM  => Boiler_Size : INTEGER range 5..22;
                           Coal_Burner : BOOLEAN;
            when DIESEL => Fuel_Inject : BOOLEAN;
            when NONE   => Speeds      : INTEGER range 1..15;
         end case;
      end record;

   Ford, Truck, Schwinn : VEHICLE;
   Stanley              : VEHICLE(STEAM);

begin

   Ford := (GAS, 1956, 4, 8);
   Ford := (DIESEL, 1985, Fuel_Inject => TRUE, Wheels => 8);

   Truck := (DIESEL, 1966, 18, TRUE);
   Truck.Model_Year := 1968;
   Truck.Fuel_Inject := FALSE;

   Stanley.Model_Year := 1908;    -- This is constant as STEAM
   Stanley.Wheels := 4;
   Stanley.Boiler_Size := 21;
   Stanley.Coal_Burner := FALSE;

   Schwinn.Speeds := 10;          -- This defaults to NONE
   Schwinn.Wheels := 2;
   Schwinn.Model_Year := 1985;

end Variant2;
