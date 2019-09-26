#include <string>

enum LoopAlgorithmId {
  PDV_WORK_RAJAAPI,
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

#if 1
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
#endif

// Liao cassert begin



namespace loopsuite {

class StructuredDomain
{
public:
  StructuredDomain();

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

  StructuredDomain(const StructuredDomain& );
  StructuredDomain& operator=(const StructuredDomain& );
};

}
typedef loopsuite::StructuredDomain Domain_t;

#if 1
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

}
#endif


// Liao cfloat start



#if 1
namespace RAJA {
    namespace Iterators {

#if 1
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
#endif

#if 1
template<typename Type = Index_type,
         typename DifferenceType = Index_type,
         typename PointerType = Type *>
class numeric_iterator : public base_iterator< Type, DifferenceType>
{
public:
    using difference_type = typename std::iterator<std::random_access_iterator_tag, Type>::difference_type;
    using base = base_iterator<Type, DifferenceType>;

    constexpr numeric_iterator() : base(0) {}
    constexpr numeric_iterator(const Type& rhs) : base(rhs) {}
    constexpr numeric_iterator(const numeric_iterator& rhs) : base(rhs.val) {}

#if 1
    inline difference_type operator+(const numeric_iterator& rhs) const {return static_cast<difference_type>(base::val)+static_cast<difference_type>(rhs.val);}
    inline difference_type operator-(const numeric_iterator& rhs) const {return static_cast<difference_type>(base::val)-static_cast<difference_type>(rhs.val);}
    inline numeric_iterator operator+(const difference_type& rhs) const {return numeric_iterator(base::val+rhs);}
    inline numeric_iterator operator-(const difference_type& rhs) const {return numeric_iterator(base::val-rhs);}
#endif
    constexpr Type operator[](difference_type rhs) const {return base::val + rhs;}
};
#endif


}
}
#endif


namespace RAJA {

struct PolicyBase { };

}



#if 1
namespace RAJA
{
class BaseSegment
{
public:


  explicit BaseSegment(SegmentType type) 
  // : m_type(type), m_private(0) 
     { ; }

};

}
#endif




#if 1
namespace RAJA
{
  class RangeSegment : public BaseSegment
{
public:

#if 1
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
    const RangeSegment* o_ptr = NULL; // dynamic_cast<const RangeSegment*>(&other);
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

  //  void print(std::ostream& os) const;

  using iterator = Iterators::numeric_iterator<Index_type>;

  iterator end() const { return iterator(m_end); }

  iterator begin() const { return iterator(m_begin); }

  Index_type size() const { return m_end - m_begin; }

private:
  Index_type m_begin;
  Index_type m_end;
#endif

};

}
#endif




#if 1
namespace RAJA
{
#if 1
template <typename EXEC_POLICY_T, typename Container, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall(Container&& c, LOOP_BODY loop_body)
{
  forall(EXEC_POLICY_T(), std::forward<Container>(c), loop_body);
}
#endif

#if 1
template <typename EXEC_POLICY_T, typename LOOP_BODY>
inline __attribute__((always_inline)) void forall(Index_type begin, Index_type end, LOOP_BODY loop_body)
{
  forall<EXEC_POLICY_T>(RangeSegment(begin, end), loop_body);
}
#endif

}
#endif








#if 1
namespace RAJA
{
struct seq_exec : public PolicyBase {
};

}
#endif

#if 1
namespace RAJA
{

template <typename InnerPolicy>
struct omp_parallel_exec {
};
struct omp_for_exec {
};

#if 1
struct omp_parallel_for_exec : public omp_parallel_exec<omp_for_exec> {
};
#endif

}
#endif

#if 1
namespace RAJA
{
#if 1
// Liao debug here
template <typename Iterable, typename InnerPolicy, typename Func>
inline __attribute__((always_inline)) void forall(const omp_parallel_exec<InnerPolicy>&,
                        Iterable&& iter,
                        Func&& loop_body)
{
        
#pragma omp parallel

  {
#if 1
    typename std::remove_reference<decltype(loop_body)>::type body = loop_body;
    forall<InnerPolicy>(std::forward<Iterable>(iter),
                        std::forward<Func>(body));
#endif
  }
}
#endif

#if 1
template <typename Iterable, typename Func>
inline __attribute__((always_inline)) void forall(const omp_for_exec&, Iterable&& iter, Func&& loop_body)
{
  auto begin = std::begin(iter);
  auto end = std::end(iter);
  auto distance = std::distance(begin, end);

  loop_body(begin[42]);
}
#endif

}
#endif

#if 1
namespace loopsuite {
namespace execution_policy {

#if 1
// struct serial {};
struct parallel {};
#endif
}

#if 1
template < typename policy >
struct policy_traits {
    typedef RAJA::seq_exec raja_policy;

  // typedef RAJA::IndexSet::ExecPolicy<RAJA::seq_segit,RAJA::seq_exec> raja_indexset_policy;
  // static const bool valid = false;
};
#endif

#if 1
template < >
struct policy_traits< execution_policy::parallel > {

    typedef RAJA::omp_parallel_for_exec raja_policy;
 // typedef RAJA::IndexSet::ExecPolicy<RAJA::seq_segit,RAJA::seq_exec> raja_indexset_policy;

 // static const bool valid = true;
};
#endif

#if 1
namespace loop_over {

// struct node_index_only {};
// struct index_only {};
// struct zone_index_only {};

struct slot_index_only {};
struct zone_and_slot_index {};

// struct index {};
// struct ij {};
// struct ijk {};

}
#endif

}
#endif

#if 1
namespace loopsuite {

namespace core {

#if 1
template < typename policy, typename kernel_type >
inline void for_all_mixed_slots( loop_over::zone_and_slot_index,
                                 Domain_t* domain,
                                 int ir,
                                 kernel_type kernel )
{
// BUG: Missing #if 0 CPP declaration in generated code.
#if 1
  const int* rgrdmix = domain->m_rgrdmix[ ir ];
  const int* rndxmix = domain->m_rndxmix[ ir ];
  const int rlenmix = domain->m_rlenmix[ ir ];
  RAJA::forall<typename policy_traits< policy >::raja_policy >(0,rlenmix,[=](int i){
   // kernel( rgrdmix[ i ], rndxmix[ i ] );
      43;
  });
#endif
}
#endif

}

}
#endif

#if 1
namespace loopsuite {

#if 1
template < typename policy,
           typename loop_type=loop_over::slot_index_only,
           typename kernel_type >
inline void for_all_mixed_slots( Domain_t* domain, int ir, kernel_type kernel)
{
#if 1
  core::for_all_mixed_slots< policy, kernel_type >(
          loop_type(),
          domain,
          ir,
          kernel );
#endif
}
#endif

}
#endif

#if 1
namespace loopsuite {

PdVWorkRajaAPI::PdVWorkRajaAPI( const StructuredDomain* domain,
                                const unsigned numberOfIterations ):
   PdVWorkBaseLoopAlgorithm( PDV_WORK_RAJAAPI, domain, numberOfIterations )
{ }


PdVWorkRajaAPI::~PdVWorkRajaAPI()
{
  this->tearDown();
}

#if 1
void PdVWorkRajaAPI::loopExecute()
{

  const int nreg = m_Domain->m_nreg;
  const double* p = m_Domain->m_p;

  typedef execution_policy::parallel exec_policy;
  typedef loop_over::zone_and_slot_index loop_type;

  StructuredDomain* domain = const_cast< StructuredDomain* >( m_Domain );
  for ( int ir=1; ir <= nreg; ++ir ) {
#if 1
     for_all_mixed_slots< exec_policy, loop_type >(
          domain, ir, [=](int zoneIdx, int slotIdx) { 42; }
     );
#endif
  }

}
#endif

}
#endif
