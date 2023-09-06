with Rep_Integrity_Types;
package Rep_SC_Types is
 function "+" (Left  : Rep_Integrity_Types.Address;
               Right : Rep_Integrity_Types.Address)
         return Rep_Integrity_Types.Address renames Rep_Integrity_Types."+";  
 Last_Data_Buffer : constant Rep_Integrity_Types.Address := Rep_Integrity_Types."+"
              (Left  => Rep_Integrity_Types.DataBuffer,
               Right => Rep_Integrity_Types.LastBuffer);
--   Semaphore_Buffer : constant Rep_Integrity_Types.Address :=             Rep_Integrity_Types."+"
--              (Left  => Rep_Integrity_Types.Immediate,
--               Right => Rep_Integrity_Types.LastBuffer);
end Rep_SC_Types;
