int foobar()
{
#ifdef STKLIB
      int j ;
      return(j) ;
#else
      int i;
      for ( i = 0; i <= 10 ; i++ )
           i = 7 ;
      return 0;
#endif
}
