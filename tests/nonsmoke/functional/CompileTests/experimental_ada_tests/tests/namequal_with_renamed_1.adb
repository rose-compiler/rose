PROCEDURE Namequal_With_Renamed_1 IS
    PACKAGE Inner IS
         PACKAGE I RENAMES Inner;
         TYPE Number IS NEW INTEGER;
         TYPE Counter IS NEW Number;
    END Inner;
BEGIN
  NULL;
END Namequal_With_Renamed_1;
