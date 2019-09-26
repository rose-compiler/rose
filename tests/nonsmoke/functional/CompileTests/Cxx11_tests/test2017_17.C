#include <algorithm>
#include <atomic>
#include <cassert>
#include <cfloat>
#include <cstddef>  // for NULL
#include <cstdlib>
#include <iosfwd>
#include <iostream>
#include <iterator>
#include <limits>
#include <omp.h>
#include <sstream>
#include <stddef.h>
#include <stdexcept>
#include <thread>
#include <utility>

enum LoopAlgorithmId {

  DEL_VOL_IJ_BASE,
  DEL_VOL_IK_BASE,
  DEL_VOL_JK_BASE,
  PDV_WORK_BASE,
  STRESS_ACC_BASE,
  STRESS_WORK_BASE,

  DEL_VOL_IJ_OPTIMIZED_V1,
  DEL_VOL_IK_OPTIMIZED_V1,
  DEL_VOL_JK_OPTIMIZED_V1,
  DEL_VOL_IJ_OPENMP,
  DEL_VOL_IK_OPENMP,
  DEL_VOL_JK_OPENMP,

  DEL_VOL_IJ_RAJA,
  DEL_VOL_IK_RAJA,
  DEL_VOL_JK_RAJA,

  DEL_VOL_IJ_RAJA_COLLAPSED,
  DEL_VOL_IK_RAJA_COLLAPSED,
  DEL_VOL_JK_RAJA_COLLAPSED,

  DEL_VOL_IJ_RAJA_PREIDXSET,
  DEL_VOL_IK_RAJA_PREIDXSET,
  DEL_VOL_JK_RAJA_PREIDXSET,

  PDV_WORK_OPTIMIZED_V1,
  PDV_WORK_OPENMP,

  PDV_WORK_RAJA,
  PDV_WORK_RAJAAPI,

  STRESS_ACC_COMPACT,
  STRESS_ACC_OPTIMIZED_V1,
  STRESS_ACC_OPENMP,
  STRESS_ACC_OPENMP_LOCAL_MEMORY,
  STRESS_ACC_OPENMP_PREIDXSET,
  STRESS_ACC_OPENMP_NODE_CENTERED,

  STRESS_ACC_RAJA,
  STRESS_ACC_RAJA_LOCAL_MEMORY,
  STRESS_ACC_RAJA_PREIDXSET,
  STRESS_ACC_RAJA_LISTINDEXSET,
  STRESS_ACC_RAJA_SPLITINDEXSET,
  STRESS_ACC_RAJA_NODE_CENTERED,
  STRESS_WORK_OPTIMIZED_V1,


  STRESS_WORK_RAJAAPI,




  NUMBER_OF_LOOP_ALGORITHMS
};
static const int LoopAlgorithmBaselines [] ={






  DEL_VOL_IJ_BASE,
  DEL_VOL_IK_BASE,
  DEL_VOL_JK_BASE,

  PDV_WORK_BASE,
  STRESS_ACC_BASE,
  STRESS_WORK_BASE,






  DEL_VOL_IJ_BASE,
  DEL_VOL_IK_BASE,
  DEL_VOL_JK_BASE,

  DEL_VOL_IJ_BASE,
  DEL_VOL_IK_BASE,
  DEL_VOL_JK_BASE,


  DEL_VOL_IJ_BASE,
  DEL_VOL_IK_BASE,
  DEL_VOL_JK_BASE,

  DEL_VOL_IJ_BASE,
  DEL_VOL_IK_BASE,
  DEL_VOL_JK_BASE,

  DEL_VOL_IJ_BASE,
  DEL_VOL_IK_BASE,
  DEL_VOL_JK_BASE,





  PDV_WORK_BASE,
  PDV_WORK_BASE,

  PDV_WORK_BASE,
  PDV_WORK_BASE,





  STRESS_ACC_BASE,
  STRESS_ACC_BASE,
  STRESS_ACC_BASE,
  STRESS_ACC_BASE,
  STRESS_ACC_BASE,
  STRESS_ACC_BASE,

  STRESS_ACC_BASE,
  STRESS_ACC_BASE,
  STRESS_ACC_BASE,
  STRESS_ACC_BASE,
  STRESS_ACC_BASE,
  STRESS_ACC_BASE,
  STRESS_WORK_BASE,


  STRESS_WORK_BASE,




  NUMBER_OF_LOOP_ALGORITHMS
};
static const char* LoopAlgorithmNames [] = {





  "DelVolIJBase",
  "DelVolIKBase",
  "DelVolJKBase",

  "PdVWorkBase",
  "StressAccBase",
  "StressWorkBase",






  "DelVolIJOptimized",
  "DelVolIKOptimized",
  "DelVolJKOptimized",
  "DelVolIJOpenMP",
  "DelVolIKOpenMP",
  "DelVolJKOpenMP",


  "DelVolIJRaja",
  "DelVolIKRaja",
  "DelVolJKRaja",

  "DelVolIJRajaCollapsed",
  "DelVolIKRajaCollapsed",
  "DelVolJKRajaCollapsed",

  "DelVolIJRajaPreComputedIndexSet",
  "DelVolIKRajaPreComputedIndexSet",
  "DelVolJKRajaPreComputedIndexSet",





  "PdVWorkOptimizedV1",
  "PdVWorkOpenMP",

  "PdVWorkRaja",
  "PdVWorkRajaAPI",





  "StressAccCompact",
  "StressAccOptimizedV1",
  "StressAccOpenMP",
  "StressAccOpenMPLocalMemory",
  "StressAccOpenMPPreComputedIndexSet",
  "StressAccOpenMPNodeCentered",

  "StressAccRaja",
  "StressAccRajaLocalMemory",
  "StressAccRajaPreComputedIndexSet",
  "StressAccRajaListIndexSet",
  "StressAccRajaSplitIndexSet",
  "StressAccRajaNodeCentered",
  "StressWorkOptimizedV1",
  "StressWorkRajaAPI",
  "UNDEFINED"
};





//Liao---begin---------------
// Liao  return from system header 


namespace loopsuite {


class StructuredDomain;
class TaskTimer;
class LoopAlgorithm
{

public:






  virtual ~LoopAlgorithm();




  double getMinTime() const { return m_MinTime; };
  double getMaxTime() const { return m_MaxTime; };
  double getAvgTime() const { return m_AvgTime; };

  int getId() const { return m_Id; };
  int getBaselineId() const { return m_BaselineId; };
  bool isBaseline() const { return( m_Id == m_BaselineId ); };

  const std::string& getName() const { return m_Name; };
  void setPassTolerance( const double t ) { m_pass_tolerance = t; };







  void setTolerance( const double t) { m_err_tolerance = t; };
  void execute();
  void computeSpeedup( double speedup[3], const LoopAlgorithm* algorithm);
  virtual bool checkResults( const LoopAlgorithm* algorithm,
                             std::ostringstream& error_stream ) = 0;
  virtual void postExecute() { return; };

protected:
  double m_pass_tolerance;
  const StructuredDomain* m_Domain;
  LoopAlgorithm() {};
  LoopAlgorithm( const int algorithmId,
                 const StructuredDomain* domain,
                 const unsigned numIterations=5 );
  void compareArrays( const double* exp, const double* comp, const int N,
                      double& minErr, double& maxErr );
  void compareArrays( const double* exp, const double* comp,
                      const int begin, const int end,
                      double& minErr, double& maxErr );
  void compareRealZoneArrays( const double* exp, const double* comp,
                              const StructuredDomain* domain,
                              double& minErr, double& maxErr );
  virtual void setup() = 0;
  virtual void tearDown() = 0;
  virtual void loopExecute() = 0;



private:

  int m_Id;
  int m_BaselineId;
  std::string m_Name;
  double m_err_tolerance;
  unsigned m_NumIterations;

  TaskTimer* m_Timer;

  double m_MinTime;
  double m_MaxTime;
  double m_AvgTime;

  LoopAlgorithm( const LoopAlgorithm& );
  LoopAlgorithm& operator=( const LoopAlgorithm& );
};

}

namespace loopsuite {
class PdVWorkBaseLoopAlgorithm : public LoopAlgorithm
{
public:
  PdVWorkBaseLoopAlgorithm( const StructuredDomain* domain,
                            const unsigned numberOfIterations );

  virtual ~PdVWorkBaseLoopAlgorithm();




  const double* getDVolOverMArray() const { return m_dVol_over_m; };
  const double* getPdVSumArray() const { return m_pdv_sum; };
  const double* getVRatioArray() const { return m_vRatio; };
  const double* getNVolArray() const { return m_nvol; };
  const double* getOVolArray() const { return m_ovol; };







  virtual bool checkResults( const LoopAlgorithm* algorithm,
                             std::ostringstream& error_stream );

protected:

  double* m_dVol_over_m;
  double* m_pdv_sum;
  double* m_vRatio;

  double* m_nvol;
  double* m_ovol;


  virtual void setup();
  virtual void tearDown();
  virtual void loopExecute();







  PdVWorkBaseLoopAlgorithm() {};
  PdVWorkBaseLoopAlgorithm( const int algorithmId,
                            const StructuredDomain* domain,
                            const unsigned numIterations=5 );
private:
  PdVWorkBaseLoopAlgorithm( const PdVWorkBaseLoopAlgorithm& );
  PdVWorkBaseLoopAlgorithm& operator=(const PdVWorkBaseLoopAlgorithm& );
};

}

namespace loopsuite {

class PdVWorkRajaAPI : public PdVWorkBaseLoopAlgorithm
{
public:
  PdVWorkRajaAPI( const StructuredDomain* domain,
                  const unsigned numberOfIterations );

  virtual ~PdVWorkRajaAPI();

protected:
  virtual void loopExecute();

private:







  PdVWorkRajaAPI() { };

  PdVWorkRajaAPI( const PdVWorkRajaAPI& );
  PdVWorkRajaAPI& operator=(const PdVWorkRajaAPI& );
};

}


// Liao cassert begin



namespace loopsuite {

enum MaterialLayout {
  ISLAB,
  JSLAB,
  KSLAB,
  SPHERICAL,

  NUMBER_OF_MATERIAL_LAYOUTS
};
class StructuredDomain
{
public:
  StructuredDomain();
  virtual ~StructuredDomain();
  int getTotalNumberOfNodes( const bool include_ghost=true ) const;
  int getTotalNumberOfZones( const bool include_ghost=true ) const;
  void getNodeDimensions(int node_dims[3], const bool include_ghost=true) const;
  void getZoneDimensions(int zone_dims[3], const bool include_ghost=true) const;







  void getSideLengths( double L[3] )
  {

    for (int i=0; i < 3; ++i ) {
       L[ i ] = m_bounds[ i*2+1 ] - m_bounds[ i*2 ];
    }

  }







  void getWholeExtent( int ext[6] )
  {
    ext[ 0 ] = 0;
    ext[ 1 ] = m_imax + m_npnr;
    ext[ 2 ] = 0;
    ext[ 3 ] = m_jmax + m_npnr;
    ext[ 4 ] = 0;
    ext[ 5 ] = m_kmax + m_npnr;
  }







  void getRealExtent( int ext[6] )
  {
    ext[ 0 ] = m_imin;
    ext[ 1 ] = m_imax;
    ext[ 2 ] = m_jmin;
    ext[ 3 ] = m_jmax;
    ext[ 4 ] = m_kmin;
    ext[ 5 ] = m_kmax;
  }
  bool withinRealExtent( const int i, const int j, const int k )
  {
    return( (i >= m_imin) && (i <= m_imax) &&
            (j >= m_jmin) && (j <= m_jmax) &&
            (k >= m_kmin) && (k <= m_kmax ) );
  }
  void generateSyntheticDataSet(
        const int nx, const int ny, const int nz,
        const int nreg, const int matLayout, const bool mixed,
        const int ngleft, const int ngright);







  void writeRawData(const std::string& filename);







  void readRawData(const std::string& filename);
  void toLegacyVtkFile(const std::string& filename);
  void getIJKCoordinates(const int idx, int& i, int& j, int& k) const
  {
    k = idx / m_kp;
    j = (idx - k*m_kp ) / m_jp;
    i = idx - j*m_jp - k*m_kp;
  }
  void getZoneNodeIds(const int zoneIdx, int nodeIdx[8]);





  int m_dimension;

  int m_npnl;
  int m_npnr;




  int m_imin,m_imax,m_jmin,m_jmax,m_kmin,m_kmax;




  int m_ip;
  int m_jp;
  int m_kp;





  int m_frn;
  int m_lrn;
  int m_frz;
  int m_lrz;

  int m_fpz;
  int m_lpz;

  int m_nnalls;
  int m_namix;
  int m_nreg;





  int** m_rndxmix;
  int** m_rgrdmix;
  int* m_rlenmix;





  double* m_p;
  double* m_zmass;
  double* m_den;
  double* m_vol;
  double* m_partvf;
  double* m_partvfh;
  double* m_sxx;
  double* m_syy;
  double* m_szz;
  double* m_sxy;
  double* m_syz;
  double* m_sxz;





  double* m_x;
  double* m_y;
  double* m_z;


  double* m_xsave;
  double* m_ysave;
  double* m_zsave;

private:

  double m_bounds[6];
  int* m_node_ireg;



  void shapeInSlabs(const int nreg, const int mode);
  void shapeInSpheres(const int nreg);




  void compute_bounds();
  void perturb_coordinates();
  void generate_coordinates();
  void allocate_coordinates(const int N);
  void allocate_zonefields();
  void make_ireg(const int mode, const bool mixedZones);
  void regndx(const int* ireg, const int* grdpnt);


  StructuredDomain(const StructuredDomain& );
  StructuredDomain& operator=(const StructuredDomain& );
};

}
typedef loopsuite::StructuredDomain Domain_t;

namespace RAJA {
const int RAJA_VERSION_MAJOR = 0;
const int RAJA_VERSION_MINOR = 2;
const int RAJA_VERSION_PATCHLEVEL = 5;
const int RANGE_ALIGN = 4;
const int RANGE_MIN_LENGTH = 32;
const int DATA_ALIGN = 64;
const int COHERENCE_BLOCK_SIZE = 64;
}

// Liao stdexcept
// Liao end of except
inline void RAJA_ABORT_OR_THROW(const char *str) {
    if (getenv ("RAJA_NO_EXCEPT") != __null) {
        abort();
    } else {
        throw std::runtime_error(str);
    }
}

namespace RAJA
{




enum SegmentType {
  _RangeSeg_,
  _RangeStrideSeg_,
  _ListSeg_,
  _UnknownSeg_
};





enum IndexOwnership { Unowned, Owned };




typedef int Index_type;





const int UndefinedValue = -9999999;
typedef double Real_type;
typedef Real_type* __restrict__ Real_ptr;
typedef const Real_type* __restrict__ const_Real_ptr;






typedef Real_type* __restrict__ UnalignedReal_ptr;
typedef const Real_type* __restrict__ const_UnalignedReal_ptr;
}


// Liao cfloat start

namespace RAJA
{

namespace operators
{

namespace detail
{

struct associative_tag {
};

template <typename Arg1, typename Arg2, typename Result>
struct binary_function {
  using first_argument_type = Arg1;
  using second_argument_type = Arg2;
  using result_type = Result;
};

template <typename Argument, typename Result>
struct unary_function {
  using argument_type = Argument;
  using result_type = Result;
};

template <typename Arg1, typename Arg2>
struct comparison_function : public binary_function<Arg1, Arg2, bool> {
};

}

namespace types
{

template <typename T>
struct is_unsigned_int {
  constexpr static const bool value =
      std::is_unsigned<T>::value && std::is_integral<T>::value;
};

template <typename T>
struct is_signed_int {
  constexpr static const bool value =
      !std::is_unsigned<T>::value && std::is_integral<T>::value;
};






template <typename T, bool GPU = false>
struct larger {
};

template <>
struct larger<uint8_t> {
  using type = uint16_t;
};

template <>
struct larger<uint16_t> {
  using type = uint32_t;
};

template <>
struct larger<uint32_t> {
  using type = uint64_t;
};

template <>
struct larger<int8_t> {
  using type = int16_t;
};

template <>
struct larger<int16_t> {
  using type = int32_t;
};

template <>
struct larger<int32_t> {
  using type = int64_t;
};

template <>
struct larger<float> {
  using type = double;
};

template <>
struct larger<double> {
  using type = long double;
};

template <>
struct larger<double, true> {
  using type = double;
};

namespace detail
{

template <typename T, bool isInt, bool isSigned, bool isFP, bool gpu = false>
struct largest {
};

template <typename T>
struct largest<T, true, false, false> {
  using type = uint64_t;
};

template <typename T>
struct largest<T, true, true, false> {
  using type = int64_t;
};

template <typename T>
struct largest<T, false, false, true, false> {
  using type = long double;
};

template <typename T>
struct largest<T, false, false, true, true> {
  using type = double;
};
}





template <typename T, bool gpu = false>
struct largest {
  using type = typename detail::largest<T,
                                        std::is_integral<T>::value,
                                        std::is_signed<T>::value,
                                        std::is_floating_point<T>::value,
                                        gpu>::type;
};


template <typename T>
struct size_of {
  enum { value = sizeof(T) };
};

namespace detail
{

template <typename T, typename U, bool lhsLarger>
struct larger_of {
};

template <typename T, typename U>
struct larger_of<T, U, true> {
  using type = T;
};

template <typename T, typename U>
struct larger_of<T, U, false> {
  using type = U;
};
}

template <typename T, typename U>
struct larger_of {
  using type =
      typename detail::larger_of<T,
                                 U,
                                 (size_of<T>::value > size_of<U>::value)>::type;
};

}

namespace constants
{

template <typename T>
 constexpr T min()
{
  return std::numeric_limits<T>::min();
}
template <typename T>
 constexpr T max()
{
  return std::numeric_limits<T>::max();
}

}



template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct plus : public detail::binary_function<Arg1, Arg2, Ret>,
              detail::associative_tag {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return Ret{lhs} + rhs;
  }
  static constexpr const Ret identity = Ret{0};
};

template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct minus : public detail::binary_function<Arg1, Arg2, Ret> {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return Ret{lhs} - rhs;
  }
};

template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct multiplies : public detail::binary_function<Arg1, Arg2, Ret>,
                    detail::associative_tag {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return Ret{lhs} * rhs;
  }
  static constexpr const Ret identity = Ret{1};
};

template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct divides : public detail::binary_function<Arg1, Arg2, Ret> {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return Ret{lhs} / rhs;
  }
};

template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct modulus : public detail::binary_function<Arg1, Arg2, Ret> {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return Ret{lhs} % rhs;
  }
};



template <typename Arg1, typename Arg2 = Arg1>
struct logical_and : public detail::comparison_function<Arg1, Arg2>,
                     detail::associative_tag {
  bool operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs && rhs;
  }
  static constexpr const bool identity = true;
};

template <typename Arg1, typename Arg2 = Arg1>
struct logical_or : public detail::comparison_function<Arg1, Arg2>,
                    detail::associative_tag {
  bool operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs || rhs;
  }
  static constexpr const bool identity = false;
};

template <typename T>
struct logical_not : public detail::unary_function<T, bool> {
  bool operator()(const T& lhs) { return !lhs; }
};



template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct bit_or : public detail::binary_function<Arg1, Arg2, Ret> {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs | rhs;
  }
};

template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct bit_and : public detail::binary_function<Arg1, Arg2, Ret> {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs & rhs;
  }
};

template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct bit_xor : public detail::binary_function<Arg1, Arg2, Ret> {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs ^ rhs;
  }
};



template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct minimum : public detail::binary_function<Arg1, Arg2, Ret>,
                 detail::associative_tag {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return (lhs < rhs) ? lhs : rhs;
  }
  static constexpr const Ret identity = constants::max<Ret>();
};

template <typename Ret, typename Arg1 = Ret, typename Arg2 = Arg1>
struct maximum : public detail::binary_function<Arg1, Arg2, Ret>,
                 detail::associative_tag {
  Ret operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return (lhs < rhs) ? rhs : lhs;
  }
  static constexpr const Ret identity = constants::min<Ret>();
};



template <typename Arg1, typename Arg2 = Arg1>
struct equal_to : public detail::comparison_function<Arg1, Arg2> {
  bool operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs == rhs;
  }
};

template <typename Arg1, typename Arg2 = Arg1>
struct not_equal_to : public detail::comparison_function<Arg1, Arg2> {
  bool operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs != rhs;
  }
};

template <typename Arg1, typename Arg2 = Arg1>
struct greater : public detail::comparison_function<Arg1, Arg2> {
  bool operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs >= rhs;
  }
};

template <typename Arg1, typename Arg2 = Arg1>
struct less : public detail::comparison_function<Arg1, Arg2> {
  bool operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs <= rhs;
  }
};


template <typename Arg1, typename Arg2 = Arg1>
struct greater_equal : public detail::comparison_function<Arg1, Arg2> {
  bool operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs >= rhs;
  }
};

template <typename Arg1, typename Arg2 = Arg1>
struct less_equal : public detail::comparison_function<Arg1, Arg2> {
  bool operator()(const Arg1& lhs, const Arg2& rhs)
  {
    return lhs <= rhs;
  }
};



template <typename Ret, typename Orig = Ret>
struct identity : public detail::unary_function<Orig, Ret> {
  Ret operator()(const Orig& lhs) { return lhs; }
};

template <typename T, typename U>
struct project1st : public detail::binary_function<T, U, T> {
  T operator()(const T& lhs, const U& ) { return lhs; }
};

template <typename T, typename U = T>
struct project2nd : public detail::binary_function<T, U, U> {
  U operator()(const T& , const U& rhs) { return rhs; }
};



template <typename T>
struct is_associative {
  constexpr static const bool value =
      std::is_base_of<detail::associative_tag, T>::value;
};

template <typename Arg1, typename Arg2 = Arg1>
struct safe_plus
    : public plus<Arg1,
                  Arg2,
                  typename types::larger<
                      typename types::larger_of<Arg1, Arg2>::type>::type> {
};

}

}
namespace RAJA
{
template <typename REDUCE_POLICY_T, typename T>
class ReduceMin;
template <typename REDUCE_POLICY_T, typename T>
class ReduceMinLoc;
template <typename REDUCE_POLICY_T, typename T>
class ReduceMax;
template <typename REDUCE_POLICY_T, typename T>
class ReduceMaxLoc;
template <typename REDUCE_POLICY_T, typename T>
class ReduceSum;

}




namespace RAJA
{
template <typename TYPE>
class IndexValue
{
public:



 
  inline __attribute__((always_inline))
  constexpr IndexValue() : value(0) {}





 
  inline __attribute__((always_inline))
  constexpr explicit IndexValue(Index_type v) : value(v) {}




 
  inline __attribute__((always_inline))
  Index_type operator*(void)const { return value; }

 
  inline __attribute__((always_inline))
  TYPE &operator++(int)
  {
    value++;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline))
  TYPE &operator++()
  {
    value++;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline))
  TYPE &operator--(int)
  {
    value--;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline))
  TYPE &operator--()
  {
    value--;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline))
  TYPE operator+(Index_type a) const { return TYPE(value + a); }

 
  inline __attribute__((always_inline))
  TYPE operator+(TYPE a) const { return TYPE(value + a.value); }

 
  inline __attribute__((always_inline))
  TYPE operator-(Index_type a) const { return TYPE(value - a); }

 
  inline __attribute__((always_inline))
  TYPE operator-(TYPE a) const { return TYPE(value - a.value); }

 
  inline __attribute__((always_inline))
  TYPE operator*(Index_type a) const { return TYPE(value * a); }

 
  inline __attribute__((always_inline))
  TYPE operator*(TYPE a) const { return TYPE(value * a.value); }

 
  inline __attribute__((always_inline))
  TYPE operator/(Index_type a) const { return TYPE(value / a); }

 
  inline __attribute__((always_inline))
  TYPE operator/(TYPE a) const { return TYPE(value / a.value); }

 
  inline __attribute__((always_inline)) TYPE &operator+=(Index_type x)
  {
    value += x;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline)) TYPE &operator+=(TYPE x)
  {
    value += x.value;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline)) TYPE &operator-=(Index_type x)
  {
    value -= x;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline)) TYPE &operator-=(TYPE x)
  {
    value -= x.value;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline)) TYPE &operator*=(Index_type x)
  {
    value *= x;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline)) TYPE &operator*=(TYPE x)
  {
    value *= x.value;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline)) TYPE &operator/=(Index_type x)
  {
    value /= x;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline)) TYPE &operator/=(TYPE x)
  {
    value /= x.value;
    return *static_cast<TYPE *>(this);
  }

 
  inline __attribute__((always_inline)) bool operator<(Index_type x) const { return (value < x); }

 
  inline __attribute__((always_inline)) bool operator<(TYPE x) const { return (value < x.value); }

 
  inline __attribute__((always_inline)) bool operator<=(Index_type x) const { return (value <= x); }

 
  inline __attribute__((always_inline)) bool operator<=(TYPE x) const { return (value <= x.value); }

 
  inline __attribute__((always_inline)) bool operator>(Index_type x) const { return (value > x); }

 
  inline __attribute__((always_inline)) bool operator>(TYPE x) const { return (value > x.value); }

 
  inline __attribute__((always_inline)) bool operator>=(Index_type x) const { return (value >= x); }

 
  inline __attribute__((always_inline)) bool operator>=(TYPE x) const { return (value >= x.value); }

 
  inline __attribute__((always_inline)) bool operator==(Index_type x) const { return (value == x); }

 
  inline __attribute__((always_inline)) bool operator==(TYPE x) const { return (value == x.value); }

 
  inline __attribute__((always_inline)) bool operator!=(Index_type x) const { return (value != x); }

 
  inline __attribute__((always_inline)) bool operator!=(TYPE x) const { return (value != x.value); }



  static std::string getName(void);

protected:
  Index_type value;
};
template <typename TO, typename FROM>
 inline __attribute__((always_inline)) TO convertIndex_helper(FROM val)
{
  return TO{val};
}
template <typename TO, typename FROM>
 inline __attribute__((always_inline)) TO convertIndex_helper(typename FROM::IndexValueType val)
{
  return TO{*val};
}

template <typename TO, typename FROM>
 inline __attribute__((always_inline)) TO convertIndex(FROM val)
{
  return convertIndex_helper<TO, FROM>(val);
}



}








namespace RAJA {
    namespace Iterators {

template<typename Container>
using IteratorCategoryOf = typename std::iterator_traits<typename std::remove_reference<Container>::type::iterator>::iterator_category;

template<typename Container>
using OffersRAI =
    std::is_base_of<
        std::random_access_iterator_tag,
        IteratorCategoryOf<Container>>;



template<typename Type,
         typename DifferenceType = std::ptrdiff_t,
         typename PointerType = Type *>
class base_iterator : public std::iterator<std::random_access_iterator_tag,
                                                Type,
                                                DifferenceType>
{
public:
    using difference_type = typename std::iterator<std::random_access_iterator_tag, Type>::difference_type;

    constexpr base_iterator() : val(0) {}
    constexpr base_iterator(Type rhs) : val(rhs) {}
    constexpr base_iterator(const base_iterator &rhs) : val(rhs.val) {}

    inline bool operator==(const base_iterator& rhs) const {return val == rhs.val;}
    inline bool operator!=(const base_iterator& rhs) const {return val != rhs.val;}
    inline bool operator>(const base_iterator& rhs) const {return val > rhs.val;}
    inline bool operator<(const base_iterator& rhs) const {return val < rhs.val;}
    inline bool operator>=(const base_iterator& rhs) const {return val >= rhs.val;}
    inline bool operator<=(const base_iterator& rhs) const {return val <= rhs.val;}
protected:
    Type val;
};

template<typename Type = Index_type,
         typename DifferenceType = Index_type,
         typename PointerType = Type *>
class numeric_iterator : public base_iterator< Type,
                                               DifferenceType>
{
public:
    using difference_type = typename std::iterator<std::random_access_iterator_tag, Type>::difference_type;
    using base = base_iterator<Type, DifferenceType>;

    constexpr numeric_iterator() : base(0) {}
    constexpr numeric_iterator(const Type& rhs) : base(rhs) {}
    constexpr numeric_iterator(const numeric_iterator& rhs) : base(rhs.val) {}

    inline numeric_iterator& operator++() {++base::val; return *this;}
    inline numeric_iterator& operator--() {--base::val; return *this;}
    inline numeric_iterator operator++(int) {numeric_iterator tmp(*this); ++base::val; return tmp;}
    inline numeric_iterator operator--(int) {numeric_iterator tmp(*this); --base::val; return tmp;}

    inline numeric_iterator& operator+=(const difference_type& rhs) {base::val+=rhs; return *this;}
    inline numeric_iterator& operator-=(const difference_type& rhs) {base::val-=rhs; return *this;}
    inline numeric_iterator& operator+=(const numeric_iterator& rhs) {base::val+=rhs.val; return *this;}
    inline numeric_iterator& operator-=(const numeric_iterator& rhs) {base::val-=rhs.val; return *this;}

    inline difference_type operator+(const numeric_iterator& rhs) const {return static_cast<difference_type>(base::val)+static_cast<difference_type>(rhs.val);}
    inline difference_type operator-(const numeric_iterator& rhs) const {return static_cast<difference_type>(base::val)-static_cast<difference_type>(rhs.val);}
    inline numeric_iterator operator+(const difference_type& rhs) const {return numeric_iterator(base::val+rhs);}
    inline numeric_iterator operator-(const difference_type& rhs) const {return numeric_iterator(base::val-rhs);}
    friend constexpr numeric_iterator operator+(difference_type lhs, const numeric_iterator& rhs) {return numeric_iterator(lhs+rhs.val);}
    friend constexpr numeric_iterator operator-(difference_type lhs, const numeric_iterator& rhs) {return numeric_iterator(lhs-rhs.val);}

    inline Type operator*() const {return base::val;}
    inline Type operator->() const {return base::val;}
    constexpr Type operator[](difference_type rhs) const {return base::val + rhs;}

};

template<typename Type = Index_type,
         typename DifferenceType = Index_type,
         typename PointerType = Type *>
class strided_numeric_iterator : public base_iterator< Type,
                                               DifferenceType>
{
public:
    using difference_type = typename std::iterator<std::random_access_iterator_tag, Type>::difference_type;
    using base = base_iterator<Type, DifferenceType>;

    constexpr strided_numeric_iterator() : base(0), stride(1) {}
    constexpr strided_numeric_iterator(const Type& rhs, DifferenceType stride = 1) : base(rhs), stride(stride) {}
    constexpr strided_numeric_iterator(const strided_numeric_iterator& rhs) : base(rhs.val), stride(rhs.stride) {}

    inline strided_numeric_iterator& operator++() {base::val += stride; return *this;}
    inline strided_numeric_iterator& operator--() {base::val -= stride; return *this;}

    inline strided_numeric_iterator& operator+=(const difference_type& rhs) {base::val+=rhs * stride; return *this;}
    inline strided_numeric_iterator& operator-=(const difference_type& rhs) {base::val-=rhs * stride; return *this;}

    inline difference_type operator+(const strided_numeric_iterator& rhs) const {
        return static_cast<difference_type>(base::val)
            + (static_cast<difference_type>(rhs.val * stride));}
    inline difference_type operator-(const strided_numeric_iterator& rhs) const {
        auto diff = static_cast<difference_type>(base::val)
            - (static_cast<difference_type>(rhs.val));
        if (diff < stride)
            return 0;
        if (diff % stride)
            return diff/stride + 1;
        return diff/stride;
    }
    inline strided_numeric_iterator operator+(const difference_type& rhs) const {return strided_numeric_iterator(base::val+rhs * stride);}
    inline strided_numeric_iterator operator-(const difference_type& rhs) const {return strided_numeric_iterator(base::val-rhs * stride);}



    inline bool operator!=(const strided_numeric_iterator& rhs) const {
        if (base::val == rhs.val) return false;
        auto rem = rhs.val % stride;
        return base::val != rhs.val + rem;
    }

    inline Type operator*() const {return base::val;}
    inline Type operator->() const {return base::val;}
    inline Type operator[](difference_type rhs) const {return base::val + rhs * stride;}

private:
    DifferenceType stride;

};



template<typename Iterator>
class Enumerater : public numeric_iterator<> {
public:
    template<typename First,
             typename Second>
    class InnerPair {
        public:
        InnerPair() = delete;
        constexpr InnerPair(First && f, Second && s) : first(f), second(s) {};
        First first;
        Second second;
    };
    using base = numeric_iterator<>;

    using pair = InnerPair<std::ptrdiff_t,
                           Iterator>;
    using value_type = pair;
    using pointer_type = pair*;
    using reference = pair&;

    Enumerater() = delete;
    constexpr Enumerater(const Iterator& rhs,
                         std::ptrdiff_t val = 0,
                         std::ptrdiff_t offset = 0)
        : base(val), offset(offset), wrapped(rhs) {}
    constexpr Enumerater(const Enumerater &rhs)
        : base(rhs.val), offset(rhs.offset), wrapped(rhs.wrapped) {}

    inline pair operator*() const {return pair(offset+val, wrapped+val);}
    constexpr pair operator[](typename base::difference_type rhs) const {
        return pair(val+offset+rhs, wrapped+val+rhs);
    }

private:
    std::ptrdiff_t offset;
    Iterator wrapped;
};

}
}



namespace RAJA {

struct PolicyBase { };

}



namespace RAJA
{
class __attribute__((aligned(256))) DepGraphNode
{
public:
  static const int _MaxDepTasks_ = 8;




  DepGraphNode()
      : m_num_dep_tasks(0), m_semaphore_reload_value(0), m_semaphore_value(0)
  {
  }





  std::atomic<int>& semaphoreValue() { return m_semaphore_value; }





  int& semaphoreReloadValue() { return m_semaphore_reload_value; }




  void reset() { m_semaphore_value.store(m_semaphore_reload_value); }




  void satisfyOne()
  {
    if (m_semaphore_value > 0) {
      --m_semaphore_value;
    }
  }




  void wait()
  {
    while (m_semaphore_value > 0) {


      std::this_thread::yield();
    }
  }





  int& numDepTasks() { return m_num_dep_tasks; }






  int& depTaskNum(int tidx) { return m_dep_task[tidx]; }




  void print(std::ostream& os) const;

private:
  int m_dep_task[_MaxDepTasks_];
  int m_num_dep_tasks;
  int m_semaphore_reload_value;
  std::atomic<int> m_semaphore_value;
};

}


namespace RAJA
{

class BaseSegment;
class IndexSetSegInfo
{
public:



  IndexSetSegInfo()
      : m_segment(0),
        m_owns_segment(false),
        m_icount(UndefinedValue),
        m_dep_graph_node(0)
  {
    ;
  }




  IndexSetSegInfo(BaseSegment* segment, bool owns_segment)
      : m_segment(segment),
        m_owns_segment(owns_segment),
        m_icount(UndefinedValue),
        m_dep_graph_node(0)
  {
    ;
  }

  ~IndexSetSegInfo()
  {
    if (m_dep_graph_node) delete m_dep_graph_node;
  }
  const BaseSegment* getSegment() const { return m_segment; }




  BaseSegment* getSegment() { return m_segment; }






  bool ownsSegment() const { return m_owns_segment; }




  void setIcount(Index_type icount) { m_icount = icount; }

  Index_type getIcount() const { return m_icount; }




  const DepGraphNode* getDepGraphNode() const { return m_dep_graph_node; }




  DepGraphNode* getDepGraphNode() { return m_dep_graph_node; }







  void initDepGraphNode() { m_dep_graph_node = new DepGraphNode(); }

private:
  BaseSegment* m_segment;
  bool m_owns_segment;

  Index_type m_icount;

  DepGraphNode* m_dep_graph_node;
};

}

namespace RAJA
{
template <typename T>
class RAJAVec
{
public:



  explicit RAJAVec(size_t init_cap = 0) : m_capacity(0), m_size(0), m_data(0)
  {
    grow_cap(init_cap);
  }




  RAJAVec(const RAJAVec<T>& other) : m_capacity(0), m_size(0), m_data(0)
  {
    copy(other);
  }




  void swap(RAJAVec<T>& other)
  {
    using std::swap;
    swap(m_capacity, other.m_capacity);
    swap(m_size, other.m_size);
    swap(m_data, other.m_data);
  }




  RAJAVec<T>& operator=(const RAJAVec<T>& rhs)
  {
    if (&rhs != this) {
      RAJAVec<T> copy(rhs);
      this->swap(copy);
    }
    return *this;
  }




  ~RAJAVec()
  {
    if (m_capacity > 0) delete[] m_data;
  }

  using iterator = T*;




  T* data() const { return m_data; }




  iterator end() const { return m_data + m_size; }




  iterator begin() const { return m_data; }




  size_t empty() const { return (m_size == 0); }




  size_t size() const { return m_size; }




  const T& operator[](size_t i) const { return m_data[i]; }




  T& operator[](size_t i) { return m_data[i]; }




  void push_back(const T& item) { push_back_private(item); }





  void push_front(const T& item) { push_front_private(item); }

private:



  void copy(const RAJAVec<T>& other)
  {
    grow_cap(other.m_capacity);
    for (size_t i = 0; i < other.m_size; ++i) {
      m_data[i] = other[i];
    }
    m_capacity = other.m_capacity;
    m_size = other.m_size;
  }






  static const size_t s_init_cap;
  static const double s_grow_fac;

  size_t nextCap(size_t current_cap)
  {
    if (current_cap == 0) {
      return s_init_cap;
    }
    return static_cast<size_t>(current_cap * s_grow_fac);
  }

  void grow_cap(size_t target_size)
  {
    size_t target_cap = m_capacity;
    while (target_cap < target_size) {
      target_cap = nextCap(target_cap);
    }

    if (m_capacity < target_cap) {
      T* tdata = new T[target_cap];

      if (m_data) {
        for (size_t i = 0; (i < m_size) && (i < target_cap); ++i) {
          tdata[i] = m_data[i];
        }
        delete[] m_data;
      }

      m_data = tdata;
      m_capacity = target_cap;
    }
  }

  void push_back_private(const T& item)
  {
    grow_cap(m_size + 1);
    m_data[m_size] = item;
    m_size++;
  }

  void push_front_private(const T& item)
  {
    size_t old_size = m_size;
    grow_cap(old_size + 1);

    for (size_t i = old_size; i > 0; --i) {
      m_data[i] = m_data[i - 1];
    }
    m_data[0] = item;
    m_size++;
  }

  size_t m_capacity;
  size_t m_size;
  T* m_data;
};
template <typename T>
const size_t RAJAVec<T>::s_init_cap = 8;
template <typename T>
const double RAJAVec<T>::s_grow_fac = 1.5;

}



namespace RAJA
{
class IndexSet
{
public:







  template <typename SEG_ITER_POLICY_T, typename SEG_EXEC_POLICY_T>
  struct ExecPolicy {
    typedef SEG_ITER_POLICY_T seg_it;
    typedef SEG_EXEC_POLICY_T seg_exec;
  };

  using SegVecT = RAJAVec<IndexSetSegInfo>;
  IndexSet();




  IndexSet(const IndexSet& other);




  IndexSet& operator=(const IndexSet& rhs);




  ~IndexSet();




  void swap(IndexSet& other);
  bool isValidSegmentType(const BaseSegment* segment) const;
  bool push_back_nocopy(BaseSegment* segment)
  {
    return (push_back_private(segment, false ));
  }




  bool push_front_nocopy(BaseSegment* segment)
  {
    return (push_front_private(segment, false ));
  }




  bool push_back(const BaseSegment& segment);




  bool push_front(const BaseSegment& segment);





  Index_type getLength() const { return m_len; }




  size_t getNumSegments() const { return m_segments.size(); }
  const BaseSegment* getSegment(size_t i) const
  {
    return m_segments[i].getSegment();
  }
  BaseSegment* getSegment(size_t i) { return m_segments[i].getSegment(); }






  const IndexSetSegInfo* getSegmentInfo(size_t i) const
  {
    return &(m_segments[i]);
  }






  IndexSetSegInfo* getSegmentInfo(size_t i) { return &(m_segments[i]); }

  using iterator = SegVecT::iterator;




  iterator end() const { return m_segments.end(); }




  iterator begin() const { return m_segments.begin(); }




  size_t size() const { return m_segments.size(); }
  IndexSet* createView(int begin, int end) const;
  IndexSet* createView(const int* segIds, int len) const;
  template <typename T>
  IndexSet* createView(const T& segIds) const;
  void setSegmentInterval(size_t interval_id, int begin, int end);







  int getSegmentIntervalBegin(size_t interval_id) const
  {
    return m_seg_interval_begin[interval_id];
  }

  int getSegmentIntervalEnd(size_t interval_id) const
  {
    return m_seg_interval_end[interval_id];
  }
  void* getPrivate() const { return m_private; }







  void setPrivate(void* ptr) { m_private = ptr; }
  bool dependencyGraphSet() const { return m_dep_graph_set; }
  void initDependencyGraph();







  void finalizeDependencyGraph() { m_dep_graph_set = true; }
  bool operator==(const IndexSet& other) const;




  bool operator!=(const IndexSet& other) const { return (!(*this == other)); }






  void print(std::ostream& os) const;

private:



  void copy(const IndexSet& other);





  bool push_back_private(BaseSegment* seg, bool owns_segment);





  bool push_front_private(BaseSegment* seg, bool owns_segment);





  BaseSegment* createSegmentCopy(const BaseSegment& segment) const;




  Index_type m_len;




  SegVecT m_segments;




  RAJAVec<int> m_seg_interval_begin;
  RAJAVec<int> m_seg_interval_end;




  void* m_private;





  bool m_dep_graph_set;
};
template <typename T>
IndexSet* IndexSet::createView(const T& segIds) const
{
  IndexSet* retVal = new IndexSet();

  size_t numSeg = m_segments.size();
  for (typename T::iterator it = segIds.begin(); it != segIds.end(); ++it) {
    if (*it >= 0 && *it < numSeg) {
      retVal->push_back_nocopy(
          const_cast<BaseSegment*>(m_segments[*it].getSegment()));
    }
  }

  return retVal;
}

}
namespace std
{

template <>
inline __attribute__((always_inline)) void swap(RAJA::IndexSet& a, RAJA::IndexSet& b)
{
  a.swap(b);
}
}
namespace RAJA
{
class BaseSegment
{
public:



  explicit BaseSegment(SegmentType type) : m_type(type), m_private(0) { ; }
  virtual ~BaseSegment() { ; }




  SegmentType getType() const { return m_type; }




  void* getPrivate() const { return m_private; }







  void setPrivate(void* ptr) { m_private = ptr; }
  virtual Index_type getLength() const = 0;





  virtual IndexOwnership getIndexOwnership() const = 0;





  virtual bool operator==(const BaseSegment& other) const = 0;





  virtual bool operator!=(const BaseSegment& other) const = 0;

private:



  BaseSegment();




  SegmentType m_type;




  void* m_private;
};

}





namespace RAJA
{
class RangeSegment : public BaseSegment
{
public:





  RangeSegment()
      : BaseSegment(_RangeSeg_), m_begin(UndefinedValue), m_end(UndefinedValue)
  {
    ;
  }




  RangeSegment(Index_type begin, Index_type end)
      : BaseSegment(_RangeSeg_), m_begin(begin), m_end(end)
  {
    ;
  }





  ~RangeSegment() { ; }





  RangeSegment(const RangeSegment& other)
      : BaseSegment(_RangeSeg_), m_begin(other.m_begin), m_end(other.m_end)
  {
    ;
  }





  RangeSegment& operator=(const RangeSegment& rhs)
  {
    if (&rhs != this) {
      RangeSegment copy(rhs);
      this->swap(copy);
    }
    return *this;
  }




  void swap(RangeSegment& other)
  {
    using std::swap;
    swap(m_begin, other.m_begin);
    swap(m_end, other.m_end);
  }




  Index_type getBegin() const { return m_begin; }




  void setBegin(Index_type begin) { m_begin = begin; }




  Index_type getEnd() const { return m_end; }




  void setEnd(Index_type end) { m_end = end; }




  Index_type getLength() const { return (m_end - m_begin); }





  IndexOwnership getIndexOwnership() const { return Owned; }




  bool operator==(const RangeSegment& other) const
  {
    return ((m_begin == other.m_begin) && (m_end == other.m_end));
  }




  bool operator!=(const RangeSegment& other) const
  {
    return (!(*this == other));
  }





  bool operator==(const BaseSegment& other) const
  {
    const RangeSegment* o_ptr = dynamic_cast<const RangeSegment*>(&other);
    if (o_ptr) {
      return (*this == *o_ptr);
    } else {
      return false;
    }
  }





  bool operator!=(const BaseSegment& other) const
  {
    return (!(*this == other));
  }




  void print(std::ostream& os) const;

  using iterator = Iterators::numeric_iterator<Index_type>;




  iterator end() const { return iterator(m_end); }




  iterator begin() const { return iterator(m_begin); }




  Index_type size() const { return m_end - m_begin; }


private:
  Index_type m_begin;
  Index_type m_end;
};
class RangeStrideSegment : public BaseSegment
{
public:





  RangeStrideSegment()
      : BaseSegment(_RangeStrideSeg_),
        m_begin(UndefinedValue),
        m_end(UndefinedValue),
        m_stride(UndefinedValue)
  {
    ;
  }




  RangeStrideSegment(Index_type begin, Index_type end, Index_type stride)
      : BaseSegment(_RangeStrideSeg_),
        m_begin(begin),
        m_end(end),
        m_stride(stride)
  {
    ;
  }





  ~RangeStrideSegment() { ; }





  RangeStrideSegment(const RangeStrideSegment& other)
      : BaseSegment(_RangeStrideSeg_),
        m_begin(other.m_begin),
        m_end(other.m_end),
        m_stride(other.m_stride)
  {
    ;
  }





  RangeStrideSegment& operator=(const RangeStrideSegment& rhs)
  {
    if (&rhs != this) {
      RangeStrideSegment copy(rhs);
      this->swap(copy);
    }
    return *this;
  }




  void swap(RangeStrideSegment& other)
  {
    using std::swap;
    swap(m_begin, other.m_begin);
    swap(m_end, other.m_end);
    swap(m_stride, other.m_stride);
  }




  Index_type getBegin() const { return m_begin; }




  void setBegin(Index_type begin) { m_begin = begin; }




  Index_type getEnd() const { return m_end; }




  void setEnd(Index_type end) { m_end = end; }




  Index_type getStride() const { return m_stride; }




  void setStride(Index_type stride) { m_stride = stride; }




  Index_type getLength() const
  {
    return (m_end - m_begin) >= m_stride
               ? (m_end - m_begin) % m_stride ? (m_end - m_begin) / m_stride + 1
                                              : (m_end - m_begin) / m_stride
               : 0;
  }





  IndexOwnership getIndexOwnership() const { return Owned; }




  bool operator==(const RangeStrideSegment& other) const
  {
    return ((m_begin == other.m_begin) && (m_end == other.m_end)
            && (m_stride == other.m_stride));
  }




  bool operator!=(const RangeStrideSegment& other) const
  {
    return (!(*this == other));
  }





  bool operator==(const BaseSegment& other) const
  {
    const RangeStrideSegment* o_ptr =
        dynamic_cast<const RangeStrideSegment*>(&other);
    if (o_ptr) {
      return (*this == *o_ptr);
    } else {
      return false;
    }
  }





  bool operator!=(const BaseSegment& other) const
  {
    return (!(*this == other));
  }




  void print(std::ostream& os) const;

  using iterator = Iterators::strided_numeric_iterator<Index_type>;




  iterator end() const { return iterator(m_end, m_stride); }




  iterator begin() const { return iterator(m_begin, m_stride); }




  Index_type size() const { return getLength(); }

private:
  Index_type m_begin;
  Index_type m_end;
  Index_type m_stride;
};






}




namespace RAJA
{
class ListSegment : public BaseSegment
{
public:
  ListSegment(const Index_type* indx,
              Index_type len,
              IndexOwnership indx_own = Owned);







  template <typename T>
  explicit ListSegment(const T& indx);




  ListSegment(const ListSegment& other);




  ListSegment& operator=(const ListSegment& rhs);




  ~ListSegment();




  void swap(ListSegment& other);




  const Index_type* getIndex() const { return m_indx; }




  Index_type getLength() const { return m_len; }

  using iterator = Index_type*;




  iterator end() const { return m_indx + m_len; }




  Index_type* begin() const { return m_indx; }




  Index_type size() const { return m_len; }





  IndexOwnership getIndexOwnership() const { return m_indx_own; }





  bool indicesEqual(const Index_type* indx, Index_type len) const;




  bool operator==(const ListSegment& other) const
  {
    return (indicesEqual(other.m_indx, other.m_len));
  }




  bool operator!=(const ListSegment& other) const
  {
    return (!(*this == other));
  }





  bool operator==(const BaseSegment& other) const
  {
    const ListSegment* o_ptr = dynamic_cast<const ListSegment*>(&other);
    if (o_ptr) {
      return (*this == *o_ptr);
    } else {
      return false;
    }
  }





  bool operator!=(const BaseSegment& other) const
  {
    return (!(*this == other));
  }




  void print(std::ostream& os) const;

private:



  ListSegment();





  void initIndexData(const Index_type* indx,
                     Index_type len,
                     IndexOwnership indx_own);

  Index_type* __restrict__ m_indx;
  Index_type m_len;
  IndexOwnership m_indx_own;
};
template <typename T>
ListSegment::ListSegment(const T& indx)
    : BaseSegment(_ListSeg_), m_indx(0), m_len(indx.size()), m_indx_own(Unowned)
{
  if (!indx.empty()) {







    m_indx = new Index_type[indx.size()];

    std::copy(indx.begin(), indx.end(), m_indx);
    m_indx_own = Owned;
  }
}

}


namespace std
{

template <>
inline __attribute__((always_inline)) void swap(RAJA::ListSegment& a, RAJA::ListSegment& b)
{
  a.swap(b);
}
}








namespace RAJA
{
template <typename EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall_Icount(const IndexSet& c, LOOP_BODY loop_body)
{

  forall_Icount(EXEC_POLICY_T(), c, loop_body);
}
template <typename EXEC_POLICY_T, typename Container, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall_Icount(Container&& c,
                               Index_type icount,
                               LOOP_BODY loop_body)
{
  using Iterator = decltype(std::begin(c));
  using category = typename std::iterator_traits<Iterator>::iterator_category;
  static_assert(
      std::is_base_of<std::random_access_iterator_tag, category>::value,
      "Iterators passed to RAJA must be Random Access or Contiguous iterators");

  forall_Icount(EXEC_POLICY_T(), std::forward<Container>(c), icount, loop_body);
}
template <typename EXEC_POLICY_T, typename Container, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall(Container&& c, LOOP_BODY loop_body)
{
  using category =
      typename std::iterator_traits<decltype(std::begin(c))>::iterator_category;
  static_assert(
      std::is_base_of<std::random_access_iterator_tag, category>::value,
      "Iterators passed to RAJA must be Random Access or Contiguous iterators");



  forall(EXEC_POLICY_T(), std::forward<Container>(c), loop_body);
}
template <typename EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall(Index_type begin, Index_type end, LOOP_BODY loop_body)
{
  forall<EXEC_POLICY_T>(RangeSegment(begin, end), loop_body);
}
template <typename EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall_Icount(Index_type begin,
                               Index_type end,
                               Index_type icount,
                               LOOP_BODY loop_body)
{
  forall_Icount(EXEC_POLICY_T(), RangeSegment(begin, end), icount, loop_body);
}
template <typename EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall(Index_type begin,
                        Index_type end,
                        Index_type stride,
                        LOOP_BODY loop_body)
{
  forall(EXEC_POLICY_T(), RangeStrideSegment(begin, end, stride), loop_body);
}
template <typename EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall_Icount(Index_type begin,
                               Index_type end,
                               Index_type stride,
                               Index_type icount,
                               LOOP_BODY loop_body)
{
  forall_Icount(EXEC_POLICY_T(),
                RangeStrideSegment(begin, end, stride),
                icount,
                loop_body);
}
template <typename EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall(const Index_type* idx,
                        Index_type len,
                        LOOP_BODY loop_body)
{

  forall<EXEC_POLICY_T>(ListSegment(idx, len, Unowned), loop_body);
}
template <typename EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall_Icount(const Index_type* idx,
                               Index_type len,
                               Index_type icount,
                               LOOP_BODY loop_body)
{

  forall_Icount<EXEC_POLICY_T>(ListSegment(idx, len, Unowned), icount, loop_body);
}

}









static_assert(
    201103L >= 201103L,
    "C++ standards below 2011 are not supported" "__cplusplus");
namespace VarOps
{
using std::tuple;
using std::tuple_element;
using std::tuple_cat;
using std::get;
using std::tuple_size;
using std::make_tuple;
}


namespace VarOps
{




template <class T>
struct remove_reference {
  typedef T type;
};
template <class T>
struct remove_reference<T&> {
  typedef T type;
};
template <class T>
struct remove_reference<T&&> {
  typedef T type;
};
template <class T>
 inline __attribute__((always_inline)) constexpr T&& forward(
    typename remove_reference<T>::type& t) noexcept
{
  return static_cast<T&&>(t);
}
template <class T>
 inline __attribute__((always_inline)) constexpr T&& forward(
    typename remove_reference<T>::type&& t) noexcept
{
  return static_cast<T&&>(t);
}


template <typename Op, typename... Rest>
struct foldl_impl;

template <typename Op, typename Arg1>
struct foldl_impl<Op, Arg1> {
  using Ret = Arg1;
};

template <typename Op, typename Arg1, typename Arg2>
struct foldl_impl<Op, Arg1, Arg2> {
  using Ret = typename std::result_of<Op(Arg1, Arg2)>::type;
};

template <typename Op,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename... Rest>
struct foldl_impl<Op, Arg1, Arg2, Arg3, Rest...> {
  using Ret =
      typename foldl_impl<Op,
                          typename std::result_of<Op(
                              typename std::result_of<Op(Arg1, Arg2)>::type,
                              Arg3)>::type,
                          Rest...>::Ret;
};

template <typename Op, typename Arg1>
 inline __attribute__((always_inline)) constexpr auto foldl(Op&& , Arg1&& arg) ->
    typename foldl_impl<Op, Arg1>::Ret
{
  return forward<Arg1&&>(arg);
}

template <typename Op, typename Arg1, typename Arg2>
 inline __attribute__((always_inline)) constexpr auto foldl(Op&& operation,
                                                  Arg1&& arg1,
                                                  Arg2&& arg2) ->
    typename foldl_impl<Op, Arg1, Arg2>::Ret
{
  return forward<Op&&>(operation)(forward<Arg1&&>(arg1), forward<Arg2&&>(arg2));
}

template <typename Op,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename... Rest>
 inline __attribute__((always_inline)) constexpr auto foldl(Op&& operation,
                                                  Arg1&& arg1,
                                                  Arg2&& arg2,
                                                  Arg3&& arg3,
                                                  Rest&&... rest) ->
    typename foldl_impl<Op, Arg1, Arg2, Arg3, Rest...>::Ret
{
  return foldl(forward<Op&&>(operation),
               forward<Op&&>(
                   operation)(forward<Op&&>(operation)(forward<Arg1&&>(arg1),
                                                       forward<Arg2&&>(arg2)),
                              forward<Arg3&&>(arg3)),
               forward<Rest&&>(rest)...);
}

struct adder {
  template <typename Result>
  inline __attribute__((always_inline)) constexpr Result operator()(const Result& l,
                                                           const Result& r) const
  {
    return l + r;
  }
};


template <typename Result, typename... Args>
 inline __attribute__((always_inline)) constexpr Result sum(Args... args)
{
  return foldl(adder(), args...);
}
template <size_t... Ints>
struct integer_sequence {
  using type = integer_sequence;
  static constexpr size_t size = sizeof...(Ints);
  static constexpr std::array<size_t, sizeof...(Ints)> value{Ints...};
};

template <template <class...> class Seq, class First, class... Ints>
 inline __attribute__((always_inline)) constexpr auto rotate_left_one(
    const Seq<First, Ints...>) -> Seq<Ints..., First>
{
  return Seq<Ints..., First>{};
}

template <size_t... Ints>
constexpr size_t integer_sequence<Ints...>::size;
template <size_t... Ints>
constexpr std::array<size_t, sizeof...(Ints)> integer_sequence<Ints...>::value;

namespace integer_sequence_detail
{

template <class T>
using Invoke = typename T::type;

template <class S1, class S2>
struct concat;

template <size_t... I1, size_t... I2>
struct concat<integer_sequence<I1...>, integer_sequence<I2...>>
    : integer_sequence<I1..., (sizeof...(I1) + I2)...> {
};

template <class S1, class S2>
using Concat = Invoke<concat<S1, S2>>;

template <size_t N>
struct gen_seq;
template <size_t N>
using GenSeq = Invoke<gen_seq<N>>;

template <size_t N>
struct gen_seq : Concat<GenSeq<N / 2>, GenSeq<N - N / 2>> {
};

template <>
struct gen_seq<0> : integer_sequence<> {
};
template <>
struct gen_seq<1> : integer_sequence<0> {
};
}

template <size_t Upper>
using make_index_sequence =
    typename integer_sequence_detail::gen_seq<Upper>::type;

template <size_t... Ints>
using index_sequence = integer_sequence<Ints...>;



template <typename Fn, size_t... Sequence, typename TupleLike>
 inline __attribute__((always_inline)) constexpr auto invoke_with_order(
    TupleLike&& t,
    Fn&& f,
    index_sequence<Sequence...>) -> decltype(f(get<Sequence>(t)...))
{
  return f(get<Sequence>(t)...);
}

template <typename Fn, typename TupleLike>
 inline __attribute__((always_inline)) constexpr auto invoke(TupleLike&& t, Fn&& f)
    -> decltype(
        invoke_with_order(t,
                          f,
                          make_index_sequence<tuple_size<TupleLike>::value>{}))
{
  return invoke_with_order(t,
                           f,
                           make_index_sequence<tuple_size<TupleLike>::value>{});
}


template <typename... Args>
 inline __attribute__((always_inline)) void ignore_args(Args...)
{
}



template <size_t... To, size_t... From, typename ToT, typename FromT>
 inline __attribute__((always_inline)) void assign(ToT&& dst,
                                         FromT src,
                                         index_sequence<To...>,
                                         index_sequence<From...>)
{
  ignore_args((dst[To] = src[From])...);
}

template <size_t... To, typename ToT, typename... Args>
 inline __attribute__((always_inline)) void assign_args(ToT&& dst,
                                              index_sequence<To...>,
                                              Args... args)
{
  ignore_args((dst[To] = args)...);
}


template <size_t index, size_t first, size_t... rest>
struct get_at {
  static constexpr size_t value = get_at<index - 1, rest...>::value;
};

template <size_t first, size_t... rest>
struct get_at<0, first, rest...> {
  static constexpr size_t value = first;
};


template <size_t index, typename first, typename... rest>
struct get_type_at {
  using type = typename get_type_at<index - 1, rest...>::type;
};

template <typename first, typename... rest>
struct get_type_at<0, first, rest...> {
  using type = first;
};


template <size_t diff, size_t off, size_t match, size_t... rest>
struct get_offset_impl {
  static constexpr size_t value =
      get_offset_impl<match - get_at<off + 1, rest...>::value,
                      off + 1,
                      match,
                      rest...>::value;
};

template <size_t off, size_t match, size_t... rest>
struct get_offset_impl<0, off, match, rest...> {
  static constexpr size_t value = off;
};

template <size_t match, size_t first, size_t... rest>
struct get_offset
    : public get_offset_impl<match - first, 0, match, first, rest...> {
};




template <size_t index>
struct get_arg_at {
  template <typename First, typename... Rest>
  inline __attribute__((always_inline)) static constexpr auto value(First&& ,
                                                           Rest&&... rest)
      -> decltype(VarOps::forward<
                  typename VarOps::get_type_at<index - 1, Rest...>::type>(
          get_arg_at<index - 1>::value(VarOps::forward<Rest>(rest)...)))
  {
    static_assert(index < sizeof...(Rest) + 1, "index is past the end");
    return VarOps::forward<
        typename VarOps::get_type_at<index - 1, Rest...>::type>(
        get_arg_at<index - 1>::value(VarOps::forward<Rest>(rest)...));
  }
};

template <>
struct get_arg_at<0> {
  template <typename First, typename... Rest>
  inline __attribute__((always_inline)) static constexpr auto value(First&& first,
                                                           Rest&&... )
      -> decltype(VarOps::forward<First>(first))
  {
    return VarOps::forward<First>(first);
  }
};
}
namespace RAJA
{

using PERM_I = VarOps::index_sequence<0>;
using PERM_IJ = VarOps::index_sequence<0, 1>;
using PERM_JI = VarOps::index_sequence<1, 0>;
using PERM_IJK = VarOps::index_sequence<0, 1, 2>;
using PERM_IKJ = VarOps::index_sequence<0, 2, 1>;
using PERM_JIK = VarOps::index_sequence<1, 0, 2>;
using PERM_JKI = VarOps::index_sequence<1, 2, 0>;
using PERM_KIJ = VarOps::index_sequence<2, 0, 1>;
using PERM_KJI = VarOps::index_sequence<2, 1, 0>;
using PERM_IJKL = VarOps::index_sequence<0, 1, 2, 3>;
using PERM_IJLK = VarOps::index_sequence<0, 1, 3, 2>;
using PERM_IKJL = VarOps::index_sequence<0, 2, 1, 3>;
using PERM_IKLJ = VarOps::index_sequence<0, 2, 3, 1>;
using PERM_ILJK = VarOps::index_sequence<0, 3, 1, 2>;
using PERM_ILKJ = VarOps::index_sequence<0, 3, 2, 1>;
using PERM_JIKL = VarOps::index_sequence<1, 0, 2, 3>;
using PERM_JILK = VarOps::index_sequence<1, 0, 3, 2>;
using PERM_JKIL = VarOps::index_sequence<1, 2, 0, 3>;
using PERM_JKLI = VarOps::index_sequence<1, 2, 3, 0>;
using PERM_JLIK = VarOps::index_sequence<1, 3, 0, 2>;
using PERM_JLKI = VarOps::index_sequence<1, 3, 2, 0>;
using PERM_KIJL = VarOps::index_sequence<2, 0, 1, 3>;
using PERM_KILJ = VarOps::index_sequence<2, 0, 3, 1>;
using PERM_KJIL = VarOps::index_sequence<2, 1, 0, 3>;
using PERM_KJLI = VarOps::index_sequence<2, 1, 3, 0>;
using PERM_KLIJ = VarOps::index_sequence<2, 3, 0, 1>;
using PERM_KLJI = VarOps::index_sequence<2, 3, 1, 0>;
using PERM_LIJK = VarOps::index_sequence<3, 0, 1, 2>;
using PERM_LIKJ = VarOps::index_sequence<3, 0, 2, 1>;
using PERM_LJIK = VarOps::index_sequence<3, 1, 0, 2>;
using PERM_LJKI = VarOps::index_sequence<3, 1, 2, 0>;
using PERM_LKIJ = VarOps::index_sequence<3, 2, 0, 1>;
using PERM_LKJI = VarOps::index_sequence<3, 2, 1, 0>;
using PERM_IJKLM = VarOps::index_sequence<0, 1, 2, 3, 4>;
using PERM_IJKML = VarOps::index_sequence<0, 1, 2, 4, 3>;
using PERM_IJLKM = VarOps::index_sequence<0, 1, 3, 2, 4>;
using PERM_IJLMK = VarOps::index_sequence<0, 1, 3, 4, 2>;
using PERM_IJMKL = VarOps::index_sequence<0, 1, 4, 2, 3>;
using PERM_IJMLK = VarOps::index_sequence<0, 1, 4, 3, 2>;
using PERM_IKJLM = VarOps::index_sequence<0, 2, 1, 3, 4>;
using PERM_IKJML = VarOps::index_sequence<0, 2, 1, 4, 3>;
using PERM_IKLJM = VarOps::index_sequence<0, 2, 3, 1, 4>;
using PERM_IKLMJ = VarOps::index_sequence<0, 2, 3, 4, 1>;
using PERM_IKMJL = VarOps::index_sequence<0, 2, 4, 1, 3>;
using PERM_IKMLJ = VarOps::index_sequence<0, 2, 4, 3, 1>;
using PERM_ILJKM = VarOps::index_sequence<0, 3, 1, 2, 4>;
using PERM_ILJMK = VarOps::index_sequence<0, 3, 1, 4, 2>;
using PERM_ILKJM = VarOps::index_sequence<0, 3, 2, 1, 4>;
using PERM_ILKMJ = VarOps::index_sequence<0, 3, 2, 4, 1>;
using PERM_ILMJK = VarOps::index_sequence<0, 3, 4, 1, 2>;
using PERM_ILMKJ = VarOps::index_sequence<0, 3, 4, 2, 1>;
using PERM_IMJKL = VarOps::index_sequence<0, 4, 1, 2, 3>;
using PERM_IMJLK = VarOps::index_sequence<0, 4, 1, 3, 2>;
using PERM_IMKJL = VarOps::index_sequence<0, 4, 2, 1, 3>;
using PERM_IMKLJ = VarOps::index_sequence<0, 4, 2, 3, 1>;
using PERM_IMLJK = VarOps::index_sequence<0, 4, 3, 1, 2>;
using PERM_IMLKJ = VarOps::index_sequence<0, 4, 3, 2, 1>;
using PERM_JIKLM = VarOps::index_sequence<1, 0, 2, 3, 4>;
using PERM_JIKML = VarOps::index_sequence<1, 0, 2, 4, 3>;
using PERM_JILKM = VarOps::index_sequence<1, 0, 3, 2, 4>;
using PERM_JILMK = VarOps::index_sequence<1, 0, 3, 4, 2>;
using PERM_JIMKL = VarOps::index_sequence<1, 0, 4, 2, 3>;
using PERM_JIMLK = VarOps::index_sequence<1, 0, 4, 3, 2>;
using PERM_JKILM = VarOps::index_sequence<1, 2, 0, 3, 4>;
using PERM_JKIML = VarOps::index_sequence<1, 2, 0, 4, 3>;
using PERM_JKLIM = VarOps::index_sequence<1, 2, 3, 0, 4>;
using PERM_JKLMI = VarOps::index_sequence<1, 2, 3, 4, 0>;
using PERM_JKMIL = VarOps::index_sequence<1, 2, 4, 0, 3>;
using PERM_JKMLI = VarOps::index_sequence<1, 2, 4, 3, 0>;
using PERM_JLIKM = VarOps::index_sequence<1, 3, 0, 2, 4>;
using PERM_JLIMK = VarOps::index_sequence<1, 3, 0, 4, 2>;
using PERM_JLKIM = VarOps::index_sequence<1, 3, 2, 0, 4>;
using PERM_JLKMI = VarOps::index_sequence<1, 3, 2, 4, 0>;
using PERM_JLMIK = VarOps::index_sequence<1, 3, 4, 0, 2>;
using PERM_JLMKI = VarOps::index_sequence<1, 3, 4, 2, 0>;
using PERM_JMIKL = VarOps::index_sequence<1, 4, 0, 2, 3>;
using PERM_JMILK = VarOps::index_sequence<1, 4, 0, 3, 2>;
using PERM_JMKIL = VarOps::index_sequence<1, 4, 2, 0, 3>;
using PERM_JMKLI = VarOps::index_sequence<1, 4, 2, 3, 0>;
using PERM_JMLIK = VarOps::index_sequence<1, 4, 3, 0, 2>;
using PERM_JMLKI = VarOps::index_sequence<1, 4, 3, 2, 0>;
using PERM_KIJLM = VarOps::index_sequence<2, 0, 1, 3, 4>;
using PERM_KIJML = VarOps::index_sequence<2, 0, 1, 4, 3>;
using PERM_KILJM = VarOps::index_sequence<2, 0, 3, 1, 4>;
using PERM_KILMJ = VarOps::index_sequence<2, 0, 3, 4, 1>;
using PERM_KIMJL = VarOps::index_sequence<2, 0, 4, 1, 3>;
using PERM_KIMLJ = VarOps::index_sequence<2, 0, 4, 3, 1>;
using PERM_KJILM = VarOps::index_sequence<2, 1, 0, 3, 4>;
using PERM_KJIML = VarOps::index_sequence<2, 1, 0, 4, 3>;
using PERM_KJLIM = VarOps::index_sequence<2, 1, 3, 0, 4>;
using PERM_KJLMI = VarOps::index_sequence<2, 1, 3, 4, 0>;
using PERM_KJMIL = VarOps::index_sequence<2, 1, 4, 0, 3>;
using PERM_KJMLI = VarOps::index_sequence<2, 1, 4, 3, 0>;
using PERM_KLIJM = VarOps::index_sequence<2, 3, 0, 1, 4>;
using PERM_KLIMJ = VarOps::index_sequence<2, 3, 0, 4, 1>;
using PERM_KLJIM = VarOps::index_sequence<2, 3, 1, 0, 4>;
using PERM_KLJMI = VarOps::index_sequence<2, 3, 1, 4, 0>;
using PERM_KLMIJ = VarOps::index_sequence<2, 3, 4, 0, 1>;
using PERM_KLMJI = VarOps::index_sequence<2, 3, 4, 1, 0>;
using PERM_KMIJL = VarOps::index_sequence<2, 4, 0, 1, 3>;
using PERM_KMILJ = VarOps::index_sequence<2, 4, 0, 3, 1>;
using PERM_KMJIL = VarOps::index_sequence<2, 4, 1, 0, 3>;
using PERM_KMJLI = VarOps::index_sequence<2, 4, 1, 3, 0>;
using PERM_KMLIJ = VarOps::index_sequence<2, 4, 3, 0, 1>;
using PERM_KMLJI = VarOps::index_sequence<2, 4, 3, 1, 0>;
using PERM_LIJKM = VarOps::index_sequence<3, 0, 1, 2, 4>;
using PERM_LIJMK = VarOps::index_sequence<3, 0, 1, 4, 2>;
using PERM_LIKJM = VarOps::index_sequence<3, 0, 2, 1, 4>;
using PERM_LIKMJ = VarOps::index_sequence<3, 0, 2, 4, 1>;
using PERM_LIMJK = VarOps::index_sequence<3, 0, 4, 1, 2>;
using PERM_LIMKJ = VarOps::index_sequence<3, 0, 4, 2, 1>;
using PERM_LJIKM = VarOps::index_sequence<3, 1, 0, 2, 4>;
using PERM_LJIMK = VarOps::index_sequence<3, 1, 0, 4, 2>;
using PERM_LJKIM = VarOps::index_sequence<3, 1, 2, 0, 4>;
using PERM_LJKMI = VarOps::index_sequence<3, 1, 2, 4, 0>;
using PERM_LJMIK = VarOps::index_sequence<3, 1, 4, 0, 2>;
using PERM_LJMKI = VarOps::index_sequence<3, 1, 4, 2, 0>;
using PERM_LKIJM = VarOps::index_sequence<3, 2, 0, 1, 4>;
using PERM_LKIMJ = VarOps::index_sequence<3, 2, 0, 4, 1>;
using PERM_LKJIM = VarOps::index_sequence<3, 2, 1, 0, 4>;
using PERM_LKJMI = VarOps::index_sequence<3, 2, 1, 4, 0>;
using PERM_LKMIJ = VarOps::index_sequence<3, 2, 4, 0, 1>;
using PERM_LKMJI = VarOps::index_sequence<3, 2, 4, 1, 0>;
using PERM_LMIJK = VarOps::index_sequence<3, 4, 0, 1, 2>;
using PERM_LMIKJ = VarOps::index_sequence<3, 4, 0, 2, 1>;
using PERM_LMJIK = VarOps::index_sequence<3, 4, 1, 0, 2>;
using PERM_LMJKI = VarOps::index_sequence<3, 4, 1, 2, 0>;
using PERM_LMKIJ = VarOps::index_sequence<3, 4, 2, 0, 1>;
using PERM_LMKJI = VarOps::index_sequence<3, 4, 2, 1, 0>;
using PERM_MIJKL = VarOps::index_sequence<4, 0, 1, 2, 3>;
using PERM_MIJLK = VarOps::index_sequence<4, 0, 1, 3, 2>;
using PERM_MIKJL = VarOps::index_sequence<4, 0, 2, 1, 3>;
using PERM_MIKLJ = VarOps::index_sequence<4, 0, 2, 3, 1>;
using PERM_MILJK = VarOps::index_sequence<4, 0, 3, 1, 2>;
using PERM_MILKJ = VarOps::index_sequence<4, 0, 3, 2, 1>;
using PERM_MJIKL = VarOps::index_sequence<4, 1, 0, 2, 3>;
using PERM_MJILK = VarOps::index_sequence<4, 1, 0, 3, 2>;
using PERM_MJKIL = VarOps::index_sequence<4, 1, 2, 0, 3>;
using PERM_MJKLI = VarOps::index_sequence<4, 1, 2, 3, 0>;
using PERM_MJLIK = VarOps::index_sequence<4, 1, 3, 0, 2>;
using PERM_MJLKI = VarOps::index_sequence<4, 1, 3, 2, 0>;
using PERM_MKIJL = VarOps::index_sequence<4, 2, 0, 1, 3>;
using PERM_MKILJ = VarOps::index_sequence<4, 2, 0, 3, 1>;
using PERM_MKJIL = VarOps::index_sequence<4, 2, 1, 0, 3>;
using PERM_MKJLI = VarOps::index_sequence<4, 2, 1, 3, 0>;
using PERM_MKLIJ = VarOps::index_sequence<4, 2, 3, 0, 1>;
using PERM_MKLJI = VarOps::index_sequence<4, 2, 3, 1, 0>;
using PERM_MLIJK = VarOps::index_sequence<4, 3, 0, 1, 2>;
using PERM_MLIKJ = VarOps::index_sequence<4, 3, 0, 2, 1>;
using PERM_MLJIK = VarOps::index_sequence<4, 3, 1, 0, 2>;
using PERM_MLJKI = VarOps::index_sequence<4, 3, 1, 2, 0>;
using PERM_MLKIJ = VarOps::index_sequence<4, 3, 2, 0, 1>;
using PERM_MLKJI = VarOps::index_sequence<4, 3, 2, 1, 0>;
}

namespace RAJA
{

template <typename Range, typename IdxLin = Index_type>
struct LayoutBase_impl { };

template <size_t... RangeInts,
        typename IdxLin>
struct LayoutBase_impl<VarOps::index_sequence<RangeInts...>,
        IdxLin> {
public:
  typedef IdxLin IndexLinear;
  typedef VarOps::make_index_sequence<sizeof...(RangeInts)> IndexRange;

  static constexpr size_t n_dims = sizeof...(RangeInts);
  static constexpr size_t limit = std::numeric_limits<IdxLin>::max();



  const IdxLin sizes[n_dims];
  IdxLin strides[n_dims];
  IdxLin mods[n_dims];



  template <typename... Types>
  inline __attribute__((always_inline)) LayoutBase_impl(Types... ns)
  : sizes{convertIndex<IdxLin>(ns)...}
  {
    static_assert(n_dims == sizeof ... (Types), "number of dimensions must match");
    for (size_t i = 0; i < n_dims; i++) {
      strides[i] = 1;
      for (size_t j = 0; j < i; j++) {
        strides[j] *= sizes[i];
      }
    }

    for (size_t i = 1; i < n_dims; i++) {
      mods[i] = strides[i - 1];
    }
    mods[0] = limit;
  }

  constexpr inline __attribute__((always_inline)) LayoutBase_impl(const LayoutBase_impl<IndexRange, IdxLin>& rhs)
  : sizes{rhs.sizes[RangeInts]...},
    strides{rhs.strides[RangeInts]...},
    mods{rhs.mods[RangeInts]...}
  { }

  template <typename... Types>
  constexpr inline __attribute__((always_inline)) LayoutBase_impl(const std::array<IdxLin, n_dims> &sizes_in,
                                        const std::array<IdxLin, n_dims> &strides_in,
                                        const std::array<IdxLin, n_dims> &mods_in)
  : sizes{sizes_in[RangeInts]...},
    strides{strides_in[RangeInts]...},
    mods{mods_in[RangeInts]...}
  { }

  template<typename... Indices>
  inline __attribute__((always_inline)) constexpr IdxLin operator()(Indices... indices) const
  {
    return VarOps::sum<IdxLin>((indices * strides[RangeInts])...);
  }

  template<typename... Indices>
  inline __attribute__((always_inline)) void toIndices(IdxLin linear_index,
                                              Indices &... indices) const
  {
    VarOps::ignore_args( (indices = (linear_index / strides[RangeInts]) % sizes[RangeInts])... );
  }
};

template <size_t... RangeInts, typename IdxLin>
constexpr size_t LayoutBase_impl<VarOps::index_sequence<RangeInts...>, IdxLin>::n_dims;
template <size_t... RangeInts, typename IdxLin>
constexpr size_t LayoutBase_impl<VarOps::index_sequence<RangeInts...>, IdxLin>::limit;

template <size_t n_dims, typename IdxLin = Index_type>
using Layout = LayoutBase_impl<VarOps::make_index_sequence<n_dims>, IdxLin>;


}

namespace RAJA
{

template <size_t Rank, typename IdxLin = Index_type>
auto make_permuted_layout(std::array<IdxLin, Rank> sizes,
                          std::array<size_t, Rank> permutation) ->
Layout<Rank, IdxLin>
{
  std::array<IdxLin, Rank> strides, mods;
  std::array<IdxLin, Rank> folded_strides, lmods;
  for (size_t i = 0; i < Rank; ++i) {
    folded_strides[i] = 1;
    for (size_t j = 0; j < i; ++j) {
      folded_strides[j] *= sizes[permutation[i]];
    }
  }

  for (size_t i = 0; i < Rank; ++i) {
    strides[permutation[i]] = folded_strides[i];
  }

  for (size_t i = 1; i < Rank; i++) {
    lmods[i] = folded_strides[i - 1];
  }
  lmods[0] = std::numeric_limits<IdxLin>::max();

  for (size_t i = 0; i < Rank; ++i) {
    mods[permutation[i]] = lmods[i];
  }

  return Layout<Rank, IdxLin>(sizes, strides, mods);
}


template<size_t ... Ints>
using Perm = VarOps::index_sequence<Ints...>;
template<size_t N>
using MakePerm = VarOps::make_index_sequence<N>;

}
namespace RAJA
{

namespace internal
{

template <typename Range, typename IdxLin>
struct OffsetLayout_impl;

template <size_t... RangeInts, typename IdxLin>
struct OffsetLayout_impl<VarOps::index_sequence<RangeInts...>, IdxLin> {
  using IndexRange = VarOps::index_sequence<RangeInts...>;
  using Base = LayoutBase_impl<IndexRange, IdxLin>;
  Base base_;

  IdxLin offsets[sizeof...(RangeInts)];

  constexpr inline __attribute__((always_inline))
  OffsetLayout_impl(std::array<IdxLin, sizeof...(RangeInts)> lower,
                    std::array<IdxLin, sizeof...(RangeInts)> upper)
      : base_{(upper[RangeInts] - lower[RangeInts] + 1)...},
        offsets{lower[RangeInts]...}
  {
  }

  template <typename... Indices>
  inline __attribute__((always_inline)) constexpr IdxLin operator()(
      Indices... indices) const
  {
    return base_((indices - offsets[RangeInts])...);
  }

  static inline __attribute__((always_inline)) OffsetLayout_impl<IndexRange, IdxLin>
  from_layout_and_offsets(
      const std::array<IdxLin, sizeof...(RangeInts)>& offsets_in,
      const Layout<sizeof...(RangeInts), IdxLin>& rhs)
  {
    return internal::OffsetLayout_impl<IndexRange, IdxLin>(offsets_in, rhs);
  }

private:
  constexpr inline __attribute__((always_inline))
  OffsetLayout_impl(const std::array<IdxLin, sizeof...(RangeInts)>& offsets_in,
                    const Layout<sizeof...(RangeInts), IdxLin>& rhs)
      : base_{rhs}, offsets{offsets_in[RangeInts]...}
  {
  }
};

}

template <size_t n_dims = 1, typename IdxLin = Index_type>
struct OffsetLayout
    : public internal::OffsetLayout_impl<VarOps::make_index_sequence<n_dims>,
                                         IdxLin> {
  using parent = internal::OffsetLayout_impl<
      VarOps::make_index_sequence<n_dims>, IdxLin>;

  using internal::OffsetLayout_impl<VarOps::make_index_sequence<n_dims>,
                                    IdxLin>::OffsetLayout_impl;

  constexpr inline __attribute__((always_inline)) OffsetLayout(
      const internal::OffsetLayout_impl<VarOps::make_index_sequence<n_dims>,
                                        IdxLin> &rhs)
      : parent{rhs}
  {
  }
};

template <size_t n_dims, typename IdxLin = Index_type>
auto make_offset_layout(const std::array<IdxLin, n_dims>& lower,
                        const std::array<IdxLin, n_dims>& upper)
    -> OffsetLayout<n_dims, IdxLin>
{
  return OffsetLayout<n_dims, IdxLin>{lower, upper};
}

template <size_t Rank, typename IdxLin = Index_type>
auto make_permuted_offset_layout(const std::array<IdxLin,
                                                            Rank>&
                                                            lower,
                                           const std::array<IdxLin,
                                                            Rank>&
                                                            upper,
                                           const std::array<size_t,
                                                            Rank>&
                                                            permutation)
    -> decltype(make_offset_layout<Rank, IdxLin>(lower, upper))
{
  std::array<IdxLin, Rank> sizes;
  for (size_t i=0; i < Rank; ++i) {
    sizes[i] = upper[i] - lower[i] + 1;
  }
  return internal::OffsetLayout_impl<VarOps::make_index_sequence<Rank>, IdxLin>::from_layout_and_offsets
      (lower, make_permuted_layout(sizes, permutation));
}

}
namespace RAJA
{

template <typename DataType, typename LayoutT>
struct View {
  LayoutT const layout;
  DataType *data;

  template <typename... Args>
  inline __attribute__((always_inline)) constexpr View(DataType *data_ptr, Args... dim_sizes)
      : layout(dim_sizes...), data(data_ptr)
  {
  }

  inline __attribute__((always_inline)) constexpr View(DataType *data_ptr, LayoutT &&layout)
      : layout(layout), data(data_ptr)
  {
  }

  inline __attribute__((always_inline)) void set_data(DataType *data_ptr) {
      data = data_ptr;
  }



  template <typename... Args>
  inline __attribute__((always_inline)) DataType &operator()(Args... args) const
  {
    return data[convertIndex<Index_type>(layout(args...))];
  }
};

template <typename DataType, typename LayoutT, typename... IndexTypes>
struct TypedView {
  using Base = View<DataType, LayoutT>;

  Base base_;

  template <typename... Args>
  inline __attribute__((always_inline)) constexpr TypedView(DataType *data_ptr, Args... dim_sizes)
      : base_(data_ptr, dim_sizes...)
  {
  }

  inline __attribute__((always_inline)) constexpr TypedView(DataType *data_ptr, LayoutT &&layout)
      : base_(data_ptr, layout)
  {
  }

  inline __attribute__((always_inline)) void set_data(DataType *data_ptr) {
      base_.set_data(data_ptr);
  }

  inline __attribute__((always_inline)) DataType &operator()(IndexTypes... args) const
  {
    return base_.operator()(convertIndex<Index_type>(args)...);
  }
};


}
namespace RAJA
{
struct seq_exec : public PolicyBase {
};




struct seq_segit : public seq_exec {
};
struct seq_reduce {
};

}

namespace RAJA
{
template <typename SEG_EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void executeRangeList_forall(const IndexSetSegInfo* seg_info,
                                         LOOP_BODY&& loop_body)
{
  const BaseSegment* iseg = seg_info->getSegment();
  SegmentType segtype = iseg->getType();

  switch (segtype) {
    case _RangeSeg_: {
      const RangeSegment* tseg = static_cast<const RangeSegment*>(iseg);
      forall<SEG_EXEC_POLICY_T>(*tseg, loop_body);
      break;
    }
    case _ListSeg_: {
      const ListSegment* tseg = static_cast<const ListSegment*>(iseg);
      forall<SEG_EXEC_POLICY_T>(*tseg, loop_body);
      break;
    }

    default: {
    }

  }
}
template <typename SEG_EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void executeRangeList_forall_Icount(const IndexSetSegInfo* seg_info,
                                                LOOP_BODY&& loop_body)
{
  const BaseSegment* iseg = seg_info->getSegment();
  SegmentType segtype = iseg->getType();

  Index_type icount = seg_info->getIcount();

  switch (segtype) {
    case _RangeSeg_: {
      const RangeSegment* tseg = static_cast<const RangeSegment*>(iseg);
      forall_Icount<SEG_EXEC_POLICY_T>(*tseg, icount, loop_body);
      break;
    }
    case _ListSeg_: {
      const ListSegment* tseg = static_cast<const ListSegment*>(iseg);
      forall_Icount<SEG_EXEC_POLICY_T>(*tseg, icount, loop_body);
      break;
    }

    default: {
    }

  }
}
template <typename SEG_EXEC_POLICY_T, typename LOOP_BODY>
struct rangeListExecutor {
  constexpr rangeListExecutor(LOOP_BODY&& body) : body(body) {}
  inline __attribute__((always_inline))
  void operator()(const IndexSetSegInfo& seg_info)
  {
    executeRangeList_forall<SEG_EXEC_POLICY_T>(&seg_info, body);
  }

private:
  LOOP_BODY body;
};

template <typename SEG_EXEC_POLICY_T, typename LOOP_BODY>
constexpr inline __attribute__((always_inline)) rangeListExecutor<SEG_EXEC_POLICY_T, LOOP_BODY>
makeRangeListExecutor(LOOP_BODY&& body)
{
  return rangeListExecutor<SEG_EXEC_POLICY_T, LOOP_BODY>(body);
}

template <typename SEG_IT_POLICY_T,
          typename SEG_EXEC_POLICY_T,
          typename LOOP_BODY>
inline __attribute__((always_inline)) void forall(
    IndexSet::ExecPolicy<SEG_IT_POLICY_T, SEG_EXEC_POLICY_T>,
    const IndexSet& iset,
    LOOP_BODY loop_body)
{
  forall<SEG_IT_POLICY_T>(iset,
                          makeRangeListExecutor<SEG_EXEC_POLICY_T>(loop_body));
}

template <typename SEG_EXEC_POLICY_T, typename LOOP_BODY>
struct rangeListIcountExecutor {
  constexpr rangeListIcountExecutor(LOOP_BODY&& body) : body(body) {}
  inline __attribute__((always_inline))
  void operator()(const IndexSetSegInfo& seg_info)
  {
    executeRangeList_forall_Icount<SEG_EXEC_POLICY_T>(&seg_info, body);
  }

private:
  LOOP_BODY body;
};

template <typename SEG_EXEC_POLICY_T, typename LOOP_BODY>
constexpr inline __attribute__((always_inline)) rangeListIcountExecutor<SEG_EXEC_POLICY_T, LOOP_BODY>
makeRangeListIcountExecutor(LOOP_BODY&& body)
{
  return rangeListIcountExecutor<SEG_EXEC_POLICY_T, LOOP_BODY>(body);
}

template <typename SEG_IT_POLICY_T,
          typename SEG_EXEC_POLICY_T,
          typename LOOP_BODY>
inline __attribute__((always_inline)) void forall_Icount(
    IndexSet::ExecPolicy<SEG_IT_POLICY_T, SEG_EXEC_POLICY_T>,
    const IndexSet& iset,
    LOOP_BODY loop_body)
{

  forall<SEG_IT_POLICY_T>(iset,
                          makeRangeListIcountExecutor<SEG_EXEC_POLICY_T>(
                              loop_body));
}


}

namespace RAJA
{
template <typename Func>
inline __attribute__((always_inline)) void forall(const PolicyBase &,
                        const RangeSegment &iter,
                        Func &&loop_body)
{
  auto end = iter.getEnd();
  for (auto ii = iter.getBegin(); ii < end; ++ii) {
    loop_body(ii);
  }
}

template <typename Iterable, typename Func>
inline __attribute__((always_inline)) void forall(const PolicyBase &, Iterable &&iter, Func &&loop_body)
{
  auto end = std::end(iter);
  for (auto ii = std::begin(iter); ii < end; ++ii) {
    loop_body(*ii);
  }
}

template <typename Iterable, typename Func>
inline __attribute__((always_inline)) void forall_Icount(const PolicyBase &,
                               Iterable &&iter,
                               Index_type icount,
                               Func &&loop_body)
{
  auto begin = std::begin(iter);
  auto end = std::end(iter);
  auto distance = std::distance(begin, end);
  for (Index_type i = 0; i < distance; ++i) {
    loop_body(i + icount, begin[i]);
  }
}

}

namespace RAJA
{




void * allocate_aligned(size_t alignment, size_t size);




template<typename T>
T * allocate_aligned_type(size_t alignment, size_t size) {
    return reinterpret_cast<T*>(allocate_aligned(alignment, size));
}





void free_aligned(void* ptr);





typedef double CPUReductionBlockDataType;
int getCPUReductionId();
void releaseCPUReductionId(int id);
CPUReductionBlockDataType* getCPUReductionMemBlock(int id);
void freeCPUReductionMemBlock();
Index_type* getCPUReductionLocBlock(int id);
void freeCPUReductionLocBlock();

}

namespace RAJA
{
template <typename T>
class ReduceMin<seq_reduce, T>
{
  using my_type = ReduceMin<seq_reduce, T>;

public:



  explicit ReduceMin(T init_m_val) :
    m_parent(__null), m_val(init_m_val)
  {
  }




  ReduceMin(const ReduceMin<seq_reduce, T>& other) :
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_val)
  {
  }





  ~ReduceMin<seq_reduce, T>()
  {
    if (m_parent) {
      m_parent->min(m_val);
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }




  ReduceMin<seq_reduce, T>& min(T rhs)
  {
    m_val = (((rhs) < (m_val)) ? (rhs) : (m_val));
    return *this;
  }

  const ReduceMin<seq_reduce, T>& min(T rhs) const
  {
    m_val = (((rhs) < (m_val)) ? (rhs) : (m_val));
    return *this;
  }

private:



  ReduceMin<seq_reduce, T>();

  const my_type * m_parent;
  mutable T m_val;
};
template <typename T>
class ReduceMinLoc<seq_reduce, T>
{
  using my_type = ReduceMinLoc<seq_reduce, T>;

public:



  explicit ReduceMinLoc(T init_m_val, Index_type init_loc) :
    m_parent(__null), m_val(init_m_val), loc(init_loc)
  {
  }




  ReduceMinLoc(const ReduceMinLoc<seq_reduce, T>& other) :
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_val),
    loc(other.loc)
  {
  }





  ~ReduceMinLoc<seq_reduce, T>()
  {
    if (m_parent) {
      m_parent->minloc(m_val, loc);
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }




  Index_type getLoc()
  {
    return loc;
  }




  ReduceMinLoc<seq_reduce, T>& minloc(T rhs, Index_type idx)
  {
    if (rhs < m_val) {
      m_val = rhs;
      loc = idx;
    }
    return *this;
  }

  const ReduceMinLoc<seq_reduce, T>& minloc(T rhs, Index_type idx) const
  {
    if (rhs < m_val) {
      m_val = rhs;
      loc = idx;
    }
    return *this;
  }

private:



  ReduceMinLoc<seq_reduce, T>();

  const my_type * m_parent;

  mutable T m_val;
  mutable Index_type loc;
};
template <typename T>
class ReduceMax<seq_reduce, T>
{
  using my_type = ReduceMax<seq_reduce, T>;

public:



  explicit ReduceMax(T init_m_val) :
    m_parent(__null),
    m_val(init_m_val)
  {
  }




  ReduceMax(const ReduceMax<seq_reduce, T>& other) :
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_val)
  {
  }





  ~ReduceMax<seq_reduce, T>()
  {
    if (m_parent) {
      m_parent->max(m_val);
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }




  ReduceMax<seq_reduce, T>& max(T rhs)
  {
    m_val = (((m_val) > (rhs)) ? (m_val) : (rhs));
    return *this;
  }

  const ReduceMax<seq_reduce, T>& max(T rhs) const
  {
    m_val = (((m_val) > (rhs)) ? (m_val) : (rhs));
    return *this;
  }

private:



  ReduceMax<seq_reduce, T>();

  const my_type * m_parent;

  mutable T m_val;
};
template <typename T>
class ReduceMaxLoc<seq_reduce, T>
{
  using my_type = ReduceMaxLoc<seq_reduce, T>;

public:



  explicit ReduceMaxLoc(T init_m_val, Index_type init_loc) :
    m_parent(__null),
    m_val(init_m_val),
    loc(init_loc)
  {
  }




  ReduceMaxLoc(const ReduceMaxLoc<seq_reduce, T>& other) :
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_val),
    loc(other.loc)
  {
  }





  ~ReduceMaxLoc<seq_reduce, T>()
  {
    if (m_parent) {
      m_parent->maxloc(m_val, loc);
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }




  Index_type getLoc()
  {
    return loc;
  }




  ReduceMaxLoc<seq_reduce, T>& maxloc(T rhs, Index_type idx)
  {
    if (rhs > m_val) {
      m_val = rhs;
      loc = idx;
    }
    return *this;
  }

  const ReduceMaxLoc<seq_reduce, T>& maxloc(T rhs, Index_type idx) const
  {
    if (rhs > m_val) {
      m_val = rhs;
      loc = idx;
    }
    return *this;
  }

private:



  ReduceMaxLoc<seq_reduce, T>();

  const my_type * m_parent;

  mutable T m_val;
  mutable Index_type loc;
};
template <typename T>
class ReduceSum<seq_reduce, T>
{
  using my_type = ReduceSum<seq_reduce, T>;

public:



  explicit ReduceSum(T init_m_val, T initializer = 0) :
    m_parent(__null),
    m_val(init_m_val),
    m_custom_init(initializer)
  {
  }




  ReduceSum(const ReduceSum<seq_reduce, T>& other) :
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_custom_init),
    m_custom_init(other.m_custom_init)
  {
  }





  ~ReduceSum<seq_reduce, T>()
  {
    if (m_parent) {
      *m_parent += m_val;
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }




  ReduceSum<seq_reduce, T>& operator+=(T rhs)
  {
    this->m_val += rhs;
    return *this;
  }

  const ReduceSum<seq_reduce, T>& operator+=(T rhs) const
  {
    this->m_val += rhs;
    return *this;
  }

private:



  ReduceSum<seq_reduce, T>();

  const my_type * m_parent;

  mutable T m_val;
  T m_custom_init;
};

}
namespace RAJA
{
namespace detail
{
namespace scan
{





template <typename Iter, typename BinFn>
void inclusive_inplace(const ::RAJA::seq_exec&,
                       Iter begin,
                       Iter end,
                       BinFn f)
{
  using Value = typename ::std::iterator_traits<Iter>::value_type;
  Value agg = *begin;
  for (Iter i = ++begin; i != end; ++i) {
    agg = f(*i, agg);
    *i = agg;
  }
}





template <typename Iter, typename BinFn, typename T>
void exclusive_inplace(const ::RAJA::seq_exec&,
                       Iter begin,
                       Iter end,
                       BinFn f,
                       T v)
{
  using Value = typename ::std::iterator_traits<Iter>::value_type;
  const int n = end - begin;
  Value agg = v;
  for (int i = 0; i < n; ++i) {
    Value t = *(begin + i);
    *(begin + i) = agg;
    agg = f(agg, t);
  }
}





template <typename Iter, typename OutIter, typename BinFn>
void inclusive(const ::RAJA::seq_exec&,
               Iter begin,
               Iter end,
               OutIter out,
               BinFn f)
{
  using Value = typename ::std::iterator_traits<Iter>::value_type;
  Value agg = *begin;
  *out++ = agg;
  for (Iter i = begin + 1; i != end; ++i) {
    agg = f(agg, *i);
    *out++ = agg;
  }
}





template <typename Iter, typename OutIter, typename BinFn, typename T>
void exclusive(const ::RAJA::seq_exec&,
               Iter begin,
               Iter end,
               OutIter out,
               BinFn f,
               T v)
{
  using Value = typename ::std::iterator_traits<Iter>::value_type;
  Value agg = v;
  OutIter o = out;
  *o++ = v;
  for (Iter i = begin; i != end - 1; ++i, ++o) {
    agg = f(*i, agg);
    *o = agg;
  }
}

}

}

}




namespace RAJA
{

struct simd_exec {
};
}
namespace RAJA
{

template <typename Iterable, typename Func>
inline __attribute__((always_inline)) void forall(const simd_exec &, Iterable &&iter, Func &&loop_body)
{
  auto end = std::end(iter);
 
  for (auto ii = std::begin(iter); ii < end; ++ii) {
    loop_body(*ii);
  }
}

template <typename Iterable, typename Func>
inline __attribute__((always_inline)) void forall_Icount(const simd_exec &,
                               Iterable &&iter,
                               Index_type icount,
                               Func &&loop_body)
{
  auto begin = std::begin(iter);
  auto end = std::end(iter);
  auto distance = std::distance(begin, end);
 
  for (Index_type i = 0; i < distance; ++i) {
    loop_body(i + icount, begin[i]);
  }
}
template <typename LOOP_BODY>
inline __attribute__((always_inline)) void forall(simd_exec, const ListSegment &iseg, LOOP_BODY loop_body)
{
  const Index_type *__restrict__ idx = iseg.getIndex();
  Index_type len = iseg.getLength();

  ;

  for (Index_type k = 0; k < len; ++k) {
    loop_body(idx[k]);
  }

  ;
}
template <typename LOOP_BODY>
inline __attribute__((always_inline)) void forall_Icount(simd_exec,
                               const ListSegment &iseg,
                               Index_type icount,
                               LOOP_BODY loop_body)
{
  const Index_type *__restrict__ idx = iseg.getIndex();
  Index_type len = iseg.getLength();

  ;

  for (Index_type k = 0; k < len; ++k) {
    loop_body(k + icount, idx[k]);
  }

  ;
}
}







namespace RAJA
{
template <typename InnerPolicy>
struct omp_parallel_exec {
};
struct omp_for_exec {
};
struct omp_parallel_for_exec : public omp_parallel_exec<omp_for_exec> {
};
template <size_t ChunkSize>
struct omp_for_static {
};
template <size_t ChunkSize>
struct omp_parallel_for_static
    : public omp_parallel_exec<omp_for_static<ChunkSize>> {
};
struct omp_for_nowait_exec {
};




struct omp_parallel_for_segit : public omp_parallel_for_exec {
};
struct omp_parallel_segit : public omp_parallel_for_segit {
};
struct omp_taskgraph_segit {
};
struct omp_taskgraph_interval_segit {
};
struct omp_reduce {
};

struct omp_reduce_ordered {
};

}

namespace RAJA
{




// Liao debug here
template <typename Iterable, typename InnerPolicy, typename Func>
inline __attribute__((always_inline)) void forall(const omp_parallel_exec<InnerPolicy>&,
                        Iterable&& iter,
                        Func&& loop_body)
{
        
#pragma omp parallel

  {
    typename std::remove_reference<decltype(loop_body)>::type body = loop_body;
    forall<InnerPolicy>(std::forward<Iterable>(iter),
                        std::forward<Func>(body));
  }
}

template <typename Iterable, typename InnerPolicy, typename Func>
inline __attribute__((always_inline)) void forall_Icount(const omp_parallel_exec<InnerPolicy>&,
                               Iterable&& iter,
                               Index_type icount,
                               Func&& loop_body)
{
        
#pragma omp parallel

  {
    typename std::remove_reference<decltype(loop_body)>::type body = loop_body;
    forall_Icount<InnerPolicy>(std::forward<Iterable>(iter),
                               icount,
                               std::forward<Func>(body));
  }
}





template <typename Iterable, typename Func>
inline __attribute__((always_inline)) void forall(const omp_for_nowait_exec&,
                        Iterable&& iter,
                        Func&& loop_body)
{
  auto begin = std::begin(iter);
  auto end = std::end(iter);
  auto distance = std::distance(begin, end);
        
#pragma omp for nowait

  for (Index_type i = 0; i < distance; ++i) {
    loop_body(begin[i]);
  }
}

template <typename Iterable, typename Func>
inline __attribute__((always_inline)) void forall_Icount(const omp_for_nowait_exec&,
                               Iterable&& iter,
                               Index_type icount,
                               Func&& loop_body)
{
  auto begin = std::begin(iter);
  auto end = std::end(iter);
  auto distance = std::distance(begin, end);
        
#pragma omp for nowait

  for (Index_type i = 0; i < distance; ++i) {
    loop_body(i + icount, begin[i]);
  }
}





template <typename Iterable, typename Func>
inline __attribute__((always_inline)) void forall(const omp_for_exec&, Iterable&& iter, Func&& loop_body)
{
  auto begin = std::begin(iter);
  auto end = std::end(iter);
  auto distance = std::distance(begin, end);
        
#pragma omp for

  for (Index_type i = 0; i < distance; ++i) {
    loop_body(begin[i]);
  }
}

template <typename Iterable, typename Func>
inline __attribute__((always_inline)) void forall_Icount(const omp_for_exec&,
                               Iterable&& iter,
                               Index_type icount,
                               Func&& loop_body)
{
  auto begin = std::begin(iter);
  auto end = std::end(iter);
  auto distance = std::distance(begin, end);
        
#pragma omp for

  for (Index_type i = 0; i < distance; ++i) {
    loop_body(i + icount, begin[i]);
  }
}





template <typename Iterable, typename Func, size_t ChunkSize>
inline __attribute__((always_inline)) void forall(const omp_for_static<ChunkSize>&,
                        Iterable&& iter,
                        Func&& loop_body)
{
  auto begin = std::begin(iter);
  auto end = std::end(iter);
  auto distance = std::distance(begin, end);
        
#pragma omp for schedule(static, ChunkSize)

  for (Index_type i = 0; i < distance; ++i) {
    loop_body(begin[i]);
  }
}

template <typename Iterable, typename Func, size_t ChunkSize>
inline __attribute__((always_inline)) void forall_Icount(const omp_for_static<ChunkSize>&,
                               Iterable&& iter,
                               Index_type icount,
                               Func&& loop_body)
{
  auto begin = std::begin(iter);
  auto end = std::end(iter);
  auto distance = std::distance(begin, end);
        
#pragma omp for schedule(static, ChunkSize)

  for (Index_type i = 0; i < distance; ++i) {
    loop_body(i + icount, begin[i]);
  }
}
template <typename SEG_EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall(
    IndexSet::ExecPolicy<omp_taskgraph_segit, SEG_EXEC_POLICY_T>,
    const IndexSet& iset,
    LOOP_BODY loop_body)
{
  if (!iset.dependencyGraphSet()) {
    std::cerr << "\n RAJA IndexSet dependency graph not set , "
              << "FILE: " << "/home/liao6/workspace/raja/installraja2017-01-27/include/RAJA/internal/exec-openmp/forall_openmp.hxx" << " line: " << 240 << std::endl;
    RAJA_ABORT_OR_THROW("IndexSet dependency graph");
  }

  IndexSet& ncis = (*const_cast<IndexSet*>(&iset));

  int num_seg = ncis.getNumSegments();

        
#pragma omp parallel for schedule(static, 1)

  for (int isi = 0; isi < num_seg; ++isi) {
    IndexSetSegInfo* seg_info = ncis.getSegmentInfo(isi);
    DepGraphNode* task = seg_info->getDepGraphNode();

    task->wait();

    executeRangeList_forall<SEG_EXEC_POLICY_T>(seg_info, loop_body);

    task->reset();

    if (task->numDepTasks() != 0) {
      for (int ii = 0; ii < task->numDepTasks(); ++ii) {




        int seg = task->depTaskNum(ii);
        DepGraphNode* dep = ncis.getSegmentInfo(seg)->getDepGraphNode();
        dep->satisfyOne();
      }
    }

  }
}

}

#if 1
namespace RAJA
{
template <typename T>
class ReduceMin<omp_reduce, T>
{
  using my_type = ReduceMin<omp_reduce, T>;

public:



  explicit ReduceMin(T init_val):
    m_parent(__null), m_val(init_val)
  {
  }




  ReduceMin(const ReduceMin<omp_reduce, T>& other):
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_val)
  {
  }




  ~ReduceMin<omp_reduce, T>()
  {
    if (m_parent) {
        
#pragma omp critical

      {
        m_parent->m_val = (((m_val) < (m_parent->m_val)) ? (m_val) : (m_parent->m_val));
      }
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }





  const ReduceMin<omp_reduce, T>& min(T rhs) const
  {
    m_val = (((rhs) < (m_val)) ? (rhs) : (m_val));
    return *this;
  }

  ReduceMin<omp_reduce, T>& min(T rhs) {
    m_val = (((rhs) < (m_val)) ? (rhs) : (m_val));
    return *this;
  }

private:



  ReduceMin<omp_reduce, T>();

  const my_type * m_parent;
  mutable T m_val;
};
template <typename T>
class ReduceMinLoc<omp_reduce, T>
{
  using my_type = ReduceMinLoc<omp_reduce, T>;

public:



  explicit ReduceMinLoc(T init_val, Index_type init_loc):
    m_parent(__null), m_val(init_val), m_idx(init_loc)
  {
  }




  ReduceMinLoc(const ReduceMinLoc<omp_reduce, T>& other):
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_val),
    m_idx(other.m_idx)
  {
  }





  ~ReduceMinLoc<omp_reduce, T>()
  {
    if (m_parent) {
        
#pragma omp critical

      {
        m_parent->minloc(m_val, m_idx);
      }
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }




  Index_type getLoc()
  {
    return m_idx;
  }




  const ReduceMinLoc<omp_reduce, T>& minloc(T rhs, Index_type rhs_idx) const
  {
    if (rhs < m_val) {
      m_val = rhs;
      m_idx = rhs_idx;
    }
    return *this;
  }

  ReduceMinLoc<omp_reduce, T>& minloc(T rhs, Index_type rhs_idx)
  {
    if (rhs < m_val) {
      m_val = rhs;
      m_idx = rhs_idx;
    }
    return *this;
  }

private:



  ReduceMinLoc<omp_reduce, T>();

  const my_type * m_parent;

  mutable T m_val;
  mutable Index_type m_idx;
};
template <typename T>
class ReduceMax<omp_reduce, T>
{
  using my_type = ReduceMax<omp_reduce, T>;

public:



  explicit ReduceMax(T init_val):
    m_parent(__null), m_val(init_val)
  {
  }




  ReduceMax(const ReduceMax<omp_reduce, T>& other) :
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_val)
  {
  }





  ~ReduceMax<omp_reduce, T>()
  {
    if (m_parent) {
        
#pragma omp critical

      {
        m_parent->m_val = (((m_val) > (m_parent->m_val)) ? (m_val) : (m_parent->m_val));
      }
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }




  const ReduceMax<omp_reduce, T>& max(T rhs) const
  {
    m_val = (((rhs) > (m_val)) ? (rhs) : (m_val));
    return *this;
  }

  ReduceMax<omp_reduce, T>& max(T rhs)
  {
    m_val = (((rhs) > (m_val)) ? (rhs) : (m_val));
    return *this;
  }

private:



  ReduceMax<omp_reduce, T>();

  const my_type * m_parent;

  mutable T m_val;
};
template <typename T>
class ReduceMaxLoc<omp_reduce, T>
{
  using my_type = ReduceMaxLoc<omp_reduce, T>;

public:



  explicit ReduceMaxLoc(T init_val, Index_type init_loc):
    m_parent(__null), m_val(init_val), m_idx(init_loc)
  {
  }




  ReduceMaxLoc(const ReduceMaxLoc<omp_reduce, T>& other):
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_val),
    m_idx(other.m_idx)
  {
  }





  ~ReduceMaxLoc<omp_reduce, T>()
  {
    if (m_parent) {
        
#pragma omp critical

      {
        m_parent->maxloc(m_val, m_idx);
      }
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }




  Index_type getLoc()
  {
    return m_idx;
  }




  const ReduceMaxLoc<omp_reduce, T>& maxloc(T rhs, Index_type rhs_idx) const
  {
    if (rhs > m_val) {
      m_val = rhs;
      m_idx = rhs_idx;
    }
    return *this;
  }

  ReduceMaxLoc<omp_reduce, T>& maxloc(T rhs, Index_type rhs_idx)
  {
    if (rhs > m_val) {
      m_val = rhs;
      m_idx = rhs_idx;
    }
    return *this;
  }

private:



  ReduceMaxLoc<omp_reduce, T>();

  const my_type * m_parent;

  mutable T m_val;
  mutable Index_type m_idx;
};
template <typename T>
class ReduceSum<omp_reduce, T>
{
  using my_type = ReduceSum<omp_reduce, T>;

public:



  explicit ReduceSum(T init_val, T initializer = 0)
    : m_parent(__null), m_val(init_val), m_custom_init(initializer)
  {
  }




  ReduceSum(const ReduceSum<omp_reduce, T>& other) :
    m_parent(other.m_parent ? other.m_parent : &other),
    m_val(other.m_custom_init),
    m_custom_init(other.m_custom_init)
  {
  }





  ~ReduceSum<omp_reduce, T>()
  {
    if (m_parent) {
        
#pragma omp critical

      {
        *m_parent += m_val;
      }
    }
  }




  operator T()
  {
    return m_val;
  }




  T get() { return operator T(); }




  const ReduceSum<omp_reduce, T>& operator+=(T rhs) const
  {
    this->m_val += rhs;
    return *this;
  }

  ReduceSum<omp_reduce, T>& operator+=(T rhs)
  {
    this->m_val += rhs;
    return *this;
  }

private:



  ReduceSum<omp_reduce, T>();

  const my_type * m_parent;

  mutable T m_val;
  T m_custom_init;

};
template <typename T>
class ReduceMin<omp_reduce_ordered, T>
{
public:



  explicit ReduceMin(T init_val)
  {
    m_is_copy = false;

    m_reduced_val = init_val;

    m_myID = getCPUReductionId();

    m_blockdata = getCPUReductionMemBlock(m_myID);

    int nthreads = omp_get_max_threads();
        
#pragma omp parallel for schedule(static, 1)

    for (int i = 0; i < nthreads; ++i) {
      m_blockdata[i * s_block_offset] = init_val;
    }
  }




  ReduceMin(const ReduceMin<omp_reduce_ordered, T>& other)
  {
    *this = other;
    m_is_copy = true;
  }





  ~ReduceMin<omp_reduce_ordered, T>()
  {
    if (!m_is_copy) {
      releaseCPUReductionId(m_myID);
    }
  }




  operator T()
  {
    int nthreads = omp_get_max_threads();
    for (int i = 0; i < nthreads; ++i) {
      m_reduced_val = (((static_cast<T>(m_blockdata[i * s_block_offset])) < (m_reduced_val)) ? (static_cast<T>(m_blockdata[i * s_block_offset])) : (m_reduced_val))
                                                                               ;
    }

    return m_reduced_val;
  }




  T get() { return operator T(); }




  ReduceMin<omp_reduce_ordered, T> min(T val) const
  {
    int tid = omp_get_thread_num();
    int idx = tid * s_block_offset;
    m_blockdata[idx] = (((val) < (static_cast<T>(m_blockdata[idx]))) ? (val) : (static_cast<T>(m_blockdata[idx])));

    return *this;
  }

private:



  ReduceMin<omp_reduce_ordered, T>();

  static const int s_block_offset =
      COHERENCE_BLOCK_SIZE / sizeof(CPUReductionBlockDataType);

  bool m_is_copy;
  int m_myID;

  T m_reduced_val;

  CPUReductionBlockDataType* m_blockdata;
};
template <typename T>
class ReduceMinLoc<omp_reduce_ordered, T>
{
public:



  explicit ReduceMinLoc(T init_val, Index_type init_loc)
  {
    m_is_copy = false;

    m_reduced_val = init_val;
    m_reduced_idx = init_loc;

    m_myID = getCPUReductionId();

    m_blockdata = getCPUReductionMemBlock(m_myID);
    m_idxdata = getCPUReductionLocBlock(m_myID);

    int nthreads = omp_get_max_threads();
        
#pragma omp parallel for schedule(static, 1)

    for (int i = 0; i < nthreads; ++i) {
      m_blockdata[i * s_block_offset] = init_val;
      m_idxdata[i * s_idx_offset] = init_loc;
    }
  }




  ReduceMinLoc(const ReduceMinLoc<omp_reduce_ordered, T>& other)
  {
    *this = other;
    m_is_copy = true;
  }





  ~ReduceMinLoc<omp_reduce_ordered, T>()
  {
    if (!m_is_copy) {
      releaseCPUReductionId(m_myID);
    }
  }




  operator T()
  {
    int nthreads = omp_get_max_threads();
    for (int i = 0; i < nthreads; ++i) {
      if (static_cast<T>(m_blockdata[i * s_block_offset]) < m_reduced_val) {
        m_reduced_val = m_blockdata[i * s_block_offset];
        m_reduced_idx = m_idxdata[i * s_idx_offset];
      }
    }

    return m_reduced_val;
  }




  T get() { return operator T(); }




  Index_type getLoc()
  {
    int nthreads = omp_get_max_threads();
    for (int i = 0; i < nthreads; ++i) {
      if (static_cast<T>(m_blockdata[i * s_block_offset]) < m_reduced_val) {
        m_reduced_val = m_blockdata[i * s_block_offset];
        m_reduced_idx = m_idxdata[i * s_idx_offset];
      }
    }

    return m_reduced_idx;
  }




  ReduceMinLoc<omp_reduce_ordered, T> minloc(T val, Index_type idx) const
  {
    int tid = omp_get_thread_num();
    if (val < static_cast<T>(m_blockdata[tid * s_block_offset])) {
      m_blockdata[tid * s_block_offset] = val;
      m_idxdata[tid * s_idx_offset] = idx;
    }

    return *this;
  }

private:



  ReduceMinLoc<omp_reduce_ordered, T>();

  static const int s_block_offset =
      COHERENCE_BLOCK_SIZE / sizeof(CPUReductionBlockDataType);
  static const int s_idx_offset = COHERENCE_BLOCK_SIZE / sizeof(Index_type);

  bool m_is_copy;
  int m_myID;

  T m_reduced_val;
  Index_type m_reduced_idx;

  CPUReductionBlockDataType* m_blockdata;
  Index_type* m_idxdata;
};
template <typename T>
class ReduceMax<omp_reduce_ordered, T>
{
public:



  explicit ReduceMax(T init_val)
  {
    m_is_copy = false;

    m_reduced_val = init_val;

    m_myID = getCPUReductionId();

    m_blockdata = getCPUReductionMemBlock(m_myID);

    int nthreads = omp_get_max_threads();
        
#pragma omp parallel for schedule(static, 1)

    for (int i = 0; i < nthreads; ++i) {
      m_blockdata[i * s_block_offset] = init_val;
    }
  }




  ReduceMax(const ReduceMax<omp_reduce_ordered, T>& other)
  {
    *this = other;
    m_is_copy = true;
  }





  ~ReduceMax<omp_reduce_ordered, T>()
  {
    if (!m_is_copy) {
      releaseCPUReductionId(m_myID);
    }
  }




  operator T()
  {
    int nthreads = omp_get_max_threads();
    for (int i = 0; i < nthreads; ++i) {
      m_reduced_val = (((static_cast<T>(m_blockdata[i * s_block_offset])) > (m_reduced_val)) ? (static_cast<T>(m_blockdata[i * s_block_offset])) : (m_reduced_val))
                                                                               ;
    }

    return m_reduced_val;
  }




  T get() { return operator T(); }




  ReduceMax<omp_reduce_ordered, T> max(T val) const
  {
    int tid = omp_get_thread_num();
    int idx = tid * s_block_offset;
    m_blockdata[idx] = (((val) > (static_cast<T>(m_blockdata[idx]))) ? (val) : (static_cast<T>(m_blockdata[idx])));

    return *this;
  }

private:



  ReduceMax<omp_reduce_ordered, T>();

  static const int s_block_offset =
      COHERENCE_BLOCK_SIZE / sizeof(CPUReductionBlockDataType);

  bool m_is_copy;
  int m_myID;

  T m_reduced_val;

  CPUReductionBlockDataType* m_blockdata;
};
template <typename T>
class ReduceMaxLoc<omp_reduce_ordered, T>
{
public:



  explicit ReduceMaxLoc(T init_val, Index_type init_loc)
  {
    m_is_copy = false;

    m_reduced_val = init_val;
    m_reduced_idx = init_loc;

    m_myID = getCPUReductionId();

    m_blockdata = getCPUReductionMemBlock(m_myID);
    m_idxdata = getCPUReductionLocBlock(m_myID);

    int nthreads = omp_get_max_threads();
        
#pragma omp parallel for schedule(static, 1)

    for (int i = 0; i < nthreads; ++i) {
      m_blockdata[i * s_block_offset] = init_val;
      m_idxdata[i * s_idx_offset] = init_loc;
    }
  }




  ReduceMaxLoc(const ReduceMaxLoc<omp_reduce_ordered, T>& other)
  {
    *this = other;
    m_is_copy = true;
  }





  ~ReduceMaxLoc<omp_reduce_ordered, T>()
  {
    if (!m_is_copy) {
      releaseCPUReductionId(m_myID);
    }
  }




  operator T()
  {
    int nthreads = omp_get_max_threads();
    for (int i = 0; i < nthreads; ++i) {
      if (static_cast<T>(m_blockdata[i * s_block_offset]) > m_reduced_val) {
        m_reduced_val = m_blockdata[i * s_block_offset];
        m_reduced_idx = m_idxdata[i * s_idx_offset];
      }
    }

    return m_reduced_val;
  }




  T get() { return operator T(); }




  Index_type getLoc()
  {
    int nthreads = omp_get_max_threads();
    for (int i = 0; i < nthreads; ++i) {
      if (static_cast<T>(m_blockdata[i * s_block_offset]) > m_reduced_val) {
        m_reduced_val = m_blockdata[i * s_block_offset];
        m_reduced_idx = m_idxdata[i * s_idx_offset];
      }
    }

    return m_reduced_idx;
  }




  ReduceMaxLoc<omp_reduce_ordered, T> maxloc(T val, Index_type idx) const
  {
    int tid = omp_get_thread_num();
    if (val > static_cast<T>(m_blockdata[tid * s_block_offset])) {
      m_blockdata[tid * s_block_offset] = val;
      m_idxdata[tid * s_idx_offset] = idx;
    }

    return *this;
  }

private:



  ReduceMaxLoc<omp_reduce_ordered, T>();

  static const int s_block_offset =
      COHERENCE_BLOCK_SIZE / sizeof(CPUReductionBlockDataType);
  static const int s_idx_offset = COHERENCE_BLOCK_SIZE / sizeof(Index_type);

  bool m_is_copy;
  int m_myID;

  T m_reduced_val;
  Index_type m_reduced_idx;

  CPUReductionBlockDataType* m_blockdata;
  Index_type* m_idxdata;
};
template <typename T>
class ReduceSum<omp_reduce_ordered, T>
{
public:



  explicit ReduceSum(T init_val)
  {
    m_is_copy = false;

    m_init_val = init_val;
    m_reduced_val = static_cast<T>(0);

    m_myID = getCPUReductionId();

    m_blockdata = getCPUReductionMemBlock(m_myID);

    int nthreads = omp_get_max_threads();
        
#pragma omp parallel for schedule(static, 1)

    for (int i = 0; i < nthreads; ++i) {
      m_blockdata[i * s_block_offset] = 0;
    }
  }




  ReduceSum(const ReduceSum<omp_reduce_ordered, T>& other)
  {
    *this = other;
    m_is_copy = true;
  }





  ~ReduceSum<omp_reduce_ordered, T>()
  {
    if (!m_is_copy) {
      releaseCPUReductionId(m_myID);
    }
  }




  operator T()
  {
    T tmp_reduced_val = static_cast<T>(0);
    int nthreads = omp_get_max_threads();
    for (int i = 0; i < nthreads; ++i) {
      tmp_reduced_val += static_cast<T>(m_blockdata[i * s_block_offset]);
    }
    m_reduced_val = m_init_val + tmp_reduced_val;

    return m_reduced_val;
  }




  T get() { return operator T(); }




  ReduceSum<omp_reduce_ordered, T> operator+=(T val) const
  {
    int tid = omp_get_thread_num();
    m_blockdata[tid * s_block_offset] += val;
    return *this;
  }

private:



  ReduceSum<omp_reduce_ordered, T>();

  static const int s_block_offset =
      COHERENCE_BLOCK_SIZE / sizeof(CPUReductionBlockDataType);

  bool m_is_copy;
  int m_myID;

  T m_init_val;
  T m_reduced_val;

  CPUReductionBlockDataType* m_blockdata;
};
}
namespace RAJA
{
namespace detail
{
namespace scan
{

inline __attribute__((always_inline))
int firstIndex(int n, int p, int pid)
{
  return static_cast<size_t>(n * pid) / p;
}




template <typename Iter, typename BinFn>
void inclusive_inplace(const ::RAJA::omp_parallel_for_exec&,
                       Iter begin,
                       Iter end,
                       BinFn f)
{
  using Value = typename ::std::iterator_traits<Iter>::value_type;
  const int n = end - begin;
  const int p = omp_get_max_threads();

#if 0
  ::std::vector<Value> sums(p, Value());
        
#pragma omp parallel

  {
    const int pid = omp_get_thread_num();
    const int i0 = firstIndex(n, p, pid);
    const int i1 = firstIndex(n, p, pid + 1);
    inclusive_inplace(::RAJA::seq_exec{}, begin + i0, begin + i1, f);
    sums[pid] = *(begin + i1 - 1);
        
#pragma omp barrier

        
#pragma omp single

    exclusive_inplace(
        ::RAJA::seq_exec{}, sums.data(), sums.data() + p, f, BinFn::identity);
    for (int i = i0; i < i1; ++i) {
      *(begin + i) = f(*(begin + i), sums[pid]);
    }
  }
#endif

}





template <typename Iter, typename BinFn, typename ValueT>
void exclusive_inplace(const ::RAJA::omp_parallel_for_exec&,
                       Iter begin,
                       Iter end,
                       BinFn f,
                       ValueT v)
{
  using Value = typename ::std::iterator_traits<Iter>::value_type;
  const int n = end - begin;
  const int p = omp_get_max_threads();

#if 0
  ::std::vector<Value> sums(p, v);
        
#pragma omp parallel

  {
    const int pid = omp_get_thread_num();
    const int i0 = firstIndex(n, p, pid);
    const int i1 = firstIndex(n, p, pid + 1);
    const Value init = ((pid == 0) ? v : *(begin + i0 - 1));
        
#pragma omp barrier

    exclusive_inplace(seq_exec{}, begin + i0, begin + i1, f, init);
    sums[pid] = *(begin + i1 - 1);
        
#pragma omp barrier

        
#pragma omp single

    exclusive_inplace(
        seq_exec{}, sums.data(), sums.data() + p, f, BinFn::identity);
    for (int i = i0; i < i1; ++i) {
      *(begin + i) = f(*(begin + i), sums[pid]);
    }
  }
#endif

}





template <typename Iter, typename OutIter, typename BinFn>
void inclusive(const ::RAJA::omp_parallel_for_exec& exec,
               Iter begin,
               Iter end,
               OutIter out,
               BinFn f)
{
  ::std::copy(begin, end, out);
  inclusive_inplace(exec, out, out + (end - begin), f);
}





template <typename Iter, typename OutIter, typename BinFn, typename ValueT>
void exclusive(const ::RAJA::omp_parallel_for_exec& exec,
               Iter begin,
               Iter end,
               OutIter out,
               BinFn f,
               ValueT v)
{
  ::std::copy(begin, end, out);
  exclusive_inplace(exec, out, out + (end - begin), f, v);
}

}

}

}






namespace RAJA
{
template <typename CONTAINER_T>
inline __attribute__((always_inline)) void getIndices(CONTAINER_T& con, const IndexSet& iset)
{
  CONTAINER_T tcon;
  forall<IndexSet::ExecPolicy<seq_segit, seq_exec> >(iset, [&](Index_type idx) {
    tcon.push_back(idx);
  });
  con = tcon;
}
template <typename CONTAINER_T, typename SEGMENT_T>
inline __attribute__((always_inline)) void getIndices(CONTAINER_T& con, const SEGMENT_T& iset)
{
  CONTAINER_T tcon;
  forall<seq_exec>(iset, [&](Index_type idx) { tcon.push_back(idx); });
  con = tcon;
}
template <typename CONTAINER_T, typename CONDITIONAL>
inline __attribute__((always_inline)) void getIndicesConditional(CONTAINER_T& con,
                                       const IndexSet& iset,
                                       CONDITIONAL conditional)
{
  CONTAINER_T tcon;
  forall<IndexSet::ExecPolicy<seq_segit, seq_exec> >(iset, [&](Index_type idx) {
    if (conditional(idx)) tcon.push_back(idx);
  });
  con = tcon;
}
template <typename CONTAINER_T, typename SEGMENT_T, typename CONDITIONAL>
inline __attribute__((always_inline)) void getIndicesConditional(CONTAINER_T& con,
                                       const SEGMENT_T& iset,
                                       CONDITIONAL conditional)
{
  CONTAINER_T tcon;
  forall<seq_exec>(iset, [&](Index_type idx) {
    if (conditional(idx)) tcon.push_back(idx);
  });
  con = tcon;
}

}
namespace RAJA
{
template <typename ExecPolicy,
          typename Iter,
          typename T = typename std::iterator_traits<Iter>::value_type,
          typename BinaryFunction = ::RAJA::operators::plus<T>>
void inclusive_scan_inplace(Iter begin,
                            Iter end,
                            BinaryFunction binop = BinaryFunction{})
{
  detail::scan::inclusive_inplace(ExecPolicy{}, begin, end, binop);
}
template <typename ExecPolicy,
          typename Iter,
          typename T = typename std::iterator_traits<Iter>::value_type,
          typename BinaryFunction = ::RAJA::operators::plus<T>>
void exclusive_scan_inplace(Iter begin,
                            Iter end,
                            BinaryFunction binop = BinaryFunction{},
                            T value = BinaryFunction::identity)
{
  detail::scan::exclusive_inplace(ExecPolicy{}, begin, end, binop, value);
}
template <typename ExecPolicy,
          typename Iter,
          typename IterOut,
          typename T = typename std::iterator_traits<Iter>::value_type,
          typename BinaryFunction = ::RAJA::operators::plus<T>>
void inclusive_scan(Iter begin,
                    Iter end,
                    IterOut out,
                    BinaryFunction binop = BinaryFunction{})
{
  detail::scan::inclusive(ExecPolicy{}, begin, end, out, binop);
}
template <typename ExecPolicy,
          typename Iter,
          typename IterOut,
          typename T = typename std::iterator_traits<Iter>::value_type,
          typename BinaryFunction = ::RAJA::operators::plus<T>>
void exclusive_scan(Iter begin,
                    Iter end,
                    IterOut out,
                    BinaryFunction binop = BinaryFunction{},
                    T value = BinaryFunction::identity)
{
  detail::scan::exclusive(ExecPolicy{}, begin, end, out, binop, value);
}

}

namespace loopsuite {
namespace execution_policy {
struct serial {};
struct parallel {};

}
template < typename policy >
struct policy_traits {
    typedef RAJA::seq_exec raja_policy;

    typedef RAJA::IndexSet::ExecPolicy<RAJA::seq_segit,RAJA::seq_exec> raja_indexset_policy;
    static const bool valid = false;
};

template < >
struct policy_traits< execution_policy::serial > {
    typedef RAJA::seq_exec raja_policy;

  typedef RAJA::IndexSet::ExecPolicy<RAJA::seq_segit,RAJA::seq_exec> raja_indexset_policy;
    static const bool valid = true;
};

template < >
struct policy_traits< execution_policy::parallel > {

    typedef RAJA::omp_parallel_for_exec raja_policy;
    typedef RAJA::IndexSet::ExecPolicy<RAJA::seq_segit,RAJA::seq_exec> raja_indexset_policy;

    static const bool valid = true;
};
namespace loop_over {

struct node_index_only {};

struct index_only {};

struct zone_index_only {};
struct slot_index_only {};
struct zone_and_slot_index {};
struct index {};
struct ij {};
struct ijk {};

}

}

namespace loopsuite {

namespace core {
template < typename policy,
           typename loop_type,
           typename index_type,
           typename kernel_type >
inline void for_all( loop_type,
                     const index_type*,
                     const index_type&,
                     const index_type&,
                     kernel_type )
{
  std::cerr << ( "for_all", "ambiguous call!" );
}
template < typename policy,
           typename index_type,
           typename kernel_type >
inline void for_all( loop_over::index_only,
                     const index_type* ndx_array,
                     const index_type& begin,
                     const index_type& end,
                     kernel_type kernel )
{
  ((begin <= end) ? static_cast<void> (0) : __assert_fail ("begin <= end", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all.hxx", 94, __PRETTY_FUNCTION__));
  ((ndx_array != __null) ? static_cast<void> (0) : __assert_fail ("ndx_array != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all.hxx", 95, __PRETTY_FUNCTION__));

  RAJA::forall< policy_traits< policy >::raja_policy >( begin, end, [=](int i) {
      kernel( ndx_array[ i ] );
  });

}
template < typename policy,
           typename index_type,
           typename kernel_type >
inline void for_all( loop_over::index,
                     const index_type* ndx_array,
                     const index_type& begin,
                     const index_type& end,
                     kernel_type kernel )
{
  ((begin <= end) ? static_cast<void> (0) : __assert_fail ("begin <= end", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all.hxx", 145, __PRETTY_FUNCTION__));
  ((ndx_array != __null) ? static_cast<void> (0) : __assert_fail ("ndx_array != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all.hxx", 146, __PRETTY_FUNCTION__));

  RAJA::forall< policy_traits< policy >::raja_policy >( begin, end, [=](int i) {
        kernel( ndx_array[ i ], i );
  });

}

}

}
typedef loopsuite::StructuredDomain Domain_t;



namespace loopsuite {

namespace RajaIndexSetBuilder {
void buildRealNodesIndexSet( Domain_t* domain, RAJA::IndexSet& index_set );
void buildRealZonesIndexSet( Domain_t* domain, RAJA::IndexSet& index_set );

}

}


namespace loopsuite {

namespace core {
template < typename policy,
           typename loop_type,
           typename kernel_type >
inline void for_all_nodes( Domain_t*, loop_type, kernel_type )
{
  std::cerr << "for_all_nodes", "ambiguous call!\n";
}
template < typename policy, typename kernel_type >
inline void for_all_nodes( Domain_t* domain,
                           loop_over::node_index_only,
                           kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_nodes.hxx", 79, __PRETTY_FUNCTION__));


  RAJA::IndexSet real_nodes;
  RajaIndexSetBuilder::buildRealNodesIndexSet(domain,real_nodes);

  RAJA::forall< policy_traits< policy >::raja_indexset_policy >(
          real_nodes, kernel );
}
template < typename policy, typename kernel_type >
inline void for_all_nodes( Domain_t* domain,
                           loop_over::ij,
                           kernel_type kernel)
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_nodes.hxx", 124, __PRETTY_FUNCTION__));


  RAJA::IndexSet real_nodes;
  RajaIndexSetBuilder::buildRealNodesIndexSet(domain,real_nodes);

  const int jp = domain->m_jp;
  RAJA::forall< policy_traits< policy >::raja_indexset_policy >(
          real_nodes, [=](int nodeIdx){

      int i=0;
      int j=0;
      int k=0;
      domain->getIJKCoordinates(nodeIdx, i, j, k );

      kernel( nodeIdx, i, j );
  });
}
template < typename policy, typename kernel_type >
inline void for_all_nodes( Domain_t* domain,
                           loop_over::ijk,
                           kernel_type kernel )
{
  ((domain->m_dimension==3) ? static_cast<void> (0) : __assert_fail ("domain->m_dimension==3", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_nodes.hxx", 178, __PRETTY_FUNCTION__));
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_nodes.hxx", 179, __PRETTY_FUNCTION__));


  RAJA::IndexSet real_nodes;
  RajaIndexSetBuilder::buildRealNodesIndexSet(domain,real_nodes);

  const int jp = domain->m_jp;
  const int kp = domain->m_kp;
  RAJA::forall< policy_traits< policy >::raja_indexset_policy >(
          real_nodes,[=](int nodeIdx) {

      int i=0;
      int j=0;
      int k=0;
      domain->getIJKCoordinates(nodeIdx, i, j , k );

      kernel( nodeIdx, i, j, k );
  });

}
template < typename policy, typename kernel_type >
inline void for_all_nodes( Domain_t* domain,
                           loop_over::index,
                           kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_nodes.hxx", 235, __PRETTY_FUNCTION__));


  RAJA::IndexSet real_nodes;
  RajaIndexSetBuilder::buildRealNodesIndexSet(domain,real_nodes);

  RAJA::forall_Icount< policy_traits< policy >::raja_indexset_policy >(
          real_nodes, [=](int icount, int nodeIdx) {
          kernel( nodeIdx, icount );
  });

}

}

}


namespace loopsuite {

namespace core {
template < typename policy,
           typename loop_type,
           typename kernel_type >
inline void for_all_zones( Domain_t*,
                           loop_type,
                           kernel_type )
{
  std::cerr << "for_all_zones", "ambiguous call!";
}
template < typename policy, typename kernel_type >
inline void for_all_zones( Domain_t* domain,
                           loop_over::zone_index_only,
                           kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_zones.hxx", 83, __PRETTY_FUNCTION__));


  RAJA::IndexSet real_zones;
  RajaIndexSetBuilder::buildRealZonesIndexSet(domain, real_zones);

  RAJA::forall<typename policy_traits< policy >::raja_indexset_policy >(
          real_zones, kernel );
}
template < typename policy, typename kernel_type >
inline void for_all_zones( Domain_t* domain,
                           loop_over::ij,
                           kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_zones.hxx", 129, __PRETTY_FUNCTION__));


  RAJA::IndexSet real_zones;
  RajaIndexSetBuilder::buildRealZonesIndexSet(domain, real_zones);

  const int jp = domain->m_jp;
  RAJA::forall< policy_traits< policy >::raja_indexset_policy >(
          real_zones,[=]( int zoneIdx ) {

       int i = 0;
       int j = 0;
       int k = 0;
       domain->getIJKCoordinates(zoneIdx, i, j, k );

       kernel( zoneIdx, i, j );
  });
}
template < typename policy, typename kernel_type >
inline void for_all_zones( Domain_t* domain,
                           loop_over::ijk,
                           kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_zones.hxx", 184, __PRETTY_FUNCTION__));
  ((domain->m_dimension == 3) ? static_cast<void> (0) : __assert_fail ("domain->m_dimension == 3", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_zones.hxx", 185, __PRETTY_FUNCTION__));


  RAJA::IndexSet real_zones;
  RajaIndexSetBuilder::buildRealZonesIndexSet(domain, real_zones);

  const int jp = domain->m_jp;
  const int kp = domain->m_kp;
  RAJA::forall< policy_traits< policy >::raja_indexset_policy >(
          real_zones,[=]( int zoneIdx ) {

       int i = 0;
       int j = 0;
       int k = 0;
       domain->getIJKCoordinates( zoneIdx, i, j, k );

       kernel( zoneIdx, i, j, k );
  });

}
template < typename policy, typename kernel_type >
inline void for_all_zones( Domain_t* domain,
                           loop_over::index,
                           kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_zones.hxx", 241, __PRETTY_FUNCTION__));


  RAJA::IndexSet real_zones;
  RajaIndexSetBuilder::buildRealZonesIndexSet(domain, real_zones);

  RAJA::forall_Icount< policy_traits< policy >::raja_indexset_policy >(
          real_zones, [=](int icount, int zoneIdx) {
          kernel( zoneIdx, icount );
  });

}

}

}

namespace loopsuite {

namespace core {
template < typename policy,
           typename loop_type,
           typename kernel_type >
inline void for_all_mixed_slots( loop_type,
                                 Domain_t*,
                                 int,
                                 kernel_type )
{
  std::cerr << "for_all_mixed_slots", "ambiguous call!";
}
template < typename policy, typename kernel_type >
inline void for_all_mixed_slots( loop_over::zone_and_slot_index,
                                 Domain_t* domain,
                                 int ir,
                                 kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_mixed_slots.hxx", 87, __PRETTY_FUNCTION__));
  ((ir >=1 && ir <= domain->m_nreg) ? static_cast<void> (0) : __assert_fail ("ir >=1 && ir <= domain->m_nreg", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_mixed_slots.hxx", 88, __PRETTY_FUNCTION__));

  const int* rgrdmix = domain->m_rgrdmix[ ir ];
  const int* rndxmix = domain->m_rndxmix[ ir ];
  const int rlenmix = domain->m_rlenmix[ ir ];
  RAJA::forall<typename policy_traits< policy >::raja_policy >(0,rlenmix,[=](int i){
     kernel( rgrdmix[ i ], rndxmix[ i ] );
  });

}
template < typename policy, typename kernel_type >
inline void for_all_mixed_slots( loop_over::slot_index_only,
                                 Domain_t* domain,
                                 int ir,
                                 kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_mixed_slots.hxx", 137, __PRETTY_FUNCTION__));
  ((ir >=1 && ir < domain->m_nreg) ? static_cast<void> (0) : __assert_fail ("ir >=1 && ir < domain->m_nreg", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_mixed_slots.hxx", 138, __PRETTY_FUNCTION__));

  const int* rndxmix = domain->m_rndxmix[ ir ];
  const int rlenmix = domain->m_rlenmix[ ir ];
  RAJA::forall< policy_traits< policy >::raja_policy >(0, rlenmix,[=](int i) {
    kernel( rndxmix[ i ] );
  });

}
template < typename policy, typename kernel_type >
inline void for_all_mixed_slots( loop_over::index,
                                 Domain_t* domain,
                                 int ir,
                                 kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_mixed_slots.hxx", 186, __PRETTY_FUNCTION__));
  ((ir >=1 && ir < domain->m_nreg) ? static_cast<void> (0) : __assert_fail ("ir >=1 && ir < domain->m_nreg", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/core/for_all_mixed_slots.hxx", 187, __PRETTY_FUNCTION__));

  const int* rndxmix = domain->m_rndxmix[ ir ];
  const int rlenmix = domain->m_rlenmix[ ir ];
  RAJA::forall< policy_traits< policy >::raja_policy >(0, rlenmix,[=](int i) {
    kernel( rndxmix[ i ], i );
  });

}

}

}


namespace loopsuite {
template < typename policy,
           typename index_type,
           typename kernel_type >
inline void for_all( const index_type& begin,
                     const index_type& end,
                     kernel_type kernel )
{
  ((begin <= end) ? static_cast<void> (0) : __assert_fail ("begin <= end", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 68, __PRETTY_FUNCTION__));
  ((policy_traits< policy >::valid) ? static_cast<void> (0) : __assert_fail ("policy_traits< policy >::valid", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 69, __PRETTY_FUNCTION__));

  RAJA::forall< policy_traits< policy >::raja_policy >( begin, end, kernel );
}
template < typename policy,
           typename loop_type=loop_over::index_only,
           typename index_type,
           typename kernel_type >
inline void for_all( const index_type* ndx_array,
                     const index_type& begin,
                     const index_type& end,
                     kernel_type kernel )
{
  ((begin <= end) ? static_cast<void> (0) : __assert_fail ("begin <= end", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 126, __PRETTY_FUNCTION__));
  ((ndx_array != __null) ? static_cast<void> (0) : __assert_fail ("ndx_array != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 127, __PRETTY_FUNCTION__));

  core::for_all< policy, index_type, kernel_type >(
          loop_type(), ndx_array, begin, end, kernel );
}
template < typename policy,
           typename loop_type=loop_over::zone_index_only,
           typename kernel_type >
inline void for_all_zones( Domain_t* domain, kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 195, __PRETTY_FUNCTION__));
  core::for_all_zones< policy, kernel_type >( domain,
                                              loop_type(),
                                              kernel
                                              );
}
template < typename policy, typename kernel_type >
inline void for_all_mixed_zones( Domain_t* domain, kernel_type kernel )
{
}
template < typename policy,
           typename loop_type=loop_over::node_index_only,
           typename kernel_type >
inline void for_all_nodes( Domain_t* domain, kernel_type kernel )
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 304, __PRETTY_FUNCTION__));
  core::for_all_nodes< policy, kernel_type >( domain, loop_type(), kernel );

}
template < typename policy, typename kernel_type >
inline void for_all_region_zones( Domain_t* domain,
                                 int ir,
                                 kernel_type kernel )
{
}
template < typename policy,
           typename loop_type=loop_over::slot_index_only,
           typename kernel_type >
inline void for_all_mixed_slots( Domain_t* domain, int ir, kernel_type kernel)
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 408, __PRETTY_FUNCTION__));
  ((ir >= 1 && ir <= domain->m_nreg) ? static_cast<void> (0) : __assert_fail ("ir >= 1 && ir <= domain->m_nreg", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 409, __PRETTY_FUNCTION__));

  core::for_all_mixed_slots< policy, kernel_type >(
          loop_type(),
          domain,
          ir,
          kernel );
}
template < typename policy, typename kernel_type >
inline void for_all_clean_zones( Domain_t* domain, int ir, kernel_type kernel)
{
  ((domain != __null) ? static_cast<void> (0) : __assert_fail ("domain != __null", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 451, __PRETTY_FUNCTION__));
  ((ir >= 1 && ir <= domain->m_nreg) ? static_cast<void> (0) : __assert_fail ("ir >= 1 && ir <= domain->m_nreg", "/home/liao6/workspace/raja/example/loopsuite.git/raja_api/RajaAPI.h", 452, __PRETTY_FUNCTION__));







}



}

namespace loopsuite {

PdVWorkRajaAPI::PdVWorkRajaAPI( const StructuredDomain* domain,
                                const unsigned numberOfIterations ):
   PdVWorkBaseLoopAlgorithm( PDV_WORK_RAJAAPI, domain, numberOfIterations )
{ }


PdVWorkRajaAPI::~PdVWorkRajaAPI()
{
  this->tearDown();
}


void PdVWorkRajaAPI::loopExecute()
{

  const int nreg = m_Domain->m_nreg;
  const double* p = m_Domain->m_p;

  typedef execution_policy::parallel exec_policy;
  typedef loop_over::zone_and_slot_index loop_type;

  StructuredDomain* domain = const_cast< StructuredDomain* >( m_Domain );
  for ( int ir=1; ir <= nreg; ++ir ) {

     for_all_mixed_slots< exec_policy, loop_type >(
            domain, ir, [=](int zoneIdx, int slotIdx) {

         const double dvol = ( (m_nvol[slotIdx] * m_nvol[zoneIdx] ) -
                               (m_ovol[slotIdx] * m_ovol[zoneIdx] ) );

         m_pdv_sum[ zoneIdx ] += p[ slotIdx ] * dvol;
     });

  }

}

}
#endif
