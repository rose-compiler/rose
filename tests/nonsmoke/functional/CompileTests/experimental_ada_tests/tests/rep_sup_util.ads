
package Rep_Sup_Util is
   procedure Move_It (Source_Address : Integer;
                      Target_Address : Integer;
                      Byte_Count     : Natural);
   pragma Interface (Unchecked,  Move_It);
   pragma Interface_Name (Move_It, "XXXXX");
   procedure Temp2;
end Rep_Sup_Util; 
