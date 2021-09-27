procedure RangeV2 is
type BitsFake is  range -53 .. 191;
type Bits30 is  range 0 .. 1073741823;
for  Bits30'Size use 30;
type Signed_Bits26 is  range -33554432 .. 33554431;
for  Signed_Bits26'Size use 26;
begin
null;
end RangeV2;
