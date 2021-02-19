bool foobar()
   {
#if 1
  // This fails with the outliner (because the normalization is not sufficent).
     bool ab_var;
     ab_var = true;
     return ab_var;
#else
  // This works with the outliner, and is what the return normalization should generate..
     bool ab_var;
     bool tmp_return_value;
     ab_var = true;
     tmp_return_value = ab_var;
     return tmp_return_value;
#endif
   }

