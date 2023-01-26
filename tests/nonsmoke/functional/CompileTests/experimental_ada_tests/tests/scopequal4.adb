procedure ScopeQual4 is

  package P is
    type Base is tagged null record;

    procedure Create(bs : out Base);
  end P;

  generic
     type Element is tagged private;
  package G is
     type Extended is new Element with null record;
  end G;

  package P2 is
    type Derived is new P.base with null record;
  end P2;

  package C is new G (Element => P2.Derived);

  package body P is
    procedure Create(bs : out Base) is
    begin
      null;
    end Create;
  end P;

  data : C.Extended;  -- Object of extended type.
begin
  C.Create(data);
end ScopeQual4;


