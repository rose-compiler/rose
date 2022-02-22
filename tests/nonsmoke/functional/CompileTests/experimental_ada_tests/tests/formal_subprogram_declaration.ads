package formal_subprogram_declaration is
  generic
    type Item is private;
    with function Func(X, Y : Item) return Item;
    with procedure Proc(X: Item);
  function test_function(X, Y: Item) return Item;
end formal_subprogram_declaration;
