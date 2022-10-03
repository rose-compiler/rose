with M_Types;
use  M_Types;

package M_Types_Test is
   function "=" (X: M_Types.selectors; Y: M_Types.selectors) return Boolean renames M_Types."=";
end M_Types_Test; 
