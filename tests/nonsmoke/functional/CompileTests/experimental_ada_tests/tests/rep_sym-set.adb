separate (rep_sym)
procedure Set (x : in out boolean; SSP : out SS_Ptr) is
begin
  -- last := x;
  x    := True;
  SSP := null;
end Set; 
