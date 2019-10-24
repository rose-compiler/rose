// This test passes when compiled normally but is an error when used to 
// build a copy since the unparsed original AST is contaminated by the 
// copy of the AST.  The duplicate statements cause a problem for the 
// unparsed code.


// This test code demonstrates an error in the unparsing of the addressing operators
// As used in the SWIG generated code.  We have places a class definition from SWIG here
// to allow us to recreate the bug in as simple of a test code as possible.

#include<vector>

class SPref {
    int refs;

  // from_new stuff is based on the SPrep in an older version of SP.hh,
  // put here for backwards compatibility
  short int from_new;
  // if from_new =0, don't 'delete p' when the reference count goes to zero.
  void not_new() { from_new = 0; };  // substitute for older constructor,
  // which had the same signature but different meanings from the newer one

  // constructor for the new, cleaner SPref, from_new added:
  SPref( int _refs =1 ) : refs(_refs), from_new(1) {}

    template<class T> friend class SP;
};

template<class T>
class SP {
   T *p;
   SPref *r;

   void validate() const  { REQUIRE2( p, "No dumb pointer bound "
         "to this smart pointer." ); }

   void detach() {
      if ( --r->refs == 0) {
         if ( r->from_new )
         {
            delete p;
         }
         delete r;
      }
   }

   public:

    SP() : p(0) { r = new SPref; }

    SP( T *_p ) : p(_p) { r = new SPref; }

   // based on older constructor, for backwards compatibility:
    SP( T *_p, short int is_new ) : p(_p) {
      r = new SPref;
      if ( !is_new ) r->not_new();
   }

   template<class X>
    SP( X *px )
   {
      T *np = dynamic_cast<T *>( px );
      p = np;
      r = new SPref;
   }

    SP( const SP<T>& sp ) : p(sp.p), r(sp.r) { r->refs++; }

   ~SP() { detach(); }

   SP& operator=( T *np )
   {
      if (p == np) return *this; // It could happen.
      detach();
      p = np;
      r = new SPref(1);
      return *this;
   }

   template<class X>
   SP& operator=( X *px )
   {
      T *np = dynamic_cast<T *>( px );
      return *this = np;
   }

   SP& operator=( const SP<T>& sp )
   {
      if ( &sp == this ) return *this;
      if ( p == sp.p ) return *this;
      detach();
      p = sp.p;
      r = sp.r;
      r->refs++;
      return *this;
   }

   template<class X>
   SP& operator=( const SP<X>& spx )
   {
      X *px = spx.p;
      T *np = dynamic_cast<T *>( px );
      if (p == np) return *this;	// It could happen.
      detach();
      p = np;
      r = spx.r;
      r->refs++;
      return *this;
   }

   T *operator->() const { validate(); return p; }
   T& operator*() const  { validate(); return *p; }

   T *bp () const { return p; }

   int referenceCount() const { return r->refs; }

   operator bool() const { return p != 0; }
   bool operator!() const { return p == 0; }

   bool operator==( const T *pt ) const { return p == pt; }
   bool operator!=( const T *pt ) const { return p != pt; }

   bool operator==( const SP<T>& sp ) const { return p == sp.p; }
   bool operator!=( const SP<T>& sp ) const { return p != sp.p; }

   bool operator==( int ptr) const { return p == reinterpret_cast<T*>(ptr); }
   bool operator!=( int ptr) const { return p != reinterpret_cast<T*>(ptr); }

   template<class X> friend class SP;
};

template<class T>
bool operator==( const T *pt, const SP<T>& sp )
{
    return pt == sp.bp();
}

template<class T>
bool operator!=( const T *pt, const SP<T>& sp )
{
    return pt != sp.bp();
}


template<class T> class SwigValueWrapper
   {
     T *tt;
     public:
       // operator T&() const;
          T *operator&() { return tt; }
   };

class X
   {
     public:
          int size();
   };

template <typename T>
class Y {};

void foo()
   {
     int i = 0;
#if 0
// DQ (8/27/2005): temporarily commented out 
// JJW 5-22-2008 Commented this out again because it fails on Mac

     std::vector< SwigValueWrapper<X> > result;

  // Attempt ot demonstrate error that appears in Swig generated KULL file (but everything works).
  // SP<X> spObj ( *(new X (((std::vector<X> &)result)[i])) );
     SP<X> spObjx ( new X (((std::vector<X> &)result)[i]) );

     SP< Y<X> > spObjy ( new Y<X> (((std::vector< Y<X> > &)result)[i]) );

     SP< Y< Y<X> > > spObjyy ( new Y< Y<X> > (((std::vector< Y< Y<X> > > &)result)[i]) );
#endif
   }

