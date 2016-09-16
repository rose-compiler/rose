// g++ -c -std=c++11 matmulcpp11-gcc-E.cxx
#include <string>
#include <tuple>
#include <iostream>
#include <math.h>
// This mimic the output code 
#include <omp.h>

namespace RAJA {
  const int RAJA_VERSION_MAJOR = 1;
  const int RAJA_VERSION_MINOR = 0;
  const int RAJA_VERSION_PATCHLEVEL = 0;
  const int RANGE_ALIGN = 4;
  const int RANGE_MIN_LENGTH = 32;
  const int DATA_ALIGN = 64;
  const int COHERENCE_BLOCK_SIZE = 64;
}

namespace RAJA {

  enum SegmentType { _RangeSeg_,
    _RangeStrideSeg_,
    _ListSeg_,
    _UnknownSeg_
  };

  enum IndexOwnership {
    Unowned,
    Owned
  };

  typedef int Index_type;

  const int UndefinedValue = -9999999;

}

namespace RAJA {
  typedef double Real_type;
  typedef Real_type* __restrict__ Real_ptr;
  typedef const Real_type* __restrict__ const_Real_ptr;

  typedef Real_type* __restrict__ UnalignedReal_ptr;
  typedef const Real_type* __restrict__ const_UnalignedReal_ptr;
}

namespace RAJA {
  template<typename REDUCE_POLICY_T,
    typename T>
      class ReduceMin;
  template<typename REDUCE_POLICY_T,
    typename T>
      class ReduceMinLoc;
  template<typename REDUCE_POLICY_T,
    typename T>
      class ReduceMax;
  template<typename REDUCE_POLICY_T,
    typename T>
      class ReduceMaxLoc;
  template<typename REDUCE_POLICY_T,
    typename T>
      class ReduceSum;

}

namespace RAJA {
  class BaseSegment
  {
    public:

      explicit BaseSegment(SegmentType type)
        : m_type(type), m_private(0) { ; }
      virtual ~BaseSegment() { ; }

      SegmentType getType() const { return m_type; }

      void* getPrivate() const { return m_private ; }
      void setPrivate(void *ptr) { m_private = ptr ; }
      virtual Index_type getLength() const = 0;

      virtual IndexOwnership getIndexOwnership() const = 0;

      virtual bool operator ==(const BaseSegment& other) const = 0;

      virtual bool operator !=(const BaseSegment& other) const = 0;

    private:

      BaseSegment();

      SegmentType m_type;

      void* m_private;
  };

}

namespace RAJA {
  class RangeSegment : public BaseSegment
  {
    public:

      RangeSegment()
        : BaseSegment( _RangeSeg_ ),
        m_begin(UndefinedValue),
        m_end(UndefinedValue) { ; }

      RangeSegment(Index_type begin, Index_type end)
        : BaseSegment( _RangeSeg_ ),
        m_begin(begin),
        m_end(end) { ; }

      ~RangeSegment() {;}

      RangeSegment(const RangeSegment& other)
        : BaseSegment( _RangeSeg_ ),
        m_begin(other.m_begin),
        m_end(other.m_end) { ; }

      RangeSegment& operator=(const RangeSegment& rhs)
      {
        if ( &rhs != this ) {
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

      Index_type getLength() const { return (m_end-m_begin); }

      IndexOwnership getIndexOwnership() const { return Owned; }

      bool operator ==(const RangeSegment& other) const
      {
        return ( (m_begin == other.m_begin) && (m_end == other.m_end) );
      }

      bool operator !=(const RangeSegment& other) const
      {
        return ( !(*this == other) );
      }

      bool operator ==(const BaseSegment& other) const
      {
        const RangeSegment* o_ptr = dynamic_cast<const RangeSegment*>(&other);
        if ( o_ptr ) {
          return ( *this == *o_ptr );
        } else {
          return false;
        }
      }

      bool operator !=(const BaseSegment& other) const
      {
        return ( !(*this == other) );
      }

      void print(std::ostream& os) const;

    private:
      Index_type m_begin;
      Index_type m_end;
  };
  class RangeStrideSegment : public BaseSegment
  {
    public:

      RangeStrideSegment()
        : BaseSegment( _RangeStrideSeg_ ),
        m_begin(UndefinedValue),
        m_end(UndefinedValue),
        m_stride(UndefinedValue) { ; }

      RangeStrideSegment(Index_type begin, Index_type end, Index_type stride)
        : BaseSegment( _RangeStrideSeg_ ),
        m_begin(begin),
        m_end(end),
        m_stride(stride) { ; }

      ~RangeStrideSegment() {;}

      RangeStrideSegment(const RangeStrideSegment& other)
        : BaseSegment( _RangeStrideSeg_ ),
        m_begin(other.m_begin),
        m_end(other.m_end),
        m_stride(other.m_stride) { ; }

      RangeStrideSegment& operator=(const RangeStrideSegment& rhs)
      {
        if ( &rhs != this ) {
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

      Index_type getLength() const { return (m_end-m_begin) >= m_stride ?
        (m_end-m_begin)/m_stride + 1 : 0; }

      IndexOwnership getIndexOwnership() const { return Owned; }

      bool operator ==(const RangeStrideSegment& other) const
      {
        return ( (m_begin == other.m_begin) &&
            (m_end == other.m_end) &&
            (m_stride == other.m_stride) );
      }

      bool operator !=(const RangeStrideSegment& other) const
      {
        return ( !(*this == other) );
      }

      bool operator ==(const BaseSegment& other) const
      {
        const RangeStrideSegment* o_ptr =
          dynamic_cast<const RangeStrideSegment*>(&other);
        if ( o_ptr ) {
          return ( *this == *o_ptr );
        } else {
          return false;
        }
      }

      bool operator !=(const BaseSegment& other) const
      {
        return ( !(*this == other) );
      }

      void print(std::ostream& os) const;

    private:
      Index_type m_begin;
      Index_type m_end;
      Index_type m_stride;
  };

}

namespace RAJA {
  class ListSegment : public BaseSegment
  {
    public:
      ListSegment(const Index_type* indx, Index_type len,
          IndexOwnership indx_own = Owned);

      template< typename T> explicit ListSegment(const T& indx);

      ListSegment(const ListSegment& other);

      ListSegment& operator=(const ListSegment& rhs);

      ~ListSegment();

      void swap(ListSegment& other);

      const Index_type* getIndex() const { return m_indx; }

      Index_type getLength() const { return m_len; }

      IndexOwnership getIndexOwnership() const { return m_indx_own; }

      bool indicesEqual(const Index_type* indx, Index_type len) const;

      bool operator ==(const ListSegment& other) const
      {
        return ( indicesEqual(other.m_indx, other.m_len) );
      }

      bool operator !=(const ListSegment& other) const
      {
        return ( !(*this == other) );
      }

      bool operator ==(const BaseSegment& other) const
      {
        const ListSegment* o_ptr = dynamic_cast<const ListSegment*>(&other);
        if ( o_ptr ) {
          return ( *this == *o_ptr );
        } else {
          return false;
        }
      }

      bool operator !=(const BaseSegment& other) const
      {
        return ( !(*this == other) );
      }

      void print(std::ostream& os) const;

    private:

      ListSegment();

      void initIndexData(const Index_type* indx, Index_type len,
          IndexOwnership indx_own);

      Index_type* __restrict__ m_indx;
      Index_type m_len;
      IndexOwnership m_indx_own;
  };
  template< typename T>
    ListSegment::ListSegment(const T& indx)
    : BaseSegment( _ListSeg_ ),
    m_indx(0), m_indx_own(Unowned), m_len( indx.size() )
  {
    if ( !indx.empty() ) {

      m_indx = new Index_type[indx.size()];

      std::copy(indx.begin(), indx.end(), m_indx);
      m_indx_own = Owned;
    }
  }

}

namespace RAJA {
  class DepGraphNode
  {
    public:
      static const int _MaxDepTasks_ = 8;
      static const int _SemaphoreValueAlign_ = 256;

      DepGraphNode()
        : m_num_dep_tasks(0),
        m_semaphore_reload_value(0),
        m_semaphore_value(0)
    {
      posix_memalign((void **)(&m_semaphore_value),
          _SemaphoreValueAlign_, sizeof(int)) ;
      *m_semaphore_value = 0 ;
    }

      ~DepGraphNode() { if (m_semaphore_value) free(m_semaphore_value); }

      int& semaphoreValue() {
        return *m_semaphore_value ;
      }

      int& semaphoreReloadValue() {
        return m_semaphore_reload_value ;
      }

      int& numDepTasks() {
        return m_num_dep_tasks ;
      }

      int& depTaskNum(int tidx) {
        return m_dep_task[tidx] ;
      }

      void print(std::ostream& os) const;

    private:

      int m_dep_task[_MaxDepTasks_] ;
      int m_num_dep_tasks ;
      int m_semaphore_reload_value ;
      int* m_semaphore_value ;

  };

}

namespace RAJA {

  class BaseSegment;
  class IndexSetSegInfo
  {
    public:

      IndexSetSegInfo()
        : m_segment(0),
        m_owns_segment(false),
        m_icount(UndefinedValue),
        m_dep_graph_node(0) { ; }

      IndexSetSegInfo(BaseSegment* segment, bool owns_segment)
        : m_segment(segment),
        m_owns_segment(owns_segment),
        m_icount(UndefinedValue),
        m_dep_graph_node(0) { ; }

      ~IndexSetSegInfo() { if (m_dep_graph_node) delete m_dep_graph_node; }
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

namespace RAJA {
  template< typename T>
    class RAJAVec
    {
      public:

        explicit RAJAVec(size_t init_cap = 0)
          : m_capacity(0), m_size(0), m_data(0)
        {
          grow_cap(init_cap);
        }

        RAJAVec(const RAJAVec<T>& other)
          : m_capacity(0), m_size(0), m_data(0)
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
          if ( &rhs != this ) {
            RAJAVec<T> copy(rhs);
            this->swap(copy);
          }
          return *this;
        }

        ~RAJAVec()
        {
          if (m_capacity > 0) delete [] m_data;
        }

        size_t empty() const {
          return (m_size == 0) ;
        }

        size_t size() const {
          return m_size;
        }

        const T& operator [] (size_t i) const
        {
          return m_data[i];
        }

        T& operator [] (size_t i)
        {
          return m_data[i];
        }

        void push_back(const T& item)
        {
          push_back_private(item);
        }

        void push_front(const T& item)
        {
          push_front_private(item);
        }

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
          if (current_cap == 0) { return s_init_cap; }
          return static_cast<size_t>( current_cap * s_grow_fac );
        }

        void grow_cap(size_t target_size)
        {
          size_t target_cap = m_capacity;
          while ( target_cap < target_size ) { target_cap = nextCap(target_cap); }

          if ( m_capacity < target_cap ) {
            T* tdata = new T[target_cap];

            if ( m_data ) {
              for (size_t i = 0; (i < m_size)&&(i < target_cap); ++i) {
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
          grow_cap(m_size+1);
          m_data[m_size] = item;
          m_size++;
        }

        void push_front_private(const T& item)
        {
          size_t old_size = m_size;
          grow_cap( old_size+1 );

          for (size_t i = old_size; i > 0; --i) {
            m_data[i] = m_data[i-1];
          }
          m_data[0] = item;
          m_size++;
        }

        size_t m_capacity;
        size_t m_size;
        T* m_data;
    };
  template< typename T>
    const size_t RAJAVec<T>::s_init_cap = 8;
  template< typename T>
    const double RAJAVec<T>::s_grow_fac = 1.5;

}

namespace RAJA {
  class IndexSet
  {
    public:
      template< typename SEG_ITER_POLICY_T,
        typename SEG_EXEC_POLICY_T > struct ExecPolicy
        {
          typedef SEG_ITER_POLICY_T seg_it;
          typedef SEG_EXEC_POLICY_T seg_exec;
        };
      IndexSet();

      IndexSet(const IndexSet& other);

      IndexSet& operator=(const IndexSet& rhs);

      ~IndexSet();

      void swap(IndexSet& other);
      bool isValidSegmentType(const BaseSegment* segment) const;
      bool push_back_nocopy(BaseSegment* segment)
      { return( push_back_private(segment, false ) ); }

      bool push_front_nocopy(BaseSegment* segment)
      { return( push_front_private(segment, false ) ); }

      bool push_back(const BaseSegment& segment);

      bool push_front(const BaseSegment& segment);

      Index_type getLength() const { return m_len; }

      unsigned getNumSegments() const {
        return m_segments.size();
      }
      const BaseSegment* getSegment(unsigned i) const {
        return m_segments[i].getSegment();
      }
      BaseSegment* getSegment(unsigned i) {
        return m_segments[i].getSegment();
      }

      const IndexSetSegInfo* getSegmentInfo(unsigned i) const {
        return &(m_segments[i]);
      }

      IndexSetSegInfo* getSegmentInfo(unsigned i) {
        return &(m_segments[i]);
      }
      IndexSet* createView(int begin, int end) const;
      IndexSet* createView(const int* segIds, int len) const;
      template< typename T>
        IndexSet* createView(const T& segIds) const;
      void setSegmentInterval(int interval_id, int begin, int end);

      int getSegmentIntervalBegin(int interval_id) const {
        return m_seg_interval_begin[interval_id];
      }

      int getSegmentIntervalEnd(int interval_id) const {
        return m_seg_interval_end[interval_id];
      }
      void* getPrivate() const { return m_private ; }

      void setPrivate(void *ptr) { m_private = ptr ; }
      bool dependencyGraphSet() const { return m_dep_graph_set; }
      void initDependencyGraph();

      void finalizeDependencyGraph() { m_dep_graph_set = true; }
      bool operator ==(const IndexSet& other) const ;

      bool operator !=(const IndexSet& other) const
      {
        return ( !(*this == other) );
      }

      void print(std::ostream& os) const;

    private:

      void copy(const IndexSet& other);

      bool push_back_private(BaseSegment* seg, bool owns_segment);

      bool push_front_private(BaseSegment* seg, bool owns_segment);

      BaseSegment* createSegmentCopy(const BaseSegment& segment) const;

      Index_type m_len;

      RAJAVec<IndexSetSegInfo> m_segments;

      RAJAVec<int> m_seg_interval_begin;
      RAJAVec<int> m_seg_interval_end;

      void* m_private;

      bool m_dep_graph_set;

  };
  template< typename T>
    IndexSet* IndexSet::createView(const T& segIds) const
    {
      IndexSet *retVal = new IndexSet() ;

      int numSeg = m_segments.size() ;
      for (typename T::iterator it = segIds.begin(); it != segIds.end(); ++it) {
        if (*it >= 0 && *it < numSeg) {
          retVal->push_back_nocopy(
              const_cast<BaseSegment*>( m_segments[ *it ].getSegment() ) ) ;
        }
      }

      return retVal ;
    }

}

#if 0
namespace std {

  template< >
    inline __attribute__((always_inline))
    void swap(RAJA::IndexSet& a, RAJA::IndexSet& b)
    {
      a.swap(b);
    }

}
#endif

namespace RAJA {
  template<typename TYPE>
    class IndexValue {

      public:

        inline __attribute__((always_inline))
          constexpr
          IndexValue() : value(0) {}

        inline __attribute__((always_inline))
          constexpr
          explicit IndexValue(Index_type v) : value(v) {}

        inline __attribute__((always_inline))
          Index_type operator*(void) const {return value;}

        inline __attribute__((always_inline))
          TYPE &operator++(int){
            value++;
            return *static_cast<TYPE *>(this);
          }

        inline __attribute__((always_inline))
          TYPE &operator++(){
            value++;
            return *static_cast<TYPE *>(this);
          }

        inline __attribute__((always_inline))
          TYPE &operator--(int){
            value--;
            return *static_cast<TYPE *>(this);
          }

        inline __attribute__((always_inline))
          TYPE &operator--(){
            value--;
            return *static_cast<TYPE *>(this);
          }

        inline __attribute__((always_inline))
          TYPE operator+(Index_type a) const { return TYPE(value+a); }

        inline __attribute__((always_inline))
          TYPE operator+(TYPE a) const { return TYPE(value+a.value); }

        inline __attribute__((always_inline))
          TYPE operator-(Index_type a) const { return TYPE(value-a); }

        inline __attribute__((always_inline))
          TYPE operator-(TYPE a) const { return TYPE(value-a.value); }

        inline __attribute__((always_inline))
          TYPE operator*(Index_type a) const { return TYPE(value*a); }

        inline __attribute__((always_inline))
          TYPE operator*(TYPE a) const { return TYPE(value*a.value); }

        inline __attribute__((always_inline))
          TYPE operator/(Index_type a) const { return TYPE(value/a); }

        inline __attribute__((always_inline))
          TYPE operator/(TYPE a) const { return TYPE(value/a.value); }

        inline __attribute__((always_inline)) TYPE &operator+=(Index_type x){
          value += x;
          return *static_cast<TYPE *>(this);
        }

        inline __attribute__((always_inline)) TYPE &operator+=(TYPE x){
          value += x.value;
          return *static_cast<TYPE *>(this);
        }

        inline __attribute__((always_inline)) TYPE &operator-=(Index_type x){
          value -= x;
          return *static_cast<TYPE *>(this);
        }

        inline __attribute__((always_inline)) TYPE &operator-=(TYPE x){
          value -= x.value;
          return *static_cast<TYPE *>(this);
        }

        inline __attribute__((always_inline)) TYPE &operator*=(Index_type x){
          value *= x;
          return *static_cast<TYPE *>(this);
        }

        inline __attribute__((always_inline)) TYPE &operator*=(TYPE x){
          value *= x.value;
          return *static_cast<TYPE *>(this);
        }

        inline __attribute__((always_inline)) TYPE &operator/=(Index_type x){
          value /= x;
          return *static_cast<TYPE *>(this);
        }

        inline __attribute__((always_inline)) TYPE &operator/=(TYPE x){
          value /= x.value;
          return *static_cast<TYPE *>(this);
        }

        inline __attribute__((always_inline)) bool operator<(Index_type x) const {
          return( value < x);
        }

        inline __attribute__((always_inline)) bool operator<(TYPE x) const {
          return( value < x.value);
        }

        inline __attribute__((always_inline)) bool operator<=(Index_type x) const {
          return( value <= x);
        }

        inline __attribute__((always_inline)) bool operator<=(TYPE x) const {
          return( value <= x.value);
        }

        inline __attribute__((always_inline)) bool operator>(Index_type x) const {
          return( value > x);
        }

        inline __attribute__((always_inline)) bool operator>(TYPE x) const {
          return( value > x.value);
        }

        inline __attribute__((always_inline)) bool operator>=(Index_type x) const {
          return( value >= x);
        }

        inline __attribute__((always_inline)) bool operator>=(TYPE x) const {
          return( value >= x.value);
        }

        inline __attribute__((always_inline)) bool operator==(Index_type x) const {
          return( value == x);
        }

        inline __attribute__((always_inline)) bool operator==(TYPE x) const {
          return( value == x.value);
        }

        inline __attribute__((always_inline)) bool operator!=(Index_type x) const {
          return( value != x);
        }

        inline __attribute__((always_inline)) bool operator!=(TYPE x) const {
          return( value != x.value);
        }

        static std::string getName(void);

      protected:
        Index_type value;

    };

  template<typename TO, typename FROM>
    struct ConvertIndexHelper {

      inline __attribute__((always_inline))
        static TO convert(FROM val){
          return TO(*val);
        }
    };

  template<typename TO>
    struct ConvertIndexHelper<TO, Index_type> {

      inline __attribute__((always_inline))
        static TO convert(Index_type val){
          return TO(val);
        }
    };

  template<typename TO, typename FROM>

    inline __attribute__((always_inline))
    TO convertIndex(FROM val){
      return ConvertIndexHelper<TO, FROM>::convert(val);
    }

}

namespace RAJA {

  template<typename IdxLin, typename Perm, typename ... IdxList>
    struct Layout {};
  struct PERM_I {};
  struct PERM_IJ {};
  struct PERM_JI {};
  struct PERM_IJK {};
  struct PERM_IKJ {};
  struct PERM_JIK {};
  struct PERM_JKI {};
  struct PERM_KIJ {};
  struct PERM_KJI {};
  struct PERM_IJKL {};
  struct PERM_IJLK {};
  struct PERM_IKJL {};
  struct PERM_IKLJ {};
  struct PERM_ILJK {};
  struct PERM_ILKJ {};
  struct PERM_JIKL {};
  struct PERM_JILK {};
  struct PERM_JKIL {};
  struct PERM_JKLI {};
  struct PERM_JLIK {};
  struct PERM_JLKI {};
  struct PERM_KIJL {};
  struct PERM_KILJ {};
  struct PERM_KJIL {};
  struct PERM_KJLI {};
  struct PERM_KLIJ {};
  struct PERM_KLJI {};
  struct PERM_LIJK {};
  struct PERM_LIKJ {};
  struct PERM_LJIK {};
  struct PERM_LJKI {};
  struct PERM_LKIJ {};
  struct PERM_LKJI {};
  struct PERM_IJKLM {};
  struct PERM_IJKML {};
  struct PERM_IJLKM {};
  struct PERM_IJLMK {};
  struct PERM_IJMKL {};
  struct PERM_IJMLK {};
  struct PERM_IKJLM {};
  struct PERM_IKJML {};
  struct PERM_IKLJM {};
  struct PERM_IKLMJ {};
  struct PERM_IKMJL {};
  struct PERM_IKMLJ {};
  struct PERM_ILJKM {};
  struct PERM_ILJMK {};
  struct PERM_ILKJM {};
  struct PERM_ILKMJ {};
  struct PERM_ILMJK {};
  struct PERM_ILMKJ {};
  struct PERM_IMJKL {};
  struct PERM_IMJLK {};
  struct PERM_IMKJL {};
  struct PERM_IMKLJ {};
  struct PERM_IMLJK {};
  struct PERM_IMLKJ {};
  struct PERM_JIKLM {};
  struct PERM_JIKML {};
  struct PERM_JILKM {};
  struct PERM_JILMK {};
  struct PERM_JIMKL {};
  struct PERM_JIMLK {};
  struct PERM_JKILM {};
  struct PERM_JKIML {};
  struct PERM_JKLIM {};
  struct PERM_JKLMI {};
  struct PERM_JKMIL {};
  struct PERM_JKMLI {};
  struct PERM_JLIKM {};
  struct PERM_JLIMK {};
  struct PERM_JLKIM {};
  struct PERM_JLKMI {};
  struct PERM_JLMIK {};
  struct PERM_JLMKI {};
  struct PERM_JMIKL {};
  struct PERM_JMILK {};
  struct PERM_JMKIL {};
  struct PERM_JMKLI {};
  struct PERM_JMLIK {};
  struct PERM_JMLKI {};
  struct PERM_KIJLM {};
  struct PERM_KIJML {};
  struct PERM_KILJM {};
  struct PERM_KILMJ {};
  struct PERM_KIMJL {};
  struct PERM_KIMLJ {};
  struct PERM_KJILM {};
  struct PERM_KJIML {};
  struct PERM_KJLIM {};
  struct PERM_KJLMI {};
  struct PERM_KJMIL {};
  struct PERM_KJMLI {};
  struct PERM_KLIJM {};
  struct PERM_KLIMJ {};
  struct PERM_KLJIM {};
  struct PERM_KLJMI {};
  struct PERM_KLMIJ {};
  struct PERM_KLMJI {};
  struct PERM_KMIJL {};
  struct PERM_KMILJ {};
  struct PERM_KMJIL {};
  struct PERM_KMJLI {};
  struct PERM_KMLIJ {};
  struct PERM_KMLJI {};
  struct PERM_LIJKM {};
  struct PERM_LIJMK {};
  struct PERM_LIKJM {};
  struct PERM_LIKMJ {};
  struct PERM_LIMJK {};
  struct PERM_LIMKJ {};
  struct PERM_LJIKM {};
  struct PERM_LJIMK {};
  struct PERM_LJKIM {};
  struct PERM_LJKMI {};
  struct PERM_LJMIK {};
  struct PERM_LJMKI {};
  struct PERM_LKIJM {};
  struct PERM_LKIMJ {};
  struct PERM_LKJIM {};
  struct PERM_LKJMI {};
  struct PERM_LKMIJ {};
  struct PERM_LKMJI {};
  struct PERM_LMIJK {};
  struct PERM_LMIKJ {};
  struct PERM_LMJIK {};
  struct PERM_LMJKI {};
  struct PERM_LMKIJ {};
  struct PERM_LMKJI {};
  struct PERM_MIJKL {};
  struct PERM_MIJLK {};
  struct PERM_MIKJL {};
  struct PERM_MIKLJ {};
  struct PERM_MILJK {};
  struct PERM_MILKJ {};
  struct PERM_MJIKL {};
  struct PERM_MJILK {};
  struct PERM_MJKIL {};
  struct PERM_MJKLI {};
  struct PERM_MJLIK {};
  struct PERM_MJLKI {};
  struct PERM_MKIJL {};
  struct PERM_MKILJ {};
  struct PERM_MKJIL {};
  struct PERM_MKJLI {};
  struct PERM_MKLIJ {};
  struct PERM_MKLJI {};
  struct PERM_MLIJK {};
  struct PERM_MLIKJ {};
  struct PERM_MLJIK {};
  struct PERM_MLJKI {};
  struct PERM_MLKIJ {};
  struct PERM_MLKJI {};

  template<typename IdxLin, typename IdxI>
    struct Layout<IdxLin, PERM_I, IdxI> {
      typedef PERM_I Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;

      Index_type const size_i;

      Index_type const stride_i;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni):
        size_i(ni), stride_i(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i) const {
        Index_type linear = convertIndex<Index_type>(lin);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ>
    struct Layout<IdxLin, PERM_IJ, IdxI, IdxJ> {
      typedef PERM_IJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;

      Index_type const size_i;
      Index_type const size_j;

      Index_type const stride_i;
      Index_type const stride_j;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj):
        size_i(ni), size_j(nj), stride_i(nj), stride_j(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ>
    struct Layout<IdxLin, PERM_JI, IdxI, IdxJ> {
      typedef PERM_JI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;

      Index_type const size_i;
      Index_type const size_j;

      Index_type const stride_i;
      Index_type const stride_j;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj):
        size_i(ni), size_j(nj), stride_i(1), stride_j(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK>
    struct Layout<IdxLin, PERM_IJK, IdxI, IdxJ, IdxK> {
      typedef PERM_IJK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk):
        size_i(ni), size_j(nj), size_k(nk), stride_i(nj*nk), stride_j(nk), stride_k(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j*size_k);
        i = IdxI(_i);
        linear -= _i*(size_j*size_k);
        Index_type _j = linear / (size_k);
        j = IdxJ(_j);
        linear -= _j*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK>
    struct Layout<IdxLin, PERM_IKJ, IdxI, IdxJ, IdxK> {
      typedef PERM_IKJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk):
        size_i(ni), size_j(nj), size_k(nk), stride_i(nk*nj), stride_j(1), stride_k(nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_k*size_j);
        i = IdxI(_i);
        linear -= _i*(size_k*size_j);
        Index_type _k = linear / (size_j);
        k = IdxK(_k);
        linear -= _k*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK>
    struct Layout<IdxLin, PERM_JIK, IdxI, IdxJ, IdxK> {
      typedef PERM_JIK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk):
        size_i(ni), size_j(nj), size_k(nk), stride_i(nk), stride_j(ni*nk), stride_k(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_k);
        Index_type _i = linear / (size_k);
        i = IdxI(_i);
        linear -= _i*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK>
    struct Layout<IdxLin, PERM_JKI, IdxI, IdxJ, IdxK> {
      typedef PERM_JKI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk):
        size_i(ni), size_j(nj), size_k(nk), stride_i(1), stride_j(nk*ni), stride_k(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_k*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_i);
        Index_type _k = linear / (size_i);
        k = IdxK(_k);
        linear -= _k*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK>
    struct Layout<IdxLin, PERM_KIJ, IdxI, IdxJ, IdxK> {
      typedef PERM_KIJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk):
        size_i(ni), size_j(nj), size_k(nk), stride_i(nj), stride_j(1), stride_k(ni*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_i*size_j);
        k = IdxK(_k);
        linear -= _k*(size_i*size_j);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK>
    struct Layout<IdxLin, PERM_KJI, IdxI, IdxJ, IdxK> {
      typedef PERM_KJI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk):
        size_i(ni), size_j(nj), size_k(nk), stride_i(1), stride_j(ni), stride_k(nj*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_j*size_i);
        k = IdxK(_k);
        linear -= _k*(size_j*size_i);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_IJKL, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_IJKL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nj*nk*nl), stride_j(nk*nl), stride_k(nl), stride_l(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j*size_k*size_l);
        i = IdxI(_i);
        linear -= _i*(size_j*size_k*size_l);
        Index_type _j = linear / (size_k*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_l);
        Index_type _k = linear / (size_l);
        k = IdxK(_k);
        linear -= _k*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_IJLK, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_IJLK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nj*nl*nk), stride_j(nl*nk), stride_k(1), stride_l(nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j*size_l*size_k);
        i = IdxI(_i);
        linear -= _i*(size_j*size_l*size_k);
        Index_type _j = linear / (size_l*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_k);
        Index_type _l = linear / (size_k);
        l = IdxL(_l);
        linear -= _l*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_IKJL, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_IKJL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nk*nj*nl), stride_j(nl), stride_k(nj*nl), stride_l(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_k*size_j*size_l);
        i = IdxI(_i);
        linear -= _i*(size_k*size_j*size_l);
        Index_type _k = linear / (size_j*size_l);
        k = IdxK(_k);
        linear -= _k*(size_j*size_l);
        Index_type _j = linear / (size_l);
        j = IdxJ(_j);
        linear -= _j*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_IKLJ, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_IKLJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nk*nl*nj), stride_j(1), stride_k(nl*nj), stride_l(nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_k*size_l*size_j);
        i = IdxI(_i);
        linear -= _i*(size_k*size_l*size_j);
        Index_type _k = linear / (size_l*size_j);
        k = IdxK(_k);
        linear -= _k*(size_l*size_j);
        Index_type _l = linear / (size_j);
        l = IdxL(_l);
        linear -= _l*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_ILJK, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_ILJK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nl*nj*nk), stride_j(nk), stride_k(1), stride_l(nj*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_l*size_j*size_k);
        i = IdxI(_i);
        linear -= _i*(size_l*size_j*size_k);
        Index_type _l = linear / (size_j*size_k);
        l = IdxL(_l);
        linear -= _l*(size_j*size_k);
        Index_type _j = linear / (size_k);
        j = IdxJ(_j);
        linear -= _j*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_ILKJ, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_ILKJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nl*nk*nj), stride_j(1), stride_k(nj), stride_l(nk*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_l*size_k*size_j);
        i = IdxI(_i);
        linear -= _i*(size_l*size_k*size_j);
        Index_type _l = linear / (size_k*size_j);
        l = IdxL(_l);
        linear -= _l*(size_k*size_j);
        Index_type _k = linear / (size_j);
        k = IdxK(_k);
        linear -= _k*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_JIKL, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_JIKL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nk*nl), stride_j(ni*nk*nl), stride_k(nl), stride_l(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i*size_k*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_k*size_l);
        Index_type _i = linear / (size_k*size_l);
        i = IdxI(_i);
        linear -= _i*(size_k*size_l);
        Index_type _k = linear / (size_l);
        k = IdxK(_k);
        linear -= _k*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_JILK, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_JILK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nl*nk), stride_j(ni*nl*nk), stride_k(1), stride_l(nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i*size_l*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_l*size_k);
        Index_type _i = linear / (size_l*size_k);
        i = IdxI(_i);
        linear -= _i*(size_l*size_k);
        Index_type _l = linear / (size_k);
        l = IdxL(_l);
        linear -= _l*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_JKIL, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_JKIL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nl), stride_j(nk*ni*nl), stride_k(ni*nl), stride_l(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_k*size_i*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_i*size_l);
        Index_type _k = linear / (size_i*size_l);
        k = IdxK(_k);
        linear -= _k*(size_i*size_l);
        Index_type _i = linear / (size_l);
        i = IdxI(_i);
        linear -= _i*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_JKLI, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_JKLI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(1), stride_j(nk*nl*ni), stride_k(nl*ni), stride_l(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_k*size_l*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_l*size_i);
        Index_type _k = linear / (size_l*size_i);
        k = IdxK(_k);
        linear -= _k*(size_l*size_i);
        Index_type _l = linear / (size_i);
        l = IdxL(_l);
        linear -= _l*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_JLIK, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_JLIK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nk), stride_j(nl*ni*nk), stride_k(1), stride_l(ni*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_l*size_i*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_i*size_k);
        Index_type _l = linear / (size_i*size_k);
        l = IdxL(_l);
        linear -= _l*(size_i*size_k);
        Index_type _i = linear / (size_k);
        i = IdxI(_i);
        linear -= _i*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_JLKI, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_JLKI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(1), stride_j(nl*nk*ni), stride_k(ni), stride_l(nk*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_l*size_k*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_k*size_i);
        Index_type _l = linear / (size_k*size_i);
        l = IdxL(_l);
        linear -= _l*(size_k*size_i);
        Index_type _k = linear / (size_i);
        k = IdxK(_k);
        linear -= _k*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_KIJL, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_KIJL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nj*nl), stride_j(nl), stride_k(ni*nj*nl), stride_l(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_i*size_j*size_l);
        k = IdxK(_k);
        linear -= _k*(size_i*size_j*size_l);
        Index_type _i = linear / (size_j*size_l);
        i = IdxI(_i);
        linear -= _i*(size_j*size_l);
        Index_type _j = linear / (size_l);
        j = IdxJ(_j);
        linear -= _j*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_KILJ, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_KILJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nl*nj), stride_j(1), stride_k(ni*nl*nj), stride_l(nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_i*size_l*size_j);
        k = IdxK(_k);
        linear -= _k*(size_i*size_l*size_j);
        Index_type _i = linear / (size_l*size_j);
        i = IdxI(_i);
        linear -= _i*(size_l*size_j);
        Index_type _l = linear / (size_j);
        l = IdxL(_l);
        linear -= _l*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_KJIL, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_KJIL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nl), stride_j(ni*nl), stride_k(nj*ni*nl), stride_l(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_j*size_i*size_l);
        k = IdxK(_k);
        linear -= _k*(size_j*size_i*size_l);
        Index_type _j = linear / (size_i*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_l);
        Index_type _i = linear / (size_l);
        i = IdxI(_i);
        linear -= _i*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_KJLI, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_KJLI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(1), stride_j(nl*ni), stride_k(nj*nl*ni), stride_l(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_j*size_l*size_i);
        k = IdxK(_k);
        linear -= _k*(size_j*size_l*size_i);
        Index_type _j = linear / (size_l*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_i);
        Index_type _l = linear / (size_i);
        l = IdxL(_l);
        linear -= _l*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_KLIJ, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_KLIJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nj), stride_j(1), stride_k(nl*ni*nj), stride_l(ni*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_l*size_i*size_j);
        k = IdxK(_k);
        linear -= _k*(size_l*size_i*size_j);
        Index_type _l = linear / (size_i*size_j);
        l = IdxL(_l);
        linear -= _l*(size_i*size_j);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_KLJI, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_KLJI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(1), stride_j(ni), stride_k(nl*nj*ni), stride_l(nj*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_l*size_j*size_i);
        k = IdxK(_k);
        linear -= _k*(size_l*size_j*size_i);
        Index_type _l = linear / (size_j*size_i);
        l = IdxL(_l);
        linear -= _l*(size_j*size_i);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_LIJK, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_LIJK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nj*nk), stride_j(nk), stride_k(1), stride_l(ni*nj*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_i*size_j*size_k);
        l = IdxL(_l);
        linear -= _l*(size_i*size_j*size_k);
        Index_type _i = linear / (size_j*size_k);
        i = IdxI(_i);
        linear -= _i*(size_j*size_k);
        Index_type _j = linear / (size_k);
        j = IdxJ(_j);
        linear -= _j*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_LIKJ, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_LIKJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nk*nj), stride_j(1), stride_k(nj), stride_l(ni*nk*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_i*size_k*size_j);
        l = IdxL(_l);
        linear -= _l*(size_i*size_k*size_j);
        Index_type _i = linear / (size_k*size_j);
        i = IdxI(_i);
        linear -= _i*(size_k*size_j);
        Index_type _k = linear / (size_j);
        k = IdxK(_k);
        linear -= _k*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_LJIK, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_LJIK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nk), stride_j(ni*nk), stride_k(1), stride_l(nj*ni*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_j*size_i*size_k);
        l = IdxL(_l);
        linear -= _l*(size_j*size_i*size_k);
        Index_type _j = linear / (size_i*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_k);
        Index_type _i = linear / (size_k);
        i = IdxI(_i);
        linear -= _i*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_LJKI, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_LJKI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(1), stride_j(nk*ni), stride_k(ni), stride_l(nj*nk*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_j*size_k*size_i);
        l = IdxL(_l);
        linear -= _l*(size_j*size_k*size_i);
        Index_type _j = linear / (size_k*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_i);
        Index_type _k = linear / (size_i);
        k = IdxK(_k);
        linear -= _k*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_LKIJ, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_LKIJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(nj), stride_j(1), stride_k(ni*nj), stride_l(nk*ni*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_k*size_i*size_j);
        l = IdxL(_l);
        linear -= _l*(size_k*size_i*size_j);
        Index_type _k = linear / (size_i*size_j);
        k = IdxK(_k);
        linear -= _k*(size_i*size_j);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL>
    struct Layout<IdxLin, PERM_LKJI, IdxI, IdxJ, IdxK, IdxL> {
      typedef PERM_LKJI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), stride_i(1), stride_j(ni), stride_k(nj*ni), stride_l(nk*nj*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_k*size_j*size_i);
        l = IdxL(_l);
        linear -= _l*(size_k*size_j*size_i);
        Index_type _k = linear / (size_j*size_i);
        k = IdxK(_k);
        linear -= _k*(size_j*size_i);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IJKLM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IJKLM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nk*nl*nm), stride_j(nk*nl*nm), stride_k(nl*nm), stride_l(nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j*size_k*size_l*size_m);
        i = IdxI(_i);
        linear -= _i*(size_j*size_k*size_l*size_m);
        Index_type _j = linear / (size_k*size_l*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_l*size_m);
        Index_type _k = linear / (size_l*size_m);
        k = IdxK(_k);
        linear -= _k*(size_l*size_m);
        Index_type _l = linear / (size_m);
        l = IdxL(_l);
        linear -= _l*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IJKML, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IJKML Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nk*nm*nl), stride_j(nk*nm*nl), stride_k(nm*nl), stride_l(1), stride_m(nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j*size_k*size_m*size_l);
        i = IdxI(_i);
        linear -= _i*(size_j*size_k*size_m*size_l);
        Index_type _j = linear / (size_k*size_m*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_m*size_l);
        Index_type _k = linear / (size_m*size_l);
        k = IdxK(_k);
        linear -= _k*(size_m*size_l);
        Index_type _m = linear / (size_l);
        m = IdxM(_m);
        linear -= _m*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IJLKM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IJLKM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nl*nk*nm), stride_j(nl*nk*nm), stride_k(nm), stride_l(nk*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j*size_l*size_k*size_m);
        i = IdxI(_i);
        linear -= _i*(size_j*size_l*size_k*size_m);
        Index_type _j = linear / (size_l*size_k*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_k*size_m);
        Index_type _l = linear / (size_k*size_m);
        l = IdxL(_l);
        linear -= _l*(size_k*size_m);
        Index_type _k = linear / (size_m);
        k = IdxK(_k);
        linear -= _k*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IJLMK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IJLMK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nl*nm*nk), stride_j(nl*nm*nk), stride_k(1), stride_l(nm*nk), stride_m(nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j*size_l*size_m*size_k);
        i = IdxI(_i);
        linear -= _i*(size_j*size_l*size_m*size_k);
        Index_type _j = linear / (size_l*size_m*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_m*size_k);
        Index_type _l = linear / (size_m*size_k);
        l = IdxL(_l);
        linear -= _l*(size_m*size_k);
        Index_type _m = linear / (size_k);
        m = IdxM(_m);
        linear -= _m*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IJMKL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IJMKL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nm*nk*nl), stride_j(nm*nk*nl), stride_k(nl), stride_l(1), stride_m(nk*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j*size_m*size_k*size_l);
        i = IdxI(_i);
        linear -= _i*(size_j*size_m*size_k*size_l);
        Index_type _j = linear / (size_m*size_k*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_k*size_l);
        Index_type _m = linear / (size_k*size_l);
        m = IdxM(_m);
        linear -= _m*(size_k*size_l);
        Index_type _k = linear / (size_l);
        k = IdxK(_k);
        linear -= _k*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IJMLK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IJMLK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nm*nl*nk), stride_j(nm*nl*nk), stride_k(1), stride_l(nk), stride_m(nl*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_j*size_m*size_l*size_k);
        i = IdxI(_i);
        linear -= _i*(size_j*size_m*size_l*size_k);
        Index_type _j = linear / (size_m*size_l*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_l*size_k);
        Index_type _m = linear / (size_l*size_k);
        m = IdxM(_m);
        linear -= _m*(size_l*size_k);
        Index_type _l = linear / (size_k);
        l = IdxL(_l);
        linear -= _l*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IKJLM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IKJLM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nj*nl*nm), stride_j(nl*nm), stride_k(nj*nl*nm), stride_l(nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_k*size_j*size_l*size_m);
        i = IdxI(_i);
        linear -= _i*(size_k*size_j*size_l*size_m);
        Index_type _k = linear / (size_j*size_l*size_m);
        k = IdxK(_k);
        linear -= _k*(size_j*size_l*size_m);
        Index_type _j = linear / (size_l*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_m);
        Index_type _l = linear / (size_m);
        l = IdxL(_l);
        linear -= _l*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IKJML, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IKJML Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nj*nm*nl), stride_j(nm*nl), stride_k(nj*nm*nl), stride_l(1), stride_m(nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_k*size_j*size_m*size_l);
        i = IdxI(_i);
        linear -= _i*(size_k*size_j*size_m*size_l);
        Index_type _k = linear / (size_j*size_m*size_l);
        k = IdxK(_k);
        linear -= _k*(size_j*size_m*size_l);
        Index_type _j = linear / (size_m*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_l);
        Index_type _m = linear / (size_l);
        m = IdxM(_m);
        linear -= _m*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IKLJM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IKLJM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nl*nj*nm), stride_j(nm), stride_k(nl*nj*nm), stride_l(nj*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_k*size_l*size_j*size_m);
        i = IdxI(_i);
        linear -= _i*(size_k*size_l*size_j*size_m);
        Index_type _k = linear / (size_l*size_j*size_m);
        k = IdxK(_k);
        linear -= _k*(size_l*size_j*size_m);
        Index_type _l = linear / (size_j*size_m);
        l = IdxL(_l);
        linear -= _l*(size_j*size_m);
        Index_type _j = linear / (size_m);
        j = IdxJ(_j);
        linear -= _j*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IKLMJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IKLMJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nl*nm*nj), stride_j(1), stride_k(nl*nm*nj), stride_l(nm*nj), stride_m(nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_k*size_l*size_m*size_j);
        i = IdxI(_i);
        linear -= _i*(size_k*size_l*size_m*size_j);
        Index_type _k = linear / (size_l*size_m*size_j);
        k = IdxK(_k);
        linear -= _k*(size_l*size_m*size_j);
        Index_type _l = linear / (size_m*size_j);
        l = IdxL(_l);
        linear -= _l*(size_m*size_j);
        Index_type _m = linear / (size_j);
        m = IdxM(_m);
        linear -= _m*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IKMJL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IKMJL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nm*nj*nl), stride_j(nl), stride_k(nm*nj*nl), stride_l(1), stride_m(nj*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_k*size_m*size_j*size_l);
        i = IdxI(_i);
        linear -= _i*(size_k*size_m*size_j*size_l);
        Index_type _k = linear / (size_m*size_j*size_l);
        k = IdxK(_k);
        linear -= _k*(size_m*size_j*size_l);
        Index_type _m = linear / (size_j*size_l);
        m = IdxM(_m);
        linear -= _m*(size_j*size_l);
        Index_type _j = linear / (size_l);
        j = IdxJ(_j);
        linear -= _j*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IKMLJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IKMLJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nm*nl*nj), stride_j(1), stride_k(nm*nl*nj), stride_l(nj), stride_m(nl*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_k*size_m*size_l*size_j);
        i = IdxI(_i);
        linear -= _i*(size_k*size_m*size_l*size_j);
        Index_type _k = linear / (size_m*size_l*size_j);
        k = IdxK(_k);
        linear -= _k*(size_m*size_l*size_j);
        Index_type _m = linear / (size_l*size_j);
        m = IdxM(_m);
        linear -= _m*(size_l*size_j);
        Index_type _l = linear / (size_j);
        l = IdxL(_l);
        linear -= _l*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_ILJKM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_ILJKM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nj*nk*nm), stride_j(nk*nm), stride_k(nm), stride_l(nj*nk*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_l*size_j*size_k*size_m);
        i = IdxI(_i);
        linear -= _i*(size_l*size_j*size_k*size_m);
        Index_type _l = linear / (size_j*size_k*size_m);
        l = IdxL(_l);
        linear -= _l*(size_j*size_k*size_m);
        Index_type _j = linear / (size_k*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_m);
        Index_type _k = linear / (size_m);
        k = IdxK(_k);
        linear -= _k*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_ILJMK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_ILJMK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nj*nm*nk), stride_j(nm*nk), stride_k(1), stride_l(nj*nm*nk), stride_m(nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_l*size_j*size_m*size_k);
        i = IdxI(_i);
        linear -= _i*(size_l*size_j*size_m*size_k);
        Index_type _l = linear / (size_j*size_m*size_k);
        l = IdxL(_l);
        linear -= _l*(size_j*size_m*size_k);
        Index_type _j = linear / (size_m*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_k);
        Index_type _m = linear / (size_k);
        m = IdxM(_m);
        linear -= _m*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_ILKJM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_ILKJM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nk*nj*nm), stride_j(nm), stride_k(nj*nm), stride_l(nk*nj*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_l*size_k*size_j*size_m);
        i = IdxI(_i);
        linear -= _i*(size_l*size_k*size_j*size_m);
        Index_type _l = linear / (size_k*size_j*size_m);
        l = IdxL(_l);
        linear -= _l*(size_k*size_j*size_m);
        Index_type _k = linear / (size_j*size_m);
        k = IdxK(_k);
        linear -= _k*(size_j*size_m);
        Index_type _j = linear / (size_m);
        j = IdxJ(_j);
        linear -= _j*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_ILKMJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_ILKMJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nk*nm*nj), stride_j(1), stride_k(nm*nj), stride_l(nk*nm*nj), stride_m(nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_l*size_k*size_m*size_j);
        i = IdxI(_i);
        linear -= _i*(size_l*size_k*size_m*size_j);
        Index_type _l = linear / (size_k*size_m*size_j);
        l = IdxL(_l);
        linear -= _l*(size_k*size_m*size_j);
        Index_type _k = linear / (size_m*size_j);
        k = IdxK(_k);
        linear -= _k*(size_m*size_j);
        Index_type _m = linear / (size_j);
        m = IdxM(_m);
        linear -= _m*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_ILMJK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_ILMJK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nm*nj*nk), stride_j(nk), stride_k(1), stride_l(nm*nj*nk), stride_m(nj*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_l*size_m*size_j*size_k);
        i = IdxI(_i);
        linear -= _i*(size_l*size_m*size_j*size_k);
        Index_type _l = linear / (size_m*size_j*size_k);
        l = IdxL(_l);
        linear -= _l*(size_m*size_j*size_k);
        Index_type _m = linear / (size_j*size_k);
        m = IdxM(_m);
        linear -= _m*(size_j*size_k);
        Index_type _j = linear / (size_k);
        j = IdxJ(_j);
        linear -= _j*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_ILMKJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_ILMKJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nm*nk*nj), stride_j(1), stride_k(nj), stride_l(nm*nk*nj), stride_m(nk*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_l*size_m*size_k*size_j);
        i = IdxI(_i);
        linear -= _i*(size_l*size_m*size_k*size_j);
        Index_type _l = linear / (size_m*size_k*size_j);
        l = IdxL(_l);
        linear -= _l*(size_m*size_k*size_j);
        Index_type _m = linear / (size_k*size_j);
        m = IdxM(_m);
        linear -= _m*(size_k*size_j);
        Index_type _k = linear / (size_j);
        k = IdxK(_k);
        linear -= _k*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IMJKL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IMJKL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nj*nk*nl), stride_j(nk*nl), stride_k(nl), stride_l(1), stride_m(nj*nk*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_m*size_j*size_k*size_l);
        i = IdxI(_i);
        linear -= _i*(size_m*size_j*size_k*size_l);
        Index_type _m = linear / (size_j*size_k*size_l);
        m = IdxM(_m);
        linear -= _m*(size_j*size_k*size_l);
        Index_type _j = linear / (size_k*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_l);
        Index_type _k = linear / (size_l);
        k = IdxK(_k);
        linear -= _k*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IMJLK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IMJLK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nj*nl*nk), stride_j(nl*nk), stride_k(1), stride_l(nk), stride_m(nj*nl*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_m*size_j*size_l*size_k);
        i = IdxI(_i);
        linear -= _i*(size_m*size_j*size_l*size_k);
        Index_type _m = linear / (size_j*size_l*size_k);
        m = IdxM(_m);
        linear -= _m*(size_j*size_l*size_k);
        Index_type _j = linear / (size_l*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_k);
        Index_type _l = linear / (size_k);
        l = IdxL(_l);
        linear -= _l*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IMKJL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IMKJL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nk*nj*nl), stride_j(nl), stride_k(nj*nl), stride_l(1), stride_m(nk*nj*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_m*size_k*size_j*size_l);
        i = IdxI(_i);
        linear -= _i*(size_m*size_k*size_j*size_l);
        Index_type _m = linear / (size_k*size_j*size_l);
        m = IdxM(_m);
        linear -= _m*(size_k*size_j*size_l);
        Index_type _k = linear / (size_j*size_l);
        k = IdxK(_k);
        linear -= _k*(size_j*size_l);
        Index_type _j = linear / (size_l);
        j = IdxJ(_j);
        linear -= _j*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IMKLJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IMKLJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nk*nl*nj), stride_j(1), stride_k(nl*nj), stride_l(nj), stride_m(nk*nl*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_m*size_k*size_l*size_j);
        i = IdxI(_i);
        linear -= _i*(size_m*size_k*size_l*size_j);
        Index_type _m = linear / (size_k*size_l*size_j);
        m = IdxM(_m);
        linear -= _m*(size_k*size_l*size_j);
        Index_type _k = linear / (size_l*size_j);
        k = IdxK(_k);
        linear -= _k*(size_l*size_j);
        Index_type _l = linear / (size_j);
        l = IdxL(_l);
        linear -= _l*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IMLJK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IMLJK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nl*nj*nk), stride_j(nk), stride_k(1), stride_l(nj*nk), stride_m(nl*nj*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_m*size_l*size_j*size_k);
        i = IdxI(_i);
        linear -= _i*(size_m*size_l*size_j*size_k);
        Index_type _m = linear / (size_l*size_j*size_k);
        m = IdxM(_m);
        linear -= _m*(size_l*size_j*size_k);
        Index_type _l = linear / (size_j*size_k);
        l = IdxL(_l);
        linear -= _l*(size_j*size_k);
        Index_type _j = linear / (size_k);
        j = IdxJ(_j);
        linear -= _j*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_IMLKJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_IMLKJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nl*nk*nj), stride_j(1), stride_k(nj), stride_l(nk*nj), stride_m(nl*nk*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _i = linear / (size_m*size_l*size_k*size_j);
        i = IdxI(_i);
        linear -= _i*(size_m*size_l*size_k*size_j);
        Index_type _m = linear / (size_l*size_k*size_j);
        m = IdxM(_m);
        linear -= _m*(size_l*size_k*size_j);
        Index_type _l = linear / (size_k*size_j);
        l = IdxL(_l);
        linear -= _l*(size_k*size_j);
        Index_type _k = linear / (size_j);
        k = IdxK(_k);
        linear -= _k*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JIKLM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JIKLM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nl*nm), stride_j(ni*nk*nl*nm), stride_k(nl*nm), stride_l(nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i*size_k*size_l*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_k*size_l*size_m);
        Index_type _i = linear / (size_k*size_l*size_m);
        i = IdxI(_i);
        linear -= _i*(size_k*size_l*size_m);
        Index_type _k = linear / (size_l*size_m);
        k = IdxK(_k);
        linear -= _k*(size_l*size_m);
        Index_type _l = linear / (size_m);
        l = IdxL(_l);
        linear -= _l*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JIKML, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JIKML Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nm*nl), stride_j(ni*nk*nm*nl), stride_k(nm*nl), stride_l(1), stride_m(nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i*size_k*size_m*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_k*size_m*size_l);
        Index_type _i = linear / (size_k*size_m*size_l);
        i = IdxI(_i);
        linear -= _i*(size_k*size_m*size_l);
        Index_type _k = linear / (size_m*size_l);
        k = IdxK(_k);
        linear -= _k*(size_m*size_l);
        Index_type _m = linear / (size_l);
        m = IdxM(_m);
        linear -= _m*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JILKM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JILKM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nk*nm), stride_j(ni*nl*nk*nm), stride_k(nm), stride_l(nk*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i*size_l*size_k*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_l*size_k*size_m);
        Index_type _i = linear / (size_l*size_k*size_m);
        i = IdxI(_i);
        linear -= _i*(size_l*size_k*size_m);
        Index_type _l = linear / (size_k*size_m);
        l = IdxL(_l);
        linear -= _l*(size_k*size_m);
        Index_type _k = linear / (size_m);
        k = IdxK(_k);
        linear -= _k*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JILMK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JILMK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nm*nk), stride_j(ni*nl*nm*nk), stride_k(1), stride_l(nm*nk), stride_m(nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i*size_l*size_m*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_l*size_m*size_k);
        Index_type _i = linear / (size_l*size_m*size_k);
        i = IdxI(_i);
        linear -= _i*(size_l*size_m*size_k);
        Index_type _l = linear / (size_m*size_k);
        l = IdxL(_l);
        linear -= _l*(size_m*size_k);
        Index_type _m = linear / (size_k);
        m = IdxM(_m);
        linear -= _m*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JIMKL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JIMKL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nk*nl), stride_j(ni*nm*nk*nl), stride_k(nl), stride_l(1), stride_m(nk*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i*size_m*size_k*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_m*size_k*size_l);
        Index_type _i = linear / (size_m*size_k*size_l);
        i = IdxI(_i);
        linear -= _i*(size_m*size_k*size_l);
        Index_type _m = linear / (size_k*size_l);
        m = IdxM(_m);
        linear -= _m*(size_k*size_l);
        Index_type _k = linear / (size_l);
        k = IdxK(_k);
        linear -= _k*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JIMLK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JIMLK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nl*nk), stride_j(ni*nm*nl*nk), stride_k(1), stride_l(nk), stride_m(nl*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_i*size_m*size_l*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_m*size_l*size_k);
        Index_type _i = linear / (size_m*size_l*size_k);
        i = IdxI(_i);
        linear -= _i*(size_m*size_l*size_k);
        Index_type _m = linear / (size_l*size_k);
        m = IdxM(_m);
        linear -= _m*(size_l*size_k);
        Index_type _l = linear / (size_k);
        l = IdxL(_l);
        linear -= _l*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JKILM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JKILM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nm), stride_j(nk*ni*nl*nm), stride_k(ni*nl*nm), stride_l(nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_k*size_i*size_l*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_i*size_l*size_m);
        Index_type _k = linear / (size_i*size_l*size_m);
        k = IdxK(_k);
        linear -= _k*(size_i*size_l*size_m);
        Index_type _i = linear / (size_l*size_m);
        i = IdxI(_i);
        linear -= _i*(size_l*size_m);
        Index_type _l = linear / (size_m);
        l = IdxL(_l);
        linear -= _l*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JKIML, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JKIML Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nl), stride_j(nk*ni*nm*nl), stride_k(ni*nm*nl), stride_l(1), stride_m(nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_k*size_i*size_m*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_i*size_m*size_l);
        Index_type _k = linear / (size_i*size_m*size_l);
        k = IdxK(_k);
        linear -= _k*(size_i*size_m*size_l);
        Index_type _i = linear / (size_m*size_l);
        i = IdxI(_i);
        linear -= _i*(size_m*size_l);
        Index_type _m = linear / (size_l);
        m = IdxM(_m);
        linear -= _m*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JKLIM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JKLIM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm), stride_j(nk*nl*ni*nm), stride_k(nl*ni*nm), stride_l(ni*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_k*size_l*size_i*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_l*size_i*size_m);
        Index_type _k = linear / (size_l*size_i*size_m);
        k = IdxK(_k);
        linear -= _k*(size_l*size_i*size_m);
        Index_type _l = linear / (size_i*size_m);
        l = IdxL(_l);
        linear -= _l*(size_i*size_m);
        Index_type _i = linear / (size_m);
        i = IdxI(_i);
        linear -= _i*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JKLMI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JKLMI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nk*nl*nm*ni), stride_k(nl*nm*ni), stride_l(nm*ni), stride_m(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_k*size_l*size_m*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_l*size_m*size_i);
        Index_type _k = linear / (size_l*size_m*size_i);
        k = IdxK(_k);
        linear -= _k*(size_l*size_m*size_i);
        Index_type _l = linear / (size_m*size_i);
        l = IdxL(_l);
        linear -= _l*(size_m*size_i);
        Index_type _m = linear / (size_i);
        m = IdxM(_m);
        linear -= _m*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JKMIL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JKMIL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl), stride_j(nk*nm*ni*nl), stride_k(nm*ni*nl), stride_l(1), stride_m(ni*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_k*size_m*size_i*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_m*size_i*size_l);
        Index_type _k = linear / (size_m*size_i*size_l);
        k = IdxK(_k);
        linear -= _k*(size_m*size_i*size_l);
        Index_type _m = linear / (size_i*size_l);
        m = IdxM(_m);
        linear -= _m*(size_i*size_l);
        Index_type _i = linear / (size_l);
        i = IdxI(_i);
        linear -= _i*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JKMLI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JKMLI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nk*nm*nl*ni), stride_k(nm*nl*ni), stride_l(ni), stride_m(nl*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_k*size_m*size_l*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_m*size_l*size_i);
        Index_type _k = linear / (size_m*size_l*size_i);
        k = IdxK(_k);
        linear -= _k*(size_m*size_l*size_i);
        Index_type _m = linear / (size_l*size_i);
        m = IdxM(_m);
        linear -= _m*(size_l*size_i);
        Index_type _l = linear / (size_i);
        l = IdxL(_l);
        linear -= _l*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JLIKM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JLIKM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nm), stride_j(nl*ni*nk*nm), stride_k(nm), stride_l(ni*nk*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_l*size_i*size_k*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_i*size_k*size_m);
        Index_type _l = linear / (size_i*size_k*size_m);
        l = IdxL(_l);
        linear -= _l*(size_i*size_k*size_m);
        Index_type _i = linear / (size_k*size_m);
        i = IdxI(_i);
        linear -= _i*(size_k*size_m);
        Index_type _k = linear / (size_m);
        k = IdxK(_k);
        linear -= _k*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JLIMK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JLIMK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nk), stride_j(nl*ni*nm*nk), stride_k(1), stride_l(ni*nm*nk), stride_m(nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_l*size_i*size_m*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_i*size_m*size_k);
        Index_type _l = linear / (size_i*size_m*size_k);
        l = IdxL(_l);
        linear -= _l*(size_i*size_m*size_k);
        Index_type _i = linear / (size_m*size_k);
        i = IdxI(_i);
        linear -= _i*(size_m*size_k);
        Index_type _m = linear / (size_k);
        m = IdxM(_m);
        linear -= _m*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JLKIM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JLKIM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm), stride_j(nl*nk*ni*nm), stride_k(ni*nm), stride_l(nk*ni*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_l*size_k*size_i*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_k*size_i*size_m);
        Index_type _l = linear / (size_k*size_i*size_m);
        l = IdxL(_l);
        linear -= _l*(size_k*size_i*size_m);
        Index_type _k = linear / (size_i*size_m);
        k = IdxK(_k);
        linear -= _k*(size_i*size_m);
        Index_type _i = linear / (size_m);
        i = IdxI(_i);
        linear -= _i*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JLKMI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JLKMI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nl*nk*nm*ni), stride_k(nm*ni), stride_l(nk*nm*ni), stride_m(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_l*size_k*size_m*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_k*size_m*size_i);
        Index_type _l = linear / (size_k*size_m*size_i);
        l = IdxL(_l);
        linear -= _l*(size_k*size_m*size_i);
        Index_type _k = linear / (size_m*size_i);
        k = IdxK(_k);
        linear -= _k*(size_m*size_i);
        Index_type _m = linear / (size_i);
        m = IdxM(_m);
        linear -= _m*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JLMIK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JLMIK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk), stride_j(nl*nm*ni*nk), stride_k(1), stride_l(nm*ni*nk), stride_m(ni*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_l*size_m*size_i*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_m*size_i*size_k);
        Index_type _l = linear / (size_m*size_i*size_k);
        l = IdxL(_l);
        linear -= _l*(size_m*size_i*size_k);
        Index_type _m = linear / (size_i*size_k);
        m = IdxM(_m);
        linear -= _m*(size_i*size_k);
        Index_type _i = linear / (size_k);
        i = IdxI(_i);
        linear -= _i*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JLMKI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JLMKI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nl*nm*nk*ni), stride_k(ni), stride_l(nm*nk*ni), stride_m(nk*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_l*size_m*size_k*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_m*size_k*size_i);
        Index_type _l = linear / (size_m*size_k*size_i);
        l = IdxL(_l);
        linear -= _l*(size_m*size_k*size_i);
        Index_type _m = linear / (size_k*size_i);
        m = IdxM(_m);
        linear -= _m*(size_k*size_i);
        Index_type _k = linear / (size_i);
        k = IdxK(_k);
        linear -= _k*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JMIKL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JMIKL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nl), stride_j(nm*ni*nk*nl), stride_k(nl), stride_l(1), stride_m(ni*nk*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_m*size_i*size_k*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_i*size_k*size_l);
        Index_type _m = linear / (size_i*size_k*size_l);
        m = IdxM(_m);
        linear -= _m*(size_i*size_k*size_l);
        Index_type _i = linear / (size_k*size_l);
        i = IdxI(_i);
        linear -= _i*(size_k*size_l);
        Index_type _k = linear / (size_l);
        k = IdxK(_k);
        linear -= _k*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JMILK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JMILK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nk), stride_j(nm*ni*nl*nk), stride_k(1), stride_l(nk), stride_m(ni*nl*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_m*size_i*size_l*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_i*size_l*size_k);
        Index_type _m = linear / (size_i*size_l*size_k);
        m = IdxM(_m);
        linear -= _m*(size_i*size_l*size_k);
        Index_type _i = linear / (size_l*size_k);
        i = IdxI(_i);
        linear -= _i*(size_l*size_k);
        Index_type _l = linear / (size_k);
        l = IdxL(_l);
        linear -= _l*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JMKIL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JMKIL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl), stride_j(nm*nk*ni*nl), stride_k(ni*nl), stride_l(1), stride_m(nk*ni*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_m*size_k*size_i*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_k*size_i*size_l);
        Index_type _m = linear / (size_k*size_i*size_l);
        m = IdxM(_m);
        linear -= _m*(size_k*size_i*size_l);
        Index_type _k = linear / (size_i*size_l);
        k = IdxK(_k);
        linear -= _k*(size_i*size_l);
        Index_type _i = linear / (size_l);
        i = IdxI(_i);
        linear -= _i*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JMKLI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JMKLI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nm*nk*nl*ni), stride_k(nl*ni), stride_l(ni), stride_m(nk*nl*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_m*size_k*size_l*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_k*size_l*size_i);
        Index_type _m = linear / (size_k*size_l*size_i);
        m = IdxM(_m);
        linear -= _m*(size_k*size_l*size_i);
        Index_type _k = linear / (size_l*size_i);
        k = IdxK(_k);
        linear -= _k*(size_l*size_i);
        Index_type _l = linear / (size_i);
        l = IdxL(_l);
        linear -= _l*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JMLIK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JMLIK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk), stride_j(nm*nl*ni*nk), stride_k(1), stride_l(ni*nk), stride_m(nl*ni*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_m*size_l*size_i*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_l*size_i*size_k);
        Index_type _m = linear / (size_l*size_i*size_k);
        m = IdxM(_m);
        linear -= _m*(size_l*size_i*size_k);
        Index_type _l = linear / (size_i*size_k);
        l = IdxL(_l);
        linear -= _l*(size_i*size_k);
        Index_type _i = linear / (size_k);
        i = IdxI(_i);
        linear -= _i*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_JMLKI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_JMLKI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nm*nl*nk*ni), stride_k(ni), stride_l(nk*ni), stride_m(nl*nk*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _j = linear / (size_m*size_l*size_k*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_l*size_k*size_i);
        Index_type _m = linear / (size_l*size_k*size_i);
        m = IdxM(_m);
        linear -= _m*(size_l*size_k*size_i);
        Index_type _l = linear / (size_k*size_i);
        l = IdxL(_l);
        linear -= _l*(size_k*size_i);
        Index_type _k = linear / (size_i);
        k = IdxK(_k);
        linear -= _k*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KIJLM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KIJLM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nl*nm), stride_j(nl*nm), stride_k(ni*nj*nl*nm), stride_l(nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_i*size_j*size_l*size_m);
        k = IdxK(_k);
        linear -= _k*(size_i*size_j*size_l*size_m);
        Index_type _i = linear / (size_j*size_l*size_m);
        i = IdxI(_i);
        linear -= _i*(size_j*size_l*size_m);
        Index_type _j = linear / (size_l*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_m);
        Index_type _l = linear / (size_m);
        l = IdxL(_l);
        linear -= _l*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KIJML, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KIJML Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nm*nl), stride_j(nm*nl), stride_k(ni*nj*nm*nl), stride_l(1), stride_m(nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_i*size_j*size_m*size_l);
        k = IdxK(_k);
        linear -= _k*(size_i*size_j*size_m*size_l);
        Index_type _i = linear / (size_j*size_m*size_l);
        i = IdxI(_i);
        linear -= _i*(size_j*size_m*size_l);
        Index_type _j = linear / (size_m*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_l);
        Index_type _m = linear / (size_l);
        m = IdxM(_m);
        linear -= _m*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KILJM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KILJM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nj*nm), stride_j(nm), stride_k(ni*nl*nj*nm), stride_l(nj*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_i*size_l*size_j*size_m);
        k = IdxK(_k);
        linear -= _k*(size_i*size_l*size_j*size_m);
        Index_type _i = linear / (size_l*size_j*size_m);
        i = IdxI(_i);
        linear -= _i*(size_l*size_j*size_m);
        Index_type _l = linear / (size_j*size_m);
        l = IdxL(_l);
        linear -= _l*(size_j*size_m);
        Index_type _j = linear / (size_m);
        j = IdxJ(_j);
        linear -= _j*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KILMJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KILMJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nm*nj), stride_j(1), stride_k(ni*nl*nm*nj), stride_l(nm*nj), stride_m(nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_i*size_l*size_m*size_j);
        k = IdxK(_k);
        linear -= _k*(size_i*size_l*size_m*size_j);
        Index_type _i = linear / (size_l*size_m*size_j);
        i = IdxI(_i);
        linear -= _i*(size_l*size_m*size_j);
        Index_type _l = linear / (size_m*size_j);
        l = IdxL(_l);
        linear -= _l*(size_m*size_j);
        Index_type _m = linear / (size_j);
        m = IdxM(_m);
        linear -= _m*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KIMJL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KIMJL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nj*nl), stride_j(nl), stride_k(ni*nm*nj*nl), stride_l(1), stride_m(nj*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_i*size_m*size_j*size_l);
        k = IdxK(_k);
        linear -= _k*(size_i*size_m*size_j*size_l);
        Index_type _i = linear / (size_m*size_j*size_l);
        i = IdxI(_i);
        linear -= _i*(size_m*size_j*size_l);
        Index_type _m = linear / (size_j*size_l);
        m = IdxM(_m);
        linear -= _m*(size_j*size_l);
        Index_type _j = linear / (size_l);
        j = IdxJ(_j);
        linear -= _j*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KIMLJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KIMLJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nl*nj), stride_j(1), stride_k(ni*nm*nl*nj), stride_l(nj), stride_m(nl*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_i*size_m*size_l*size_j);
        k = IdxK(_k);
        linear -= _k*(size_i*size_m*size_l*size_j);
        Index_type _i = linear / (size_m*size_l*size_j);
        i = IdxI(_i);
        linear -= _i*(size_m*size_l*size_j);
        Index_type _m = linear / (size_l*size_j);
        m = IdxM(_m);
        linear -= _m*(size_l*size_j);
        Index_type _l = linear / (size_j);
        l = IdxL(_l);
        linear -= _l*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KJILM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KJILM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nm), stride_j(ni*nl*nm), stride_k(nj*ni*nl*nm), stride_l(nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_j*size_i*size_l*size_m);
        k = IdxK(_k);
        linear -= _k*(size_j*size_i*size_l*size_m);
        Index_type _j = linear / (size_i*size_l*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_l*size_m);
        Index_type _i = linear / (size_l*size_m);
        i = IdxI(_i);
        linear -= _i*(size_l*size_m);
        Index_type _l = linear / (size_m);
        l = IdxL(_l);
        linear -= _l*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KJIML, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KJIML Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nl), stride_j(ni*nm*nl), stride_k(nj*ni*nm*nl), stride_l(1), stride_m(nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_j*size_i*size_m*size_l);
        k = IdxK(_k);
        linear -= _k*(size_j*size_i*size_m*size_l);
        Index_type _j = linear / (size_i*size_m*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_m*size_l);
        Index_type _i = linear / (size_m*size_l);
        i = IdxI(_i);
        linear -= _i*(size_m*size_l);
        Index_type _m = linear / (size_l);
        m = IdxM(_m);
        linear -= _m*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KJLIM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KJLIM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm), stride_j(nl*ni*nm), stride_k(nj*nl*ni*nm), stride_l(ni*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_j*size_l*size_i*size_m);
        k = IdxK(_k);
        linear -= _k*(size_j*size_l*size_i*size_m);
        Index_type _j = linear / (size_l*size_i*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_i*size_m);
        Index_type _l = linear / (size_i*size_m);
        l = IdxL(_l);
        linear -= _l*(size_i*size_m);
        Index_type _i = linear / (size_m);
        i = IdxI(_i);
        linear -= _i*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KJLMI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KJLMI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nl*nm*ni), stride_k(nj*nl*nm*ni), stride_l(nm*ni), stride_m(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_j*size_l*size_m*size_i);
        k = IdxK(_k);
        linear -= _k*(size_j*size_l*size_m*size_i);
        Index_type _j = linear / (size_l*size_m*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_m*size_i);
        Index_type _l = linear / (size_m*size_i);
        l = IdxL(_l);
        linear -= _l*(size_m*size_i);
        Index_type _m = linear / (size_i);
        m = IdxM(_m);
        linear -= _m*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KJMIL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KJMIL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl), stride_j(nm*ni*nl), stride_k(nj*nm*ni*nl), stride_l(1), stride_m(ni*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_j*size_m*size_i*size_l);
        k = IdxK(_k);
        linear -= _k*(size_j*size_m*size_i*size_l);
        Index_type _j = linear / (size_m*size_i*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_i*size_l);
        Index_type _m = linear / (size_i*size_l);
        m = IdxM(_m);
        linear -= _m*(size_i*size_l);
        Index_type _i = linear / (size_l);
        i = IdxI(_i);
        linear -= _i*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KJMLI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KJMLI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nm*nl*ni), stride_k(nj*nm*nl*ni), stride_l(ni), stride_m(nl*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_j*size_m*size_l*size_i);
        k = IdxK(_k);
        linear -= _k*(size_j*size_m*size_l*size_i);
        Index_type _j = linear / (size_m*size_l*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_l*size_i);
        Index_type _m = linear / (size_l*size_i);
        m = IdxM(_m);
        linear -= _m*(size_l*size_i);
        Index_type _l = linear / (size_i);
        l = IdxL(_l);
        linear -= _l*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KLIJM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KLIJM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nm), stride_j(nm), stride_k(nl*ni*nj*nm), stride_l(ni*nj*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_l*size_i*size_j*size_m);
        k = IdxK(_k);
        linear -= _k*(size_l*size_i*size_j*size_m);
        Index_type _l = linear / (size_i*size_j*size_m);
        l = IdxL(_l);
        linear -= _l*(size_i*size_j*size_m);
        Index_type _i = linear / (size_j*size_m);
        i = IdxI(_i);
        linear -= _i*(size_j*size_m);
        Index_type _j = linear / (size_m);
        j = IdxJ(_j);
        linear -= _j*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KLIMJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KLIMJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nj), stride_j(1), stride_k(nl*ni*nm*nj), stride_l(ni*nm*nj), stride_m(nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_l*size_i*size_m*size_j);
        k = IdxK(_k);
        linear -= _k*(size_l*size_i*size_m*size_j);
        Index_type _l = linear / (size_i*size_m*size_j);
        l = IdxL(_l);
        linear -= _l*(size_i*size_m*size_j);
        Index_type _i = linear / (size_m*size_j);
        i = IdxI(_i);
        linear -= _i*(size_m*size_j);
        Index_type _m = linear / (size_j);
        m = IdxM(_m);
        linear -= _m*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KLJIM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KLJIM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm), stride_j(ni*nm), stride_k(nl*nj*ni*nm), stride_l(nj*ni*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_l*size_j*size_i*size_m);
        k = IdxK(_k);
        linear -= _k*(size_l*size_j*size_i*size_m);
        Index_type _l = linear / (size_j*size_i*size_m);
        l = IdxL(_l);
        linear -= _l*(size_j*size_i*size_m);
        Index_type _j = linear / (size_i*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_m);
        Index_type _i = linear / (size_m);
        i = IdxI(_i);
        linear -= _i*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KLJMI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KLJMI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nm*ni), stride_k(nl*nj*nm*ni), stride_l(nj*nm*ni), stride_m(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_l*size_j*size_m*size_i);
        k = IdxK(_k);
        linear -= _k*(size_l*size_j*size_m*size_i);
        Index_type _l = linear / (size_j*size_m*size_i);
        l = IdxL(_l);
        linear -= _l*(size_j*size_m*size_i);
        Index_type _j = linear / (size_m*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_i);
        Index_type _m = linear / (size_i);
        m = IdxM(_m);
        linear -= _m*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KLMIJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KLMIJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj), stride_j(1), stride_k(nl*nm*ni*nj), stride_l(nm*ni*nj), stride_m(ni*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_l*size_m*size_i*size_j);
        k = IdxK(_k);
        linear -= _k*(size_l*size_m*size_i*size_j);
        Index_type _l = linear / (size_m*size_i*size_j);
        l = IdxL(_l);
        linear -= _l*(size_m*size_i*size_j);
        Index_type _m = linear / (size_i*size_j);
        m = IdxM(_m);
        linear -= _m*(size_i*size_j);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KLMJI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KLMJI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(ni), stride_k(nl*nm*nj*ni), stride_l(nm*nj*ni), stride_m(nj*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_l*size_m*size_j*size_i);
        k = IdxK(_k);
        linear -= _k*(size_l*size_m*size_j*size_i);
        Index_type _l = linear / (size_m*size_j*size_i);
        l = IdxL(_l);
        linear -= _l*(size_m*size_j*size_i);
        Index_type _m = linear / (size_j*size_i);
        m = IdxM(_m);
        linear -= _m*(size_j*size_i);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KMIJL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KMIJL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nl), stride_j(nl), stride_k(nm*ni*nj*nl), stride_l(1), stride_m(ni*nj*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_m*size_i*size_j*size_l);
        k = IdxK(_k);
        linear -= _k*(size_m*size_i*size_j*size_l);
        Index_type _m = linear / (size_i*size_j*size_l);
        m = IdxM(_m);
        linear -= _m*(size_i*size_j*size_l);
        Index_type _i = linear / (size_j*size_l);
        i = IdxI(_i);
        linear -= _i*(size_j*size_l);
        Index_type _j = linear / (size_l);
        j = IdxJ(_j);
        linear -= _j*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KMILJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KMILJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nj), stride_j(1), stride_k(nm*ni*nl*nj), stride_l(nj), stride_m(ni*nl*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_m*size_i*size_l*size_j);
        k = IdxK(_k);
        linear -= _k*(size_m*size_i*size_l*size_j);
        Index_type _m = linear / (size_i*size_l*size_j);
        m = IdxM(_m);
        linear -= _m*(size_i*size_l*size_j);
        Index_type _i = linear / (size_l*size_j);
        i = IdxI(_i);
        linear -= _i*(size_l*size_j);
        Index_type _l = linear / (size_j);
        l = IdxL(_l);
        linear -= _l*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KMJIL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KMJIL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl), stride_j(ni*nl), stride_k(nm*nj*ni*nl), stride_l(1), stride_m(nj*ni*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_m*size_j*size_i*size_l);
        k = IdxK(_k);
        linear -= _k*(size_m*size_j*size_i*size_l);
        Index_type _m = linear / (size_j*size_i*size_l);
        m = IdxM(_m);
        linear -= _m*(size_j*size_i*size_l);
        Index_type _j = linear / (size_i*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_l);
        Index_type _i = linear / (size_l);
        i = IdxI(_i);
        linear -= _i*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KMJLI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KMJLI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nl*ni), stride_k(nm*nj*nl*ni), stride_l(ni), stride_m(nj*nl*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_m*size_j*size_l*size_i);
        k = IdxK(_k);
        linear -= _k*(size_m*size_j*size_l*size_i);
        Index_type _m = linear / (size_j*size_l*size_i);
        m = IdxM(_m);
        linear -= _m*(size_j*size_l*size_i);
        Index_type _j = linear / (size_l*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_i);
        Index_type _l = linear / (size_i);
        l = IdxL(_l);
        linear -= _l*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KMLIJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KMLIJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj), stride_j(1), stride_k(nm*nl*ni*nj), stride_l(ni*nj), stride_m(nl*ni*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_m*size_l*size_i*size_j);
        k = IdxK(_k);
        linear -= _k*(size_m*size_l*size_i*size_j);
        Index_type _m = linear / (size_l*size_i*size_j);
        m = IdxM(_m);
        linear -= _m*(size_l*size_i*size_j);
        Index_type _l = linear / (size_i*size_j);
        l = IdxL(_l);
        linear -= _l*(size_i*size_j);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_KMLJI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_KMLJI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(ni), stride_k(nm*nl*nj*ni), stride_l(nj*ni), stride_m(nl*nj*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _k = linear / (size_m*size_l*size_j*size_i);
        k = IdxK(_k);
        linear -= _k*(size_m*size_l*size_j*size_i);
        Index_type _m = linear / (size_l*size_j*size_i);
        m = IdxM(_m);
        linear -= _m*(size_l*size_j*size_i);
        Index_type _l = linear / (size_j*size_i);
        l = IdxL(_l);
        linear -= _l*(size_j*size_i);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LIJKM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LIJKM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nk*nm), stride_j(nk*nm), stride_k(nm), stride_l(ni*nj*nk*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_i*size_j*size_k*size_m);
        l = IdxL(_l);
        linear -= _l*(size_i*size_j*size_k*size_m);
        Index_type _i = linear / (size_j*size_k*size_m);
        i = IdxI(_i);
        linear -= _i*(size_j*size_k*size_m);
        Index_type _j = linear / (size_k*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_m);
        Index_type _k = linear / (size_m);
        k = IdxK(_k);
        linear -= _k*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LIJMK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LIJMK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nm*nk), stride_j(nm*nk), stride_k(1), stride_l(ni*nj*nm*nk), stride_m(nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_i*size_j*size_m*size_k);
        l = IdxL(_l);
        linear -= _l*(size_i*size_j*size_m*size_k);
        Index_type _i = linear / (size_j*size_m*size_k);
        i = IdxI(_i);
        linear -= _i*(size_j*size_m*size_k);
        Index_type _j = linear / (size_m*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_k);
        Index_type _m = linear / (size_k);
        m = IdxM(_m);
        linear -= _m*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LIKJM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LIKJM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nj*nm), stride_j(nm), stride_k(nj*nm), stride_l(ni*nk*nj*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_i*size_k*size_j*size_m);
        l = IdxL(_l);
        linear -= _l*(size_i*size_k*size_j*size_m);
        Index_type _i = linear / (size_k*size_j*size_m);
        i = IdxI(_i);
        linear -= _i*(size_k*size_j*size_m);
        Index_type _k = linear / (size_j*size_m);
        k = IdxK(_k);
        linear -= _k*(size_j*size_m);
        Index_type _j = linear / (size_m);
        j = IdxJ(_j);
        linear -= _j*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LIKMJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LIKMJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nm*nj), stride_j(1), stride_k(nm*nj), stride_l(ni*nk*nm*nj), stride_m(nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_i*size_k*size_m*size_j);
        l = IdxL(_l);
        linear -= _l*(size_i*size_k*size_m*size_j);
        Index_type _i = linear / (size_k*size_m*size_j);
        i = IdxI(_i);
        linear -= _i*(size_k*size_m*size_j);
        Index_type _k = linear / (size_m*size_j);
        k = IdxK(_k);
        linear -= _k*(size_m*size_j);
        Index_type _m = linear / (size_j);
        m = IdxM(_m);
        linear -= _m*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LIMJK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LIMJK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nj*nk), stride_j(nk), stride_k(1), stride_l(ni*nm*nj*nk), stride_m(nj*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_i*size_m*size_j*size_k);
        l = IdxL(_l);
        linear -= _l*(size_i*size_m*size_j*size_k);
        Index_type _i = linear / (size_m*size_j*size_k);
        i = IdxI(_i);
        linear -= _i*(size_m*size_j*size_k);
        Index_type _m = linear / (size_j*size_k);
        m = IdxM(_m);
        linear -= _m*(size_j*size_k);
        Index_type _j = linear / (size_k);
        j = IdxJ(_j);
        linear -= _j*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LIMKJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LIMKJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nk*nj), stride_j(1), stride_k(nj), stride_l(ni*nm*nk*nj), stride_m(nk*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_i*size_m*size_k*size_j);
        l = IdxL(_l);
        linear -= _l*(size_i*size_m*size_k*size_j);
        Index_type _i = linear / (size_m*size_k*size_j);
        i = IdxI(_i);
        linear -= _i*(size_m*size_k*size_j);
        Index_type _m = linear / (size_k*size_j);
        m = IdxM(_m);
        linear -= _m*(size_k*size_j);
        Index_type _k = linear / (size_j);
        k = IdxK(_k);
        linear -= _k*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LJIKM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LJIKM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nm), stride_j(ni*nk*nm), stride_k(nm), stride_l(nj*ni*nk*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_j*size_i*size_k*size_m);
        l = IdxL(_l);
        linear -= _l*(size_j*size_i*size_k*size_m);
        Index_type _j = linear / (size_i*size_k*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_k*size_m);
        Index_type _i = linear / (size_k*size_m);
        i = IdxI(_i);
        linear -= _i*(size_k*size_m);
        Index_type _k = linear / (size_m);
        k = IdxK(_k);
        linear -= _k*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LJIMK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LJIMK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nk), stride_j(ni*nm*nk), stride_k(1), stride_l(nj*ni*nm*nk), stride_m(nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_j*size_i*size_m*size_k);
        l = IdxL(_l);
        linear -= _l*(size_j*size_i*size_m*size_k);
        Index_type _j = linear / (size_i*size_m*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_m*size_k);
        Index_type _i = linear / (size_m*size_k);
        i = IdxI(_i);
        linear -= _i*(size_m*size_k);
        Index_type _m = linear / (size_k);
        m = IdxM(_m);
        linear -= _m*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LJKIM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LJKIM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm), stride_j(nk*ni*nm), stride_k(ni*nm), stride_l(nj*nk*ni*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_j*size_k*size_i*size_m);
        l = IdxL(_l);
        linear -= _l*(size_j*size_k*size_i*size_m);
        Index_type _j = linear / (size_k*size_i*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_i*size_m);
        Index_type _k = linear / (size_i*size_m);
        k = IdxK(_k);
        linear -= _k*(size_i*size_m);
        Index_type _i = linear / (size_m);
        i = IdxI(_i);
        linear -= _i*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LJKMI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LJKMI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nk*nm*ni), stride_k(nm*ni), stride_l(nj*nk*nm*ni), stride_m(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_j*size_k*size_m*size_i);
        l = IdxL(_l);
        linear -= _l*(size_j*size_k*size_m*size_i);
        Index_type _j = linear / (size_k*size_m*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_m*size_i);
        Index_type _k = linear / (size_m*size_i);
        k = IdxK(_k);
        linear -= _k*(size_m*size_i);
        Index_type _m = linear / (size_i);
        m = IdxM(_m);
        linear -= _m*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LJMIK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LJMIK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk), stride_j(nm*ni*nk), stride_k(1), stride_l(nj*nm*ni*nk), stride_m(ni*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_j*size_m*size_i*size_k);
        l = IdxL(_l);
        linear -= _l*(size_j*size_m*size_i*size_k);
        Index_type _j = linear / (size_m*size_i*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_i*size_k);
        Index_type _m = linear / (size_i*size_k);
        m = IdxM(_m);
        linear -= _m*(size_i*size_k);
        Index_type _i = linear / (size_k);
        i = IdxI(_i);
        linear -= _i*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LJMKI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LJMKI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nm*nk*ni), stride_k(ni), stride_l(nj*nm*nk*ni), stride_m(nk*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_j*size_m*size_k*size_i);
        l = IdxL(_l);
        linear -= _l*(size_j*size_m*size_k*size_i);
        Index_type _j = linear / (size_m*size_k*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_k*size_i);
        Index_type _m = linear / (size_k*size_i);
        m = IdxM(_m);
        linear -= _m*(size_k*size_i);
        Index_type _k = linear / (size_i);
        k = IdxK(_k);
        linear -= _k*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LKIJM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LKIJM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nm), stride_j(nm), stride_k(ni*nj*nm), stride_l(nk*ni*nj*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_k*size_i*size_j*size_m);
        l = IdxL(_l);
        linear -= _l*(size_k*size_i*size_j*size_m);
        Index_type _k = linear / (size_i*size_j*size_m);
        k = IdxK(_k);
        linear -= _k*(size_i*size_j*size_m);
        Index_type _i = linear / (size_j*size_m);
        i = IdxI(_i);
        linear -= _i*(size_j*size_m);
        Index_type _j = linear / (size_m);
        j = IdxJ(_j);
        linear -= _j*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LKIMJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LKIMJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm*nj), stride_j(1), stride_k(ni*nm*nj), stride_l(nk*ni*nm*nj), stride_m(nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_k*size_i*size_m*size_j);
        l = IdxL(_l);
        linear -= _l*(size_k*size_i*size_m*size_j);
        Index_type _k = linear / (size_i*size_m*size_j);
        k = IdxK(_k);
        linear -= _k*(size_i*size_m*size_j);
        Index_type _i = linear / (size_m*size_j);
        i = IdxI(_i);
        linear -= _i*(size_m*size_j);
        Index_type _m = linear / (size_j);
        m = IdxM(_m);
        linear -= _m*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LKJIM, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LKJIM Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nm), stride_j(ni*nm), stride_k(nj*ni*nm), stride_l(nk*nj*ni*nm), stride_m(1)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(m));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_k*size_j*size_i*size_m);
        l = IdxL(_l);
        linear -= _l*(size_k*size_j*size_i*size_m);
        Index_type _k = linear / (size_j*size_i*size_m);
        k = IdxK(_k);
        linear -= _k*(size_j*size_i*size_m);
        Index_type _j = linear / (size_i*size_m);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_m);
        Index_type _i = linear / (size_m);
        i = IdxI(_i);
        linear -= _i*(size_m);
        m = IdxM(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LKJMI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LKJMI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nm*ni), stride_k(nj*nm*ni), stride_l(nk*nj*nm*ni), stride_m(ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_k*size_j*size_m*size_i);
        l = IdxL(_l);
        linear -= _l*(size_k*size_j*size_m*size_i);
        Index_type _k = linear / (size_j*size_m*size_i);
        k = IdxK(_k);
        linear -= _k*(size_j*size_m*size_i);
        Index_type _j = linear / (size_m*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_m*size_i);
        Index_type _m = linear / (size_i);
        m = IdxM(_m);
        linear -= _m*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LKMIJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LKMIJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj), stride_j(1), stride_k(nm*ni*nj), stride_l(nk*nm*ni*nj), stride_m(ni*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_k*size_m*size_i*size_j);
        l = IdxL(_l);
        linear -= _l*(size_k*size_m*size_i*size_j);
        Index_type _k = linear / (size_m*size_i*size_j);
        k = IdxK(_k);
        linear -= _k*(size_m*size_i*size_j);
        Index_type _m = linear / (size_i*size_j);
        m = IdxM(_m);
        linear -= _m*(size_i*size_j);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LKMJI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LKMJI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(ni), stride_k(nm*nj*ni), stride_l(nk*nm*nj*ni), stride_m(nj*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_k*size_m*size_j*size_i);
        l = IdxL(_l);
        linear -= _l*(size_k*size_m*size_j*size_i);
        Index_type _k = linear / (size_m*size_j*size_i);
        k = IdxK(_k);
        linear -= _k*(size_m*size_j*size_i);
        Index_type _m = linear / (size_j*size_i);
        m = IdxM(_m);
        linear -= _m*(size_j*size_i);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LMIJK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LMIJK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nk), stride_j(nk), stride_k(1), stride_l(nm*ni*nj*nk), stride_m(ni*nj*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_m*size_i*size_j*size_k);
        l = IdxL(_l);
        linear -= _l*(size_m*size_i*size_j*size_k);
        Index_type _m = linear / (size_i*size_j*size_k);
        m = IdxM(_m);
        linear -= _m*(size_i*size_j*size_k);
        Index_type _i = linear / (size_j*size_k);
        i = IdxI(_i);
        linear -= _i*(size_j*size_k);
        Index_type _j = linear / (size_k);
        j = IdxJ(_j);
        linear -= _j*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LMIKJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LMIKJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nj), stride_j(1), stride_k(nj), stride_l(nm*ni*nk*nj), stride_m(ni*nk*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_m*size_i*size_k*size_j);
        l = IdxL(_l);
        linear -= _l*(size_m*size_i*size_k*size_j);
        Index_type _m = linear / (size_i*size_k*size_j);
        m = IdxM(_m);
        linear -= _m*(size_i*size_k*size_j);
        Index_type _i = linear / (size_k*size_j);
        i = IdxI(_i);
        linear -= _i*(size_k*size_j);
        Index_type _k = linear / (size_j);
        k = IdxK(_k);
        linear -= _k*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LMJIK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LMJIK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk), stride_j(ni*nk), stride_k(1), stride_l(nm*nj*ni*nk), stride_m(nj*ni*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_m*size_j*size_i*size_k);
        l = IdxL(_l);
        linear -= _l*(size_m*size_j*size_i*size_k);
        Index_type _m = linear / (size_j*size_i*size_k);
        m = IdxM(_m);
        linear -= _m*(size_j*size_i*size_k);
        Index_type _j = linear / (size_i*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_k);
        Index_type _i = linear / (size_k);
        i = IdxI(_i);
        linear -= _i*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LMJKI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LMJKI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nk*ni), stride_k(ni), stride_l(nm*nj*nk*ni), stride_m(nj*nk*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_m*size_j*size_k*size_i);
        l = IdxL(_l);
        linear -= _l*(size_m*size_j*size_k*size_i);
        Index_type _m = linear / (size_j*size_k*size_i);
        m = IdxM(_m);
        linear -= _m*(size_j*size_k*size_i);
        Index_type _j = linear / (size_k*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_i);
        Index_type _k = linear / (size_i);
        k = IdxK(_k);
        linear -= _k*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LMKIJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LMKIJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj), stride_j(1), stride_k(ni*nj), stride_l(nm*nk*ni*nj), stride_m(nk*ni*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_m*size_k*size_i*size_j);
        l = IdxL(_l);
        linear -= _l*(size_m*size_k*size_i*size_j);
        Index_type _m = linear / (size_k*size_i*size_j);
        m = IdxM(_m);
        linear -= _m*(size_k*size_i*size_j);
        Index_type _k = linear / (size_i*size_j);
        k = IdxK(_k);
        linear -= _k*(size_i*size_j);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_LMKJI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_LMKJI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(ni), stride_k(nj*ni), stride_l(nm*nk*nj*ni), stride_m(nk*nj*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _l = linear / (size_m*size_k*size_j*size_i);
        l = IdxL(_l);
        linear -= _l*(size_m*size_k*size_j*size_i);
        Index_type _m = linear / (size_k*size_j*size_i);
        m = IdxM(_m);
        linear -= _m*(size_k*size_j*size_i);
        Index_type _k = linear / (size_j*size_i);
        k = IdxK(_k);
        linear -= _k*(size_j*size_i);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MIJKL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MIJKL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nk*nl), stride_j(nk*nl), stride_k(nl), stride_l(1), stride_m(ni*nj*nk*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_i*size_j*size_k*size_l);
        m = IdxM(_m);
        linear -= _m*(size_i*size_j*size_k*size_l);
        Index_type _i = linear / (size_j*size_k*size_l);
        i = IdxI(_i);
        linear -= _i*(size_j*size_k*size_l);
        Index_type _j = linear / (size_k*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_l);
        Index_type _k = linear / (size_l);
        k = IdxK(_k);
        linear -= _k*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MIJLK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MIJLK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nl*nk), stride_j(nl*nk), stride_k(1), stride_l(nk), stride_m(ni*nj*nl*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_i*size_j*size_l*size_k);
        m = IdxM(_m);
        linear -= _m*(size_i*size_j*size_l*size_k);
        Index_type _i = linear / (size_j*size_l*size_k);
        i = IdxI(_i);
        linear -= _i*(size_j*size_l*size_k);
        Index_type _j = linear / (size_l*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_k);
        Index_type _l = linear / (size_k);
        l = IdxL(_l);
        linear -= _l*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MIKJL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MIKJL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nj*nl), stride_j(nl), stride_k(nj*nl), stride_l(1), stride_m(ni*nk*nj*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_i*size_k*size_j*size_l);
        m = IdxM(_m);
        linear -= _m*(size_i*size_k*size_j*size_l);
        Index_type _i = linear / (size_k*size_j*size_l);
        i = IdxI(_i);
        linear -= _i*(size_k*size_j*size_l);
        Index_type _k = linear / (size_j*size_l);
        k = IdxK(_k);
        linear -= _k*(size_j*size_l);
        Index_type _j = linear / (size_l);
        j = IdxJ(_j);
        linear -= _j*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MIKLJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MIKLJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nl*nj), stride_j(1), stride_k(nl*nj), stride_l(nj), stride_m(ni*nk*nl*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_i*size_k*size_l*size_j);
        m = IdxM(_m);
        linear -= _m*(size_i*size_k*size_l*size_j);
        Index_type _i = linear / (size_k*size_l*size_j);
        i = IdxI(_i);
        linear -= _i*(size_k*size_l*size_j);
        Index_type _k = linear / (size_l*size_j);
        k = IdxK(_k);
        linear -= _k*(size_l*size_j);
        Index_type _l = linear / (size_j);
        l = IdxL(_l);
        linear -= _l*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MILJK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MILJK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nj*nk), stride_j(nk), stride_k(1), stride_l(nj*nk), stride_m(ni*nl*nj*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_i*size_l*size_j*size_k);
        m = IdxM(_m);
        linear -= _m*(size_i*size_l*size_j*size_k);
        Index_type _i = linear / (size_l*size_j*size_k);
        i = IdxI(_i);
        linear -= _i*(size_l*size_j*size_k);
        Index_type _l = linear / (size_j*size_k);
        l = IdxL(_l);
        linear -= _l*(size_j*size_k);
        Index_type _j = linear / (size_k);
        j = IdxJ(_j);
        linear -= _j*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MILKJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MILKJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nk*nj), stride_j(1), stride_k(nj), stride_l(nk*nj), stride_m(ni*nl*nk*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_i*size_l*size_k*size_j);
        m = IdxM(_m);
        linear -= _m*(size_i*size_l*size_k*size_j);
        Index_type _i = linear / (size_l*size_k*size_j);
        i = IdxI(_i);
        linear -= _i*(size_l*size_k*size_j);
        Index_type _l = linear / (size_k*size_j);
        l = IdxL(_l);
        linear -= _l*(size_k*size_j);
        Index_type _k = linear / (size_j);
        k = IdxK(_k);
        linear -= _k*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MJIKL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MJIKL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nl), stride_j(ni*nk*nl), stride_k(nl), stride_l(1), stride_m(nj*ni*nk*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_j*size_i*size_k*size_l);
        m = IdxM(_m);
        linear -= _m*(size_j*size_i*size_k*size_l);
        Index_type _j = linear / (size_i*size_k*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_k*size_l);
        Index_type _i = linear / (size_k*size_l);
        i = IdxI(_i);
        linear -= _i*(size_k*size_l);
        Index_type _k = linear / (size_l);
        k = IdxK(_k);
        linear -= _k*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MJILK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MJILK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nk), stride_j(ni*nl*nk), stride_k(1), stride_l(nk), stride_m(nj*ni*nl*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_j*size_i*size_l*size_k);
        m = IdxM(_m);
        linear -= _m*(size_j*size_i*size_l*size_k);
        Index_type _j = linear / (size_i*size_l*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_l*size_k);
        Index_type _i = linear / (size_l*size_k);
        i = IdxI(_i);
        linear -= _i*(size_l*size_k);
        Index_type _l = linear / (size_k);
        l = IdxL(_l);
        linear -= _l*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MJKIL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MJKIL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl), stride_j(nk*ni*nl), stride_k(ni*nl), stride_l(1), stride_m(nj*nk*ni*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_j*size_k*size_i*size_l);
        m = IdxM(_m);
        linear -= _m*(size_j*size_k*size_i*size_l);
        Index_type _j = linear / (size_k*size_i*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_i*size_l);
        Index_type _k = linear / (size_i*size_l);
        k = IdxK(_k);
        linear -= _k*(size_i*size_l);
        Index_type _i = linear / (size_l);
        i = IdxI(_i);
        linear -= _i*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MJKLI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MJKLI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nk*nl*ni), stride_k(nl*ni), stride_l(ni), stride_m(nj*nk*nl*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_j*size_k*size_l*size_i);
        m = IdxM(_m);
        linear -= _m*(size_j*size_k*size_l*size_i);
        Index_type _j = linear / (size_k*size_l*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_l*size_i);
        Index_type _k = linear / (size_l*size_i);
        k = IdxK(_k);
        linear -= _k*(size_l*size_i);
        Index_type _l = linear / (size_i);
        l = IdxL(_l);
        linear -= _l*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MJLIK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MJLIK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk), stride_j(nl*ni*nk), stride_k(1), stride_l(ni*nk), stride_m(nj*nl*ni*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_j*size_l*size_i*size_k);
        m = IdxM(_m);
        linear -= _m*(size_j*size_l*size_i*size_k);
        Index_type _j = linear / (size_l*size_i*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_i*size_k);
        Index_type _l = linear / (size_i*size_k);
        l = IdxL(_l);
        linear -= _l*(size_i*size_k);
        Index_type _i = linear / (size_k);
        i = IdxI(_i);
        linear -= _i*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MJLKI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MJLKI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nl*nk*ni), stride_k(ni), stride_l(nk*ni), stride_m(nj*nl*nk*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_j*size_l*size_k*size_i);
        m = IdxM(_m);
        linear -= _m*(size_j*size_l*size_k*size_i);
        Index_type _j = linear / (size_l*size_k*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_k*size_i);
        Index_type _l = linear / (size_k*size_i);
        l = IdxL(_l);
        linear -= _l*(size_k*size_i);
        Index_type _k = linear / (size_i);
        k = IdxK(_k);
        linear -= _k*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MKIJL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MKIJL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nl), stride_j(nl), stride_k(ni*nj*nl), stride_l(1), stride_m(nk*ni*nj*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_k*size_i*size_j*size_l);
        m = IdxM(_m);
        linear -= _m*(size_k*size_i*size_j*size_l);
        Index_type _k = linear / (size_i*size_j*size_l);
        k = IdxK(_k);
        linear -= _k*(size_i*size_j*size_l);
        Index_type _i = linear / (size_j*size_l);
        i = IdxI(_i);
        linear -= _i*(size_j*size_l);
        Index_type _j = linear / (size_l);
        j = IdxJ(_j);
        linear -= _j*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MKILJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MKILJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl*nj), stride_j(1), stride_k(ni*nl*nj), stride_l(nj), stride_m(nk*ni*nl*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_k*size_i*size_l*size_j);
        m = IdxM(_m);
        linear -= _m*(size_k*size_i*size_l*size_j);
        Index_type _k = linear / (size_i*size_l*size_j);
        k = IdxK(_k);
        linear -= _k*(size_i*size_l*size_j);
        Index_type _i = linear / (size_l*size_j);
        i = IdxI(_i);
        linear -= _i*(size_l*size_j);
        Index_type _l = linear / (size_j);
        l = IdxL(_l);
        linear -= _l*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MKJIL, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MKJIL Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nl), stride_j(ni*nl), stride_k(nj*ni*nl), stride_l(1), stride_m(nk*nj*ni*nl)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(l));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_k*size_j*size_i*size_l);
        m = IdxM(_m);
        linear -= _m*(size_k*size_j*size_i*size_l);
        Index_type _k = linear / (size_j*size_i*size_l);
        k = IdxK(_k);
        linear -= _k*(size_j*size_i*size_l);
        Index_type _j = linear / (size_i*size_l);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_l);
        Index_type _i = linear / (size_l);
        i = IdxI(_i);
        linear -= _i*(size_l);
        l = IdxL(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MKJLI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MKJLI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nl*ni), stride_k(nj*nl*ni), stride_l(ni), stride_m(nk*nj*nl*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_k*size_j*size_l*size_i);
        m = IdxM(_m);
        linear -= _m*(size_k*size_j*size_l*size_i);
        Index_type _k = linear / (size_j*size_l*size_i);
        k = IdxK(_k);
        linear -= _k*(size_j*size_l*size_i);
        Index_type _j = linear / (size_l*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_l*size_i);
        Index_type _l = linear / (size_i);
        l = IdxL(_l);
        linear -= _l*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MKLIJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MKLIJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj), stride_j(1), stride_k(nl*ni*nj), stride_l(ni*nj), stride_m(nk*nl*ni*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_k*size_l*size_i*size_j);
        m = IdxM(_m);
        linear -= _m*(size_k*size_l*size_i*size_j);
        Index_type _k = linear / (size_l*size_i*size_j);
        k = IdxK(_k);
        linear -= _k*(size_l*size_i*size_j);
        Index_type _l = linear / (size_i*size_j);
        l = IdxL(_l);
        linear -= _l*(size_i*size_j);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MKLJI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MKLJI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(ni), stride_k(nl*nj*ni), stride_l(nj*ni), stride_m(nk*nl*nj*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_k*size_l*size_j*size_i);
        m = IdxM(_m);
        linear -= _m*(size_k*size_l*size_j*size_i);
        Index_type _k = linear / (size_l*size_j*size_i);
        k = IdxK(_k);
        linear -= _k*(size_l*size_j*size_i);
        Index_type _l = linear / (size_j*size_i);
        l = IdxL(_l);
        linear -= _l*(size_j*size_i);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MLIJK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MLIJK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj*nk), stride_j(nk), stride_k(1), stride_l(ni*nj*nk), stride_m(nl*ni*nj*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_l*size_i*size_j*size_k);
        m = IdxM(_m);
        linear -= _m*(size_l*size_i*size_j*size_k);
        Index_type _l = linear / (size_i*size_j*size_k);
        l = IdxL(_l);
        linear -= _l*(size_i*size_j*size_k);
        Index_type _i = linear / (size_j*size_k);
        i = IdxI(_i);
        linear -= _i*(size_j*size_k);
        Index_type _j = linear / (size_k);
        j = IdxJ(_j);
        linear -= _j*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MLIKJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MLIKJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk*nj), stride_j(1), stride_k(nj), stride_l(ni*nk*nj), stride_m(nl*ni*nk*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_l*size_i*size_k*size_j);
        m = IdxM(_m);
        linear -= _m*(size_l*size_i*size_k*size_j);
        Index_type _l = linear / (size_i*size_k*size_j);
        l = IdxL(_l);
        linear -= _l*(size_i*size_k*size_j);
        Index_type _i = linear / (size_k*size_j);
        i = IdxI(_i);
        linear -= _i*(size_k*size_j);
        Index_type _k = linear / (size_j);
        k = IdxK(_k);
        linear -= _k*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MLJIK, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MLJIK Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nk), stride_j(ni*nk), stride_k(1), stride_l(nj*ni*nk), stride_m(nl*nj*ni*nk)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(k));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_l*size_j*size_i*size_k);
        m = IdxM(_m);
        linear -= _m*(size_l*size_j*size_i*size_k);
        Index_type _l = linear / (size_j*size_i*size_k);
        l = IdxL(_l);
        linear -= _l*(size_j*size_i*size_k);
        Index_type _j = linear / (size_i*size_k);
        j = IdxJ(_j);
        linear -= _j*(size_i*size_k);
        Index_type _i = linear / (size_k);
        i = IdxI(_i);
        linear -= _i*(size_k);
        k = IdxK(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MLJKI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MLJKI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(nk*ni), stride_k(ni), stride_l(nj*nk*ni), stride_m(nl*nj*nk*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_l*size_j*size_k*size_i);
        m = IdxM(_m);
        linear -= _m*(size_l*size_j*size_k*size_i);
        Index_type _l = linear / (size_j*size_k*size_i);
        l = IdxL(_l);
        linear -= _l*(size_j*size_k*size_i);
        Index_type _j = linear / (size_k*size_i);
        j = IdxJ(_j);
        linear -= _j*(size_k*size_i);
        Index_type _k = linear / (size_i);
        k = IdxK(_k);
        linear -= _k*(size_i);
        i = IdxI(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MLKIJ, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MLKIJ Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(nj), stride_j(1), stride_k(ni*nj), stride_l(nk*ni*nj), stride_m(nl*nk*ni*nj)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(i)*stride_i + convertIndex<Index_type>(j));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_l*size_k*size_i*size_j);
        m = IdxM(_m);
        linear -= _m*(size_l*size_k*size_i*size_j);
        Index_type _l = linear / (size_k*size_i*size_j);
        l = IdxL(_l);
        linear -= _l*(size_k*size_i*size_j);
        Index_type _k = linear / (size_i*size_j);
        k = IdxK(_k);
        linear -= _k*(size_i*size_j);
        Index_type _i = linear / (size_j);
        i = IdxI(_i);
        linear -= _i*(size_j);
        j = IdxJ(linear);
      }
    };

  template<typename IdxLin, typename IdxI, typename IdxJ, typename IdxK, typename IdxL, typename IdxM>
    struct Layout<IdxLin, PERM_MLKJI, IdxI, IdxJ, IdxK, IdxL, IdxM> {
      typedef PERM_MLKJI Permutation;
      typedef IdxLin IndexLinear;
      typedef IdxI IndexI;
      typedef IdxJ IndexJ;
      typedef IdxK IndexK;
      typedef IdxL IndexL;
      typedef IdxM IndexM;

      Index_type const size_i;
      Index_type const size_j;
      Index_type const size_k;
      Index_type const size_l;
      Index_type const size_m;

      Index_type const stride_i;
      Index_type const stride_j;
      Index_type const stride_k;
      Index_type const stride_l;
      Index_type const stride_m;

      inline __attribute__((always_inline)) constexpr Layout(Index_type ni, Index_type nj, Index_type nk, Index_type nl, Index_type nm):
        size_i(ni), size_j(nj), size_k(nk), size_l(nl), size_m(nm), stride_i(1), stride_j(ni), stride_k(nj*ni), stride_l(nk*nj*ni), stride_m(nl*nk*nj*ni)
      {}

      inline __attribute__((always_inline)) constexpr IdxLin operator()(IdxI i, IdxJ j, IdxK k, IdxL l, IdxM m) const {
        return convertIndex<IdxLin>(convertIndex<Index_type>(m)*stride_m + convertIndex<Index_type>(l)*stride_l + convertIndex<Index_type>(k)*stride_k + convertIndex<Index_type>(j)*stride_j + convertIndex<Index_type>(i));
      }

      inline __attribute__((always_inline)) void toIndices(IdxLin lin, IdxI &i, IdxJ &j, IdxK &k, IdxL &l, IdxM &m) const {
        Index_type linear = convertIndex<Index_type>(lin);
        Index_type _m = linear / (size_l*size_k*size_j*size_i);
        m = IdxM(_m);
        linear -= _m*(size_l*size_k*size_j*size_i);
        Index_type _l = linear / (size_k*size_j*size_i);
        l = IdxL(_l);
        linear -= _l*(size_k*size_j*size_i);
        Index_type _k = linear / (size_j*size_i);
        k = IdxK(_k);
        linear -= _k*(size_j*size_i);
        Index_type _j = linear / (size_i);
        j = IdxJ(_j);
        linear -= _j*(size_i);
        i = IdxI(linear);
      }
    };

}
namespace RAJA {

  template<typename DataType, typename LayoutT>
    struct View {
      LayoutT const layout;
      DataType *data;

      template<typename ...Args>

        inline __attribute__((always_inline))
        constexpr View(DataType *data_ptr, Args... dim_sizes )
        : layout(dim_sizes...), data(data_ptr)
        { }

      template<typename ...Args>

        inline __attribute__((always_inline))
        DataType &operator()(Args... args) const {
          return data[convertIndex<Index_type>(layout(args...))];
        }
    };

}
namespace RAJA {
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall(Index_type begin, Index_type end,
                 LOOP_BODY loop_body)
             {
               forall( EXEC_POLICY_T(),
                   begin, end,
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount(Index_type begin, Index_type end,
                 Index_type icount,
                 LOOP_BODY loop_body)
             {
               forall_Icount( EXEC_POLICY_T(),
                   begin, end,
                   icount,
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))  // Liao, the original template declaration to be specialized
             void forall(const RangeSegment& iseg,
                 LOOP_BODY loop_body)
             {
               forall( EXEC_POLICY_T(),
                   iseg.getBegin(), iseg.getEnd(),
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount(const RangeSegment& iseg,
                 Index_type icount,
                 LOOP_BODY loop_body)
             {
               forall_Icount( EXEC_POLICY_T(),
                   iseg.getBegin(), iseg.getEnd(),
                   icount,
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall(Index_type begin, Index_type end,
                 Index_type stride,
                 LOOP_BODY loop_body)
             {
               forall( EXEC_POLICY_T(),
                   begin, end, stride,
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount(Index_type begin, Index_type end,
                 Index_type stride,
                 Index_type icount,
                 LOOP_BODY loop_body)
             {
               forall_Icount( EXEC_POLICY_T(),
                   begin, end, stride,
                   icount,
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall(const RangeStrideSegment& iseg,
                 LOOP_BODY loop_body)
             {
               forall( EXEC_POLICY_T(),
                   iseg.getBegin(), iseg.getEnd(), iseg.getStride(),
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount(const RangeStrideSegment& iseg,
                 Index_type icount,
                 LOOP_BODY loop_body)
             {
               forall_Icount( EXEC_POLICY_T(),
                   iseg.getBegin(), iseg.getEnd(), iseg.getStride(),
                   icount,
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall(const Index_type* idx, Index_type len,
                 LOOP_BODY loop_body)
             {
               forall( EXEC_POLICY_T(),
                   idx, len,
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount(const Index_type* idx, Index_type len,
                 Index_type icount,
                 LOOP_BODY loop_body)
             {
               forall_Icount( EXEC_POLICY_T(),
                   idx, len,
                   icount,
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall(const ListSegment& iseg,
                 LOOP_BODY loop_body)
             {
               forall( EXEC_POLICY_T(),
                   iseg.getIndex(), iseg.getLength(),
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount(const ListSegment& iseg,
                 Index_type icount,
                 LOOP_BODY loop_body)
             {
               forall_Icount( EXEC_POLICY_T(),
                   iseg.getIndex(), iseg.getLength(),
                   icount,
                   loop_body );
             }
  template <typename EXEC_POLICY_T,
           typename INDEXSET_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall(const INDEXSET_T& iset, LOOP_BODY loop_body)
             {
               forall(EXEC_POLICY_T(),
                   iset, loop_body);
             }
  template <typename EXEC_POLICY_T,
           typename INDEXSET_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount(const INDEXSET_T& iset, LOOP_BODY loop_body)
             {
               forall_Icount(EXEC_POLICY_T(),
                   iset, loop_body);
             }
  template <typename EXEC_POLICY_T,
           typename INDEXSET_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount(const INDEXSET_T& iset,
                 Index_type icount,
                 LOOP_BODY loop_body)
             {
               forall_Icount(EXEC_POLICY_T(),
                   iset,
                   icount,
                   loop_body);
             }
  template <typename EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_segments(const IndexSet& iset,
                 LOOP_BODY loop_body)
             {
               forall_segments(EXEC_POLICY_T(),
                   iset,
                   loop_body);
             }

}

namespace RAJA {

  template<typename P, typename I>
    struct ForallN_PolicyPair : public I {

      typedef P POLICY;
      typedef I ISET;

      inline __attribute__((always_inline))
        explicit
        constexpr
        ForallN_PolicyPair(ISET const &i) : ISET(i) {}
    };

  template<typename ... PLIST>
    struct ExecList{
      constexpr const static size_t num_loops = sizeof...(PLIST);
      typedef std::tuple<PLIST...> tuple;
    };

  struct ForallN_Execute_Tag {};

  struct Execute {
    typedef ForallN_Execute_Tag PolicyTag;
  };

  template<typename EXEC, typename NEXT=Execute>
    struct NestedPolicy {
      typedef NEXT NextPolicy;
      typedef EXEC ExecPolicies;
    };
  template<typename BODY, typename INDEX_TYPE=Index_type>
    struct ForallN_BindFirstArg_HostDevice {
      BODY const body;
      INDEX_TYPE const i;

      inline __attribute__((always_inline))
        constexpr
        ForallN_BindFirstArg_HostDevice(BODY b, INDEX_TYPE i0) : body(b), i(i0) {}

      template<typename ... ARGS>
        inline __attribute__((always_inline))

        void operator()(ARGS ... args) const {
          body(i, args...);
        }
    };

  template<typename BODY, typename INDEX_TYPE=Index_type>
    struct ForallN_BindFirstArg_Host {

      BODY const body;
      INDEX_TYPE const i;

      inline __attribute__((always_inline))
        constexpr
        ForallN_BindFirstArg_Host(BODY const &b, INDEX_TYPE i0) : body(b), i(i0) {}

      template<typename ... ARGS>
        inline __attribute__((always_inline))
        void operator()(ARGS ... args) const {
          body(i, args...);
        }
    };

  template<typename NextExec, typename BODY>
    struct ForallN_PeelOuter {

      NextExec const next_exec;
      BODY const body;

      inline __attribute__((always_inline))
        constexpr
        ForallN_PeelOuter(NextExec const &ne, BODY const &b) : next_exec(ne), body(b) {}

      inline __attribute__((always_inline))
        void operator()(Index_type i) const {
          ForallN_BindFirstArg_HostDevice<BODY> inner(body, i);
          next_exec(inner);
        }

      inline __attribute__((always_inline))
        void operator()(Index_type i, Index_type j) const {
          ForallN_BindFirstArg_HostDevice<BODY> inner_i(body, i);
          ForallN_BindFirstArg_HostDevice<decltype(inner_i)> inner_j(inner_i, j);
          next_exec(inner_j);
        }

      inline __attribute__((always_inline))
        void operator()(Index_type i, Index_type j, Index_type k) const {
          ForallN_BindFirstArg_HostDevice<BODY> inner_i(body, i);
          ForallN_BindFirstArg_HostDevice<decltype(inner_i)> inner_j(inner_i, j);
          ForallN_BindFirstArg_HostDevice<decltype(inner_j)> inner_k(inner_j, k);
          next_exec(inner_k);
        }
    };

  template<typename ... PREST>
    struct ForallN_Executor {};
  template<typename PI, typename ... PREST>
    struct ForallN_Executor<PI, PREST...> {
      typedef typename PI::ISET TI;
      typedef typename PI::POLICY POLICY_I;

      typedef ForallN_Executor<PREST...> NextExec;

      PI const is_i;
      NextExec const next_exec;

      template<typename ... TREST>
        constexpr
        ForallN_Executor(PI const &is_i0, TREST const &... is_rest) : is_i(is_i0), next_exec(is_rest...) {}

      template<typename BODY>
        inline __attribute__((always_inline))
        void operator()(BODY const &body) const {
          ForallN_PeelOuter<NextExec, BODY> outer(next_exec, body);
          RAJA::forall<POLICY_I>(is_i, outer);
        }
    };

  template<>
    struct ForallN_Executor<> {
      constexpr
        ForallN_Executor() {}

      template<typename BODY>
        inline __attribute__((always_inline)) void operator()(BODY const &body) const {
          body();
        }
    };

  template<typename POLICY, typename BODY, typename ... ARGS>
    inline __attribute__((always_inline))
    void forallN_policy(ForallN_Execute_Tag, BODY const &body, ARGS const &... args){

      ForallN_Executor<ARGS...> exec(args...);

      exec(body);
    }

  template<typename BODY, typename ... Idx>
    struct ForallN_IndexTypeConverter {

      inline __attribute__((always_inline))

        constexpr
        explicit ForallN_IndexTypeConverter(BODY const &b) : body(b) {}

      template<typename ... ARGS>
        inline __attribute__((always_inline))

        void operator()(ARGS ... arg) const {
          body(Idx(arg)...);
        }

      BODY const &body;

    };

}

namespace RAJA {

  template<typename POLICY, typename IdxI=Index_type, typename TI, typename BODY>
    inline __attribute__((always_inline))
    void forallN(TI const &is_i, BODY const &body){

      typedef typename POLICY::NextPolicy NextPolicy;
      typedef typename POLICY::NextPolicy::PolicyTag NextPolicyTag;

      using ExecPolicies = typename POLICY::ExecPolicies;
      using PolicyI = typename std::tuple_element<0, typename ExecPolicies::tuple>::type;

      typedef ForallN_IndexTypeConverter<BODY, IdxI> IDX_CONV;

      forallN_policy<NextPolicy, IDX_CONV>(NextPolicyTag(), IDX_CONV(body),
          ForallN_PolicyPair<PolicyI, TI>(is_i));
    }

  template<typename POLICY, typename IdxI=Index_type, typename IdxJ=Index_type, typename TI, typename TJ, typename BODY>
    inline __attribute__((always_inline))
    void forallN(TI const &is_i, TJ const &is_j, BODY const &body){

      typedef typename POLICY::NextPolicy NextPolicy;
      typedef typename POLICY::NextPolicy::PolicyTag NextPolicyTag;

      using ExecPolicies = typename POLICY::ExecPolicies;
      using PolicyI = typename std::tuple_element<0, typename ExecPolicies::tuple>::type;
      using PolicyJ = typename std::tuple_element<1, typename ExecPolicies::tuple>::type;

      typedef ForallN_IndexTypeConverter<BODY, IdxI, IdxJ> IDX_CONV;

      forallN_policy<NextPolicy, IDX_CONV>(NextPolicyTag(), IDX_CONV(body),
          ForallN_PolicyPair<PolicyI, TI>(is_i),
          ForallN_PolicyPair<PolicyJ, TJ>(is_j));
    }

  template<typename POLICY, typename IdxI=Index_type, typename IdxJ=Index_type, typename IdxK=Index_type, typename TI, typename TJ, typename TK, typename BODY>
    inline __attribute__((always_inline))
    void forallN(TI const &is_i, TJ const &is_j, TK const &is_k, BODY const &body){

      typedef typename POLICY::NextPolicy NextPolicy;
      typedef typename POLICY::NextPolicy::PolicyTag NextPolicyTag;

      using ExecPolicies = typename POLICY::ExecPolicies;
      using PolicyI = typename std::tuple_element<0, typename ExecPolicies::tuple>::type;
      using PolicyJ = typename std::tuple_element<1, typename ExecPolicies::tuple>::type;
      using PolicyK = typename std::tuple_element<2, typename ExecPolicies::tuple>::type;

      typedef ForallN_IndexTypeConverter<BODY, IdxI, IdxJ, IdxK> IDX_CONV;

      forallN_policy<NextPolicy, IDX_CONV>(NextPolicyTag(), IDX_CONV(body),
          ForallN_PolicyPair<PolicyI, TI>(is_i),
          ForallN_PolicyPair<PolicyJ, TJ>(is_j),
          ForallN_PolicyPair<PolicyK, TK>(is_k));
    }

  template<typename POLICY, typename IdxI=Index_type, typename IdxJ=Index_type, typename IdxK=Index_type, typename IdxL=Index_type, typename TI, typename TJ, typename TK, typename TL, typename BODY>
    inline __attribute__((always_inline))
    void forallN(TI const &is_i, TJ const &is_j, TK const &is_k, TL const &is_l, BODY const &body){

      typedef typename POLICY::NextPolicy NextPolicy;
      typedef typename POLICY::NextPolicy::PolicyTag NextPolicyTag;

      using ExecPolicies = typename POLICY::ExecPolicies;
      using PolicyI = typename std::tuple_element<0, typename ExecPolicies::tuple>::type;
      using PolicyJ = typename std::tuple_element<1, typename ExecPolicies::tuple>::type;
      using PolicyK = typename std::tuple_element<2, typename ExecPolicies::tuple>::type;
      using PolicyL = typename std::tuple_element<3, typename ExecPolicies::tuple>::type;

      typedef ForallN_IndexTypeConverter<BODY, IdxI, IdxJ, IdxK, IdxL> IDX_CONV;

      forallN_policy<NextPolicy, IDX_CONV>(NextPolicyTag(), IDX_CONV(body),
          ForallN_PolicyPair<PolicyI, TI>(is_i),
          ForallN_PolicyPair<PolicyJ, TJ>(is_j),
          ForallN_PolicyPair<PolicyK, TK>(is_k),
          ForallN_PolicyPair<PolicyL, TL>(is_l));
    }

  template<typename POLICY, typename IdxI=Index_type, typename IdxJ=Index_type, typename IdxK=Index_type, typename IdxL=Index_type, typename IdxM=Index_type, typename TI, typename TJ, typename TK, typename TL, typename TM, typename BODY>
    inline __attribute__((always_inline))
    void forallN(TI const &is_i, TJ const &is_j, TK const &is_k, TL const &is_l, TM const &is_m, BODY const &body){

      typedef typename POLICY::NextPolicy NextPolicy;
      typedef typename POLICY::NextPolicy::PolicyTag NextPolicyTag;

      using ExecPolicies = typename POLICY::ExecPolicies;
      using PolicyI = typename std::tuple_element<0, typename ExecPolicies::tuple>::type;
      using PolicyJ = typename std::tuple_element<1, typename ExecPolicies::tuple>::type;
      using PolicyK = typename std::tuple_element<2, typename ExecPolicies::tuple>::type;
      using PolicyL = typename std::tuple_element<3, typename ExecPolicies::tuple>::type;
      using PolicyM = typename std::tuple_element<4, typename ExecPolicies::tuple>::type;

      typedef ForallN_IndexTypeConverter<BODY, IdxI, IdxJ, IdxK, IdxL, IdxM> IDX_CONV;

      forallN_policy<NextPolicy, IDX_CONV>(NextPolicyTag(), IDX_CONV(body),
          ForallN_PolicyPair<PolicyI, TI>(is_i),
          ForallN_PolicyPair<PolicyJ, TJ>(is_j),
          ForallN_PolicyPair<PolicyK, TK>(is_k),
          ForallN_PolicyPair<PolicyL, TL>(is_l),
          ForallN_PolicyPair<PolicyM, TM>(is_m));
    }

}
namespace RAJA {
  struct seq_exec {};

  struct seq_segit {};
  struct seq_reduce {};

}

namespace RAJA {

  typedef double CPUReductionBlockDataType;
  int getCPUReductionId();
  void releaseCPUReductionId(int id);
  CPUReductionBlockDataType* getCPUReductionMemBlock(int id);
  void freeCPUReductionMemBlock();
  Index_type* getCPUReductionLocBlock(int id);
  void freeCPUReductionLocBlock();

}

namespace RAJA {
  template <typename T>
    class ReduceMin<seq_reduce, T>
    {
      public:

        explicit ReduceMin(T init_val)
        {
          m_is_copy = false;

          m_reduced_val = init_val;

          m_myID = getCPUReductionId();

          m_blockdata = getCPUReductionMemBlock(m_myID);

          m_blockdata[0] = init_val;
        }

        ReduceMin( const ReduceMin<seq_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceMin<seq_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          m_reduced_val = (((static_cast<T>(m_blockdata[0])) < (m_reduced_val)) ? (static_cast<T>(m_blockdata[0])) : (m_reduced_val));

          return m_reduced_val;
        }

        ReduceMin<seq_reduce, T> min(T val) const
        {
          m_blockdata[0] = (((val) < (static_cast<T>(m_blockdata[0]))) ? (val) : (static_cast<T>(m_blockdata[0])));
          return *this ;
        }

      private:

        ReduceMin<seq_reduce, T>();

        bool m_is_copy;
        int m_myID;

        T m_reduced_val;

        CPUReductionBlockDataType* m_blockdata;
    } ;
  template <typename T>
    class ReduceMinLoc<seq_reduce, T>
    {
      public:

        explicit ReduceMinLoc(T init_val, Index_type init_loc)
        {
          m_is_copy = false;

          m_reduced_val = init_val;

          m_myID = getCPUReductionId();

          m_blockdata = getCPUReductionMemBlock(m_myID);
          m_blockdata[0] = init_val;

          m_idxdata = getCPUReductionLocBlock(m_myID);
          m_idxdata[0] = init_loc;
        }

        ReduceMinLoc( const ReduceMinLoc<seq_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceMinLoc<seq_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          if ( static_cast<T>(m_blockdata[0]) <= m_reduced_val ) {
            m_reduced_val = m_blockdata[0];
            m_reduced_idx = m_idxdata[0];
          }
          return m_reduced_val;
        }

        Index_type getMinLoc()
        {
          if ( static_cast<T>(m_blockdata[0]) <= m_reduced_val ) {
            m_reduced_val = m_blockdata[0];
            m_reduced_idx = m_idxdata[0];
          }
          return m_reduced_idx;
        }

        ReduceMinLoc<seq_reduce, T> minloc(T val, Index_type idx) const
        {
          if ( val <= static_cast<T>(m_blockdata[0]) ) {
            m_blockdata[0] = val;
            m_idxdata[0] = idx;
          }
          return *this ;
        }

      private:

        ReduceMinLoc<seq_reduce, T>();

        bool m_is_copy;
        int m_myID;

        T m_reduced_val;
        Index_type m_reduced_idx;

        CPUReductionBlockDataType* m_blockdata;
        Index_type* m_idxdata;
    } ;
  template <typename T>
    class ReduceMax<seq_reduce, T>
    {
      public:

        explicit ReduceMax(T init_val)
        {
          m_is_copy = false;

          m_reduced_val = init_val;

          m_myID = getCPUReductionId();

          m_blockdata = getCPUReductionMemBlock(m_myID);

          m_blockdata[0] = init_val;
        }

        ReduceMax( const ReduceMax<seq_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceMax<seq_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          m_reduced_val = (((static_cast<T>(m_blockdata[0])) > (m_reduced_val)) ? (static_cast<T>(m_blockdata[0])) : (m_reduced_val));

          return m_reduced_val;
        }

        ReduceMax<seq_reduce, T> max(T val) const
        {
          m_blockdata[0] = (((val) > (static_cast<T>(m_blockdata[0]))) ? (val) : (static_cast<T>(m_blockdata[0])));
          return *this ;
        }

      private:

        ReduceMax<seq_reduce, T>();

        bool m_is_copy;
        int m_myID;

        T m_reduced_val;

        CPUReductionBlockDataType* m_blockdata;
    } ;
  template <typename T>
    class ReduceMaxLoc<seq_reduce, T>
    {
      public:

        explicit ReduceMaxLoc(T init_val, Index_type init_loc)
        {
          m_is_copy = false;

          m_reduced_val = init_val;

          m_myID = getCPUReductionId();

          m_blockdata = getCPUReductionMemBlock(m_myID);
          m_blockdata[0] = init_val;

          m_idxdata = getCPUReductionLocBlock(m_myID);
          m_idxdata[0] = init_loc;
        }

        ReduceMaxLoc( const ReduceMaxLoc<seq_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceMaxLoc<seq_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          if ( static_cast<T>(m_blockdata[0]) >= m_reduced_val ) {
            m_reduced_val = m_blockdata[0];
            m_reduced_idx = m_idxdata[0];
          }
          return m_reduced_val;
        }

        Index_type getMaxLoc()
        {
          if ( static_cast<T>(m_blockdata[0]) >= m_reduced_val ) {
            m_reduced_val = m_blockdata[0];
            m_reduced_idx = m_idxdata[0];
          }
          return m_reduced_idx;
        }

        ReduceMaxLoc<seq_reduce, T> maxloc(T val, Index_type idx) const
        {
          if ( val >= static_cast<T>(m_blockdata[0]) ) {
            m_blockdata[0] = val;
            m_idxdata[0] = idx;
          }
          return *this ;
        }

      private:

        ReduceMaxLoc<seq_reduce, T>();

        bool m_is_copy;
        int m_myID;

        T m_reduced_val;
        Index_type m_reduced_idx;

        CPUReductionBlockDataType* m_blockdata;
        Index_type* m_idxdata;
    } ;
  template <typename T>
    class ReduceSum<seq_reduce, T>
    {
      public:

        explicit ReduceSum(T init_val)
        {
          m_is_copy = false;

          m_init_val = init_val;
          m_reduced_val = static_cast<T>(0);

          m_myID = getCPUReductionId();

          m_blockdata = getCPUReductionMemBlock(m_myID);

          m_blockdata[0] = 0;
        }

        ReduceSum( const ReduceSum<seq_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceSum<seq_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          m_reduced_val = m_init_val + static_cast<T>(m_blockdata[0]);

          return m_reduced_val;
        }

        ReduceSum<seq_reduce, T> operator+=(T val) const
        {
          m_blockdata[0] += val;
          return *this ;
        }

      private:

        ReduceSum<seq_reduce, T>();

        bool m_is_copy;
        int m_myID;

        T m_init_val;
        T m_reduced_val;

        CPUReductionBlockDataType* m_blockdata;
    } ;

}

namespace RAJA {
  template <typename SEG_EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void executeRangeList_forall(const IndexSetSegInfo* seg_info,
                 LOOP_BODY loop_body)
             {
               const BaseSegment* iseg = seg_info->getSegment();
               SegmentType segtype = iseg->getType();

               switch ( segtype ) {

                 case _RangeSeg_ : {
                   const RangeSegment* tseg =
                     static_cast<const RangeSegment*>(iseg);
                   forall(
                       SEG_EXEC_POLICY_T(),
                       tseg->getBegin(), tseg->getEnd(),
                       loop_body
                       );
                   break;
                 }
                 case _ListSeg_ : {
                   const ListSegment* tseg =
                     static_cast<const ListSegment*>(iseg);
                   forall(
                       SEG_EXEC_POLICY_T(),
                       tseg->getIndex(), tseg->getLength(),
                       loop_body
                       );
                   break;
                 }

                 default : {
                 }

               }
             }
  template <typename SEG_EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void executeRangeList_forall_Icount(const IndexSetSegInfo* seg_info,
                 LOOP_BODY loop_body)
             {
               const BaseSegment* iseg = seg_info->getSegment();
               SegmentType segtype = iseg->getType();

               Index_type icount = seg_info->getIcount();

               switch ( segtype ) {

                 case _RangeSeg_ : {
                   const RangeSegment* tseg =
                     static_cast<const RangeSegment*>(iseg);
                   forall_Icount(
                       SEG_EXEC_POLICY_T(),
                       tseg->getBegin(), tseg->getEnd(),
                       icount,
                       loop_body
                       );
                   break;
                 }
                 case _ListSeg_ : {
                   const ListSegment* tseg =
                     static_cast<const ListSegment*>(iseg);
                   forall_Icount(
                       SEG_EXEC_POLICY_T(),
                       tseg->getIndex(), tseg->getLength(),
                       icount,
                       loop_body
                       );
                   break;
                 }

                 default : {
                 }

               }
             }

}

namespace RAJA {
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(seq_exec,
        Index_type begin, Index_type end,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector
      for ( Index_type ii = begin ; ii < end ; ++ii ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(seq_exec,
        Index_type begin, Index_type end,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type loop_end = end - begin;

      ;

#pragma novector
      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, ii+begin );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(seq_exec,
        const RangeSegment& iseg,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type end = iseg.getEnd();

      ;

#pragma novector
      for ( Index_type ii = begin ; ii < end ; ++ii ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(seq_exec,
        const RangeSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type loop_end = iseg.getEnd() - begin;

      ;

#pragma novector
      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, ii+begin );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(seq_exec,
        Index_type begin, Index_type end,
        Index_type stride,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector
      for ( Index_type ii = begin ; ii < end ; ii += stride ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(seq_exec,
        Index_type begin, Index_type end,
        Index_type stride,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type loop_end = (end-begin)/stride;
      if ( (end-begin) % stride != 0 ) loop_end++;

      ;

#pragma novector
      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, begin + ii*stride );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(seq_exec,
        const RangeStrideSegment& iseg,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type end = iseg.getEnd();
      Index_type stride = iseg.getStride();

      ;

#pragma novector
      for ( Index_type ii = begin ; ii < end ; ii += stride ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(seq_exec,
        const RangeStrideSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type stride = iseg.getStride();
      Index_type loop_end = (iseg.getEnd()-begin)/stride;
      if ( (iseg.getEnd()-begin) % stride != 0 ) loop_end++;

      ;

#pragma novector
      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, begin + ii*stride );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(seq_exec,
        const Index_type* __restrict__ idx, Index_type len,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector
      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(seq_exec,
        const Index_type* __restrict__ idx, Index_type len,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector
      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( k+icount, idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(seq_exec,
        const ListSegment& iseg,
        LOOP_BODY loop_body)
    {
      const Index_type* __restrict__ idx = iseg.getIndex();
      Index_type len = iseg.getLength();

      ;

#pragma novector
      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(seq_exec,
        const ListSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      const Index_type* __restrict__ idx = iseg.getIndex();
      Index_type len = iseg.getLength();

      ;

#pragma novector
      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( k+icount, idx[k] );
      }

      ;
    }
  template <typename SEG_EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall( IndexSet::ExecPolicy<seq_segit, SEG_EXEC_POLICY_T>,
                 const IndexSet& iset,
                 LOOP_BODY loop_body )
             {
               int num_seg = iset.getNumSegments();
               for ( int isi = 0; isi < num_seg; ++isi ) {

                 const IndexSetSegInfo* seg_info = iset.getSegmentInfo(isi);
                 executeRangeList_forall<SEG_EXEC_POLICY_T>(seg_info, loop_body);

               }
             }
  template <typename SEG_EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount( IndexSet::ExecPolicy<seq_segit, SEG_EXEC_POLICY_T>,
                 const IndexSet& iset,
                 LOOP_BODY loop_body )
             {
               int num_seg = iset.getNumSegments();
               for ( int isi = 0; isi < num_seg; ++isi ) {

                 const IndexSetSegInfo* seg_info = iset.getSegmentInfo(isi);
                 executeRangeList_forall_Icount<SEG_EXEC_POLICY_T>(seg_info, loop_body);

               }
             }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_segments(seq_segit,
        const IndexSet& iset,
        LOOP_BODY loop_body)
    {
      IndexSet& ncis = (*const_cast<IndexSet *>(&iset)) ;
      int num_seg = ncis.getNumSegments();

      IndexSet is_tmp;
      is_tmp.push_back( RangeSegment(0, 0) ) ;

      RangeSegment* segTmp = static_cast<RangeSegment*>(is_tmp.getSegment(0));

      for ( int isi = 0; isi < num_seg; ++isi ) {

        RangeSegment* isetSeg =
          static_cast<RangeSegment*>(ncis.getSegment(isi));

        segTmp->setBegin(isetSeg->getBegin()) ;
        segTmp->setEnd(isetSeg->getEnd()) ;
        segTmp->setPrivate(isetSeg->getPrivate()) ;

        loop_body(&is_tmp) ;

      }
    }

}

namespace RAJA {

  struct simd_exec {};

}
namespace RAJA {
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(simd_exec,
        Index_type begin, Index_type end,
        LOOP_BODY loop_body)
    {
      ;

      for ( Index_type ii = begin ; ii < end ; ++ii ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(simd_exec,
        Index_type begin, Index_type end,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type loop_end = end - begin;

      ;

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, ii+begin );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(simd_exec,
        const RangeSegment& iseg,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type end = iseg.getEnd();

      ;

      for ( Index_type ii = begin ; ii < end ; ++ii ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(simd_exec,
        const RangeSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type loop_end = iseg.getEnd() - iseg.getBegin();

      ;

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, ii+begin );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(simd_exec,
        Index_type begin, Index_type end,
        Index_type stride,
        LOOP_BODY loop_body)
    {
      ;

      for ( Index_type ii = begin ; ii < end ; ii += stride ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(simd_exec,
        Index_type begin, Index_type end,
        Index_type stride,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type loop_end = (end-begin)/stride;
      if ( (end-begin) % stride != 0 ) loop_end++;

      ;

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, begin + ii*stride );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(simd_exec,
        const RangeStrideSegment& iseg,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type end = iseg.getEnd();
      Index_type stride = iseg.getStride();

      ;

      for ( Index_type ii = begin ; ii < end ; ii += stride ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(simd_exec,
        const RangeStrideSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type stride = iseg.getStride();
      Index_type loop_end = (iseg.getEnd()-begin)/stride;
      if ( (iseg.getEnd()-begin) % stride != 0 ) loop_end++;

      ;

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, begin + ii*stride );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(simd_exec,
        const Index_type* __restrict__ idx, Index_type len,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector
      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(simd_exec,
        const Index_type* __restrict__ idx, Index_type len,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector
      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( k+icount, idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(simd_exec,
        const ListSegment& iseg,
        LOOP_BODY loop_body)
    {
      const Index_type* __restrict__ idx = iseg.getIndex();
      Index_type len = iseg.getLength();

      ;

#pragma novector
      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(simd_exec,
        const ListSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      const Index_type* __restrict__ idx = iseg.getIndex();
      Index_type len = iseg.getLength();

      ;

#pragma novector
      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( k+icount, idx[k] );
      }

      ;
    }
}

namespace RAJA {
  struct omp_parallel_for_exec {};

  struct omp_for_nowait_exec {};

  struct omp_parallel_for_segit {};
  struct omp_parallel_segit {};
  struct omp_taskgraph_segit {};
  struct omp_taskgraph_interval_segit {};
  struct omp_reduce {};

}

namespace RAJA {
  template <typename T>
    class ReduceMin<omp_reduce, T>
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

          for ( int i = 0; i < nthreads; ++i ) {
            m_blockdata[i*s_block_offset] = init_val ;
          }
        }

        ReduceMin( const ReduceMin<omp_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceMin<omp_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          int nthreads = omp_get_max_threads();
          for ( int i = 0; i < nthreads; ++i ) {
            m_reduced_val =
              (((static_cast<T>(m_blockdata[i*s_block_offset])) < (m_reduced_val)) ? (static_cast<T>(m_blockdata[i*s_block_offset])) : (m_reduced_val))
              ;
          }

          return m_reduced_val;
        }

        ReduceMin<omp_reduce, T> min(T val) const
        {
          int tid = omp_get_thread_num();
          int idx = tid*s_block_offset;
          m_blockdata[idx] =
            (((val) < (static_cast<T>(m_blockdata[idx]))) ? (val) : (static_cast<T>(m_blockdata[idx])));

          return *this ;
        }

      private:

        ReduceMin<omp_reduce, T>();

        static const int s_block_offset =
          COHERENCE_BLOCK_SIZE/sizeof(CPUReductionBlockDataType);

        bool m_is_copy;
        int m_myID;

        T m_reduced_val;

        CPUReductionBlockDataType* m_blockdata;
    } ;
  template <typename T>
    class ReduceMinLoc<omp_reduce, T>
    {
      public:

        explicit ReduceMinLoc(T init_val, Index_type init_loc)
        {
          m_is_copy = false;

          m_reduced_val = init_val;

          m_myID = getCPUReductionId();

          m_blockdata = getCPUReductionMemBlock(m_myID);
          m_idxdata = getCPUReductionLocBlock(m_myID);

          int nthreads = omp_get_max_threads();

#pragma omp parallel for schedule(static, 1)

          for ( int i = 0; i < nthreads; ++i ) {
            m_blockdata[i*s_block_offset] = init_val ;
            m_idxdata[i*s_idx_offset] = init_loc ;
          }
        }

        ReduceMinLoc( const ReduceMinLoc<omp_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceMinLoc<omp_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          int nthreads = omp_get_max_threads();
          for ( int i = 0; i < nthreads; ++i ) {
            if ( static_cast<T>(m_blockdata[i*s_block_offset]) <= m_reduced_val ) {
              m_reduced_val = m_blockdata[i*s_block_offset];
              m_reduced_idx = m_idxdata[i*s_idx_offset];
            }
          }

          return m_reduced_val;
        }

        Index_type getMinLoc()
        {
          int nthreads = omp_get_max_threads();
          for ( int i = 0; i < nthreads; ++i ) {
            if ( static_cast<T>(m_blockdata[i*s_block_offset]) <= m_reduced_val ) {
              m_reduced_val = m_blockdata[i*s_block_offset];
              m_reduced_idx = m_idxdata[i*s_idx_offset];
            }
          }

          return m_reduced_idx;
        }

        ReduceMinLoc<omp_reduce, T> minloc(T val, Index_type idx) const
        {
          int tid = omp_get_thread_num();
          if ( val <= static_cast<T>(m_blockdata[tid*s_block_offset]) ) {
            m_blockdata[tid*s_block_offset] = val;
            m_idxdata[tid*s_idx_offset] = idx;
          }

          return *this ;
        }

      private:

        ReduceMinLoc<omp_reduce, T>();

        static const int s_block_offset =
          COHERENCE_BLOCK_SIZE/sizeof(CPUReductionBlockDataType);
        static const int s_idx_offset =
          COHERENCE_BLOCK_SIZE/sizeof(Index_type);

        bool m_is_copy;
        int m_myID;

        T m_reduced_val;
        Index_type m_reduced_idx;

        CPUReductionBlockDataType* m_blockdata;
        Index_type* m_idxdata;
    } ;
  template <typename T>
    class ReduceMax<omp_reduce, T>
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

          for ( int i = 0; i < nthreads; ++i ) {
            m_blockdata[i*s_block_offset] = init_val ;
          }
        }

        ReduceMax( const ReduceMax<omp_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceMax<omp_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          int nthreads = omp_get_max_threads();
          for ( int i = 0; i < nthreads; ++i ) {
            m_reduced_val =
              (((static_cast<T>(m_blockdata[i*s_block_offset])) > (m_reduced_val)) ? (static_cast<T>(m_blockdata[i*s_block_offset])) : (m_reduced_val))
              ;
          }

          return m_reduced_val;
        }

        ReduceMax<omp_reduce, T> max(T val) const
        {
          int tid = omp_get_thread_num();
          int idx = tid*s_block_offset;
          m_blockdata[idx] =
            (((val) > (static_cast<T>(m_blockdata[idx]))) ? (val) : (static_cast<T>(m_blockdata[idx])));

          return *this ;
        }

      private:

        ReduceMax<omp_reduce, T>();

        static const int s_block_offset =
          COHERENCE_BLOCK_SIZE/sizeof(CPUReductionBlockDataType);

        bool m_is_copy;
        int m_myID;

        T m_reduced_val;

        CPUReductionBlockDataType* m_blockdata;
    } ;
  template <typename T>
    class ReduceMaxLoc<omp_reduce, T>
    {
      public:

        explicit ReduceMaxLoc(T init_val, Index_type init_loc)
        {
          m_is_copy = false;

          m_reduced_val = init_val;

          m_myID = getCPUReductionId();

          m_blockdata = getCPUReductionMemBlock(m_myID);
          m_idxdata = getCPUReductionLocBlock(m_myID);

          int nthreads = omp_get_max_threads();

#pragma omp parallel for schedule(static, 1)

          for ( int i = 0; i < nthreads; ++i ) {
            m_blockdata[i*s_block_offset] = init_val ;
            m_idxdata[i*s_idx_offset] = init_loc ;
          }
        }

        ReduceMaxLoc( const ReduceMinLoc<omp_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceMaxLoc<omp_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          int nthreads = omp_get_max_threads();
          for ( int i = 0; i < nthreads; ++i ) {
            if ( static_cast<T>(m_blockdata[i*s_block_offset]) >= m_reduced_val ) {
              m_reduced_val = m_blockdata[i*s_block_offset];
              m_reduced_idx = m_idxdata[i*s_idx_offset];
            }
          }

          return m_reduced_val;
        }

        Index_type getMaxLoc()
        {
          int nthreads = omp_get_max_threads();
          for ( int i = 0; i < nthreads; ++i ) {
            if ( static_cast<T>(m_blockdata[i*s_block_offset]) >= m_reduced_val ) {
              m_reduced_val = m_blockdata[i*s_block_offset];
              m_reduced_idx = m_idxdata[i*s_idx_offset];
            }
          }

          return m_reduced_idx;
        }

        ReduceMaxLoc<omp_reduce, T> maxloc(T val, Index_type idx) const
        {
          int tid = omp_get_thread_num();
          if ( val >= static_cast<T>(m_blockdata[tid*s_block_offset]) ) {
            m_blockdata[tid*s_block_offset] = val;
            m_idxdata[tid*s_idx_offset] = idx;
          }

          return *this ;
        }

      private:

        ReduceMaxLoc<omp_reduce, T>();

        static const int s_block_offset =
          COHERENCE_BLOCK_SIZE/sizeof(CPUReductionBlockDataType);
        static const int s_idx_offset =
          COHERENCE_BLOCK_SIZE/sizeof(Index_type);

        bool m_is_copy;
        int m_myID;

        T m_reduced_val;
        Index_type m_reduced_idx;

        CPUReductionBlockDataType* m_blockdata;
        Index_type* m_idxdata;
    } ;
  template <typename T>
    class ReduceSum<omp_reduce, T>
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

          for ( int i = 0; i < nthreads; ++i ) {
            m_blockdata[i*s_block_offset] = 0 ;
          }
        }

        ReduceSum( const ReduceSum<omp_reduce, T>& other )
        {
          *this = other;
          m_is_copy = true;
        }

        ~ReduceSum<omp_reduce, T>()
        {
          if (!m_is_copy) {
            releaseCPUReductionId(m_myID);

          }
        }

        operator T()
        {
          T tmp_reduced_val = static_cast<T>(0);
          int nthreads = omp_get_max_threads();
          for ( int i = 0; i < nthreads; ++i ) {
            tmp_reduced_val += static_cast<T>(m_blockdata[i*s_block_offset]);
          }
          m_reduced_val = m_init_val + tmp_reduced_val;

          return m_reduced_val;
        }

        ReduceSum<omp_reduce, T> operator+=(T val) const
        {
          int tid = omp_get_thread_num();
          m_blockdata[tid*s_block_offset] += val;
          return *this ;
        }

      private:

        ReduceSum<omp_reduce, T>();

        static const int s_block_offset =
          COHERENCE_BLOCK_SIZE/sizeof(CPUReductionBlockDataType);

        bool m_is_copy;
        int m_myID;

        T m_init_val;
        T m_reduced_val;

        CPUReductionBlockDataType* m_blockdata;
    } ;

}

namespace RAJA {
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_parallel_for_exec,
        Index_type begin, Index_type end,
        LOOP_BODY loop_body)
    {
      ;

#pragma omp parallel for schedule(static)

      for ( Index_type ii = begin ; ii < end ; ++ii ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_for_nowait_exec,
        Index_type begin, Index_type end,
        LOOP_BODY loop_body)
    {
      ;

#pragma omp for schedule(static) nowait

      for ( Index_type ii = begin ; ii < end ; ++ii ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_parallel_for_exec,
        Index_type begin, Index_type end,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type loop_end = end - begin;

      ;

#pragma omp parallel for schedule(static)

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, ii+begin );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_for_nowait_exec,
        Index_type begin, Index_type end,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type loop_end = end - begin;

      ;

#pragma omp for schedule(static) nowait

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, ii+begin );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_parallel_for_exec,
        const RangeSegment& iseg,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type end = iseg.getEnd();

      ;

#pragma omp parallel for schedule(static)

      for ( Index_type ii = begin ; ii < end ; ++ii ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_for_nowait_exec,
        const RangeSegment& iseg,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type end = iseg.getEnd();

      ;

#pragma omp for schedule(static) nowait

      for ( Index_type ii = begin ; ii < end ; ++ii ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_parallel_for_exec,
        const RangeSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type loop_end = iseg.getEnd() - begin;

      ;

#pragma omp parallel for schedule(static)

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, ii+begin );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_for_nowait_exec,
        const RangeSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type loop_end = iseg.getEnd() - begin;

      ;

#pragma omp for schedule(static) nowait

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, ii+begin );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_parallel_for_exec,
        Index_type begin, Index_type end,
        Index_type stride,
        LOOP_BODY loop_body)
    {
      ;

#pragma omp parallel for schedule(static)

      for ( Index_type ii = begin ; ii < end ; ii += stride ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_for_nowait_exec,
        Index_type begin, Index_type end,
        Index_type stride,
        LOOP_BODY loop_body)
    {
      ;

#pragma omp for schedule(static) nowait

      for ( Index_type ii = begin ; ii < end ; ii += stride ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_parallel_for_exec,
        Index_type begin, Index_type end,
        Index_type stride,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type loop_end = (end-begin)/stride;
      if ( (end-begin) % stride != 0 ) loop_end++;

      ;

#pragma omp parallel for schedule(static)

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, begin + ii*stride );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_for_nowait_exec,
        Index_type begin, Index_type end,
        Index_type stride,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type loop_end = (end-begin)/stride;
      if ( (end-begin) % stride != 0 ) loop_end++;

      ;

#pragma omp for schedule(static) nowait

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, begin + ii*stride );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_parallel_for_exec,
        const RangeStrideSegment& iseg,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type end = iseg.getEnd();
      Index_type stride = iseg.getStride();

      ;

#pragma omp parallel for schedule(static)

      for ( Index_type ii = begin ; ii < end ; ii += stride ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_for_nowait_exec,
        const RangeStrideSegment& iseg,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type end = iseg.getEnd();
      Index_type stride = iseg.getStride();

      ;

#pragma omp for schedule(static) nowait

      for ( Index_type ii = begin ; ii < end ; ii += stride ) {
        loop_body( ii );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_parallel_for_exec,
        const RangeStrideSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type stride = iseg.getStride();
      Index_type loop_end = (iseg.getEnd()-begin)/stride;
      if ( (iseg.getEnd()-begin) % stride != 0 ) loop_end++;

      ;

#pragma omp parallel for schedule(static)

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, begin + ii*stride );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_for_nowait_exec,
        const RangeStrideSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      Index_type begin = iseg.getBegin();
      Index_type stride = iseg.getStride();
      Index_type loop_end = (iseg.getEnd()-begin)/stride;
      if ( (iseg.getEnd()-begin) % stride != 0 ) loop_end++;

      ;

#pragma omp for schedule(static) nowait

      for ( Index_type ii = 0 ; ii < loop_end ; ++ii ) {
        loop_body( ii+icount, begin + ii*stride );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_parallel_for_exec,
        const Index_type* __restrict__ idx, Index_type len,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector

#pragma omp parallel for schedule(static)

      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_for_nowait_exec,
        const Index_type* __restrict__ idx, Index_type len,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector

#pragma omp for schedule(static) nowait

      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_parallel_for_exec,
        const Index_type* __restrict__ idx, Index_type len,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector

#pragma omp parallel for schedule(static)

      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( k+icount, idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_for_nowait_exec,
        const Index_type* __restrict__ idx, Index_type len,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      ;

#pragma novector

#pragma omp for schedule(static) nowait

      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( k+icount, idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_parallel_for_exec,
        const ListSegment& iseg,
        LOOP_BODY loop_body)
    {
      const Index_type* __restrict__ idx = iseg.getIndex();
      Index_type len = iseg.getLength();

      ;

#pragma novector

#pragma omp parallel for schedule(static)

      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall(omp_for_nowait_exec,
        const ListSegment& iseg,
        LOOP_BODY loop_body)
    {
      const Index_type* __restrict__ idx = iseg.getIndex();
      Index_type len = iseg.getLength();

      ;

#pragma novector

#pragma omp for schedule(static) nowait

      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_parallel_for_exec,
        const ListSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      const Index_type* __restrict__ idx = iseg.getIndex();
      Index_type len = iseg.getLength();

      ;

#pragma novector

#pragma omp parallel for schedule(static)

      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( k+icount, idx[k] );
      }

      ;
    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_Icount(omp_for_nowait_exec,
        const ListSegment& iseg,
        Index_type icount,
        LOOP_BODY loop_body)
    {
      const Index_type* __restrict__ idx = iseg.getIndex();
      Index_type len = iseg.getLength();

      ;

#pragma novector

#pragma omp for schedule(static) nowait

      for ( Index_type k = 0 ; k < len ; ++k ) {
        loop_body( k+icount, idx[k] );
      }

      ;
    }
  template <typename SEG_EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall( IndexSet::ExecPolicy<omp_parallel_for_segit, SEG_EXEC_POLICY_T>,
                 const IndexSet& iset, LOOP_BODY loop_body )
             {
               int num_seg = iset.getNumSegments();

#pragma omp parallel for schedule(static, 1)

               for ( int isi = 0; isi < num_seg; ++isi ) {

                 const IndexSetSegInfo* seg_info = iset.getSegmentInfo(isi);
                 executeRangeList_forall<SEG_EXEC_POLICY_T>(seg_info, loop_body);

               }
             }
  template <typename SEG_EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall( IndexSet::ExecPolicy<omp_taskgraph_segit, SEG_EXEC_POLICY_T>,
                 const IndexSet& iset, LOOP_BODY loop_body )
             {
               if ( !iset.dependencyGraphSet() ) {
                 std::cerr << "\n RAJA IndexSet dependency graph not set , "
                   << "FILE: "<< "./raja/include/RAJA/exec-openmp/forall_openmp.hxx" << " line: "<< 860 << std::endl;
                 exit(1);
               }

               IndexSet& ncis = (*const_cast<IndexSet *>(&iset)) ;

               int num_seg = ncis.getNumSegments();

#pragma omp parallel for schedule(static, 1)

               for ( int isi = 0; isi < num_seg; ++isi ) {

                 IndexSetSegInfo* seg_info = ncis.getSegmentInfo(isi);
                 DepGraphNode* task = seg_info->getDepGraphNode();
                 volatile int* __restrict__ semVal = &(task->semaphoreValue());

                 while(*semVal != 0) {
                   ;

                   sched_yield() ;
                 }

                 executeRangeList_forall<SEG_EXEC_POLICY_T>(seg_info, loop_body);

                 if (task->semaphoreReloadValue() != 0) {
                   task->semaphoreValue() = task->semaphoreReloadValue() ;
                 }

                 if (task->numDepTasks() != 0) {
                   for (int ii = 0; ii < task->numDepTasks(); ++ii) {

                     int seg = task->depTaskNum(ii) ;
                     DepGraphNode* dep = ncis.getSegmentInfo(seg)->getDepGraphNode();
                     __sync_fetch_and_sub(&(dep->semaphoreValue()), 1) ;
                   }
                 }

               }
             }
  template <typename SEG_EXEC_POLICY_T,
           typename LOOP_BODY>
             inline __attribute__((always_inline))
             void forall_Icount( IndexSet::ExecPolicy<omp_parallel_for_segit, SEG_EXEC_POLICY_T>,
                 const IndexSet& iset, LOOP_BODY loop_body )
             {
               int num_seg = iset.getNumSegments();

#pragma omp parallel for schedule(static, 1)

               for ( int isi = 0; isi < num_seg; ++isi ) {

                 const IndexSetSegInfo* seg_info = iset.getSegmentInfo(isi);
                 executeRangeList_forall_Icount<SEG_EXEC_POLICY_T>(seg_info, loop_body);

               }
             }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_segments(omp_parallel_segit,
        const IndexSet& iset,
        LOOP_BODY loop_body)
    {
      IndexSet& ncis = (*const_cast<IndexSet *>(&iset)) ;
      int num_seg = ncis.getNumSegments();

#pragma omp parallel

      {
        int numThreads = omp_get_max_threads() ;
        int tid = omp_get_thread_num() ;

        IndexSet is_tmp;
        is_tmp.push_back( RangeSegment(0, 0) ) ;

        RangeSegment* segTmp = static_cast<RangeSegment*>(is_tmp.getSegment(0));

        for ( int isi = tid; isi < num_seg; isi += numThreads ) {

          RangeSegment* isetSeg =
            static_cast<RangeSegment*>(ncis.getSegment(isi));

          segTmp->setBegin(isetSeg->getBegin()) ;
          segTmp->setEnd(isetSeg->getEnd()) ;
          segTmp->setPrivate(isetSeg->getPrivate()) ;

          loop_body(&is_tmp) ;

        }

      }

    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_segments(omp_taskgraph_segit,
        const IndexSet& iset,
        LOOP_BODY loop_body)
    {
      if ( !iset.dependencyGraphSet() ) {
        std::cerr << "\n RAJA IndexSet dependency graph not set , "
          << "FILE: "<< "./raja/include/RAJA/exec-openmp/forall_openmp.hxx" << " line: "<< 1019 << std::endl;
        exit(1);
      }

      IndexSet& ncis = (*const_cast<IndexSet *>(&iset)) ;
      int num_seg = ncis.getNumSegments();

#pragma omp parallel

      {
        int numThreads = omp_get_max_threads() ;
        int tid = omp_get_thread_num() ;

        IndexSet is_tmp;
        is_tmp.push_back( RangeSegment(0, 0) ) ;

        RangeSegment* segTmp = static_cast<RangeSegment*>(is_tmp.getSegment(0));

        for ( int isi = tid; isi < num_seg; isi += numThreads ) {

          IndexSetSegInfo* seg_info = ncis.getSegmentInfo(isi);
          DepGraphNode* task = seg_info->getDepGraphNode();
          volatile int* __restrict__ semVal = &(task->semaphoreValue());

          while (*semVal != 0) {
            ;

            sched_yield() ;
          }

          RangeSegment* isetSeg =
            static_cast<RangeSegment*>(ncis.getSegment(isi));

          segTmp->setBegin(isetSeg->getBegin()) ;
          segTmp->setEnd(isetSeg->getEnd()) ;
          segTmp->setPrivate(isetSeg->getPrivate()) ;

          loop_body(&is_tmp) ;

          if (task->semaphoreReloadValue() != 0) {
            task->semaphoreValue() = task->semaphoreReloadValue() ;
          }

          if (task->numDepTasks() != 0) {
            for (int ii = 0; ii < task->numDepTasks(); ++ii) {

              int seg = task->depTaskNum(ii) ;
              DepGraphNode* dep = ncis.getSegmentInfo(seg)->getDepGraphNode();
              __sync_fetch_and_sub(&(dep->semaphoreValue()), 1) ;
            }
          }

        }

      }

    }
  template <typename LOOP_BODY>
    inline __attribute__((always_inline))
    void forall_segments(omp_taskgraph_interval_segit,
        const IndexSet& iset,
        LOOP_BODY loop_body)
    {
      if ( !iset.dependencyGraphSet() ) {
        std::cerr << "\n RAJA IndexSet dependency graph not set , "
          << "FILE: "<< "./raja/include/RAJA/exec-openmp/forall_openmp.hxx" << " line: "<< 1122 << std::endl;
        exit(1);
      }

      IndexSet& ncis = (*const_cast<IndexSet *>(&iset)) ;
      int num_seg = ncis.getNumSegments();

#pragma omp parallel

      {
        int tid = omp_get_thread_num() ;

        IndexSet is_tmp;
        is_tmp.push_back( RangeSegment(0, 0) ) ;

        RangeSegment* segTmp = static_cast<RangeSegment*>(is_tmp.getSegment(0));

        const int tbegin = ncis.getSegmentIntervalBegin(tid);
        const int tend = ncis.getSegmentIntervalEnd(tid);

        for ( int isi = tbegin; isi < tend; ++isi ) {

          IndexSetSegInfo* seg_info = ncis.getSegmentInfo(isi);
          DepGraphNode* task = seg_info->getDepGraphNode();
          volatile int* __restrict__ semVal = &(task->semaphoreValue());

          while (*semVal != 0) {
            ;

            sched_yield() ;
          }

          RangeSegment* isetSeg =
            static_cast<RangeSegment*>(ncis.getSegment(isi));

          segTmp->setBegin(isetSeg->getBegin()) ;
          segTmp->setEnd(isetSeg->getEnd()) ;
          segTmp->setPrivate(isetSeg->getPrivate()) ;

          loop_body(&is_tmp) ;

          if (task->semaphoreReloadValue() != 0) {
            task->semaphoreValue() = task->semaphoreReloadValue() ;
          }

          if (task->numDepTasks() != 0) {
            for (int ii = 0; ii < task->numDepTasks(); ++ii) {

              int seg = task->depTaskNum(ii) ;
              DepGraphNode* dep = ncis.getSegmentInfo(seg)->getDepGraphNode();
              __sync_fetch_and_sub(&(dep->semaphoreValue()), 1) ;
            }
          }

        }

      }
    }

}

namespace RAJA {

  struct ForallN_OMP_Parallel_Tag {};
  template<typename NEXT=Execute>
    struct OMP_Parallel {

      typedef ForallN_OMP_Parallel_Tag PolicyTag;

      typedef NEXT NextPolicy;
    };

  struct omp_collapse_nowait_exec {};

  template<typename ... PREST>
    struct ForallN_Executor<
    ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment>,
    ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment>,
    PREST... >
    {
      ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> iset_i;
      ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> iset_j;

      typedef ForallN_Executor<PREST...> NextExec;
      NextExec next_exec;

      inline __attribute__((always_inline))
        constexpr
        ForallN_Executor(
            ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> const &iseti_,
            ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> const &isetj_,
            PREST const &... prest)
        : iset_i(iseti_), iset_j(isetj_), next_exec(prest...)
        { }

      template<typename BODY>
        inline __attribute__((always_inline))
        void operator()(BODY body) const {

          int begin_i = iset_i.getBegin();
          int begin_j = iset_j.getBegin();
          int end_i = iset_i.getEnd();
          int end_j = iset_j.getEnd();

          ForallN_PeelOuter<NextExec, BODY> outer(next_exec, body);

#pragma omp for nowait collapse(2)

          for(int i = begin_i;i < end_i;++ i){
            for(int j = begin_j;j < end_j;++ j){
              outer(i,j);
            }
          }
        }
    };

  template<typename ... PREST>
    struct ForallN_Executor<
    ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment>,
    ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment>,
    ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment>,
    PREST... >
    {

      ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> iset_i;
      ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> iset_j;
      ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> iset_k;

      typedef ForallN_Executor<PREST...> NextExec;
      NextExec next_exec;

      inline __attribute__((always_inline))
        constexpr
        ForallN_Executor(
            ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> const &iseti_,
            ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> const &isetj_,
            ForallN_PolicyPair<omp_collapse_nowait_exec, RangeSegment> const &isetk_,
            PREST ... prest)
        : iset_i(iseti_), iset_j(isetj_), iset_k(isetk_), next_exec(prest...)
        { }

      template<typename BODY>
        inline __attribute__((always_inline))
        void operator()(BODY body) const {

          int begin_i = iset_i.getBegin();
          int begin_j = iset_j.getBegin();
          int begin_k = iset_k.getBegin();
          int end_i = iset_i.getEnd();
          int end_j = iset_j.getEnd();
          int end_k = iset_k.getEnd();

          ForallN_PeelOuter<NextExec, BODY> outer(next_exec, body);

#pragma omp for nowait collapse(3)

          for(int i = begin_i;i < end_i;++ i){
            for(int j = begin_j;j < end_j;++ j){
              for(int k = begin_k;k < end_k;++ k){
                outer(i,j,k);
              }
            }
          }

        }
    };
  template<typename POLICY, typename BODY, typename ... PARGS>
    inline __attribute__((always_inline)) void forallN_policy(ForallN_OMP_Parallel_Tag, BODY body, PARGS ... pargs){
      typedef typename POLICY::NextPolicy NextPolicy;
      typedef typename POLICY::NextPolicy::PolicyTag NextPolicyTag;

#pragma omp parallel

      {
        forallN_policy<NextPolicy>(NextPolicyTag(), body, pargs...);
      }
    }

}

namespace RAJA {

  struct switcher_exec {}; // This can be provided as INPUT
// Liao, policy switcher: generating a valid policy
  namespace switcher {
// We can assume a list of execution policy as the INPUT
    enum POLICY_TYPE {
      seq_exec = 0,
      omp_parallel_for_exec,
      NUM_POLICIES
    };

    inline __attribute__((always_inline))
      POLICY_TYPE
      getRandomPolicy() {
        int coin_toss = rand() % 2;
        if (coin_toss >= 1) {
          return seq_exec;
        } else {
          return omp_parallel_for_exec;
        }
      }
 // This is only used for C++14 version
    template <typename BODY>
      inline __attribute__((always_inline))
      void policySwitcher(POLICY_TYPE policy, BODY body) {
        switch (policy) {
          case seq_exec: body(RAJA::seq_exec()); break;
          case omp_parallel_for_exec: body(RAJA::omp_parallel_for_exec()); break;
        }
      }

  } // end switcher
#if 1 // must have to instantiate forall later!!!
//Liao, The specialization of forall, using switcher policy  , the Key of the transformation
// From the input code's loop in question (or all forall templates),
// generate a specialized version using RAJA::switcher_exec
  template <typename LOOP_BODY>
//    inline __attribute__((always_inline))
    void forall(switcher_exec,
        Index_type begin, Index_type end,
        LOOP_BODY loop_body)
    {
      switch(switcher::getRandomPolicy()) {
        case switcher::seq_exec:
          RAJA::forall( RAJA::seq_exec(), begin, end, loop_body ); break;
        case switcher::omp_parallel_for_exec:
          RAJA::forall( RAJA::omp_parallel_for_exec(), begin, end, loop_body ); break;
      }
    }
#endif    
}

namespace RAJA {
  template <typename CONTAINER_T>
    inline __attribute__((always_inline))
    void getIndices(CONTAINER_T& con, const IndexSet& iset)
    {
      CONTAINER_T tcon;
      forall< IndexSet::ExecPolicy<seq_segit, seq_exec> >(iset,
          [&] (Index_type idx) {
          tcon.push_back(idx);
          } );
      con = tcon;
    }
  template <typename CONTAINER_T,
           typename SEGMENT_T>
             inline __attribute__((always_inline))
             void getIndices(CONTAINER_T& con, const SEGMENT_T& iset)
             {
               CONTAINER_T tcon;
               forall< seq_exec >(iset, [&] (Index_type idx) {
                   tcon.push_back(idx);
                   } );
               con = tcon;
             }
  template <typename CONTAINER_T,
           typename CONDITIONAL>
             inline __attribute__((always_inline))
             void getIndicesConditional(CONTAINER_T& con, const IndexSet& iset,
                 CONDITIONAL conditional)
             {
               CONTAINER_T tcon;
               forall< IndexSet::ExecPolicy<seq_segit, seq_exec> >(iset,
                   [&] (Index_type idx) {
                   if ( conditional( idx ) ) tcon.push_back(idx);
                   } );
               con = tcon;
             }
  template <typename CONTAINER_T,
           typename SEGMENT_T,
           typename CONDITIONAL>
             inline __attribute__((always_inline))
             void getIndicesConditional(CONTAINER_T& con, const SEGMENT_T& iset,
                 CONDITIONAL conditional)
             {
               CONTAINER_T tcon;
               forall< seq_exec >(iset, [&] (Index_type idx) {
                   if ( conditional( idx ) ) tcon.push_back(idx);
                   } );
               con = tcon;
             }

}
namespace RAJA {

  struct tile_none {};

  template<int TileSize>
    struct tile_fixed {};

  template<typename ... PLIST>
    struct TileList{
      constexpr const static size_t num_loops = sizeof...(PLIST);
      typedef std::tuple<PLIST...> tuple;
    };

  struct ForallN_Tile_Tag {};
  template<typename TILE_LIST, typename NEXT=Execute>
    struct Tile {

      typedef ForallN_Tile_Tag PolicyTag;

      typedef TILE_LIST TilePolicy;

      typedef NEXT NextPolicy;
    };
  template<typename BODY, typename TilePolicy, int TIDX, typename PI, typename ... PREST>
    inline __attribute__((always_inline)) void forallN_peel_tile(BODY body, PI const &pi, PREST const &... prest);

  template<typename BODY, typename TilePolicy, int TIDX, typename PI, typename ... PREST>
    inline __attribute__((always_inline)) void forallN_apply_tile(tile_none, BODY body, PI const &pi, PREST const &... prest){

      typedef ForallN_BindFirstArg_Host<BODY, PI> BOUND;
      BOUND new_body(body, pi);

      forallN_peel_tile<BOUND, TilePolicy, TIDX+1>(new_body, prest...);

    }

  template<typename BODY, typename TilePolicy, int TIDX, int TileSize, typename PI, typename ... PREST>
    inline __attribute__((always_inline)) void forallN_apply_tile(tile_fixed<TileSize>, BODY body, PI const &pi, PREST const &... prest){

      typedef ForallN_BindFirstArg_Host<BODY, PI> BOUND;

      Index_type i_begin = pi.getBegin();
      Index_type i_end = pi.getEnd();
      for(Index_type i0 = i_begin;i0 < i_end;i0 += TileSize){

        Index_type i1 = std::min(i0+TileSize, i_end);
        PI pi_tile(RangeSegment(i0, i1));

        BOUND new_body(body, pi_tile);

        forallN_peel_tile<BOUND, TilePolicy, TIDX+1>(new_body, prest...);
      }
    }

  template<typename NextPolicy, typename BODY, typename ... ARGS>
    struct ForallN_NextPolicyWrapper {

      BODY body;

      explicit ForallN_NextPolicyWrapper(BODY b) : body(b) {}

      inline __attribute__((always_inline))
        void operator()(ARGS const &... args) const {
          typedef typename NextPolicy::PolicyTag NextPolicyTag;
          forallN_policy<NextPolicy>(NextPolicyTag(), body, args...);
        }

    };
  template<typename BODY, typename TilePolicy, int TIDX>
    inline __attribute__((always_inline)) void forallN_peel_tile(BODY body){

      body();
    }
  template<typename BODY, typename TilePolicy, int TIDX, typename PI, typename ... PREST>
    inline __attribute__((always_inline)) void forallN_peel_tile(BODY body, PI const &pi, PREST const &... prest){

      using TP = typename std::tuple_element<TIDX, typename TilePolicy::tuple>::type;

      forallN_apply_tile<BODY, TilePolicy, TIDX>(TP(), body, pi, prest...);
    }
  template<typename POLICY, typename BODY, typename ... PARGS>
    inline __attribute__((always_inline)) void forallN_policy(ForallN_Tile_Tag, BODY body, PARGS ... pargs){
      typedef typename POLICY::NextPolicy NextPolicy;
      typedef typename POLICY::NextPolicy::PolicyTag NextPolicyTag;

      using TilePolicy = typename POLICY::TilePolicy;

      typedef ForallN_NextPolicyWrapper<NextPolicy, BODY, PARGS...> WRAPPER;
      WRAPPER wrapper(body);
      forallN_peel_tile<WRAPPER, TilePolicy, 0>(wrapper, pargs...);
    }

}

namespace RAJA {

  struct ForallN_Permute_Tag {};
  template<typename LOOP_ORDER, typename NEXT=Execute>
    struct Permute {
      typedef ForallN_Permute_Tag PolicyTag;

      typedef LOOP_ORDER LoopOrder;

      typedef NEXT NextPolicy;
    };
  template<typename PERM, typename BODY>
    struct ForallN_Permute_Functor;

  template<typename POLICY, typename BODY, typename ... ARGS>
    inline __attribute__((always_inline)) void forallN_policy(ForallN_Permute_Tag, BODY body, ARGS ... args){

      typedef typename POLICY::LoopOrder LoopOrder;

      typedef typename POLICY::NextPolicy NextPolicy;
      typedef typename POLICY::NextPolicy::PolicyTag NextPolicyTag;

      typedef ForallN_Permute_Functor<LoopOrder, BODY> PERM_FUNC;
      PERM_FUNC perm_func(body);

      perm_func.template callNextPolicy<NextPolicy, NextPolicyTag>(args...);
    }

}

namespace RAJA {

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j) const {
          body(i, j);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i) const {
          body(i, j);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j, Index_type k) const {
          body(i, j, k);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IKJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type k, Index_type j) const {
          body(i, j, k);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pk, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JIK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i, Index_type k) const {
          body(i, j, k);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JKI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type k, Index_type i) const {
          body(i, j, k);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pk, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KIJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type i, Index_type j) const {
          body(i, j, k);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KJI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type j, Index_type i) const {
          body(i, j, k);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pj, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJKL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j, Index_type k, Index_type l) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj, pk, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJLK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j, Index_type l, Index_type k) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj, pl, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IKJL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type k, Index_type j, Index_type l) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pk, pj, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IKLJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type k, Index_type l, Index_type j) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pk, pl, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_ILJK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type l, Index_type j, Index_type k) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pl, pj, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_ILKJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type l, Index_type k, Index_type j) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pl, pk, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JIKL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i, Index_type k, Index_type l) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi, pk, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JILK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i, Index_type l, Index_type k) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi, pl, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JKIL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type k, Index_type i, Index_type l) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pk, pi, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JKLI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type k, Index_type l, Index_type i) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pk, pl, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JLIK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type l, Index_type i, Index_type k) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pl, pi, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JLKI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type l, Index_type k, Index_type i) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pl, pk, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KIJL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type i, Index_type j, Index_type l) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pi, pj, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KILJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type i, Index_type l, Index_type j) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pi, pl, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KJIL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type j, Index_type i, Index_type l) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pj, pi, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KJLI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type j, Index_type l, Index_type i) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pj, pl, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KLIJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type l, Index_type i, Index_type j) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pl, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KLJI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type l, Index_type j, Index_type i) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pl, pj, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LIJK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type i, Index_type j, Index_type k) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pi, pj, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LIKJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type i, Index_type k, Index_type j) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pi, pk, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LJIK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type j, Index_type i, Index_type k) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pj, pi, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LJKI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type j, Index_type k, Index_type i) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pj, pk, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LKIJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type k, Index_type i, Index_type j) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pk, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LKJI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type k, Index_type j, Index_type i) const {
          body(i, j, k, l);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pk, pj, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJKLM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j, Index_type k, Index_type l, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj, pk, pl, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJKML, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j, Index_type k, Index_type m, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj, pk, pm, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJLKM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j, Index_type l, Index_type k, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj, pl, pk, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJLMK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j, Index_type l, Index_type m, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj, pl, pm, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJMKL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j, Index_type m, Index_type k, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj, pm, pk, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IJMLK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type j, Index_type m, Index_type l, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pj, pm, pl, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IKJLM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type k, Index_type j, Index_type l, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pk, pj, pl, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IKJML, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type k, Index_type j, Index_type m, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pk, pj, pm, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IKLJM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type k, Index_type l, Index_type j, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pk, pl, pj, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IKLMJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type k, Index_type l, Index_type m, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pk, pl, pm, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IKMJL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type k, Index_type m, Index_type j, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pk, pm, pj, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IKMLJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type k, Index_type m, Index_type l, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pk, pm, pl, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_ILJKM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type l, Index_type j, Index_type k, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pl, pj, pk, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_ILJMK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type l, Index_type j, Index_type m, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pl, pj, pm, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_ILKJM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type l, Index_type k, Index_type j, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pl, pk, pj, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_ILKMJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type l, Index_type k, Index_type m, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pl, pk, pm, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_ILMJK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type l, Index_type m, Index_type j, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pl, pm, pj, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_ILMKJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type l, Index_type m, Index_type k, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pl, pm, pk, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IMJKL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type m, Index_type j, Index_type k, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pm, pj, pk, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IMJLK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type m, Index_type j, Index_type l, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pm, pj, pl, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IMKJL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type m, Index_type k, Index_type j, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pm, pk, pj, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IMKLJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type m, Index_type k, Index_type l, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pm, pk, pl, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IMLJK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type m, Index_type l, Index_type j, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pm, pl, pj, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_IMLKJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type i, Index_type m, Index_type l, Index_type k, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pi, pm, pl, pk, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JIKLM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i, Index_type k, Index_type l, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi, pk, pl, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JIKML, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i, Index_type k, Index_type m, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi, pk, pm, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JILKM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i, Index_type l, Index_type k, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi, pl, pk, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JILMK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i, Index_type l, Index_type m, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi, pl, pm, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JIMKL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i, Index_type m, Index_type k, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi, pm, pk, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JIMLK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type i, Index_type m, Index_type l, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pi, pm, pl, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JKILM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type k, Index_type i, Index_type l, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pk, pi, pl, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JKIML, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type k, Index_type i, Index_type m, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pk, pi, pm, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JKLIM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type k, Index_type l, Index_type i, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pk, pl, pi, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JKLMI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type k, Index_type l, Index_type m, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pk, pl, pm, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JKMIL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type k, Index_type m, Index_type i, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pk, pm, pi, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JKMLI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type k, Index_type m, Index_type l, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pk, pm, pl, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JLIKM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type l, Index_type i, Index_type k, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pl, pi, pk, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JLIMK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type l, Index_type i, Index_type m, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pl, pi, pm, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JLKIM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type l, Index_type k, Index_type i, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pl, pk, pi, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JLKMI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type l, Index_type k, Index_type m, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pl, pk, pm, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JLMIK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type l, Index_type m, Index_type i, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pl, pm, pi, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JLMKI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type l, Index_type m, Index_type k, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pl, pm, pk, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JMIKL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type m, Index_type i, Index_type k, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pm, pi, pk, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JMILK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type m, Index_type i, Index_type l, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pm, pi, pl, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JMKIL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type m, Index_type k, Index_type i, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pm, pk, pi, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JMKLI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type m, Index_type k, Index_type l, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pm, pk, pl, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JMLIK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type m, Index_type l, Index_type i, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pm, pl, pi, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_JMLKI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type j, Index_type m, Index_type l, Index_type k, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pj, pm, pl, pk, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KIJLM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type i, Index_type j, Index_type l, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pi, pj, pl, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KIJML, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type i, Index_type j, Index_type m, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pi, pj, pm, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KILJM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type i, Index_type l, Index_type j, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pi, pl, pj, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KILMJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type i, Index_type l, Index_type m, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pi, pl, pm, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KIMJL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type i, Index_type m, Index_type j, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pi, pm, pj, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KIMLJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type i, Index_type m, Index_type l, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pi, pm, pl, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KJILM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type j, Index_type i, Index_type l, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pj, pi, pl, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KJIML, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type j, Index_type i, Index_type m, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pj, pi, pm, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KJLIM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type j, Index_type l, Index_type i, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pj, pl, pi, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KJLMI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type j, Index_type l, Index_type m, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pj, pl, pm, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KJMIL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type j, Index_type m, Index_type i, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pj, pm, pi, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KJMLI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type j, Index_type m, Index_type l, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pj, pm, pl, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KLIJM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type l, Index_type i, Index_type j, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pl, pi, pj, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KLIMJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type l, Index_type i, Index_type m, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pl, pi, pm, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KLJIM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type l, Index_type j, Index_type i, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pl, pj, pi, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KLJMI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type l, Index_type j, Index_type m, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pl, pj, pm, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KLMIJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type l, Index_type m, Index_type i, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pl, pm, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KLMJI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type l, Index_type m, Index_type j, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pl, pm, pj, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KMIJL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type m, Index_type i, Index_type j, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pm, pi, pj, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KMILJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type m, Index_type i, Index_type l, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pm, pi, pl, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KMJIL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type m, Index_type j, Index_type i, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pm, pj, pi, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KMJLI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type m, Index_type j, Index_type l, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pm, pj, pl, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KMLIJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type m, Index_type l, Index_type i, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pm, pl, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_KMLJI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type k, Index_type m, Index_type l, Index_type j, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pk, pm, pl, pj, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LIJKM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type i, Index_type j, Index_type k, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pi, pj, pk, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LIJMK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type i, Index_type j, Index_type m, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pi, pj, pm, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LIKJM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type i, Index_type k, Index_type j, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pi, pk, pj, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LIKMJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type i, Index_type k, Index_type m, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pi, pk, pm, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LIMJK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type i, Index_type m, Index_type j, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pi, pm, pj, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LIMKJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type i, Index_type m, Index_type k, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pi, pm, pk, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LJIKM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type j, Index_type i, Index_type k, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pj, pi, pk, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LJIMK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type j, Index_type i, Index_type m, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pj, pi, pm, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LJKIM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type j, Index_type k, Index_type i, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pj, pk, pi, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LJKMI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type j, Index_type k, Index_type m, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pj, pk, pm, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LJMIK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type j, Index_type m, Index_type i, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pj, pm, pi, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LJMKI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type j, Index_type m, Index_type k, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pj, pm, pk, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LKIJM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type k, Index_type i, Index_type j, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pk, pi, pj, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LKIMJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type k, Index_type i, Index_type m, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pk, pi, pm, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LKJIM, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type k, Index_type j, Index_type i, Index_type m) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pk, pj, pi, pm);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LKJMI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type k, Index_type j, Index_type m, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pk, pj, pm, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LKMIJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type k, Index_type m, Index_type i, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pk, pm, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LKMJI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type k, Index_type m, Index_type j, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pk, pm, pj, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LMIJK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type m, Index_type i, Index_type j, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pm, pi, pj, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LMIKJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type m, Index_type i, Index_type k, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pm, pi, pk, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LMJIK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type m, Index_type j, Index_type i, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pm, pj, pi, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LMJKI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type m, Index_type j, Index_type k, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pm, pj, pk, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LMKIJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type m, Index_type k, Index_type i, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pm, pk, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_LMKJI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type l, Index_type m, Index_type k, Index_type j, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pl, pm, pk, pj, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MIJKL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type i, Index_type j, Index_type k, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pi, pj, pk, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MIJLK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type i, Index_type j, Index_type l, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pi, pj, pl, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MIKJL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type i, Index_type k, Index_type j, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pi, pk, pj, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MIKLJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type i, Index_type k, Index_type l, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pi, pk, pl, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MILJK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type i, Index_type l, Index_type j, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pi, pl, pj, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MILKJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type i, Index_type l, Index_type k, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pi, pl, pk, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MJIKL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type j, Index_type i, Index_type k, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pj, pi, pk, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MJILK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type j, Index_type i, Index_type l, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pj, pi, pl, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MJKIL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type j, Index_type k, Index_type i, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pj, pk, pi, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MJKLI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type j, Index_type k, Index_type l, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pj, pk, pl, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MJLIK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type j, Index_type l, Index_type i, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pj, pl, pi, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MJLKI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type j, Index_type l, Index_type k, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pj, pl, pk, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MKIJL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type k, Index_type i, Index_type j, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pk, pi, pj, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MKILJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type k, Index_type i, Index_type l, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pk, pi, pl, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MKJIL, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type k, Index_type j, Index_type i, Index_type l) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pk, pj, pi, pl);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MKJLI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type k, Index_type j, Index_type l, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pk, pj, pl, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MKLIJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type k, Index_type l, Index_type i, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pk, pl, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MKLJI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type k, Index_type l, Index_type j, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pk, pl, pj, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MLIJK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type l, Index_type i, Index_type j, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pl, pi, pj, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MLIKJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type l, Index_type i, Index_type k, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pl, pi, pk, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MLJIK, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type l, Index_type j, Index_type i, Index_type k) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pl, pj, pi, pk);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MLJKI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type l, Index_type j, Index_type k, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pl, pj, pk, pi);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MLKIJ, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type l, Index_type k, Index_type i, Index_type j) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pl, pk, pi, pj);
        }

      BODY body;
    };

  template<typename BODY>
    struct ForallN_Permute_Functor<PERM_MLKJI, BODY>{

      inline __attribute__((always_inline))
        constexpr
        explicit ForallN_Permute_Functor(BODY const &b) : body(b) {}

      inline __attribute__((always_inline))

        void operator()(Index_type m, Index_type l, Index_type k, Index_type j, Index_type i) const {
          body(i, j, k, l, m);
        }

      template<typename NextPolicy, typename TAG, typename PI, typename PJ, typename PK, typename PL, typename PM>
        inline __attribute__((always_inline))
        void callNextPolicy(PI const &pi, PJ const &pj, PK const &pk, PL const &pl, PM const &pm) const {
          forallN_policy<NextPolicy>(TAG(), *this, pm, pl, pk, pj, pi);
        }

      BODY body;
    };

}
// A new switcher policy type
typedef RAJA::switcher_exec policy_one;
typedef RAJA::NestedPolicy<RAJA::ExecList<RAJA::seq_exec, RAJA::seq_exec> > policy_n;

typedef RAJA::omp_reduce reduce_policy;

int main(int argc, char* argv[]) { // pdf 21810
  const int n = 15;

  double* a = new double[n*n];
  double* b = new double[n*n];
  double* c = new double[n*n];

  RAJA::RangeSegment is(0,n);
  RAJA::RangeSegment js(0,n);
  RAJA::RangeSegment ks(0,n);

  RAJA::forallN<policy_n>(is, js, [=] (int i, int j) {
      a[i*n+j] = i + 0.1*j;
      b[i*n+j] = fabs(i-j);

      c[i*n+j] = 0.0;
      });

  RAJA::forallN<policy_n>(is, js, [=] (int i, int j) {

      RAJA::ReduceSum<reduce_policy, double> sum(c[i*n+j]) ;
//Liao, use the switcher policy here  // SgTemplateFunctionDeclaration* p_templateDeclaration : 0x7feb3a61b620 pdf line 8182
      RAJA::forall<policy_one>(ks, [=] (int k) { // pdf 21982, SgType* p_expression_type : 0x8f01270, SgTemplateInstantiationFunctionDecl 26651 
        sum += a[i*n+j]*b[k*n+j];
        });

      c[i*n+j] = double(sum);
      });

  printf("Element at c[3][3] is %f\n", c[3*n+3]);
}
