// This is the simple version of laplacian.cpp that builds 
// the smallest reasonable AST.  This simpler AST make it 
// easier to work with in the context of understanding 
// the use of the DSL and the interpretation of it's semantics 
// to implement the code generation for the DSL.

// This is the second simple test that applies a Laplacian operator
// with 2*DIM+1 size stencil with one ghost cell along each face
// of the data box. Thus the size of the destination box is 
//BLOCKSIZE^DIM and the size of the source box is (BLOCKSIZE+2)^DIM
//FOR these tests BLOCKSIZE=DOMAINSIZE because we are working with 
//a single box. BLOCKSIZE, DOMAINSIZE and DIM are set in the Makefile

#ifdef __cplusplus
   #warning "__cplusplus IS defined as: "__cplusplus
#if __cplusplus < 201103L
   #warning "__cplusplus IS less than 201103L"
   #if __cplusplus < 199711L
      #warning "__cplusplus IS less than 199711L"
   #else
      #warning "__cplusplus is NOT less than 199711L"
   #endif
#else
   #warning "__cplusplus is NOT less than 201103L"
   #if __cplusplus == 199711L
      #warning "__cplusplus IS equal to 199711L"
   #else
      #warning "__cplusplus is NOT equal to 199711L"
      #if __cplusplus == 1L
         #warning "__cplusplus IS equal to 1L"
      #else
         #warning "__cplusplus is NOT equal to 1L"
      #endif
   #endif
#endif
#else
   #warning "__cplusplus in NOT defined"
#endif

#error "Exiting as a test!"

#define BLOCKSIZE 32
#define DIM       3
#define assert(x)

#include <vector>
#include <memory>

// DQ (2/3/2015): This is not required for g++ using c++11 mode, but is required for ROSE.
#include <array>

// #include "Point.H"
using namespace std;

class Point
   {
     public:
      /// Default Constructor
          Point();
      /// Constructor
          Point(const int a_tuple[DIM]);
          inline Point(const array<int,DIM> a_tuple);
      /// Copy constructor
          Point(const Point& a_pt);
      /// Returns true if the left hand side is less than the right hand side.
          inline bool operator<(const Point& a_rhs) const;
      /// Adds two points
          Point operator+(const Point& a_rhsPoint) const;
      /// Subtracts two points
          inline Point operator-(const Point& a_rhsPoint) const;
      /// division by an integer - rounds down.
          inline Point operator/(int a_nref) const;
      /// Componentwise division by a Point - rounds down.
          inline Point operator/(const Point& a_pt) const;
      /// multiplication by an integer.
          Point operator*(int a_nref) const;
      /// Componentwise multiplication by a Point.
          inline Point operator*(const Point& a_pt) const;
      /// Update in place arithmetic operations.
          inline void operator*=(const Point& a_pt);
          inline void operator+=(const Point& a_pt);
          inline void operator-=(const Point& a_pt);
          inline void operator/=(const Point& a_pt);
      /// Update in place aritmetic operations. Adds integer to every component.
          inline void operator*=(int a_n);
          inline void operator+=(int a_n);
          inline void operator-=(int a_n);
          inline void operator/=(int a_n);
      /// are two points equal.
          bool operator==(const Point& a_pt) const;
      /// are two points equal.
          inline bool operator!=(const Point& a_pt) const;
      /// Get a component of a point.
          inline const int&  operator[](const int& a_index) const
             {
            // assert(a_index < DIM);
            // assert(a_index >= 0);
               return m_tuple[a_index];
             };
          inline void print() const
             {
               for (int dir = 0; dir < DIM ; dir++)
                  {
                 // cout << m_tuple[dir] << " " ;
                  }
            // cout << endl;
             };
     private:
          int m_tuple[DIM];
   };

Point getZeros();
Point getOnes();

inline Point getUnitv(int a_dir)
   {
     assert((a_dir < DIM) && (a_dir >= 0));
     int unitv[DIM];
     for (int k = 0 ; k < DIM; k++)
        {
          unitv[k] = 0;
        }
     unitv[a_dir] = 1;
     return Point(unitv);
   };


// #include "Shift.H"

class Shift
   {
     public:
          inline Shift(){m_shift = getZeros();};
          inline Shift(Point a_pt){m_shift = a_pt;};
          inline Shift operator*(Shift a_s){return Shift(m_shift + a_s.m_shift);};
  
          inline Point getExp(){return m_shift;};
          inline int getExp(int dir){return m_shift[dir];};
          Point m_shift;
          inline void print(){m_shift.print();};
   };

////  Implementation
inline Shift getUnitShift(){return Shift(getOnes());};
inline array<Shift,DIM> getShiftVec()
   {
     array<Shift,DIM> ret;
     for (int dir = 0; dir < DIM; dir++)
        {
          ret[dir] = Shift(getUnitv(dir));
        }
     return ret;
   };

inline Shift operator^(array<Shift,DIM> a_shiftvec,Point a_exp)
   {
     Shift ret;
     for (int dir = 0; dir < DIM; dir++)
        {
          ret = ret * Shift((a_shiftvec[dir].m_shift)*a_exp[dir]);
        }
     return ret; 
   }



// #include "RectMDArray.H"

class Box
   {
     public:
          Box();
      /// constructor for nontrivial Box. 
          Box(const Point& a_lowCorner,const Point& a_highCorner);
      /// copy constructor.
          Box(const Box& a_Box);
      /// Computes the intersection of the Box with rightBox.  Box AND operation
          Box operator&(const Box& a_rightBox) const;
      /// Computes shifted Box by a_offset in direction a_direction.
          Box shift(int a_direction, int a_offset) const;
      /// Computes shifted Box by a_pt.
          Box shift(const Point& a_pt) const;
      /// Grow in all of the coordinate directions by a_numpoints.
          Box grow(int a_numpoints) const;
      /// Grow in each coordinate direction by an amount given by the component of a_pt.
          Box grow(const Point& a_pt) const;
      /// coarsen in all of the coordinate directions by a_numpoints.
          Box coarsen(int a_numpoints) const;
      /// coarsen in each coordinate direction by an amount given by the component of a_pt.
          Box coarsen(const Point& a_pt) const;
      /// refine in all of the coordinate directions by a_numpoints.
          Box refine(int a_numpoints) const;
      /// refine in each coordinate direction by an amount given by the component of a_pt.
          Box refine(const Point& a_pt) const;
          Box refineCC(const Point& a_pt) const;
          Box refineCC(int a_nref) const;
      /// Access functions for low, high bounds of Box.
          const Point& getLowCorner() const {return m_lowCorner;};
          const Point& getHighCorner() const {return m_highCorner;}; 
      /// Computes a linear index corresponding to the input array. 
          int linearIndex(const Point& a_pt) const;
      /// Computes the size of the Box.
          const int& sizeOf() const{return m_size;};
      /// When are two boxes equal.
          bool operator==(const Box& a_rhsBox) const { return ((m_lowCorner == a_rhsBox.m_lowCorner) && (m_highCorner == a_rhsBox.m_highCorner)); };
      /// Contains the Point a_pt.
          bool contains(const Point& a_pt) const;
          bool contains(const Box& a_rhs) const{ return this->contains(a_rhs.m_lowCorner) && this->contains(a_rhs.m_highCorner); }
          bool contains(const Box& a_rhs, const Box& a_extent)
             {
               return this->contains(a_rhs.m_lowCorner +a_extent.m_lowCorner) && this->contains(a_rhs.m_highCorner+a_extent.m_highCorner);
             }
          bool hasPoint(const Point& a_point) const;
      /// Finds periodic image of input that is contained in the Box.
          Point mod(const Point& a_pt) const;
      /// Get linear index of a Point in a Box.
          inline unsigned int  getIndex(const Point& a_pt) const;
          inline bool notDone(const Point& a_pt) const;

          unsigned int size(unsigned char a_dim) const { return m_highCorner[a_dim]-m_lowCorner[a_dim]; }
      /// iteration through the points in a Box. a_pt is incremented to the next point
      /// in the Box.
          void increment(Point& a_pt) const;
      /// Get Point corresponding to a linear index in [0, ... sizeOf()-1] inside the Box
          Point getPoint(unsigned int k) const;
          void print() const; 

          bool isEmpty() const { bool ret = (m_size < 1); return ret; };

     private:  
          Point m_lowCorner,m_highCorner;  
          int m_size;
   };


class Interval 
   {
     public:
          unsigned int low, high;
          Interval(const unsigned int & a_low, const unsigned int & a_high)
             {
               low  = a_low;
               high = a_high;
             }
   };

template<class T, unsigned int C, unsigned char... RestD>
class Tensor : public std::function<T&(unsigned int,decltype(RestD)...)>
   {
     public:
          Tensor(std::function<T&(unsigned int,decltype(RestD)...)>& a_input) : std::function<T&(unsigned int,decltype(RestD)...)>(a_input) { }
          void operator=(std::function<T&(unsigned int,decltype(RestD)...)>& a_input) {((std::function<T&(unsigned int,decltype(RestD)...)>&)(*this)) = a_input;}
   };

template<class T, unsigned int C, unsigned char... RestD>
class CTensor : public std::function<const T&(unsigned int,decltype(RestD)...)>
   {
     public:
          CTensor(std::function<const T&(unsigned int,decltype(RestD)...)>& a_input) : std::function<const T&(unsigned int,decltype(RestD)...)>(a_input) { }
          void operator=(std::function<const T&(unsigned int,decltype(RestD)...)>& a_input) {((std::function<const T&(unsigned int,decltype(RestD)...)>&)(*this)) = a_input;}
   };


template <class T=double, unsigned int C=1, unsigned char D=1, unsigned char E=1> class RectMDArray 
   {
     public:
      /// Default constructor.
          RectMDArray();
      /// Constructs an RectMDArray over the Box a_box;
          RectMDArray(const Box& a_box);
      /// Defines a default-constructed RectMDArray.  
          void define(const Box& a_box);
      /// Copy constructor.  This is a deep copy, *this and a_srcArray are not sharing data
          RectMDArray(const RectMDArray<T,C,D,E>& a_srcArray);
      /// alias constructor for RectMDArray, used by the alias functions
          RectMDArray(std::shared_ptr<T>& a_data, T* a_ptr, const Box& a_box);
      /// Destructor.
         ~RectMDArray(); 
      /// Setting a RectMDArray to a constant value.
          void setVal(const T& a_val) const;
      /// Assignment operator. This is a deep copy, *this and a_srcArray are not sharing data
          RectMDArray& operator=(const RectMDArray<T,C,D,E>& a_srcArray);
      /// Gets box over which array is defined.

       // put in a function here that makes use of C++11 extended syntax for
       // function specification

          Box getBox() const {return m_box;};
      /// Copy on Intersection.
          void copyTo(RectMDArray<T,C,D,E>& a_dest) const;

      /// Copy with a shift
          void copyTo(RectMDArray<T,C,D,E>& a_dest, const Point& a_shift) const;
      /// Shift in place.
          void shift(const Point a_pt){m_box.shift(a_pt);};
      /// Indexing operator. only works for scalar RectMDArray objects
          inline T& operator[](const Point& a_iv);
          inline const T& operator[](const Point& a_iv) const;
      /// Indexing operator for vector-valued RectMDArray objects
          inline T& operator()(const Point& a_iv, unsigned int a_comp);
          inline const T& operator()(const Point& a_iv, unsigned int a_comp) const;

      /// Index with integer.
          inline T& operator[](int a_index) const;

          void print();
          inline size_t dataSize() const;
          inline bool defined() const;
  
          T& get(const Point& a_iv, unsigned int a_comp=0);
          const T& getConst(const Point& a_iv, unsigned int a_comp=0) const;

       // not for public use.  but can't make C++ template friends do what I want
          std::shared_ptr<T>& m_aliasData() {return m_data;}
          T* m_aliasPtr() {return m_rawPtr;}
     private:
          std::shared_ptr<T> m_data;
          T*  m_rawPtr;
          Box m_box;
   };

/// aliasing.    building a shallow copy of data referencing a larger data holder.
template<class T, unsigned int C0, unsigned int C1>
RectMDArray<T,C1> alias(RectMDArray<T,C0> a_original, const Interval& a_interval);

///pointwise operator
template<class T, unsigned int Cdest, unsigned int Csrc> 
void forall(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src, void (*F)(Tensor<T,Cdest>&, CTensor<T,Csrc>&), const Box& a_box);

// pointwise operator with Point included in function call.
template<class T, unsigned int Cdest, unsigned int Csrc> 
void forall(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src, void (*F)(Tensor<T,Cdest>&, CTensor<T,Csrc>&, const Point&), const Box& a_box);

/// pointwise operator with max reduction
template<class T, unsigned int Cdest, unsigned int Csrc> 
T forall_max(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src, T (*F)(CTensor<T,Csrc>&), const Box& a_box);

/// paired pointwise operator with max reduction
template<class T, unsigned int Cdest, unsigned int Csrc> 
T forall_max(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src, T (*F)(Tensor<T,Cdest>&, CTensor<T,Csrc>&), const Box& a_box);


// #include "RectMDArrayImplem.H"  

template <class T> class Stencil
   {
     public:
          Stencil();
          Stencil(pair<Shift,T> a_pair,Point a_destRefratio=getOnes(),Shift a_destShift=getZeros(),Point a_srcRefratio=getOnes());
          Stencil<T> operator*(const Stencil<T> a_stencil) const;
          void operator*=(const T& a_coef);
          Stencil<T> operator+(const Stencil<T> a_stencil) const;
          void stencilDump() const; 
          void setDestRefratio(Point a_pt){m_destRefratio = a_pt;};
          void setSrcRefratio(Point a_pt){m_srcRefratio = a_pt;}; 
          void setDestShift(Point a_pt){m_destShift = a_pt;};
          Stencil makeInterpStencil(RectMDArray<Stencil>){};

       // This is the secrect sauce of making a Stencil into an operator, using C++ forwarding.
       // A real extended DSL would allow us to use right-binding for operator(), but standard C++ cannot
       // use right-binding, or return-type disambiguation.
          std::tuple<const Stencil<T>&&, const RectMDArray<T>&&,const Box&&> operator()(const RectMDArray<T>& a_phi,const Box& a_bx) 
             {
               return std::make_tuple(*this, a_phi, a_bx);
             }

          static void apply(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx);
          static void apply2(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx);

     private:
          vector<T> m_coef;
          vector<Point> m_offsets;
          Point m_srcRefratio;
          Point m_destRefratio;
          Point m_destShift;
          Stencil<T>(vector<T > a_vecT, vector<Point > a_vecPt,Point a_destRefratio=getOnes(),Point a_destShift=getZeros(),Point a_srcRefratio=getOnes());
          Box m_extent;
          mutable std::vector<unsigned int> m_indexer;
   };
  
template <class T> Stencil<T> operator*(T a_coef, Shift a_shift) { return Stencil<T>(pair<Shift,T>(a_shift,a_coef)); }

template <class T>
RectMDArray<T>& operator|=(RectMDArray<T>& a_lofPhi, const std::tuple<const Stencil<T>&&, const RectMDArray<T>&&,const Box&& >& a_token)
   {
     return a_lofPhi;
   }

template <class T> void Stencil<T>::apply(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx) {}

template<class T, unsigned char D> class Loop
   {
     Loop(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx) {}
   };

template<class T> class Loop<T,0>
   {
     Loop(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx) {}
   };

template <class T> void Stencil<T>::apply2(const Stencil<T>& a_stencil, const RectMDArray<T>& a_phi, RectMDArray<T>& a_lofPhi, const Box& a_bx) {}

template <class TSrc,class TDest,class F>
class PointwiseOperator
   {
     public:
          PointwiseOperator(){};
          RectMDArray<TDest> operator()(F a_f,Box& a_bx, int n, ...)
             {
               RectMDArray<TDest> fOfPhi(a_bx);
               return fOfPhi;
             }
   };

// function to initialize the src data
void initialize(RectMDArray<double>& patch) {}

int main(int argc, char* argv[])
   {
     Point zero = getZeros();
     Point ones = getOnes();
     Point negones = ones*(-1);
     Point lo = zero;
     Point hi = getOnes()*(BLOCKSIZE-1);
     Box bxdest(lo,hi); //box low and high corners for destination
  
  // This will grow the box by one ghost
  // along each face and become the box for
  // the source box. 
     Box bxsrc=bxdest.grow(1);
  
  // source and destination data containers
     RectMDArray<double,1> Asrc(bxsrc);
     RectMDArray<double,1> Adest(bxdest);

  // all the coefficients I need for this operation
     double ident=1.0;
     double C0=-4.0;

     initialize(Asrc);
     initialize(Adest);

  // cout <<" The source Box" << endl;
  // Asrc.print();
  // cout << endl;

  // build the stencil, and the stencil operator
  // Stencil<double> laplace(wt,shft);
     array<Shift,DIM> S = getShiftVec();
     Stencil<double> laplace = C0*(S^zero);

     for (int dir=0;dir<DIM;dir++)
        {
          Point thishft = getUnitv(dir);
          laplace = laplace + ident*(S^thishft);
          laplace = laplace + ident*(S^(thishft*(-1)));
        }
     laplace.stencilDump();
  
  // StencilOperator<double,double, double> op;

  // apply stencil operator
     Stencil<double>::apply(laplace,Asrc,Adest,bxdest);

  // cout <<" The destination Box" << endl;
  // Adest.print();
   }
