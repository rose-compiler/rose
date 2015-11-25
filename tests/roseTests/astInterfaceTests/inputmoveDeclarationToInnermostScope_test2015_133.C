// This test code demonstrate an unparsing error in the matching of #if to #endif when unparsing 
// from the AST and when using -rose:unparse_tokens

int foobar()
   {
     int x;
     int y;

     while ( true ) 
        {
#if 1
          x = 42;

          if ( true ) { }
            else 
             {
#if 1
                y  = 42;
#endif
             }
#endif
        }

     return 0;
   }


/*
This unparses to:
USING commandline: moveDeclarationToInnermostScope -rose:merge_decl_assign -c test2015_xx.C

int foobar()
{
  while(true){
    int x = 42;
    if (true) {
    }
     else {
      int y = 42;
#endif
    }
#if 1
#endif
  }
#if 1
  return 0;
}

 */

