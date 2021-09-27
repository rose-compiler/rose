procedure RangeV2 is
    
  type BitsFake is range -(123-35*2) .. 6*(2 + 30) - 1;

  type Bits30 is range 0 .. (2 ** 30) - 1;
  for Bits30'Size use 30;

  type Signed_Bits26 is range -2 ** 25 .. (2 ** 25) - 1;
  for Signed_Bits26'Size use 26;

begin
   null;
end RangeV2;

