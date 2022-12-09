procedure enum_routine is
  package E is
    type counter is (zero, one, two, three, four);

    procedure init(x : out counter);
  end E;

  package body E is
    procedure init(x : out counter) is
    begin
      x := zero;
    end init;
  end E;

  type C is new E.counter;

  ctr : C;
begin
  init(ctr);
end enum_routine;
