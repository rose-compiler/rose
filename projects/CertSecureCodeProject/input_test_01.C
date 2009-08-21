int rand();

int
main()
   {
  // Violation warning: EXP12-C: Do not ignore values returned by functions: rand
     rand();

     return 0;
   }
