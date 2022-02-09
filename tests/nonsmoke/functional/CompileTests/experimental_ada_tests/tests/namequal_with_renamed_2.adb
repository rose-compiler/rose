with Ada.Text_IO.Editing;

procedure Namequal_With_Renamed_2 is
    type Decimal_Data_Type is delta 0.01 digits 16;

    package Ed_Out is new Ada.Text_IO.Editing.Decimal_Output (Decimal_Data_Type);
begin
  null;
end Namequal_With_Renamed_2;
