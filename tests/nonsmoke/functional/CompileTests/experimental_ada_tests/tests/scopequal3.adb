
procedure ScopeQual3 is

  package P is
    type Base is tagged null record;

    procedure Create(bs : out Base);
  end P;

  generic
     type Element is tagged private;
  package G is
     type Extended is new Element with null record;
  end G;

  package C is new G (Element => P.Base);

  package body P is
    procedure Create(bs : out Base) is null;
  end P;

  data : C.Extended;  -- Object of extended type.
begin
  C.Create(data);
end ScopeQual3;


