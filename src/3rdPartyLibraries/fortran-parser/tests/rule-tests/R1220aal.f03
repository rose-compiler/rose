! R1220aal.f03 actual-arg-spec
a = foo(1)(keyword=.not.B)
a = foo(1)(keyword = b)
a = foo(1)(keyword = udt%bar)
a = foo(1)(keyword=*10)
end
