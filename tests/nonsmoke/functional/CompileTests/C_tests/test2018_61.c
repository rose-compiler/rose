void foo()
   {
  // BUG: This is unparsed as: "union union_tag1 foobar();"
     union union_tag1 { int a; } foobar();
   }
