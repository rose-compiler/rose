void foobar()
   {
  // BUG: Should be unparsed as: switch (constexpr int i = 42); but we are loosing the constexpr and just getting const.
  // However, I am unclear if this is not semantically equivalent.
     switch (constexpr int i = 42);
   }

