with Comm;

package Sender is
 
  XFATAL : constant Comm.Errorcode_Type := Comm.Errorcode_Type(-2);  
 
  procedure report(err : in Comm.Errorcode_Type);

end Sender;
