template <typename loop_type>
void for_all (loop_type)
{
  static_assert (true, "no match");   
}

void foo(int nslide, int min1, int max1, int* ndx, int offn, int offt1,
         int start, int len, int type, int ISLIDE, int JSLIDE,
        double * ax31, double * ay31, double* ax42, double* ay42,
        double* xacc, double* yacc)
{
  for ( int nsld = 0 ; nsld < nslide ; nsld++ ) {

    const int min1  = min1 ;
    const int max1  = max1 ;
    const int* ndx  = ndx   ;
    const int offn  = offn  ;
    const int offt  = offt1 ;
    double* xacct   = xacc  + start ;
    double* yacct   = yacc  + start ;
    double* xaccb   = xacct + len ;
    double* yaccb   = yacct + len ;

    const int offnt = offn + offt ;

    if ( type == ISLIDE ) {

      for ( int i = min1 ; i <= max1 ; i++ ) {

        xacct[i] -= ( ax31[ndx[i]]      + ax42[ndx[i]-offt]  ) ;
        yacct[i] -= ( ay31[ndx[i]]      - ay42[ndx[i]-offt]  ) ;
        xaccb[i] += ( ax42[ndx[i]-offn] + ax31[ndx[i]-offnt] ) ;
        yaccb[i] -= ( ay42[ndx[i]-offn] - ay31[ndx[i]-offnt] ) ;

      }

    } 

    if ( type == JSLIDE ) {

      for ( int i = min1 ; i <= max1; i++ ) {

        xacct[i] -= ( ax31[ndx[i]]      - ax42[ndx[i]-offt]  ) ;
        yacct[i] -= ( ay31[ndx[i]]      + ay42[ndx[i]-offt]  ) ;
        xaccb[i] -= ( ax42[ndx[i]-offn] - ax31[ndx[i]-offnt] ) ;
        yaccb[i] += ( ay42[ndx[i]-offn] + ay31[ndx[i]-offnt] ) ;

      }

    } 
  } 
}
