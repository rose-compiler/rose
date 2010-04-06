/*
Contributed by Jeff Keasler

Liao 2/16/2010
*/

typedef double real8 ;

class ZZ {
public:
   int len ;
   int *values ;
} ;

class YY {
public:
   class YY *next ;
   class ZZ *bb ;
} ;

class XX {
public:
   class YY *aa[7] ;
} ;

void foo(XX *xx)
{
  int i, b ;
  YY *aa ;
  real8 *v1, *v2, *v3, *v4, *v5, *v6 ;

  for(b=0 ; b<9 ; b++) {
    real8 *ddb = 0, *dda = 0;

    switch(b) {
      case 0: aa = xx->aa[0] ; break ;
      case 1: aa = xx->aa[1] ; break ;
      case 2: aa = xx->aa[2] ; break ;
      case 3: aa = xx->aa[3] ; break ;
      case 4: aa = xx->aa[4] ; break ;
      case 5: aa = xx->aa[5] ; break ;
      case 6: aa = xx->aa[6] ; break ;
      case 7: aa = xx->aa[6] ; break ;
      case 8: aa = xx->aa[6] ; break ;
      default: /* do nothing */ ;
    }

    switch(b) {
      case 0: case 3: case 6:
        dda = v1 ; ddb = v2 ; break ;
      case 1: case 4: case 7:
        dda = v3 ; ddb = v4 ; break ;
      case 2: case 5: case 8:
        dda = v5 ; ddb = v6 ; break ;
    }
    while(aa) {

      for(i=0 ; i<aa->bb->len ; i++) {
        int idx = aa->bb->values[i] ;

        ddb[idx] = dda[idx] ;
      }
      aa = aa->next ;
    }
  }

  return ;
}

