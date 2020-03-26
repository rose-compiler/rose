package Constants is

  Max_Entries       : constant Integer       := 400;        -- constant
  Avogadros_Number  : constant := 6.022137 * 10**23;  -- named number
  Bytes_Per_Page    : constant := 512;
  Pages_Per_Buffer  : constant := 10;
  Buffer_Size       : constant := Pages_Per_Buffer * Bytes_Per_Page;
  Buffer_Size10     : constant := 5_120;
  New_Character     : constant Character :='$';
  --~ Author            : constant := "George Dantzig";
end Constants;
