
int
main ( int argc, char* argv[] )
   {
  // Simple command line option --h or --help
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
     int optionCount = sla(&argc, argv, "--", "($)", "(h|help)",1);
     if( optionCount > 0 )
        {
          printf ("\nROSE (pre-release alpha version: %s) \n",VERSION);
          rose::usage(0);
          exit(0);
        }

  // option with parameter to option
     int integerOption = 0;
  // Use 1 at end of argument list to SLA to force removal of option from argv and decrement of agrc
     optionCount = sla(&argc, argv, "-rose:", "($)^", "test", &integerOption, 1);
     if( optionCount > 0 )
        {
          switch (integerOption)
             {
               case 0 :
                 // do something with "-rose:test 0" option
                    break;
               case 1 :
                 // do something with "-rose:test 1" option
                    break;
               default:
             }
        }
   }

.



