with Ada.Characters.Handling;

function UpperCase (L, R : String) return String is
  res : String (L'First .. L'Last);
begin
  res := Ada.Characters.Handling.To_Upper (L);
  return res;
end UpperCase;
   
