procedure genops1 is

  generic
    type T is range <>;
    with function F(x : in T) return T;
  procedure init(el:out T);

  procedure init(el:out T) is
  begin
    el := F(0);
  end init;

  procedure initint is new init(integer, "+");

begin
  null;
end genops1;
