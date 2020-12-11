--Just testing all the procedure pragmas I could think of.
Procedure Pragmas is 
  pragma List(Off);
  pragma Optimize(Off);

  Procedure TryInline is
    Foo : Integer := 0;
  begin
     null;
  end;
  pragma Inline(TryInline);

begin
  null;
end;

