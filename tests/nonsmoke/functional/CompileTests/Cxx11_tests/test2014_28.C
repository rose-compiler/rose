
// This will currently be unparsed as: "int f(int p)"
auto f(int p) -> decltype(p)
   {
     return p;
   }

