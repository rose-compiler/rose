package body rep_sym is
  type SS; 
  type SS_Ptr is access SS; 
  type SS is null record; 
--    record 
--      SS_Data : boolean; 
--      Next    : SS_Ptr; 
--    end record; 
 
  procedure Set (x : in out boolean) is 
    begin 
      x := True; 
    end Set; 
 
  procedure Set (x : in out boolean; SSP : out SS_Ptr) is separate;   
end rep_sym; 

