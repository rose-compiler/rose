 
 
extern void __assert_fail (const char *__assertion, const char *__file, unsigned int __line, const char *__function) throw () __attribute__ ((__noreturn__)); 

void foobar()
   {
     __PRETTY_FUNCTION__;
     __func__;
  // ((1 == 1) ? static_cast<void> (0) : __assert_fail ("1 == 1", "test2001_03.C", 22, __PRETTY_FUNCTION__));
     __assert_fail ("1 == 1", "test2001_03.C", 22, __PRETTY_FUNCTION__);
   }

