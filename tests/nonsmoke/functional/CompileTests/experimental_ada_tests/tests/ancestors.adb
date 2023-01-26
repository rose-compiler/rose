
procedure ancestors is
  package Y is
    type AbstrT is abstract tagged null record;

    procedure init (el : in out AbstrT);
  end Y;

  type InterT is new Y.AbstrT with null record;

  procedure init (elem: in out InterT) is null;

  package Z is
     type ConcrT is new Y.AbstrT with private;
  private
     type ConcrT is new InterT with null record;
  end Z;

  package body Y is
    procedure init (el : in out AbstrT) is null;
  end Y;

  obj : Z.ConcrT;
begin
  -- the following call uses ConcrT's public ancestor type (AbstrT) for resolving
  --   a named argument position.
  -- the callee itself will be resolved through ConcrT's actual parent (InterT).
  Z.init (el => obj);
end ancestors;
