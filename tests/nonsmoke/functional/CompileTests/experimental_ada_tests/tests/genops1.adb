procedure genops1 is

  generic
    type T is (<>);
    with function F return T;
  procedure init(el:out T);

  procedure init(el:out T) is
  begin
    el := F;
  end init;

  type colors is (red);

  procedure initcolors is new init(colors,red);

begin
  null;
end genops1;
