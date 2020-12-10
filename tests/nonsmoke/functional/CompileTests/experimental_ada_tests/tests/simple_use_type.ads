
with SimpleAda;

package Simple_Use_Type is
  use type SimpleAda.Device;
  
  stdcout : constant SimpleAda.Device := 0;
  stdcerr : constant SimpleAda.Device := 1;
  stdcurr : SimpleAda.Device := stdcerr;

end Simple_Use_Type;
