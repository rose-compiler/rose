// This is an interesting case of where EDG reports that cases 1-4 are unreachable.

double H0( double v1, double v2, double v3 );
double H1( double v1, double v2, double v3 );
double H2( double v1, double v2, double v3 );
double H3( double v1, double v2, double v3 );
double H4( double v1, double v2, double v3 );

double
H( int dof, double v1, double v2, double v3 )
   {
     switch(dof)
        {
          case 0: return H0( v1, v2, v3); break;
          case 1: return H1( v1, v2, v3); break;
          case 2: return H2( v1, v2, v3); break;
          case 3: return H3( v1, v2, v3); break;
          case 4: return H4( v1, v2, v3); break;
          default: // throw ("Invalid shape function.");
        };
   }
