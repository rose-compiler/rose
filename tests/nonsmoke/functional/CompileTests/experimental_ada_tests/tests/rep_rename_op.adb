procedure Rep_Rename_Ops is
  package Universal_Types is
    type Unsigned_Integer_8 is range 0..255;
  end Universal_Types;

  function "=" (X, Y : UNIVERSAL_TYPES.Unsigned_Integer_8) return boolean
    renames UNIVERSAL_TYPES."="; 

  function "<=" (X, Y : UNIVERSAL_TYPES.Unsigned_Integer_8) return boolean
    renames UNIVERSAL_TYPES."<=";
 
  function "<" (X, Y : UNIVERSAL_TYPES.Unsigned_Integer_8) return boolean
    renames UNIVERSAL_TYPES."<";
 
  function ">" (X, Y : UNIVERSAL_TYPES.Unsigned_Integer_8) return boolean
    renames UNIVERSAL_TYPES.">";
begin
   null;
end Rep_Rename_Ops;
