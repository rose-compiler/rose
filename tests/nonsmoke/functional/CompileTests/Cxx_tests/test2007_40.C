/*
Dan,

The small attached example is crashing ROSE.  It compiles with g++:

   g++ -c StrRot.cc

Parsing this will be critical to the work I want to do.

Thanks,
-Jeff
*/

#if 1

class View {
public:
   View() {}
   ~View() {}
   int x ;
   operator bool() const { return true ; }
} ;

class Field {
public:
   int x ;
   Field() {}
   ~Field() {}
} ;

Field operator+(Field x, Field y) { return Field() ; }
Field operator+(double x, Field y) { return Field() ; }
Field operator+(Field x, double y) { return Field() ; }

Field operator-(Field x, Field y) { return Field() ; }
Field operator-(double x, Field y) { return Field() ; }
Field operator-(Field x, double y) { return Field() ; }
Field operator-(Field x) { return Field() ; }

Field operator*(Field x, Field y) { return Field() ; }
Field operator*(double x, Field y) { return Field() ; }
Field operator*(Field x, double y) { return Field() ; }

Field operator/(Field x, Field y) { return Field() ; }
Field operator/(double x, Field y) { return Field() ; }
Field operator/(Field x, double y) { return Field() ; }

// #include "DummyVista.h"

void StressRotate(Field sxx, Field syy,
                  Field txy, Field txz, Field tyz,
                  Field wxx, Field wyy, Field wzz,
                  View material,
                  Field newSxx, Field newSyy, Field newSzz,
                  Field newTxy, Field newTxz, Field newTyz,
                  double deltaTime)
{
   while(material) {
      newSxx = sxx + deltaTime*( -2.*txy*wzz + 2.*txz*wyy ) ;
      newSyy = syy + deltaTime*(  2.*txy*wzz - 2.*tyz*wxx ) ;

      newSzz = - sxx - syy + deltaTime*( -2.*txz*wyy + 2.*tyz*wxx );
      newSzz = - sxx + (txy + wzz); // Error: make: *** [test2007_40.o] Segmentation fault
      newTxy = txy + deltaTime*( wzz*(    sxx - syy ) +  wyy*tyz - wxx*txz ) ;
      newTyz = tyz + deltaTime*( wxx*( 2.*syy + sxx ) +  wzz*txz - wyy*txy ) ;
      newTxz = txz + deltaTime*( wyy*( -syy - 2.*sxx ) + wxx*txy - wzz*tyz ) ;
   }
}

#endif


#if 0
class Field {};

Field operator+(Field x, Field y);
Field operator-(Field x);

void StressRotate(
   Field sxx,
   Field txy,
   Field wzz,
   Field newSzz )
   {
     newSzz = - sxx + (txy + wzz); // Error: make: *** [test2007_40.o] Segmentation fault
   }
#endif







