// structured bindings

auto f() -> int(&)[2];
auto [ x, y ] = f();   // x and y refer to elements in a copy of the array return value
auto& [ xr, yr ] = f();  // xr and yr refer to elements in the array referred to by f's return value


