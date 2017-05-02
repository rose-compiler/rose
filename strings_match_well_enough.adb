with Ada.Strings.Fixed;
with Ada.Strings.Maps.Constants;

function Strings_Match_Well_Enough (L, R : in String) return Boolean is
    -- true for non-case-sensitive match after lead and trail blanks trimmed
    use Ada.Strings.Fixed;
    use Ada.Strings;
    use Ada.Strings.Maps.Constants;
begin
    return Trim (Translate (L, Lower_Case_Map), Both) =
	      Trim (Translate (R, Lower_Case_Map), Both);
end Strings_Match_Well_Enough;
