procedure abstract_fun is

  type AbstractPoint is abstract tagged null record;

  function x(p : in AbstractPoint) return integer is abstract;

begin
  null;
end abstract_fun;
