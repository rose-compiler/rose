void foobar()
   {
  // Unparses to: static char buf[sizeof(char [])];
#if 1
     static char buf[sizeof(".xxx.xxx.xxx.xxx")];
#else
     char* xxx = ".xxx.xxx.xxx.xxx";
     static char buf[sizeof(xxx)];
#endif
  // int constantFoldedValue = 1 + 2;
   }
