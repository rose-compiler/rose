// define include guards (SIMPLE_APP_HEADER_FILE)
#ifndef SIMPLE_APP_HEADER_FILE
#define SIMPLE_APP_HEADER_FILE

#define APP_RESTRICT

double sqrt ( double rhs);
double fabs( double lhs );
double pow( double lhs, double );
double sin( double);
void assert(bool);

class ios {
public:
 static void sync_with_stdio();
};

void printf( char* format, ...);


class InternalIndex
   {
     public:
       // member function declarations
          int base;
          int bound;
          int stride;
          int length;

          InternalIndex();
          InternalIndex(int n);
          InternalIndex(int base, int bound);
          InternalIndex(int base, int bound, int stride);

       // operators for InternalIndex objects
       // This is consistant with A++/P++ InternalIndex (Index and Range) objects
          friend InternalIndex operator+ ( const InternalIndex & lhs, int x );
          friend InternalIndex operator+ ( int x, const InternalIndex & rhs );
          friend InternalIndex operator- ( const InternalIndex & lhs, int x );
          friend InternalIndex operator- ( int x, const InternalIndex & rhs );
   };

enum {ON, OFF};
// Support for A++/P++ Index and Range objects
class Index : public InternalIndex
   {
     public:
          Index();
          Index(int n);
          Index(int base, int length);
          Index(int base, int length, int stride);

        void static setBoundsCheck(int);
   };
class Range : public InternalIndex
   {
     public:
          Range();
          Range(int n);
          Range(int base, int bound);
          Range(int base, int bound, int stride);
   };

class DomainType
   {
     public:
       // data member declarations
          int Size  [6];
          int Stride[6];
       // member function declarations
          int getBase(int);
          int getBound(int);
          int getLength(int);
   };
class DescriptorType 
   { 
     public:
       // data member declarations 
          DomainType Array_Domain;
          double* APP_RESTRICT Array_View_Pointer1;
          double* APP_RESTRICT Array_View_Pointer2;
          double* APP_RESTRICT Array_View_Pointer3;
          double* APP_RESTRICT Array_View_Pointer4;
          double* APP_RESTRICT Array_View_Pointer5;
          double* APP_RESTRICT Array_View_Pointer6;
   };
class intArray 
   { 
     public: 
       // data member declarations 
          DescriptorType Array_Descriptor; 
       // member function declarations 
          intArray( int);
          int getBase(int);  
          int getBound(int); 
          int getLength( int dim);
       // declaration of indexing operators
          intArray& operator()( const InternalIndex & I );
          intArray& operator()( const InternalIndex & I, const InternalIndex & J );
          intArray& operator()( const InternalIndex & I, const InternalIndex & J,
                                  const InternalIndex & K );
          intArray& operator()( const InternalIndex & I, const InternalIndex & J,
                                  const InternalIndex & K, const InternalIndex & L );

   };
class doubleArray
   {
     public:
       // data member declarations
          DescriptorType Array_Descriptor;
       // member function declarations
          int getBase(int);
          int getBound(int);

       // destructor and constructors
         ~doubleArray ();
          doubleArray ();
          doubleArray (int i);
          doubleArray (int i, int j);
          doubleArray (int i, int j, int k);
          doubleArray (int i, int j, int k, int l);
          doubleArray (int i, int j, int k, int l, int m);
          doubleArray (int i, int j, int k, int l, int m, int n);

       // Indexing operators
          double & operator[](int n);

        friend intArray operator <= ( const doubleArray& lhs, double rhs);
        friend intArray operator <= ( double lhs, const doubleArray& rhs);
        friend intArray operator <= ( const doubleArray& lhs, const doubleArray& rhs);
        friend intArray operator >= ( const doubleArray& lhs, double rhs);
        friend intArray operator >= ( double lhs, const doubleArray& rhs);
        friend intArray operator >= ( const doubleArray& lhs, const doubleArray& rhs);

       // declaration of indexing operators
          doubleArray& operator()( const InternalIndex & I );
          doubleArray& operator()( const InternalIndex & I, const InternalIndex & J );
          doubleArray& operator()( const InternalIndex & I, const InternalIndex & J,
                                  const InternalIndex & K );
          doubleArray& operator()( const InternalIndex & I, const InternalIndex & J,
                                  const InternalIndex & K, const InternalIndex & L );

       // declaration of scalar indexing operators
          double & operator()( int i );
          double & operator()( int i, int j );
          double & operator()( int i, int j, int k );
          double & operator()( int i, int j, int k, int l );

          doubleArray & operator- ();

       // example array operators
          doubleArray & operator= ( double x );
          doubleArray & operator= ( const doubleArray & X );
          friend doubleArray & operator+ ( const doubleArray & lhs, const doubleArray & rhs);
          friend doubleArray & operator+ ( double lhs, const doubleArray & rhs);
          friend doubleArray & operator+ ( const doubleArray & lhs, double rhs);
          friend doubleArray & operator- ( const doubleArray & lhs, const doubleArray & rhs);
          friend doubleArray & operator- ( const doubleArray & lhs, double rhs);
          friend doubleArray & operator* ( const doubleArray & lhs, const doubleArray & rhs);
          friend doubleArray & operator* ( double lhs, const doubleArray & rhs);
          friend doubleArray & operator* ( const doubleArray & lhs, double rhs);
          friend doubleArray & operator/ ( const doubleArray & lhs, const doubleArray & rhs);
          friend doubleArray & operator/ ( const doubleArray & lhs, double rhs);
          friend doubleArray & operator/ ( double lhs, const doubleArray & rhs);
          friend doubleArray & sqrt ( const doubleArray & rhs);
          friend doubleArray & pow (const doubleArray& lhs, double rhs);
          friend doubleArray & pow (const doubleArray& lhs, const doubleArray& rhs);

          friend doubleArray & sin ( doubleArray & rhs);

          double* getAdjustedDataPointer();
          double* getDataPointer();
          int getLocalBase(int dim);
          int getLength( int dim);

          doubleArray & redim ( doubleArray & X );
          doubleArray & resize ( int i );

          void fill( double );
          void replace( const intArray& cond, double val);
          void replace( const intArray& cond, const doubleArray& val);
   };

class floatArray
   {
     public:
       // data member declarations
          DescriptorType Array_Descriptor;
       // member function declarations
          int getBase(int);
          int getBound(int);

       // destructor and constructors
         ~floatArray ();
          floatArray ();
          floatArray (int i);
          floatArray (int i, int j);
          floatArray (int i, int j, int k);
          floatArray (int i, int j, int k, int l);
          floatArray (int i, int j, int k, int l, int m);
          floatArray (int i, int j, int k, int l, int m, int n);

       // Indexing operators
          float & operator[](int n);

        friend intArray operator <= ( const floatArray& lhs, float rhs);
        friend intArray operator <= ( float lhs, const floatArray& rhs);
        friend intArray operator <= ( const floatArray& lhs, const floatArray& rhs);
        friend intArray operator >= ( const floatArray& lhs, float rhs);
        friend intArray operator >= ( float lhs, const floatArray& rhs);
        friend intArray operator >= ( const floatArray& lhs, const floatArray& rhs);

       // declaration of indexing operators
          floatArray& operator()( const InternalIndex & I );
          floatArray& operator()( const InternalIndex & I, const InternalIndex & J );
          floatArray& operator()( const InternalIndex & I, const InternalIndex & J,
                                  const InternalIndex & K );
          floatArray& operator()( const InternalIndex & I, const InternalIndex & J,
                                  const InternalIndex & K, const InternalIndex & L );

       // declaration of scalar indexing operators
          float & operator()( int i );
          float & operator()( int i, int j );
          float & operator()( int i, int j, int k );
          float & operator()( int i, int j, int k, int l );

          floatArray & operator- ();

       // example array operators
          floatArray & operator= ( float x );
          floatArray & operator= ( const floatArray & X );
          friend floatArray & operator+ ( const floatArray & lhs, const floatArray & rhs);
          friend floatArray & operator+ ( float lhs, const floatArray & rhs);
          friend floatArray & operator+ ( const floatArray & lhs, float rhs);
          friend floatArray & operator- ( const floatArray & lhs, const floatArray & rhs);
          friend floatArray & operator- ( const floatArray & lhs, float rhs);
          friend floatArray & operator* ( const floatArray & lhs, const floatArray & rhs);
          friend floatArray & operator* ( float lhs, const floatArray & rhs);
          friend floatArray & operator* ( const floatArray & lhs, float rhs);
          friend floatArray & operator/ ( const floatArray & lhs, const floatArray & rhs);
          friend floatArray & operator/ ( const floatArray & lhs, float rhs);
          friend floatArray & operator/ ( float lhs, const floatArray & rhs);
          friend floatArray & sqrt ( const floatArray & rhs);
          friend floatArray & pow (const floatArray& lhs, float rhs);
          friend floatArray & pow (const floatArray& lhs, const floatArray& rhs);

          friend floatArray & sin ( floatArray & rhs);

          float* getAdjustedDataPointer();
          float* getDataPointer();
          int getLocalBase(int dim);
          int getLength( int dim);

          floatArray & redim ( floatArray & X );
          floatArray & resize ( int i );

          void fill( float );
          void replace( const intArray& cond, float val);
          void replace( const intArray& cond, const floatArray& val);
   };


class Optimization_Manager {
public:
 static void Initialize_Virtual_Machine (char* ,int nprocs,int argc,char** argv);
 static void Exit_Virtual_Machine ();
};

void APP_ABORT();

// endif for SIMPLE_APP_HEADER_FILE (include guard)
//#include "/home/qingyi/A++P++/A++/include/where.h"
#endif

