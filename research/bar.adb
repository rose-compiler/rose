procedure Bar is
   procedure Foo;
   pragma Import (C, Foo);
begin
   Foo;
end Bar;
