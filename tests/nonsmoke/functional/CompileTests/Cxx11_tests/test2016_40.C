/*

                 Copyright (c) 2010.
      Lawrence Livermore National Security, LLC.
Produced at the Lawrence Livermore National Laboratory.
                  LLNL-CODE-461231
                All rights reserved.

This file is part of LULESH, Version 1.0.
Please also read this link -- http://www.opensource.org/licenses/index.php

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the disclaimer below.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the disclaimer (as noted below)
     in the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the LLNS/LLNL nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY, LLC,
THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Additional BSD Notice

1. This notice is required to be provided under our contract with the U.S.
   Department of Energy (DOE). This work was produced at Lawrence Livermore
   National Laboratory under Contract No. DE-AC52-07NA27344 with the DOE.

2. Neither the United States Government nor Lawrence Livermore National
   Security, LLC nor any of their employees, makes any warranty, express
   or implied, or assumes any liability or responsibility for the accuracy,
   completeness, or usefulness of any information, apparatus, product, or
   process disclosed, or represents that its use would not infringe
   privately-owned rights.

3. Also, reference herein to any specific commercial products, process, or
   services by trade name, trademark, manufacturer or otherwise does not
   necessarily constitute or imply its endorsement, recommendation, or
   favoring by the United States Government or Lawrence Livermore National
   Security, LLC. The views and opinions of authors expressed herein do not
   necessarily state or reflect those of the United States Government or
   Lawrence Livermore National Security, LLC, and shall not be used for
   advertising or product endorsement purposes.

Single file LULESH-V1.0 RAJA version
Ideally, we should only extract troublesome portions. 
But this file is the smallest we got so far
Liao 7/1/2016
*/

#include <iostream>
#include <chrono>
#include <stdlib.h>
#include <math.h>
#include <string.h>

namespace RAJA {

  const int RAJA_VERSION_MAJOR = 0;
  const int RAJA_VERSION_MINOR = 1;
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
  //typedef Real_type* __restrict__ Real_ptr;
  typedef Real_type* Real_ptr;
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

namespace std {

  template< >
    void swap(RAJA::ListSegment& a, RAJA::ListSegment& b)
    {
      a.swap(b);
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
namespace std {

  template< >
    void swap(RAJA::IndexSet& a, RAJA::IndexSet& b)
    {
      a.swap(b);
    }

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
             inline __attribute__((always_inline))
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

typedef struct
{
  unsigned char _x[4]
    __attribute__((__aligned__(4)));
} omp_lock_t;

typedef struct
{
  unsigned char _x[16]
    __attribute__((__aligned__(8)));
} omp_nest_lock_t;

typedef enum omp_sched_t
{
  omp_sched_static = 1,
  omp_sched_dynamic = 2,
  omp_sched_guided = 3,
  omp_sched_auto = 4
} omp_sched_t;

extern "C" {

  extern void omp_set_num_threads (int) throw ();
  extern int omp_get_num_threads (void) throw ();
  extern int omp_get_max_threads (void) throw ();
  extern int omp_get_thread_num (void) throw ();
  extern int omp_get_num_procs (void) throw ();

  extern int omp_in_parallel (void) throw ();

  extern void omp_set_dynamic (int) throw ();
  extern int omp_get_dynamic (void) throw ();

  extern void omp_set_nested (int) throw ();
  extern int omp_get_nested (void) throw ();

  extern void omp_init_lock (omp_lock_t *) throw ();
  extern void omp_destroy_lock (omp_lock_t *) throw ();
  extern void omp_set_lock (omp_lock_t *) throw ();
  extern void omp_unset_lock (omp_lock_t *) throw ();
  extern int omp_test_lock (omp_lock_t *) throw ();

  extern void omp_init_nest_lock (omp_nest_lock_t *) throw ();
  extern void omp_destroy_nest_lock (omp_nest_lock_t *) throw ();
  extern void omp_set_nest_lock (omp_nest_lock_t *) throw ();
  extern void omp_unset_nest_lock (omp_nest_lock_t *) throw ();
  extern int omp_test_nest_lock (omp_nest_lock_t *) throw ();

  extern double omp_get_wtime (void) throw ();
  extern double omp_get_wtick (void) throw ();

  void omp_set_schedule (omp_sched_t, int) throw ();
  void omp_get_schedule (omp_sched_t *, int *) throw ();
  int omp_get_thread_limit (void) throw ();
  void omp_set_max_active_levels (int) throw ();
  int omp_get_max_active_levels (void) throw ();
  int omp_get_level (void) throw ();
  int omp_get_ancestor_thread_num (int) throw ();
  int omp_get_team_size (int) throw ();
  int omp_get_active_level (void) throw ();

  int omp_in_final (void) throw ();

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
                   << "FILE: "<< "../../../../installraja/include/RAJA/exec-openmp/forall_openmp.hxx" << " line: "<< 860 << std::endl;
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
        int numThreads = omp_get_num_threads() ;
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
          << "FILE: "<< "../../../../installraja/include/RAJA/exec-openmp/forall_openmp.hxx" << " line: "<< 1019 << std::endl;
        exit(1);
      }

      IndexSet& ncis = (*const_cast<IndexSet *>(&iset)) ;
      int num_seg = ncis.getNumSegments();

#pragma omp parallel

      {
        int numThreads = omp_get_num_threads() ;
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
          << "FILE: "<< "../../../../installraja/include/RAJA/exec-openmp/forall_openmp.hxx" << " line: "<< 1122 << std::endl;
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

  class IndexSet;
  void buildIndexSetAligned(IndexSet& hiset,
      const Index_type* const indices_in,
      Index_type length);
  void buildLockFreeBlockIndexset(IndexSet& iset,
      int fastDim, int midDim, int slowDim);
  void buildLockFreeColorIndexset(IndexSet& iset,
      int *domainToRange, int numEntity,
      int numRangePerDomain, int numEntityRange,
      int *elemPermutation = 0l,
      int *ielemPermutation = 0l);

}

namespace RAJA {

  class ChronoTimer {

    using clock = std::chrono::steady_clock;

    using TimeType = clock::time_point;
    using Duration = std::chrono::duration<long double, std::ratio<1>>;
    public:
    ChronoTimer() : tstart(clock::now()), tstop(clock::now()), telapsed(0) {}
    void start() { tstart = clock::now(); }
    void stop() {
      tstop = clock::now();
      telapsed += std::chrono::duration_cast<std::chrono::seconds>(tstop - tstart);
    }

    Duration::rep elapsed()
    { return telapsed.count(); }

    private:
    TimeType tstart;
    TimeType tstop;
    Duration telapsed;
  };

  using TimerBase = ChronoTimer;
}
namespace RAJA {

  class Timer : public TimerBase {
    public:
      using TimerBase::start;
      using TimerBase::stop;
      void start(const char*){
        start();
      }
      void stop(const char*){
        stop();
      }

  };

}

//------------- start of user code -------- Liao
bool show_run_progress = false;
const double lulesh_stop_time = 1.0e-2;
const double lulesh_time_step = -1.0e-7;

const int lulesh_edge_elems = 45;

const int lulesh_xtile = 2;
const int lulesh_ytile = 2;
const int lulesh_ztile = 2;

enum TilingMode
{
  Canonical,
  Tiled_Index,
  Tiled_Order,
  Tiled_LockFree,
  Tiled_LockFreeColor,
  Tiled_LockFreeColorSIMD
};
TilingMode const lulesh_tiling_mode = Tiled_LockFree;

typedef RAJA::omp_parallel_for_segit Hybrid_Seg_Iter;
typedef RAJA::simd_exec Segment_Exec;

typedef RAJA::IndexSet::ExecPolicy<RAJA::seq_segit, RAJA::omp_parallel_for_exec> node_exec_policy;
typedef RAJA::IndexSet::ExecPolicy<RAJA::omp_taskgraph_segit, RAJA::simd_exec> elem_exec_policy;
typedef RAJA::IndexSet::ExecPolicy<RAJA::omp_taskgraph_segit, RAJA::simd_exec> mat_exec_policy;
typedef RAJA::IndexSet::ExecPolicy<RAJA::seq_segit, RAJA::omp_parallel_for_exec> symnode_exec_policy;

typedef RAJA::omp_reduce reduce_policy;

typedef RAJA::Index_type Index_t ;
typedef RAJA::Real_type Real_t ;
typedef RAJA::Real_ptr Real_p;
typedef RAJA::const_Real_ptr const_Real_p;
typedef RAJA::Index_type* Index_p;
typedef float real4 ;
typedef double real8 ;
typedef long double real10 ;

typedef int Int_t ;

inline
real4 SQRT(real4 arg) { return sqrtf(arg) ; }
inline
real8 SQRT(real8 arg) { return sqrt(arg) ; }
inline
real10 SQRT(real10 arg) { return sqrtl(arg) ; }

inline
real4 CBRT(real4 arg) { return cbrtf(arg) ; }
inline
real8 CBRT(real8 arg) { return cbrt(arg) ; }
inline
real10 CBRT(real10 arg) { return cbrtl(arg) ; }

inline
real4 FABS(real4 arg) { return fabsf(arg) ; }
inline
real8 FABS(real8 arg) { return fabs(arg) ; }
inline
real10 FABS(real10 arg) { return fabsl(arg) ; }

  template <typename T>
inline T *Allocate(size_t size)
{
  T *retVal ;
  posix_memalign((void **)&retVal, RAJA::DATA_ALIGN, sizeof(T)*size);

  return retVal ;
}

  template <typename EXEC_POLICY_T, typename T>
inline T *AllocateTouch(RAJA::IndexSet *is, size_t size)
{
  T *retVal ;
  posix_memalign((void **)&retVal, RAJA::DATA_ALIGN, sizeof(T)*size);

  RAJA::forall<EXEC_POLICY_T>( *is, [=] (int i) {
      retVal[i] = 0 ;
      } ) ;

  return retVal ;
}

template <typename T>
inline void Release(T **ptr)
{
   if (*ptr != __null) {
      free(*ptr) ;
      *ptr = __null ;
   }
}

#if 0
template <typename T>
inline void Release(T * __restrict__ *ptr)
{
   if (*ptr != __null) {
      free(*ptr) ;
      *ptr = __null ;
   }
}
#endif
namespace RAJA {

  template <typename VARTYPE >
    struct MemoryPool {
      public:
        MemoryPool()
        {
          for (int i=0; i<32; ++i) {
            lenType[i] = 0 ;
            ptr[i] = 0 ;
          }
        }

        VARTYPE *allocate(int len) {
          VARTYPE *retVal ;
          int i ;
          for (i=0; i<32; ++i) {
            if (lenType[i] == len) {
              lenType[i] = -lenType[i] ;
              retVal = ptr[i] ;
              break ;
            }
            else if (lenType[i] == 0) {
              lenType[i] = -len ;
              ptr[i] = Allocate<VARTYPE>(len) ;
              retVal = ptr[i] ;
              break ;
            }
          }
          if (i == 32) {
            retVal = 0 ;
          }
          return retVal ;
        }

        bool release(VARTYPE **oldPtr) {
          int i ;
          bool success = true ;
          for (i=0; i<32; ++i) {
            if (ptr[i] == *oldPtr) {
              lenType[i] = -lenType[i] ;
              *oldPtr = 0 ;
              break ;
            }
          }
          if (i == 32) {
            success = false ;
          }
          return success ;
        }

#if 0
        bool release(VARTYPE * __restrict__ *oldPtr) {
          int i ;
          bool success = true ;
          for (i=0; i<32; ++i) {
            if (ptr[i] == *oldPtr) {
              lenType[i] = -lenType[i] ;
              *oldPtr = 0 ;
              break ;
            }
          }
          if (i == 32) {
            success = false ;
          }
          return success ;
        }
#endif        

        VARTYPE *ptr[32] ;
        int lenType[32] ;
    } ;

}

enum { VolumeError = -1, QStopError = -2 } ;

RAJA::MemoryPool< Real_t > elemMemPool ;
struct Domain {

  RAJA::IndexSet *domElemList ;
  RAJA::IndexSet *matElemList ;
  Index_p nodelist ;

  Index_p lxim ;
  Index_p lxip ;
  Index_p letam ;
  Index_p letap ;
  Index_p lzetam ;
  Index_p lzetap ;

  Int_t *elemBC ;

  Real_p e ;

  Real_p p ;

  Real_p q ;
  Real_p ql ;
  Real_p qq ;

  Real_p v ;

  Real_p volo ;
  Real_p delv ;
  Real_p vdov ;

  Real_p arealg ;

  Real_p ss ;

  Real_p elemMass ;

  Real_p vnew ;

  Real_p delv_xi ;
  Real_p delv_eta ;
  Real_p delv_zeta ;

  Real_p delx_xi ;
  Real_p delx_eta ;
  Real_p delx_zeta ;

  Real_p dxx ;
  Real_p dyy ;
  Real_p dzz ;

  RAJA::IndexSet *domNodeList ;

  RAJA::IndexSet *symmX ;
  RAJA::IndexSet *symmY ;
  RAJA::IndexSet *symmZ ;

  Real_p x ;
  Real_p y ;
  Real_p z ;

  Real_p xd ;
  Real_p yd ;
  Real_p zd ;

  Real_p xdd ;
  Real_p ydd ;
  Real_p zdd ;

  Real_p fx ;
  Real_p fy ;
  Real_p fz ;

  Real_p nodalMass ;
  Real_t dtfixed ;
  Real_t time ;
  Real_t deltatime ;
  Real_t deltatimemultlb ;
  Real_t deltatimemultub ;
  Real_t stoptime ;

  Real_t u_cut ;
  Real_t hgcoef ;
  Real_t qstop ;
  Real_t monoq_max_slope ;
  Real_t monoq_limiter_mult ;
  Real_t e_cut ;
  Real_t p_cut ;
  Real_t ss4o3 ;
  Real_t q_cut ;
  Real_t v_cut ;
  Real_t qlc_monoq ;
  Real_t qqc_monoq ;
  Real_t qqc ;
  Real_t eosvmax ;
  Real_t eosvmin ;
  Real_t pmin ;
  Real_t emin ;
  Real_t dvovmax ;
  Real_t refdens ;

  Real_t dtcourant ;
  Real_t dthydro ;
  Real_t dtmax ;

  Int_t cycle ;

  Index_t sizeX ;
  Index_t sizeY ;
  Index_t sizeZ ;
  Index_t numElem ;

  Index_t numNode ;

  Index_t idx ;
} ;



// first function
void CalcHydroConstraintForElems(RAJA::IndexSet *matElemList, Real_p vdov,
    Real_t dvovmax, Real_t *dthydro)
{
  RAJA::ReduceMin<reduce_policy, Real_t> dthydroLoc(Real_t(1.0e+20)) ;

  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int indx) {

      Real_t dtvov_cmp = (vdov[indx] != Real_t(0.))
      ? (dvovmax / (FABS(vdov[indx])+Real_t(1.e-20)))
      : Real_t(1.0e+10) ;

      dthydroLoc.min(dtvov_cmp) ;
      } ) ;

  Real_t result = Real_t(dthydroLoc);
  if (result < Real_t(1.0e+20)) {
    *dthydro = result ;
  }

  return ;
}



void TimeIncrement(Domain *domain)
{
  Real_t targetdt = domain->stoptime - domain->time ;

  if ((domain->dtfixed <= Real_t(0.0)) && (domain->cycle != Int_t(0))) {
    Real_t ratio ;
    Real_t olddt = domain->deltatime ;

    Real_t newdt = Real_t(1.0e+20) ;
    if (domain->dtcourant < newdt) {
      newdt = domain->dtcourant / Real_t(2.0) ;
    }
    if (domain->dthydro < newdt) {
      newdt = domain->dthydro * Real_t(2.0) / Real_t(3.0) ;
    }

    ratio = newdt / olddt ;
    if (ratio >= Real_t(1.0)) {
      if (ratio < domain->deltatimemultlb) {
        newdt = olddt ;
      }
      else if (ratio > domain->deltatimemultub) {
        newdt = olddt*domain->deltatimemultub ;
      }
    }

    if (newdt > domain->dtmax) {
      newdt = domain->dtmax ;
    }
    domain->deltatime = newdt ;
  }

  if ((targetdt > domain->deltatime) &&
      (targetdt < (Real_t(4.0) * domain->deltatime / Real_t(3.0))) ) {
    targetdt = Real_t(2.0) * domain->deltatime / Real_t(3.0) ;
  }

  if (targetdt < domain->deltatime) {
    domain->deltatime = targetdt ;
  }

  domain->time += domain->deltatime ;

  ++domain->cycle ;
}

void InitStressTermsForElems(Real_p p, Real_p q,
    Real_p sigxx, Real_p sigyy, Real_p sigzz,
    RAJA::IndexSet *domElemList)
{

  RAJA::forall<elem_exec_policy>(*domElemList, [=] (int idx) {
      sigxx[idx] = sigyy[idx] = sigzz[idx] = - p[idx] - q[idx] ;
      }
      ) ;
}

void CalcElemShapeFunctionDerivatives( const_Real_p x,
    const_Real_p y,
    const_Real_p z,
    Real_t b[][8],
    Real_t* const volume
    )
{
  const Real_t x0 = x[0] ; const Real_t x1 = x[1] ;
  const Real_t x2 = x[2] ; const Real_t x3 = x[3] ;
  const Real_t x4 = x[4] ; const Real_t x5 = x[5] ;
  const Real_t x6 = x[6] ; const Real_t x7 = x[7] ;

  const Real_t y0 = y[0] ; const Real_t y1 = y[1] ;
  const Real_t y2 = y[2] ; const Real_t y3 = y[3] ;
  const Real_t y4 = y[4] ; const Real_t y5 = y[5] ;
  const Real_t y6 = y[6] ; const Real_t y7 = y[7] ;

  const Real_t z0 = z[0] ; const Real_t z1 = z[1] ;
  const Real_t z2 = z[2] ; const Real_t z3 = z[3] ;
  const Real_t z4 = z[4] ; const Real_t z5 = z[5] ;
  const Real_t z6 = z[6] ; const Real_t z7 = z[7] ;

  Real_t fjxxi, fjxet, fjxze;
  Real_t fjyxi, fjyet, fjyze;
  Real_t fjzxi, fjzet, fjzze;
  Real_t cjxxi, cjxet, cjxze;
  Real_t cjyxi, cjyet, cjyze;
  Real_t cjzxi, cjzet, cjzze;

  fjxxi = Real_t(.125) * ( (x6-x0) + (x5-x3) - (x7-x1) - (x4-x2) );
  fjxet = Real_t(.125) * ( (x6-x0) - (x5-x3) + (x7-x1) - (x4-x2) );
  fjxze = Real_t(.125) * ( (x6-x0) + (x5-x3) + (x7-x1) + (x4-x2) );

  fjyxi = Real_t(.125) * ( (y6-y0) + (y5-y3) - (y7-y1) - (y4-y2) );
  fjyet = Real_t(.125) * ( (y6-y0) - (y5-y3) + (y7-y1) - (y4-y2) );
  fjyze = Real_t(.125) * ( (y6-y0) + (y5-y3) + (y7-y1) + (y4-y2) );

  fjzxi = Real_t(.125) * ( (z6-z0) + (z5-z3) - (z7-z1) - (z4-z2) );
  fjzet = Real_t(.125) * ( (z6-z0) - (z5-z3) + (z7-z1) - (z4-z2) );
  fjzze = Real_t(.125) * ( (z6-z0) + (z5-z3) + (z7-z1) + (z4-z2) );

  cjxxi = (fjyet * fjzze) - (fjzet * fjyze);
  cjxet = - (fjyxi * fjzze) + (fjzxi * fjyze);
  cjxze = (fjyxi * fjzet) - (fjzxi * fjyet);

  cjyxi = - (fjxet * fjzze) + (fjzet * fjxze);
  cjyet = (fjxxi * fjzze) - (fjzxi * fjxze);
  cjyze = - (fjxxi * fjzet) + (fjzxi * fjxet);

  cjzxi = (fjxet * fjyze) - (fjyet * fjxze);
  cjzet = - (fjxxi * fjyze) + (fjyxi * fjxze);
  cjzze = (fjxxi * fjyet) - (fjyxi * fjxet);

  b[0][0] = - cjxxi - cjxet - cjxze;
  b[0][1] = cjxxi - cjxet - cjxze;
  b[0][2] = cjxxi + cjxet - cjxze;
  b[0][3] = - cjxxi + cjxet - cjxze;
  b[0][4] = -b[0][2];
  b[0][5] = -b[0][3];
  b[0][6] = -b[0][0];
  b[0][7] = -b[0][1];

  b[1][0] = - cjyxi - cjyet - cjyze;
  b[1][1] = cjyxi - cjyet - cjyze;
  b[1][2] = cjyxi + cjyet - cjyze;
  b[1][3] = - cjyxi + cjyet - cjyze;
  b[1][4] = -b[1][2];
  b[1][5] = -b[1][3];
  b[1][6] = -b[1][0];
  b[1][7] = -b[1][1];

  b[2][0] = - cjzxi - cjzet - cjzze;
  b[2][1] = cjzxi - cjzet - cjzze;
  b[2][2] = cjzxi + cjzet - cjzze;
  b[2][3] = - cjzxi + cjzet - cjzze;
  b[2][4] = -b[2][2];
  b[2][5] = -b[2][3];
  b[2][6] = -b[2][0];
  b[2][7] = -b[2][1];

  *volume = Real_t(8.) * ( fjxet * cjxet + fjyet * cjyet + fjzet * cjzet);
}

void SumElemFaceNormal(Real_t *normalX0, Real_t *normalY0, Real_t *normalZ0,
    Real_t *normalX1, Real_t *normalY1, Real_t *normalZ1,
    Real_t *normalX2, Real_t *normalY2, Real_t *normalZ2,
    Real_t *normalX3, Real_t *normalY3, Real_t *normalZ3,
    const Real_t x0, const Real_t y0, const Real_t z0,
    const Real_t x1, const Real_t y1, const Real_t z1,
    const Real_t x2, const Real_t y2, const Real_t z2,
    const Real_t x3, const Real_t y3, const Real_t z3)
{
  Real_t bisectX0 = Real_t(0.5) * (x3 + x2 - x1 - x0);
  Real_t bisectY0 = Real_t(0.5) * (y3 + y2 - y1 - y0);
  Real_t bisectZ0 = Real_t(0.5) * (z3 + z2 - z1 - z0);
  Real_t bisectX1 = Real_t(0.5) * (x2 + x1 - x3 - x0);
  Real_t bisectY1 = Real_t(0.5) * (y2 + y1 - y3 - y0);
  Real_t bisectZ1 = Real_t(0.5) * (z2 + z1 - z3 - z0);
  Real_t areaX = Real_t(0.25) * (bisectY0 * bisectZ1 - bisectZ0 * bisectY1);
  Real_t areaY = Real_t(0.25) * (bisectZ0 * bisectX1 - bisectX0 * bisectZ1);
  Real_t areaZ = Real_t(0.25) * (bisectX0 * bisectY1 - bisectY0 * bisectX1);

  *normalX0 += areaX;
  *normalX1 += areaX;
  *normalX2 += areaX;
  *normalX3 += areaX;

  *normalY0 += areaY;
  *normalY1 += areaY;
  *normalY2 += areaY;
  *normalY3 += areaY;

  *normalZ0 += areaZ;
  *normalZ1 += areaZ;
  *normalZ2 += areaZ;
  *normalZ3 += areaZ;
}

void CalcElemNodeNormals(
    Real_p pfx,
    Real_p pfy,
    Real_p pfz,
    const_Real_p x,
    const_Real_p y,
    const_Real_p z
    )
{
  for (Index_t i = 0 ; i < 8 ; ++i) {
    pfx[i] = Real_t(0.0);
    pfy[i] = Real_t(0.0);
    pfz[i] = Real_t(0.0);
  }

  SumElemFaceNormal(&pfx[0], &pfy[0], &pfz[0],
      &pfx[1], &pfy[1], &pfz[1],
      &pfx[2], &pfy[2], &pfz[2],
      &pfx[3], &pfy[3], &pfz[3],
      x[0], y[0], z[0], x[1], y[1], z[1],
      x[2], y[2], z[2], x[3], y[3], z[3]);

  SumElemFaceNormal(&pfx[0], &pfy[0], &pfz[0],
      &pfx[4], &pfy[4], &pfz[4],
      &pfx[5], &pfy[5], &pfz[5],
      &pfx[1], &pfy[1], &pfz[1],
      x[0], y[0], z[0], x[4], y[4], z[4],
      x[5], y[5], z[5], x[1], y[1], z[1]);

  SumElemFaceNormal(&pfx[1], &pfy[1], &pfz[1],
      &pfx[5], &pfy[5], &pfz[5],
      &pfx[6], &pfy[6], &pfz[6],
      &pfx[2], &pfy[2], &pfz[2],
      x[1], y[1], z[1], x[5], y[5], z[5],
      x[6], y[6], z[6], x[2], y[2], z[2]);

  SumElemFaceNormal(&pfx[2], &pfy[2], &pfz[2],
      &pfx[6], &pfy[6], &pfz[6],
      &pfx[7], &pfy[7], &pfz[7],
      &pfx[3], &pfy[3], &pfz[3],
      x[2], y[2], z[2], x[6], y[6], z[6],
      x[7], y[7], z[7], x[3], y[3], z[3]);

  SumElemFaceNormal(&pfx[3], &pfy[3], &pfz[3],
      &pfx[7], &pfy[7], &pfz[7],
      &pfx[4], &pfy[4], &pfz[4],
      &pfx[0], &pfy[0], &pfz[0],
      x[3], y[3], z[3], x[7], y[7], z[7],
      x[4], y[4], z[4], x[0], y[0], z[0]);

  SumElemFaceNormal(&pfx[4], &pfy[4], &pfz[4],
      &pfx[7], &pfy[7], &pfz[7],
      &pfx[6], &pfy[6], &pfz[6],
      &pfx[5], &pfy[5], &pfz[5],
      x[4], y[4], z[4], x[7], y[7], z[7],
      x[6], y[6], z[6], x[5], y[5], z[5]);
}

void SumElemStressesToNodeForces( const Real_t B[][8],
    const Real_t stress_xx,
    const Real_t stress_yy,
    const Real_t stress_zz,
    Real_p fx, Real_p fy, Real_p fz
    )
{
  Real_t pfx0 = B[0][0] ; Real_t pfx1 = B[0][1] ;
  Real_t pfx2 = B[0][2] ; Real_t pfx3 = B[0][3] ;
  Real_t pfx4 = B[0][4] ; Real_t pfx5 = B[0][5] ;
  Real_t pfx6 = B[0][6] ; Real_t pfx7 = B[0][7] ;

  Real_t pfy0 = B[1][0] ; Real_t pfy1 = B[1][1] ;
  Real_t pfy2 = B[1][2] ; Real_t pfy3 = B[1][3] ;
  Real_t pfy4 = B[1][4] ; Real_t pfy5 = B[1][5] ;
  Real_t pfy6 = B[1][6] ; Real_t pfy7 = B[1][7] ;

  Real_t pfz0 = B[2][0] ; Real_t pfz1 = B[2][1] ;
  Real_t pfz2 = B[2][2] ; Real_t pfz3 = B[2][3] ;
  Real_t pfz4 = B[2][4] ; Real_t pfz5 = B[2][5] ;
  Real_t pfz6 = B[2][6] ; Real_t pfz7 = B[2][7] ;

  fx[0] = -( stress_xx * pfx0 );
  fx[1] = -( stress_xx * pfx1 );
  fx[2] = -( stress_xx * pfx2 );
  fx[3] = -( stress_xx * pfx3 );
  fx[4] = -( stress_xx * pfx4 );
  fx[5] = -( stress_xx * pfx5 );
  fx[6] = -( stress_xx * pfx6 );
  fx[7] = -( stress_xx * pfx7 );

  fy[0] = -( stress_yy * pfy0 );
  fy[1] = -( stress_yy * pfy1 );
  fy[2] = -( stress_yy * pfy2 );
  fy[3] = -( stress_yy * pfy3 );
  fy[4] = -( stress_yy * pfy4 );
  fy[5] = -( stress_yy * pfy5 );
  fy[6] = -( stress_yy * pfy6 );
  fy[7] = -( stress_yy * pfy7 );

  fz[0] = -( stress_zz * pfz0 );
  fz[1] = -( stress_zz * pfz1 );
  fz[2] = -( stress_zz * pfz2 );
  fz[3] = -( stress_zz * pfz3 );
  fz[4] = -( stress_zz * pfz4 );
  fz[5] = -( stress_zz * pfz5 );
  fz[6] = -( stress_zz * pfz6 );
  fz[7] = -( stress_zz * pfz7 );
}

void IntegrateStressForElems( Index_p nodelist,
    Real_p x, Real_p y, Real_p z,
    Real_p fx, Real_p fy, Real_p fz,
    Real_p sigxx, Real_p sigyy, Real_p sigzz,
    Real_p determ,
    RAJA::IndexSet *domElemList,
    RAJA::IndexSet *domNodeList

    )
{

  RAJA::forall<elem_exec_policy>(*domElemList, [=] (int k) {
      Real_t B[3][8] __attribute__((aligned(32))) ;
      Real_t x_local[8] ;
      Real_t y_local[8] ;
      Real_t z_local[8] ;

      Real_t fx_local[8] ;
      Real_t fy_local[8] ;
      Real_t fz_local[8] ;

      const Index_p elemNodes = &nodelist[8*k];

      for( Index_t lnode=0 ; lnode<8 ; ++lnode )
      {
      Index_t gnode = elemNodes[lnode];
      x_local[lnode] = x[gnode];
      y_local[lnode] = y[gnode];
      z_local[lnode] = z[gnode];
      }

      CalcElemShapeFunctionDerivatives(x_local, y_local, z_local,
        B, &determ[k]);

      CalcElemNodeNormals( B[0] , B[1], B[2],
          x_local, y_local, z_local );

      SumElemStressesToNodeForces( B, sigxx[k], sigyy[k], sigzz[k],

          fx_local, fy_local, fz_local

          ) ;

      for( Index_t lnode=0 ; lnode<8 ; ++lnode )
      {
        Index_t gnode = elemNodes[lnode];
        fx[gnode] += fx_local[lnode] ;
        fy[gnode] += fy_local[lnode] ;
        fz[gnode] += fz_local[lnode] ;
      }

  }
  ) ;
}

void CollectDomainNodesToElemNodes(Real_p x, Real_p y, Real_p z,
    Index_p elemToNode,
    Real_p elemX,
    Real_p elemY,
    Real_p elemZ
    )
{
  Index_t nd0i = elemToNode[0] ;
  Index_t nd1i = elemToNode[1] ;
  Index_t nd2i = elemToNode[2] ;
  Index_t nd3i = elemToNode[3] ;
  Index_t nd4i = elemToNode[4] ;
  Index_t nd5i = elemToNode[5] ;
  Index_t nd6i = elemToNode[6] ;
  Index_t nd7i = elemToNode[7] ;

  elemX[0] = x[nd0i];
  elemX[1] = x[nd1i];
  elemX[2] = x[nd2i];
  elemX[3] = x[nd3i];
  elemX[4] = x[nd4i];
  elemX[5] = x[nd5i];
  elemX[6] = x[nd6i];
  elemX[7] = x[nd7i];

  elemY[0] = y[nd0i];
  elemY[1] = y[nd1i];
  elemY[2] = y[nd2i];
  elemY[3] = y[nd3i];
  elemY[4] = y[nd4i];
  elemY[5] = y[nd5i];
  elemY[6] = y[nd6i];
  elemY[7] = y[nd7i];

  elemZ[0] = z[nd0i];
  elemZ[1] = z[nd1i];
  elemZ[2] = z[nd2i];
  elemZ[3] = z[nd3i];
  elemZ[4] = z[nd4i];
  elemZ[5] = z[nd5i];
  elemZ[6] = z[nd6i];
  elemZ[7] = z[nd7i];

}

void VoluDer(const Real_t x0, const Real_t x1, const Real_t x2,
    const Real_t x3, const Real_t x4, const Real_t x5,
    const Real_t y0, const Real_t y1, const Real_t y2,
    const Real_t y3, const Real_t y4, const Real_t y5,
    const Real_t z0, const Real_t z1, const Real_t z2,
    const Real_t z3, const Real_t z4, const Real_t z5,
    Real_t* dvdx, Real_t* dvdy, Real_t* dvdz)
{
  const Real_t twelfth = Real_t(1.0) / Real_t(12.0) ;

  *dvdx =
    (y1 + y2) * (z0 + z1) - (y0 + y1) * (z1 + z2) +
    (y0 + y4) * (z3 + z4) - (y3 + y4) * (z0 + z4) -
    (y2 + y5) * (z3 + z5) + (y3 + y5) * (z2 + z5);
  *dvdy =
    - (x1 + x2) * (z0 + z1) + (x0 + x1) * (z1 + z2) -
    (x0 + x4) * (z3 + z4) + (x3 + x4) * (z0 + z4) +
    (x2 + x5) * (z3 + z5) - (x3 + x5) * (z2 + z5);

  *dvdz =
    - (y1 + y2) * (x0 + x1) + (y0 + y1) * (x1 + x2) -
    (y0 + y4) * (x3 + x4) + (y3 + y4) * (x0 + x4) +
    (y2 + y5) * (x3 + x5) - (y3 + y5) * (x2 + x5);

  *dvdx *= twelfth;
  *dvdy *= twelfth;
  *dvdz *= twelfth;
}

void CalcElemVolumeDerivative(
    Real_p dvdx,
    Real_p dvdy,
    Real_p dvdz,
    const_Real_p x,
    const_Real_p y,
    const_Real_p z
    )
{
  VoluDer(x[1], x[2], x[3], x[4], x[5], x[7],
      y[1], y[2], y[3], y[4], y[5], y[7],
      z[1], z[2], z[3], z[4], z[5], z[7],
      &dvdx[0], &dvdy[0], &dvdz[0]);
  VoluDer(x[0], x[1], x[2], x[7], x[4], x[6],
      y[0], y[1], y[2], y[7], y[4], y[6],
      z[0], z[1], z[2], z[7], z[4], z[6],
      &dvdx[3], &dvdy[3], &dvdz[3]);
  VoluDer(x[3], x[0], x[1], x[6], x[7], x[5],
      y[3], y[0], y[1], y[6], y[7], y[5],
      z[3], z[0], z[1], z[6], z[7], z[5],
      &dvdx[2], &dvdy[2], &dvdz[2]);
  VoluDer(x[2], x[3], x[0], x[5], x[6], x[4],
      y[2], y[3], y[0], y[5], y[6], y[4],
      z[2], z[3], z[0], z[5], z[6], z[4],
      &dvdx[1], &dvdy[1], &dvdz[1]);
  VoluDer(x[7], x[6], x[5], x[0], x[3], x[1],
      y[7], y[6], y[5], y[0], y[3], y[1],
      z[7], z[6], z[5], z[0], z[3], z[1],
      &dvdx[4], &dvdy[4], &dvdz[4]);
  VoluDer(x[4], x[7], x[6], x[1], x[0], x[2],
      y[4], y[7], y[6], y[1], y[0], y[2],
      z[4], z[7], z[6], z[1], z[0], z[2],
      &dvdx[5], &dvdy[5], &dvdz[5]);
  VoluDer(x[5], x[4], x[7], x[2], x[1], x[3],
      y[5], y[4], y[7], y[2], y[1], y[3],
      z[5], z[4], z[7], z[2], z[1], z[3],
      &dvdx[6], &dvdy[6], &dvdz[6]);
  VoluDer(x[6], x[5], x[4], x[3], x[2], x[0],
      y[6], y[5], y[4], y[3], y[2], y[0],
      z[6], z[5], z[4], z[3], z[2], z[0],
      &dvdx[7], &dvdy[7], &dvdz[7]);
}

void CalcElemFBHourglassForce(
    Real_p xd, Real_p yd, Real_p zd,
    Real_p hourgam0, Real_p hourgam1,
    Real_p hourgam2, Real_p hourgam3,
    Real_p hourgam4, Real_p hourgam5,
    Real_p hourgam6, Real_p hourgam7,
    Real_t coefficient,
    Real_p hgfx, Real_p hgfy, Real_p hgfz
    )
{
  const Index_t i00=0;
  const Index_t i01=1;
  const Index_t i02=2;
  const Index_t i03=3;

  Real_t h00 =
    hourgam0[i00] * xd[0] + hourgam1[i00] * xd[1] +
    hourgam2[i00] * xd[2] + hourgam3[i00] * xd[3] +
    hourgam4[i00] * xd[4] + hourgam5[i00] * xd[5] +
    hourgam6[i00] * xd[6] + hourgam7[i00] * xd[7];

  Real_t h01 =
    hourgam0[i01] * xd[0] + hourgam1[i01] * xd[1] +
    hourgam2[i01] * xd[2] + hourgam3[i01] * xd[3] +
    hourgam4[i01] * xd[4] + hourgam5[i01] * xd[5] +
    hourgam6[i01] * xd[6] + hourgam7[i01] * xd[7];

  Real_t h02 =
    hourgam0[i02] * xd[0] + hourgam1[i02] * xd[1]+
    hourgam2[i02] * xd[2] + hourgam3[i02] * xd[3]+
    hourgam4[i02] * xd[4] + hourgam5[i02] * xd[5]+
    hourgam6[i02] * xd[6] + hourgam7[i02] * xd[7];

  Real_t h03 =
    hourgam0[i03] * xd[0] + hourgam1[i03] * xd[1] +
    hourgam2[i03] * xd[2] + hourgam3[i03] * xd[3] +
    hourgam4[i03] * xd[4] + hourgam5[i03] * xd[5] +
    hourgam6[i03] * xd[6] + hourgam7[i03] * xd[7];

  hgfx[0] = coefficient *
    (hourgam0[i00] * h00 + hourgam0[i01] * h01 +
     hourgam0[i02] * h02 + hourgam0[i03] * h03);

  hgfx[1] = coefficient *
    (hourgam1[i00] * h00 + hourgam1[i01] * h01 +
     hourgam1[i02] * h02 + hourgam1[i03] * h03);

  hgfx[2] = coefficient *
    (hourgam2[i00] * h00 + hourgam2[i01] * h01 +
     hourgam2[i02] * h02 + hourgam2[i03] * h03);

  hgfx[3] = coefficient *
    (hourgam3[i00] * h00 + hourgam3[i01] * h01 +
     hourgam3[i02] * h02 + hourgam3[i03] * h03);

  hgfx[4] = coefficient *
    (hourgam4[i00] * h00 + hourgam4[i01] * h01 +
     hourgam4[i02] * h02 + hourgam4[i03] * h03);

  hgfx[5] = coefficient *
    (hourgam5[i00] * h00 + hourgam5[i01] * h01 +
     hourgam5[i02] * h02 + hourgam5[i03] * h03);

  hgfx[6] = coefficient *
    (hourgam6[i00] * h00 + hourgam6[i01] * h01 +
     hourgam6[i02] * h02 + hourgam6[i03] * h03);

  hgfx[7] = coefficient *
    (hourgam7[i00] * h00 + hourgam7[i01] * h01 +
     hourgam7[i02] * h02 + hourgam7[i03] * h03);

  h00 =
    hourgam0[i00] * yd[0] + hourgam1[i00] * yd[1] +
    hourgam2[i00] * yd[2] + hourgam3[i00] * yd[3] +
    hourgam4[i00] * yd[4] + hourgam5[i00] * yd[5] +
    hourgam6[i00] * yd[6] + hourgam7[i00] * yd[7];

  h01 =
    hourgam0[i01] * yd[0] + hourgam1[i01] * yd[1] +
    hourgam2[i01] * yd[2] + hourgam3[i01] * yd[3] +
    hourgam4[i01] * yd[4] + hourgam5[i01] * yd[5] +
    hourgam6[i01] * yd[6] + hourgam7[i01] * yd[7];

  h02 =
    hourgam0[i02] * yd[0] + hourgam1[i02] * yd[1]+
    hourgam2[i02] * yd[2] + hourgam3[i02] * yd[3]+
    hourgam4[i02] * yd[4] + hourgam5[i02] * yd[5]+
    hourgam6[i02] * yd[6] + hourgam7[i02] * yd[7];

  h03 =
    hourgam0[i03] * yd[0] + hourgam1[i03] * yd[1] +
    hourgam2[i03] * yd[2] + hourgam3[i03] * yd[3] +
    hourgam4[i03] * yd[4] + hourgam5[i03] * yd[5] +
    hourgam6[i03] * yd[6] + hourgam7[i03] * yd[7];

  hgfy[0] = coefficient *
    (hourgam0[i00] * h00 + hourgam0[i01] * h01 +
     hourgam0[i02] * h02 + hourgam0[i03] * h03);

  hgfy[1] = coefficient *
    (hourgam1[i00] * h00 + hourgam1[i01] * h01 +
     hourgam1[i02] * h02 + hourgam1[i03] * h03);

  hgfy[2] = coefficient *
    (hourgam2[i00] * h00 + hourgam2[i01] * h01 +
     hourgam2[i02] * h02 + hourgam2[i03] * h03);

  hgfy[3] = coefficient *
    (hourgam3[i00] * h00 + hourgam3[i01] * h01 +
     hourgam3[i02] * h02 + hourgam3[i03] * h03);

  hgfy[4] = coefficient *
    (hourgam4[i00] * h00 + hourgam4[i01] * h01 +
     hourgam4[i02] * h02 + hourgam4[i03] * h03);

  hgfy[5] = coefficient *
    (hourgam5[i00] * h00 + hourgam5[i01] * h01 +
     hourgam5[i02] * h02 + hourgam5[i03] * h03);

  hgfy[6] = coefficient *
    (hourgam6[i00] * h00 + hourgam6[i01] * h01 +
     hourgam6[i02] * h02 + hourgam6[i03] * h03);

  hgfy[7] = coefficient *
    (hourgam7[i00] * h00 + hourgam7[i01] * h01 +
     hourgam7[i02] * h02 + hourgam7[i03] * h03);

  h00 =
    hourgam0[i00] * zd[0] + hourgam1[i00] * zd[1] +
    hourgam2[i00] * zd[2] + hourgam3[i00] * zd[3] +
    hourgam4[i00] * zd[4] + hourgam5[i00] * zd[5] +
    hourgam6[i00] * zd[6] + hourgam7[i00] * zd[7];

  h01 =
    hourgam0[i01] * zd[0] + hourgam1[i01] * zd[1] +
    hourgam2[i01] * zd[2] + hourgam3[i01] * zd[3] +
    hourgam4[i01] * zd[4] + hourgam5[i01] * zd[5] +
    hourgam6[i01] * zd[6] + hourgam7[i01] * zd[7];

  h02 =
    hourgam0[i02] * zd[0] + hourgam1[i02] * zd[1]+
    hourgam2[i02] * zd[2] + hourgam3[i02] * zd[3]+
    hourgam4[i02] * zd[4] + hourgam5[i02] * zd[5]+
    hourgam6[i02] * zd[6] + hourgam7[i02] * zd[7];

  h03 =
    hourgam0[i03] * zd[0] + hourgam1[i03] * zd[1] +
    hourgam2[i03] * zd[2] + hourgam3[i03] * zd[3] +
    hourgam4[i03] * zd[4] + hourgam5[i03] * zd[5] +
    hourgam6[i03] * zd[6] + hourgam7[i03] * zd[7];

  hgfz[0] = coefficient *
    (hourgam0[i00] * h00 + hourgam0[i01] * h01 +
     hourgam0[i02] * h02 + hourgam0[i03] * h03);

  hgfz[1] = coefficient *
    (hourgam1[i00] * h00 + hourgam1[i01] * h01 +
     hourgam1[i02] * h02 + hourgam1[i03] * h03);

  hgfz[2] = coefficient *
    (hourgam2[i00] * h00 + hourgam2[i01] * h01 +
     hourgam2[i02] * h02 + hourgam2[i03] * h03);

  hgfz[3] = coefficient *
    (hourgam3[i00] * h00 + hourgam3[i01] * h01 +
     hourgam3[i02] * h02 + hourgam3[i03] * h03);

  hgfz[4] = coefficient *
    (hourgam4[i00] * h00 + hourgam4[i01] * h01 +
     hourgam4[i02] * h02 + hourgam4[i03] * h03);

  hgfz[5] = coefficient *
    (hourgam5[i00] * h00 + hourgam5[i01] * h01 +
     hourgam5[i02] * h02 + hourgam5[i03] * h03);

  hgfz[6] = coefficient *
    (hourgam6[i00] * h00 + hourgam6[i01] * h01 +
     hourgam6[i02] * h02 + hourgam6[i03] * h03);

  hgfz[7] = coefficient *
    (hourgam7[i00] * h00 + hourgam7[i01] * h01 +
     hourgam7[i02] * h02 + hourgam7[i03] * h03);
}

void CalcFBHourglassForceForElems( Index_p nodelist,
    Real_p ss, Real_p elemMass,
    Real_p xd, Real_p yd, Real_p zd,
    Real_p fx, Real_p fy, Real_p fz,
    Real_p determ,
    Real_p x8n, Real_p y8n, Real_p z8n,
    Real_p dvdx, Real_p dvdy, Real_p dvdz,
    Real_t hourg,
    RAJA::IndexSet *domElemList,
    RAJA::IndexSet *domNodeList

    )
{
  RAJA::forall<elem_exec_policy>(*domElemList, [=] (int i2) {

      Real_t hgfx[8], hgfy[8], hgfz[8] ;

      Real_t coefficient;

      Real_t hourgam0[4], hourgam1[4], hourgam2[4], hourgam3[4] ;
      Real_t hourgam4[4], hourgam5[4], hourgam6[4], hourgam7[4];
      Real_t xd1[8], yd1[8], zd1[8] ;

      const Real_t ggamma[4][8] =
      {
      { Real_t( 1.), Real_t( 1.), Real_t(-1.), Real_t(-1.),
      Real_t(-1.), Real_t(-1.), Real_t( 1.), Real_t( 1.) },

      { Real_t( 1.), Real_t(-1.), Real_t(-1.), Real_t( 1.),
      Real_t(-1.), Real_t( 1.), Real_t( 1.), Real_t(-1.) },

      { Real_t( 1.), Real_t(-1.), Real_t( 1.), Real_t(-1.),
      Real_t( 1.), Real_t(-1.), Real_t( 1.), Real_t(-1.) },

      { Real_t(-1.), Real_t( 1.), Real_t(-1.), Real_t( 1.),
        Real_t( 1.), Real_t(-1.), Real_t( 1.), Real_t(-1.) }
      } ;

  Index_p elemToNode = &nodelist[8*i2];
  Index_t i3=8*i2;
  Real_t volinv=Real_t(1.0)/determ[i2];
  Real_t ss1, mass1, volume13 ;
  for(Index_t i1=0;i1<4;++i1){

    Real_t hourmodx =
      x8n[i3] * ggamma[i1][0] + x8n[i3+1] * ggamma[i1][1] +
      x8n[i3+2] * ggamma[i1][2] + x8n[i3+3] * ggamma[i1][3] +
      x8n[i3+4] * ggamma[i1][4] + x8n[i3+5] * ggamma[i1][5] +
      x8n[i3+6] * ggamma[i1][6] + x8n[i3+7] * ggamma[i1][7];

    Real_t hourmody =
      y8n[i3] * ggamma[i1][0] + y8n[i3+1] * ggamma[i1][1] +
      y8n[i3+2] * ggamma[i1][2] + y8n[i3+3] * ggamma[i1][3] +
      y8n[i3+4] * ggamma[i1][4] + y8n[i3+5] * ggamma[i1][5] +
      y8n[i3+6] * ggamma[i1][6] + y8n[i3+7] * ggamma[i1][7];

    Real_t hourmodz =
      z8n[i3] * ggamma[i1][0] + z8n[i3+1] * ggamma[i1][1] +
      z8n[i3+2] * ggamma[i1][2] + z8n[i3+3] * ggamma[i1][3] +
      z8n[i3+4] * ggamma[i1][4] + z8n[i3+5] * ggamma[i1][5] +
      z8n[i3+6] * ggamma[i1][6] + z8n[i3+7] * ggamma[i1][7];

    hourgam0[i1] = ggamma[i1][0] - volinv*(dvdx[i3 ] * hourmodx +
        dvdy[i3 ] * hourmody +
        dvdz[i3 ] * hourmodz );

    hourgam1[i1] = ggamma[i1][1] - volinv*(dvdx[i3+1] * hourmodx +
        dvdy[i3+1] * hourmody +
        dvdz[i3+1] * hourmodz );

    hourgam2[i1] = ggamma[i1][2] - volinv*(dvdx[i3+2] * hourmodx +
        dvdy[i3+2] * hourmody +
        dvdz[i3+2] * hourmodz );

    hourgam3[i1] = ggamma[i1][3] - volinv*(dvdx[i3+3] * hourmodx +
        dvdy[i3+3] * hourmody +
        dvdz[i3+3] * hourmodz );

    hourgam4[i1] = ggamma[i1][4] - volinv*(dvdx[i3+4] * hourmodx +
        dvdy[i3+4] * hourmody +
        dvdz[i3+4] * hourmodz );

    hourgam5[i1] = ggamma[i1][5] - volinv*(dvdx[i3+5] * hourmodx +
        dvdy[i3+5] * hourmody +
        dvdz[i3+5] * hourmodz );

    hourgam6[i1] = ggamma[i1][6] - volinv*(dvdx[i3+6] * hourmodx +
        dvdy[i3+6] * hourmody +
        dvdz[i3+6] * hourmodz );

    hourgam7[i1] = ggamma[i1][7] - volinv*(dvdx[i3+7] * hourmodx +
        dvdy[i3+7] * hourmody +
        dvdz[i3+7] * hourmodz );

  }

  ss1=ss[i2];
  mass1=elemMass[i2];
  volume13=CBRT(determ[i2]);

  Index_t n0si2 = elemToNode[0];
  Index_t n1si2 = elemToNode[1];
  Index_t n2si2 = elemToNode[2];
  Index_t n3si2 = elemToNode[3];
  Index_t n4si2 = elemToNode[4];
  Index_t n5si2 = elemToNode[5];
  Index_t n6si2 = elemToNode[6];
  Index_t n7si2 = elemToNode[7];

  xd1[0] = xd[n0si2];
  xd1[1] = xd[n1si2];
  xd1[2] = xd[n2si2];
  xd1[3] = xd[n3si2];
  xd1[4] = xd[n4si2];
  xd1[5] = xd[n5si2];
  xd1[6] = xd[n6si2];
  xd1[7] = xd[n7si2];

  yd1[0] = yd[n0si2];
  yd1[1] = yd[n1si2];
  yd1[2] = yd[n2si2];
  yd1[3] = yd[n3si2];
  yd1[4] = yd[n4si2];
  yd1[5] = yd[n5si2];
  yd1[6] = yd[n6si2];
  yd1[7] = yd[n7si2];

  zd1[0] = zd[n0si2];
  zd1[1] = zd[n1si2];
  zd1[2] = zd[n2si2];
  zd1[3] = zd[n3si2];
  zd1[4] = zd[n4si2];
  zd1[5] = zd[n5si2];
  zd1[6] = zd[n6si2];
  zd1[7] = zd[n7si2];

  coefficient = - hourg * Real_t(0.01) * ss1 * mass1 / volume13;

  CalcElemFBHourglassForce(xd1,yd1,zd1,
      hourgam0,hourgam1,hourgam2,hourgam3,
      hourgam4,hourgam5,hourgam6,hourgam7, coefficient,

      hgfx, hgfy, hgfz

      );

  fx[n0si2] += hgfx[0] ;
  fy[n0si2] += hgfy[0] ;
  fz[n0si2] += hgfz[0] ;

  fx[n1si2] += hgfx[1] ;
  fy[n1si2] += hgfy[1] ;
  fz[n1si2] += hgfz[1] ;

  fx[n2si2] += hgfx[2] ;
  fy[n2si2] += hgfy[2] ;
  fz[n2si2] += hgfz[2] ;

  fx[n3si2] += hgfx[3] ;
  fy[n3si2] += hgfy[3] ;
  fz[n3si2] += hgfz[3] ;

  fx[n4si2] += hgfx[4] ;
  fy[n4si2] += hgfy[4] ;
  fz[n4si2] += hgfz[4] ;

  fx[n5si2] += hgfx[5] ;
  fy[n5si2] += hgfy[5] ;
  fz[n5si2] += hgfz[5] ;

  fx[n6si2] += hgfx[6] ;
  fy[n6si2] += hgfy[6] ;
  fz[n6si2] += hgfz[6] ;

  fx[n7si2] += hgfx[7] ;
  fy[n7si2] += hgfy[7] ;
  fz[n7si2] += hgfz[7] ;

  }
  ) ;
}

void CalcHourglassControlForElems(Domain *domain,
    Real_p determ,
    Real_t hgcoef)
{
  Index_t numElem = domain->numElem ;
  Index_t numElem8 = numElem * 8 ;
  Real_p dvdx = elemMemPool.allocate(numElem8) ;
  Real_p dvdy = elemMemPool.allocate(numElem8) ;
  Real_p dvdz = elemMemPool.allocate(numElem8) ;
  Real_p x8n = elemMemPool.allocate(numElem8) ;
  Real_p y8n = elemMemPool.allocate(numElem8) ;
  Real_p z8n = elemMemPool.allocate(numElem8) ;

  RAJA::ReduceMin<reduce_policy, Real_t> minvol(Real_t(1.0e+20));

  RAJA::forall<elem_exec_policy>(*domain->domElemList, [=] (int idx) {

      Index_p elemToNode = &domain->nodelist[8*idx];
      CollectDomainNodesToElemNodes(domain->x, domain->y, domain->z, elemToNode,
        &x8n[8*idx], &y8n[8*idx], &z8n[8*idx] );

      CalcElemVolumeDerivative(&dvdx[8*idx], &dvdy[8*idx], &dvdz[8*idx],
        &x8n[8*idx], &y8n[8*idx], &z8n[8*idx]);

      determ[idx] = domain->volo[idx] * domain->v[idx];

      minvol.min(domain->v[idx]);

      }
      ) ;

  if ( Real_t(minvol) <= Real_t(0.0) ) {
    exit(VolumeError) ;
  }

  if ( hgcoef > Real_t(0.) ) {
    CalcFBHourglassForceForElems( domain->nodelist,
        domain->ss, domain->elemMass,
        domain->xd, domain->yd, domain->zd,
        domain->fx, domain->fy, domain->fz,
        determ,
        x8n, y8n, z8n,
        dvdx, dvdy, dvdz,
        hgcoef,
        domain->domElemList, domain->domNodeList

        ) ;
  }

  elemMemPool.release(&z8n) ;
  elemMemPool.release(&y8n) ;
  elemMemPool.release(&x8n) ;
  elemMemPool.release(&dvdz) ;
  elemMemPool.release(&dvdy) ;
  elemMemPool.release(&dvdx) ;

  return ;
}

void CalcVolumeForceForElems(Domain *domain)
{
  Index_t numElem = domain->numElem ;
  if (numElem != 0) {
    Real_t hgcoef = domain->hgcoef ;
    Real_p sigxx = elemMemPool.allocate(numElem) ;
    Real_p sigyy = elemMemPool.allocate(numElem) ;
    Real_p sigzz = elemMemPool.allocate(numElem) ;
    Real_p determ = elemMemPool.allocate(numElem) ;

    InitStressTermsForElems(domain->p, domain->q,
        sigxx, sigyy, sigzz,
        domain->domElemList);

    IntegrateStressForElems( domain->nodelist,
        domain->x, domain->y, domain->z,
        domain->fx, domain->fy, domain->fz,
        sigxx,
        sigyy,
        sigzz,
        determ,
        domain->domElemList,
        domain->domNodeList

        ) ;

    RAJA::ReduceMin<reduce_policy, Real_t> minvol(Real_t(1.0e+20));
    RAJA::forall<elem_exec_policy>(*domain->domElemList, [=] (int k) {
        minvol.min(determ[k]);
        }
        ) ;

    if ( Real_t(minvol) <= Real_t(0.0)) {
      exit(VolumeError) ;
    }

    CalcHourglassControlForElems(domain, determ, hgcoef) ;

    elemMemPool.release(&determ) ;
    elemMemPool.release(&sigzz) ;
    elemMemPool.release(&sigyy) ;
    elemMemPool.release(&sigxx) ;
  }
}

void CalcForceForNodes(Domain *domain)
{
  RAJA::forall<node_exec_policy>(*domain->domNodeList, [=] (int i) {
      domain->fx[i] = Real_t(0.0) ;
      domain->fy[i] = Real_t(0.0) ;
      domain->fz[i] = Real_t(0.0) ;
      }
      ) ;

  CalcVolumeForceForElems(domain) ;

}

void CalcAccelerationForNodes(Real_p xdd, Real_p ydd, Real_p zdd,
    Real_p fx, Real_p fy, Real_p fz,
    Real_p nodalMass, RAJA::IndexSet *domNodeList)
{
  RAJA::forall<node_exec_policy>(*domNodeList, [=] (int i) {
      xdd[i] = fx[i] / nodalMass[i];
      ydd[i] = fy[i] / nodalMass[i];
      zdd[i] = fz[i] / nodalMass[i];
      }
      ) ;
}

void ApplyAccelerationBoundaryConditionsForNodes(Real_p xdd, Real_p ydd,
    Real_p zdd,
    RAJA::IndexSet *symmX,
    RAJA::IndexSet *symmY,
    RAJA::IndexSet *symmZ)
{
  RAJA::forall<symnode_exec_policy>(*symmX, [=] (int i) {
      xdd[i] = Real_t(0.0) ;
      } );

  RAJA::forall<symnode_exec_policy>(*symmY, [=] (int i) {
      ydd[i] = Real_t(0.0) ;
      } );

  RAJA::forall<symnode_exec_policy>(*symmZ, [=] (int i) {
      zdd[i] = Real_t(0.0) ;
      } );
}

void CalcVelocityForNodes(Real_p xd, Real_p yd, Real_p zd,
    Real_p xdd, Real_p ydd, Real_p zdd,
    const Real_t dt, const Real_t u_cut,
    RAJA::IndexSet *domNodeList)
{
  RAJA::forall<node_exec_policy>( *domNodeList, [=] (int i) {

      Real_t xdtmp = xd[i] + xdd[i] * dt ;
      if( FABS(xdtmp) < u_cut ) xdtmp = Real_t(0.0);
      xd[i] = xdtmp ;

      Real_t ydtmp = yd[i] + ydd[i] * dt ;
      if( FABS(ydtmp) < u_cut ) ydtmp = Real_t(0.0);
      yd[i] = ydtmp ;

      Real_t zdtmp = zd[i] + zdd[i] * dt ;
      if( FABS(zdtmp) < u_cut ) zdtmp = Real_t(0.0);
      zd[i] = zdtmp ;
      }
      ) ;
}

void CalcPositionForNodes(Real_p x, Real_p y, Real_p z,
    Real_p xd, Real_p yd, Real_p zd,
    const Real_t dt, RAJA::IndexSet *domNodeList)
{
  RAJA::forall<node_exec_policy>( *domNodeList, [=] (int i) {
      x[i] += xd[i] * dt ;
      y[i] += yd[i] * dt ;
      z[i] += zd[i] * dt ;
      }
      ) ;
}

void LagrangeNodal(Domain *domain)
{
  const Real_t delt = domain->deltatime ;
  Real_t u_cut = domain->u_cut ;

  CalcForceForNodes(domain);

  CalcAccelerationForNodes(domain->xdd, domain->ydd, domain->zdd,
      domain->fx, domain->fy, domain->fz,
      domain->nodalMass, domain->domNodeList);

  ApplyAccelerationBoundaryConditionsForNodes(domain->xdd, domain->ydd,
      domain->zdd, domain->symmX,
      domain->symmY, domain->symmZ);

  CalcVelocityForNodes( domain->xd, domain->yd, domain->zd,
      domain->xdd, domain->ydd, domain->zdd,
      delt, u_cut, domain->domNodeList) ;

  CalcPositionForNodes( domain->x, domain->y, domain->z,
      domain->xd, domain->yd, domain->zd,
      delt, domain->domNodeList );

  return;
}

Real_t CalcElemVolume( const Real_t x0, const Real_t x1,
    const Real_t x2, const Real_t x3,
    const Real_t x4, const Real_t x5,
    const Real_t x6, const Real_t x7,
    const Real_t y0, const Real_t y1,
    const Real_t y2, const Real_t y3,
    const Real_t y4, const Real_t y5,
    const Real_t y6, const Real_t y7,
    const Real_t z0, const Real_t z1,
    const Real_t z2, const Real_t z3,
    const Real_t z4, const Real_t z5,
    const Real_t z6, const Real_t z7 )
{
  Real_t twelveth = Real_t(1.0)/Real_t(12.0);

  Real_t dx61 = x6 - x1;
  Real_t dy61 = y6 - y1;
  Real_t dz61 = z6 - z1;

  Real_t dx70 = x7 - x0;
  Real_t dy70 = y7 - y0;
  Real_t dz70 = z7 - z0;

  Real_t dx63 = x6 - x3;
  Real_t dy63 = y6 - y3;
  Real_t dz63 = z6 - z3;

  Real_t dx20 = x2 - x0;
  Real_t dy20 = y2 - y0;
  Real_t dz20 = z2 - z0;

  Real_t dx50 = x5 - x0;
  Real_t dy50 = y5 - y0;
  Real_t dz50 = z5 - z0;

  Real_t dx64 = x6 - x4;
  Real_t dy64 = y6 - y4;
  Real_t dz64 = z6 - z4;

  Real_t dx31 = x3 - x1;
  Real_t dy31 = y3 - y1;
  Real_t dz31 = z3 - z1;

  Real_t dx72 = x7 - x2;
  Real_t dy72 = y7 - y2;
  Real_t dz72 = z7 - z2;

  Real_t dx43 = x4 - x3;
  Real_t dy43 = y4 - y3;
  Real_t dz43 = z4 - z3;

  Real_t dx57 = x5 - x7;
  Real_t dy57 = y5 - y7;
  Real_t dz57 = z5 - z7;

  Real_t dx14 = x1 - x4;
  Real_t dy14 = y1 - y4;
  Real_t dz14 = z1 - z4;

  Real_t dx25 = x2 - x5;
  Real_t dy25 = y2 - y5;
  Real_t dz25 = z2 - z5;

  Real_t volume =
    ((dx31 + dx72)*((dy63)*(dz20) - (dy20)*(dz63)) + (dy31 + dy72)*((dx20)*(dz63) - (dx63)*(dz20)) + (dz31 + dz72)*((dx63)*(dy20) - (dx20)*(dy63)))

    +
    ((dx43 + dx57)*((dy64)*(dz70) - (dy70)*(dz64)) + (dy43 + dy57)*((dx70)*(dz64) - (dx64)*(dz70)) + (dz43 + dz57)*((dx64)*(dy70) - (dx70)*(dy64)))

    +
    ((dx14 + dx25)*((dy61)*(dz50) - (dy50)*(dz61)) + (dy14 + dy25)*((dx50)*(dz61) - (dx61)*(dz50)) + (dz14 + dz25)*((dx61)*(dy50) - (dx50)*(dy61)))

    ;

  volume *= twelveth;

  return volume ;
}

Real_t CalcElemVolume(
    const_Real_p x, const_Real_p y, const_Real_p z
    )
{
  return CalcElemVolume( x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7],
      y[0], y[1], y[2], y[3], y[4], y[5], y[6], y[7],
      z[0], z[1], z[2], z[3], z[4], z[5], z[6], z[7]);
}

Real_t AreaFace( const Real_t x0, const Real_t x1,
    const Real_t x2, const Real_t x3,
    const Real_t y0, const Real_t y1,
    const Real_t y2, const Real_t y3,
    const Real_t z0, const Real_t z1,
    const Real_t z2, const Real_t z3)
{
  Real_t fx = (x2 - x0) - (x3 - x1);
  Real_t fy = (y2 - y0) - (y3 - y1);
  Real_t fz = (z2 - z0) - (z3 - z1);
  Real_t gx = (x2 - x0) + (x3 - x1);
  Real_t gy = (y2 - y0) + (y3 - y1);
  Real_t gz = (z2 - z0) + (z3 - z1);
  Real_t area =
    (fx * fx + fy * fy + fz * fz) *
    (gx * gx + gy * gy + gz * gz) -
    (fx * gx + fy * gy + fz * gz) *
    (fx * gx + fy * gy + fz * gz);
  return area ;
}

Real_t CalcElemCharacteristicLength( const Real_t x[8],
    const Real_t y[8],
    const Real_t z[8],
    const Real_t volume)
{
  Real_t a, charLength = Real_t(0.0);

  a = AreaFace(x[0],x[1],x[2],x[3],
      y[0],y[1],y[2],y[3],
      z[0],z[1],z[2],z[3]) ;
  charLength = ( ((a) > (charLength)) ? (a) : (charLength)) ;

  a = AreaFace(x[4],x[5],x[6],x[7],
      y[4],y[5],y[6],y[7],
      z[4],z[5],z[6],z[7]) ;
  charLength = ( ((a) > (charLength)) ? (a) : (charLength)) ;

  a = AreaFace(x[0],x[1],x[5],x[4],
      y[0],y[1],y[5],y[4],
      z[0],z[1],z[5],z[4]) ;
  charLength = ( ((a) > (charLength)) ? (a) : (charLength)) ;

  a = AreaFace(x[1],x[2],x[6],x[5],
      y[1],y[2],y[6],y[5],
      z[1],z[2],z[6],z[5]) ;
  charLength = ( ((a) > (charLength)) ? (a) : (charLength)) ;

  a = AreaFace(x[2],x[3],x[7],x[6],
      y[2],y[3],y[7],y[6],
      z[2],z[3],z[7],z[6]) ;
  charLength = ( ((a) > (charLength)) ? (a) : (charLength)) ;

  a = AreaFace(x[3],x[0],x[4],x[7],
      y[3],y[0],y[4],y[7],
      z[3],z[0],z[4],z[7]) ;
  charLength = ( ((a) > (charLength)) ? (a) : (charLength)) ;

  charLength = Real_t(4.0) * volume / SQRT(charLength);

  return charLength;
}

void CalcElemVelocityGrandient( const Real_t* const xvel,
    const Real_t* const yvel,
    const Real_t* const zvel,
    const Real_t b[][8],
    const Real_t detJ,
    Real_t* const d )
{
  const Real_t inv_detJ = Real_t(1.0) / detJ ;
  Real_t dyddx, dxddy, dzddx, dxddz, dzddy, dyddz;
  const Real_t* const pfx = b[0];
  const Real_t* const pfy = b[1];
  const Real_t* const pfz = b[2];

  d[0] = inv_detJ * ( pfx[0] * (xvel[0]-xvel[6])
      + pfx[1] * (xvel[1]-xvel[7])
      + pfx[2] * (xvel[2]-xvel[4])
      + pfx[3] * (xvel[3]-xvel[5]) );

  d[1] = inv_detJ * ( pfy[0] * (yvel[0]-yvel[6])
      + pfy[1] * (yvel[1]-yvel[7])
      + pfy[2] * (yvel[2]-yvel[4])
      + pfy[3] * (yvel[3]-yvel[5]) );

  d[2] = inv_detJ * ( pfz[0] * (zvel[0]-zvel[6])
      + pfz[1] * (zvel[1]-zvel[7])
      + pfz[2] * (zvel[2]-zvel[4])
      + pfz[3] * (zvel[3]-zvel[5]) );

  dyddx = inv_detJ * ( pfx[0] * (yvel[0]-yvel[6])
      + pfx[1] * (yvel[1]-yvel[7])
      + pfx[2] * (yvel[2]-yvel[4])
      + pfx[3] * (yvel[3]-yvel[5]) );

  dxddy = inv_detJ * ( pfy[0] * (xvel[0]-xvel[6])
      + pfy[1] * (xvel[1]-xvel[7])
      + pfy[2] * (xvel[2]-xvel[4])
      + pfy[3] * (xvel[3]-xvel[5]) );

  dzddx = inv_detJ * ( pfx[0] * (zvel[0]-zvel[6])
      + pfx[1] * (zvel[1]-zvel[7])
      + pfx[2] * (zvel[2]-zvel[4])
      + pfx[3] * (zvel[3]-zvel[5]) );

  dxddz = inv_detJ * ( pfz[0] * (xvel[0]-xvel[6])
      + pfz[1] * (xvel[1]-xvel[7])
      + pfz[2] * (xvel[2]-xvel[4])
      + pfz[3] * (xvel[3]-xvel[5]) );

  dzddy = inv_detJ * ( pfy[0] * (zvel[0]-zvel[6])
      + pfy[1] * (zvel[1]-zvel[7])
      + pfy[2] * (zvel[2]-zvel[4])
      + pfy[3] * (zvel[3]-zvel[5]) );

  dyddz = inv_detJ * ( pfz[0] * (yvel[0]-yvel[6])
      + pfz[1] * (yvel[1]-yvel[7])
      + pfz[2] * (yvel[2]-yvel[4])
      + pfz[3] * (yvel[3]-yvel[5]) );
  d[5] = Real_t( .5) * ( dxddy + dyddx );
  d[4] = Real_t( .5) * ( dxddz + dzddx );
  d[3] = Real_t( .5) * ( dzddy + dyddz );
}

void CalcKinematicsForElems( Index_p nodelist,
    Real_p x, Real_p y, Real_p z,
    Real_p xd, Real_p yd, Real_p zd,
    Real_p dxx, Real_p dyy, Real_p dzz,
    Real_p v, Real_p volo,
    Real_p vnew, Real_p delv, Real_p arealg,
    Real_t deltaTime, RAJA::IndexSet *domElemList )
{

  RAJA::forall<elem_exec_policy>(*domElemList, [=] (int k) {
      Real_t B[3][8] ;
      Real_t D[6] ;
      Real_t x_local[8] ;
      Real_t y_local[8] ;
      Real_t z_local[8] ;
      Real_t xd_local[8] ;
      Real_t yd_local[8] ;
      Real_t zd_local[8] ;
      Real_t detJ = Real_t(0.0) ;

      Real_t volume ;
      Real_t relativeVolume ;
      const Index_p elemToNode = &nodelist[8*k] ;

      for( Index_t lnode=0 ; lnode<8 ; ++lnode )
      {
      Index_t gnode = elemToNode[lnode];
      x_local[lnode] = x[gnode];
      y_local[lnode] = y[gnode];
      z_local[lnode] = z[gnode];
      }

      volume = CalcElemVolume(x_local, y_local, z_local );
      relativeVolume = volume / volo[k] ;
      vnew[k] = relativeVolume ;
      delv[k] = relativeVolume - v[k] ;

      arealg[k] = CalcElemCharacteristicLength(x_local, y_local, z_local,
          volume);

      for( Index_t lnode=0 ; lnode<8 ; ++lnode )
      {
        Index_t gnode = elemToNode[lnode];
        xd_local[lnode] = xd[gnode];
        yd_local[lnode] = yd[gnode];
        zd_local[lnode] = zd[gnode];
      }

      Real_t dt2 = Real_t(0.5) * deltaTime;
      for ( Index_t j=0 ; j<8 ; ++j )
      {
        x_local[j] -= dt2 * xd_local[j];
        y_local[j] -= dt2 * yd_local[j];
        z_local[j] -= dt2 * zd_local[j];
      }

      CalcElemShapeFunctionDerivatives( x_local, y_local, z_local,
          B, &detJ );

      CalcElemVelocityGrandient( xd_local, yd_local, zd_local,
          B, detJ, D );

      dxx[k] = D[0];
      dyy[k] = D[1];
      dzz[k] = D[2];
  }
  ) ;
}

void CalcLagrangeElements(Domain *domain)
{
  Index_t numElem = domain->numElem ;
  if (numElem > 0) {
    const Real_t deltatime = domain->deltatime ;

    domain->dxx = elemMemPool.allocate(numElem) ;
    domain->dyy = elemMemPool.allocate(numElem) ;
    domain->dzz = elemMemPool.allocate(numElem) ;

    CalcKinematicsForElems(domain->nodelist,
        domain->x, domain->y, domain->z,
        domain->xd, domain->yd, domain->zd,
        domain->dxx, domain->dyy, domain->dzz,
        domain->v, domain->volo,
        domain->vnew, domain->delv, domain->arealg,
        deltatime, domain->domElemList) ;

    RAJA::ReduceMin<reduce_policy, Real_t> minvol(Real_t(1.0e+20));

    RAJA::forall<elem_exec_policy>( *domain->domElemList, [=] (int k) {

        Real_t vdov = domain->dxx[k] + domain->dyy[k] + domain->dzz[k] ;
        Real_t vdovthird = vdov/Real_t(3.0) ;

        domain->vdov[k] = vdov ;
        domain->dxx[k] -= vdovthird ;
        domain->dyy[k] -= vdovthird ;
        domain->dzz[k] -= vdovthird ;

        minvol.min(domain->vnew[k]);
        }
        ) ;

    if ( Real_t(minvol) <= Real_t(0.0)) {
      exit(VolumeError) ;
    }

    elemMemPool.release(&domain->dzz) ;
    elemMemPool.release(&domain->dyy) ;
    elemMemPool.release(&domain->dxx) ;
  }
}

void CalcMonotonicQGradientsForElems(Real_p x, Real_p y, Real_p z,
    Real_p xd, Real_p yd, Real_p zd,
    Real_p volo, Real_p vnew,
    Real_p delv_xi,
    Real_p delv_eta,
    Real_p delv_zeta,
    Real_p delx_xi,
    Real_p delx_eta,
    Real_p delx_zeta,
    Index_p nodelist,
    RAJA::IndexSet *domElemList)
{

  RAJA::forall<elem_exec_policy>(*domElemList, [=] (int i) {
      const Real_t ptiny = Real_t(1.e-36) ;
      Real_t ax,ay,az ;
      Real_t dxv,dyv,dzv ;

      Index_p elemToNode = &nodelist[8*i];
      Index_t n0 = elemToNode[0] ;
      Index_t n1 = elemToNode[1] ;
      Index_t n2 = elemToNode[2] ;
      Index_t n3 = elemToNode[3] ;
      Index_t n4 = elemToNode[4] ;
      Index_t n5 = elemToNode[5] ;
      Index_t n6 = elemToNode[6] ;
      Index_t n7 = elemToNode[7] ;

      Real_t x0 = x[n0] ;
      Real_t x1 = x[n1] ;
      Real_t x2 = x[n2] ;
      Real_t x3 = x[n3] ;
      Real_t x4 = x[n4] ;
      Real_t x5 = x[n5] ;
      Real_t x6 = x[n6] ;
      Real_t x7 = x[n7] ;

      Real_t y0 = y[n0] ;
      Real_t y1 = y[n1] ;
      Real_t y2 = y[n2] ;
      Real_t y3 = y[n3] ;
      Real_t y4 = y[n4] ;
      Real_t y5 = y[n5] ;
      Real_t y6 = y[n6] ;
      Real_t y7 = y[n7] ;

      Real_t z0 = z[n0] ;
      Real_t z1 = z[n1] ;
      Real_t z2 = z[n2] ;
      Real_t z3 = z[n3] ;
      Real_t z4 = z[n4] ;
      Real_t z5 = z[n5] ;
      Real_t z6 = z[n6] ;
      Real_t z7 = z[n7] ;

      Real_t xv0 = xd[n0] ;
      Real_t xv1 = xd[n1] ;
      Real_t xv2 = xd[n2] ;
      Real_t xv3 = xd[n3] ;
      Real_t xv4 = xd[n4] ;
      Real_t xv5 = xd[n5] ;
      Real_t xv6 = xd[n6] ;
      Real_t xv7 = xd[n7] ;

      Real_t yv0 = yd[n0] ;
      Real_t yv1 = yd[n1] ;
      Real_t yv2 = yd[n2] ;
      Real_t yv3 = yd[n3] ;
      Real_t yv4 = yd[n4] ;
      Real_t yv5 = yd[n5] ;
      Real_t yv6 = yd[n6] ;
      Real_t yv7 = yd[n7] ;

      Real_t zv0 = zd[n0] ;
      Real_t zv1 = zd[n1] ;
      Real_t zv2 = zd[n2] ;
      Real_t zv3 = zd[n3] ;
      Real_t zv4 = zd[n4] ;
      Real_t zv5 = zd[n5] ;
      Real_t zv6 = zd[n6] ;
      Real_t zv7 = zd[n7] ;

      Real_t vol = volo[i]*vnew[i] ;
      Real_t norm = Real_t(1.0) / ( vol + ptiny ) ;

      Real_t dxj = Real_t(-0.25)*((x0 + x1 + x5 + x4) - (x3 + x2 + x6 + x7)) ;
      Real_t dyj = Real_t(-0.25)*((y0 + y1 + y5 + y4) - (y3 + y2 + y6 + y7)) ;
      Real_t dzj = Real_t(-0.25)*((z0 + z1 + z5 + z4) - (z3 + z2 + z6 + z7)) ;

      Real_t dxi = Real_t( 0.25)*((x1 + x2 + x6 + x5) - (x0 + x3 + x7 + x4)) ;
      Real_t dyi = Real_t( 0.25)*((y1 + y2 + y6 + y5) - (y0 + y3 + y7 + y4)) ;
      Real_t dzi = Real_t( 0.25)*((z1 + z2 + z6 + z5) - (z0 + z3 + z7 + z4)) ;

      Real_t dxk = Real_t( 0.25)*((x4 + x5 + x6 + x7) - (x0 + x1 + x2 + x3)) ;
      Real_t dyk = Real_t( 0.25)*((y4 + y5 + y6 + y7) - (y0 + y1 + y2 + y3)) ;
      Real_t dzk = Real_t( 0.25)*((z4 + z5 + z6 + z7) - (z0 + z1 + z2 + z3)) ;

      ax = dyi*dzj - dzi*dyj ;
      ay = dzi*dxj - dxi*dzj ;
      az = dxi*dyj - dyi*dxj ;

      delx_zeta[i] = vol / SQRT(ax*ax + ay*ay + az*az + ptiny) ;

      ax *= norm ;
      ay *= norm ;
      az *= norm ;

      dxv = Real_t(0.25)*((xv4 + xv5 + xv6 + xv7) - (xv0 + xv1 + xv2 + xv3)) ;
      dyv = Real_t(0.25)*((yv4 + yv5 + yv6 + yv7) - (yv0 + yv1 + yv2 + yv3)) ;
      dzv = Real_t(0.25)*((zv4 + zv5 + zv6 + zv7) - (zv0 + zv1 + zv2 + zv3)) ;

      delv_zeta[i] = ax*dxv + ay*dyv + az*dzv ;

      ax = dyj*dzk - dzj*dyk ;
      ay = dzj*dxk - dxj*dzk ;
      az = dxj*dyk - dyj*dxk ;

      delx_xi[i] = vol / SQRT(ax*ax + ay*ay + az*az + ptiny) ;

      ax *= norm ;
      ay *= norm ;
      az *= norm ;

      dxv = Real_t(0.25)*((xv1 + xv2 + xv6 + xv5) - (xv0 + xv3 + xv7 + xv4)) ;
      dyv = Real_t(0.25)*((yv1 + yv2 + yv6 + yv5) - (yv0 + yv3 + yv7 + yv4)) ;
      dzv = Real_t(0.25)*((zv1 + zv2 + zv6 + zv5) - (zv0 + zv3 + zv7 + zv4)) ;

      delv_xi[i] = ax*dxv + ay*dyv + az*dzv ;

      ax = dyk*dzi - dzk*dyi ;
      ay = dzk*dxi - dxk*dzi ;
      az = dxk*dyi - dyk*dxi ;

      delx_eta[i] = vol / SQRT(ax*ax + ay*ay + az*az + ptiny) ;

      ax *= norm ;
      ay *= norm ;
      az *= norm ;

      dxv = Real_t(-0.25)*((xv0 + xv1 + xv5 + xv4) - (xv3 + xv2 + xv6 + xv7)) ;
      dyv = Real_t(-0.25)*((yv0 + yv1 + yv5 + yv4) - (yv3 + yv2 + yv6 + yv7)) ;
      dzv = Real_t(-0.25)*((zv0 + zv1 + zv5 + zv4) - (zv3 + zv2 + zv6 + zv7)) ;

      delv_eta[i] = ax*dxv + ay*dyv + az*dzv ;
  }
  ) ;

}

void CalcMonotonicQRegionForElems(
    RAJA::IndexSet *matElemList, Index_p elemBC,
    Index_p lxim, Index_p lxip,
    Index_p letam, Index_p letap,
    Index_p lzetam, Index_p lzetap,
    Real_p delv_xi,Real_p delv_eta,Real_p delv_zeta,
    Real_p delx_xi,Real_p delx_eta,Real_p delx_zeta,
    Real_p vdov, Real_p volo, Real_p vnew,
    Real_p elemMass, Real_p qq, Real_p ql,
    Real_t qlc_monoq, Real_t qqc_monoq,
    Real_t monoq_limiter_mult,
    Real_t monoq_max_slope,
    Real_t ptiny )
{
  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int i) {
      Real_t qlin, qquad ;
      Real_t phixi, phieta, phizeta ;
      Int_t bcMask = elemBC[i] ;
      Real_t delvm, delvp ;

      Real_t norm = Real_t(1.) / ( delv_xi[i] + ptiny ) ;

      switch (bcMask & 0x003) {
      case 0: delvm = delv_xi[lxim[i]] ; break ;
      case 0x001: delvm = delv_xi[i] ; break ;
      case 0x002: delvm = Real_t(0.0) ; break ;
      default: ; break ;
      }
      switch (bcMask & 0x00c) {
      case 0: delvp = delv_xi[lxip[i]] ; break ;
      case 0x004: delvp = delv_xi[i] ; break ;
      case 0x008: delvp = Real_t(0.0) ; break ;
      default: ; break ;
      }

      delvm = delvm * norm ;
      delvp = delvp * norm ;

      phixi = Real_t(.5) * ( delvm + delvp ) ;

      delvm *= monoq_limiter_mult ;
      delvp *= monoq_limiter_mult ;

      if ( delvm < phixi ) phixi = delvm ;
      if ( delvp < phixi ) phixi = delvp ;
      if ( phixi < Real_t(0.)) phixi = Real_t(0.) ;
      if ( phixi > monoq_max_slope) phixi = monoq_max_slope;

      norm = Real_t(1.) / ( delv_eta[i] + ptiny ) ;

      switch (bcMask & 0x030) {
        case 0: delvm = delv_eta[letam[i]] ; break ;
        case 0x010: delvm = delv_eta[i] ; break ;
        case 0x020: delvm = Real_t(0.0) ; break ;
        default: ; break ;
      }
      switch (bcMask & 0x0c0) {
        case 0: delvp = delv_eta[letap[i]] ; break ;
        case 0x040: delvp = delv_eta[i] ; break ;
        case 0x080: delvp = Real_t(0.0) ; break ;
        default: ; break ;
      }

      delvm = delvm * norm ;
      delvp = delvp * norm ;

      phieta = Real_t(.5) * ( delvm + delvp ) ;

      delvm *= monoq_limiter_mult ;
      delvp *= monoq_limiter_mult ;

      if ( delvm < phieta ) phieta = delvm ;
      if ( delvp < phieta ) phieta = delvp ;
      if ( phieta < Real_t(0.)) phieta = Real_t(0.) ;
      if ( phieta > monoq_max_slope) phieta = monoq_max_slope;

      norm = Real_t(1.) / ( delv_zeta[i] + ptiny ) ;

      switch (bcMask & 0x300) {
        case 0: delvm = delv_zeta[lzetam[i]] ; break ;
        case 0x100: delvm = delv_zeta[i] ; break ;
        case 0x200: delvm = Real_t(0.0) ; break ;
        default: ; break ;
      }
      switch (bcMask & 0xc00) {
        case 0: delvp = delv_zeta[lzetap[i]] ; break ;
        case 0x400: delvp = delv_zeta[i] ; break ;
        case 0x800: delvp = Real_t(0.0) ; break ;
        default: ; break ;
      }

      delvm = delvm * norm ;
      delvp = delvp * norm ;

      phizeta = Real_t(.5) * ( delvm + delvp ) ;

      delvm *= monoq_limiter_mult ;
      delvp *= monoq_limiter_mult ;

      if ( delvm < phizeta ) phizeta = delvm ;
      if ( delvp < phizeta ) phizeta = delvp ;
      if ( phizeta < Real_t(0.)) phizeta = Real_t(0.);
      if ( phizeta > monoq_max_slope ) phizeta = monoq_max_slope;

      if ( vdov[i] > Real_t(0.) ) {
        qlin = Real_t(0.) ;
        qquad = Real_t(0.) ;
      }
      else {
        Real_t delvxxi = delv_xi[i] * delx_xi[i] ;
        Real_t delvxeta = delv_eta[i] * delx_eta[i] ;
        Real_t delvxzeta = delv_zeta[i] * delx_zeta[i] ;

        if ( delvxxi > Real_t(0.) ) delvxxi = Real_t(0.) ;
        if ( delvxeta > Real_t(0.) ) delvxeta = Real_t(0.) ;
        if ( delvxzeta > Real_t(0.) ) delvxzeta = Real_t(0.) ;

        Real_t rho = elemMass[i] / (volo[i] * vnew[i]) ;

        qlin = -qlc_monoq * rho *
          ( delvxxi * (Real_t(1.) - phixi) +
            delvxeta * (Real_t(1.) - phieta) +
            delvxzeta * (Real_t(1.) - phizeta) ) ;

        qquad = qqc_monoq * rho *
          ( delvxxi*delvxxi * (Real_t(1.) - phixi*phixi) +
            delvxeta*delvxeta * (Real_t(1.) - phieta*phieta) +
            delvxzeta*delvxzeta * (Real_t(1.) - phizeta*phizeta) ) ;
      }

      qq[i] = qquad ;
      ql[i] = qlin ;
  }
  ) ;
}

void CalcMonotonicQForElems(Domain *domain)
{

  Index_t numElem = domain->numElem ;
  if (numElem > 0) {

    const Real_t ptiny = Real_t(1.e-36) ;

    CalcMonotonicQRegionForElems(
        domain->matElemList, domain->elemBC,
        domain->lxim, domain->lxip,
        domain->letam, domain->letap,
        domain->lzetam, domain->lzetap,
        domain->delv_xi,domain->delv_eta,domain->delv_zeta,
        domain->delx_xi,domain->delx_eta,domain->delx_zeta,
        domain->vdov, domain->volo, domain->vnew,
        domain->elemMass, domain->qq, domain->ql,
        domain->qlc_monoq, domain->qqc_monoq,
        domain->monoq_limiter_mult,
        domain->monoq_max_slope,
        ptiny );
  }
}

void CalcQForElems(Domain *domain)
{

  Index_t numElem = domain->numElem ;

  if (numElem != 0) {

    domain->delv_xi = elemMemPool.allocate(numElem) ;
    domain->delv_eta = elemMemPool.allocate(numElem) ;
    domain->delv_zeta = elemMemPool.allocate(numElem) ;

    domain->delx_xi = elemMemPool.allocate(numElem) ;
    domain->delx_eta = elemMemPool.allocate(numElem) ;
    domain->delx_zeta = elemMemPool.allocate(numElem) ;

    CalcMonotonicQGradientsForElems(domain->x, domain->y, domain->z,
        domain->xd, domain->yd, domain->zd,
        domain->volo, domain->vnew,
        domain->delv_xi,
        domain->delv_eta,
        domain->delv_zeta,
        domain->delx_xi,
        domain->delx_eta,
        domain->delx_zeta,
        domain->nodelist,
        domain->domElemList) ;

    CalcMonotonicQForElems(domain) ;

    elemMemPool.release(&domain->delx_zeta) ;
    elemMemPool.release(&domain->delx_eta) ;
    elemMemPool.release(&domain->delx_xi) ;

    elemMemPool.release(&domain->delv_zeta) ;
    elemMemPool.release(&domain->delv_eta) ;
    elemMemPool.release(&domain->delv_xi) ;

    Real_t qstop = domain->qstop ;
    domain->idx = -1;

    RAJA::forall<elem_exec_policy>( *domain->domElemList, [=] (int i) {
        if ( domain->q[i] > qstop ) {
        domain->idx = i ;

        }
        }
        ) ;

    if (domain->idx >= 0) {
      exit(QStopError) ;
    }
  }
}

void CalcPressureForElems(Real_p p_new, Real_p bvc,
    Real_p pbvc, Real_p e_old,
    Real_p compression, Real_p vnewc,
    Real_t pmin,
    Real_t p_cut, Real_t eosvmax,
    RAJA::IndexSet *matElemList)
{
  const Real_t c1s = Real_t(2.0)/Real_t(3.0) ;
  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int i) {
      bvc[i] = c1s * (compression[i] + Real_t(1.));
      pbvc[i] = c1s;
      }
      ) ;

  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int i) {
      p_new[i] = bvc[i] * e_old[i] ;

      if (FABS(p_new[i]) < p_cut )
      p_new[i] = Real_t(0.0) ;

      if ( vnewc[i] >= eosvmax )
      p_new[i] = Real_t(0.0) ;

      if (p_new[i] < pmin)
      p_new[i] = pmin ;
      }
      ) ;
}

void CalcEnergyForElems(Real_p p_new, Real_p e_new, Real_p q_new,
    Real_p bvc, Real_p pbvc,
    Real_p p_old, Real_p e_old, Real_p q_old,
    Real_p compression, Real_p compHalfStep,
    Real_p vnewc, Real_p work, Real_p delvc, Real_t pmin,
    Real_t p_cut, Real_t e_cut, Real_t q_cut, Real_t emin,
    Real_p qq_old, Real_p ql_old,
    Real_t rho0,
    Real_t eosvmax,
    RAJA::IndexSet *matElemList,
    Index_t length)
{
  const Real_t sixth = Real_t(1.0) / Real_t(6.0) ;
  Real_p pHalfStep = elemMemPool.allocate(length) ;

  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int i) {
      e_new[i] = e_old[i] - Real_t(0.5) * delvc[i] * (p_old[i] + q_old[i])
      + Real_t(0.5) * work[i];

      if (e_new[i] < emin ) {
      e_new[i] = emin ;
      }
      }
      ) ;

  CalcPressureForElems(pHalfStep, bvc, pbvc, e_new, compHalfStep, vnewc,
      pmin, p_cut, eosvmax, matElemList);

  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int i) {
      Real_t vhalf = Real_t(1.) / (Real_t(1.) + compHalfStep[i]) ;

      if ( delvc[i] > Real_t(0.) ) {
      q_new[i] = Real_t(0.) ;
      }
      else {
      Real_t ssc = ( pbvc[i] * e_new[i]
        + vhalf * vhalf * bvc[i] * pHalfStep[i] ) / rho0 ;

      if ( ssc <= Real_t(.1111111e-36) ) {
      ssc = Real_t(.3333333e-18) ;
      } else {
      ssc = SQRT(ssc) ;
      }

      q_new[i] = (ssc*ql_old[i] + qq_old[i]) ;
      }

      e_new[i] = e_new[i] + Real_t(0.5) * (delvc[i]
        * ( Real_t(3.0)*(p_old[i] + q_old[i])
          - Real_t(4.0)*(pHalfStep[i] + q_new[i])) + work[i] ) ;

      if (FABS(e_new[i]) < e_cut) {
        e_new[i] = Real_t(0.) ;
      }
      if ( e_new[i] < emin ) {
        e_new[i] = emin ;
      }
  }
  ) ;

  CalcPressureForElems(p_new, bvc, pbvc, e_new, compression, vnewc,
      pmin, p_cut, eosvmax, matElemList);

  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int i) {
      Real_t q_tilde ;

      if (delvc[i] > Real_t(0.)) {
      q_tilde = Real_t(0.) ;
      }
      else {
      Real_t ssc = ( pbvc[i] * e_new[i]
        + vnewc[i] * vnewc[i] * bvc[i] * p_new[i] ) / rho0 ;

      if ( ssc <= Real_t(.1111111e-36) ) {
      ssc = Real_t(.3333333e-18) ;
      } else {
      ssc = SQRT(ssc) ;
      }

      q_tilde = (ssc*ql_old[i] + qq_old[i]) ;
      }

      e_new[i] = e_new[i] - ( Real_t(7.0)*(p_old[i] + q_old[i])
        - Real_t(8.0)*(pHalfStep[i] + q_new[i])
        + (p_new[i] + q_tilde)) * delvc[i]*sixth ;

      if (FABS(e_new[i]) < e_cut) {
        e_new[i] = Real_t(0.) ;
      }
      if ( e_new[i] < emin ) {
        e_new[i] = emin ;
      }
  }
  ) ;

  CalcPressureForElems(p_new, bvc, pbvc, e_new, compression, vnewc,
      pmin, p_cut, eosvmax, matElemList);

  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int i) {

      if ( delvc[i] <= Real_t(0.) ) {
      Real_t ssc = ( pbvc[i] * e_new[i]
        + vnewc[i] * vnewc[i] * bvc[i] * p_new[i] ) / rho0 ;

      if ( ssc <= Real_t(.1111111e-36) ) {
      ssc = Real_t(.3333333e-18) ;
      } else {
      ssc = SQRT(ssc) ;
      }

      q_new[i] = (ssc*ql_old[i] + qq_old[i]) ;

      if (FABS(q_new[i]) < q_cut) q_new[i] = Real_t(0.) ;
      }
      }
      ) ;

  elemMemPool.release(&pHalfStep) ;

  return ;
}

void CalcSoundSpeedForElems(RAJA::IndexSet *matElemList, Real_p ss,
    Real_p vnewc, Real_t rho0, Real_p enewc,
    Real_p pnewc, Real_p pbvc,
    Real_p bvc, Real_t ss4o3)
{
  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int iz) {
      Real_t ssTmp = (pbvc[iz] * enewc[iz] + vnewc[iz] * vnewc[iz] *
        bvc[iz] * pnewc[iz]) / rho0;
      if (ssTmp <= Real_t(.1111111e-36)) {
      ssTmp = Real_t(.3333333e-18);
      }
      else {
      ssTmp = SQRT(ssTmp);
      }
      ss[iz] = ssTmp ;
      }
      ) ;
}

void EvalEOSForElems(Domain *domain, Real_p vnewc, Index_t numElem)
{
  Real_t e_cut = domain->e_cut ;
  Real_t p_cut = domain->p_cut ;
  Real_t ss4o3 = domain->ss4o3 ;
  Real_t q_cut = domain->q_cut ;

  Real_t eosvmax = domain->eosvmax ;
  Real_t eosvmin = domain->eosvmin ;
  Real_t pmin = domain->pmin ;
  Real_t emin = domain->emin ;
  Real_t rho0 = domain->refdens ;

  Real_p delvc = domain->delv ;
  Real_p p_old = elemMemPool.allocate(numElem) ;
  Real_p compression = elemMemPool.allocate(numElem) ;
  Real_p compHalfStep = elemMemPool.allocate(numElem) ;
  Real_p work = elemMemPool.allocate(numElem) ;
  Real_p p_new = elemMemPool.allocate(numElem) ;
  Real_p e_new = elemMemPool.allocate(numElem) ;
  Real_p q_new = elemMemPool.allocate(numElem) ;
  Real_p bvc = elemMemPool.allocate(numElem) ;
  Real_p pbvc = elemMemPool.allocate(numElem) ;

  RAJA::forall<mat_exec_policy>( *domain->matElemList, [=] (int zidx) {
      p_old[zidx] = domain->p[zidx] ;
      }
      ) ;

  RAJA::forall<mat_exec_policy>( *domain->matElemList, [=] (int zidx) {
      Real_t vchalf ;
      compression[zidx] = Real_t(1.) / vnewc[zidx] - Real_t(1.);
      vchalf = vnewc[zidx] - delvc[zidx] * Real_t(.5);
      compHalfStep[zidx] = Real_t(1.) / vchalf - Real_t(1.);

      if ( eosvmin != Real_t(0.) ) {
      if (vnewc[zidx] <= eosvmin) {
      compHalfStep[zidx] = compression[zidx] ;
      }
      }

      if ( eosvmax != Real_t(0.) ) {
      if (vnewc[zidx] >= eosvmax) {
      p_old[zidx] = Real_t(0.) ;
      compression[zidx] = Real_t(0.) ;
      compHalfStep[zidx] = Real_t(0.) ;
      }
      }
      }
      ) ;

  RAJA::forall<mat_exec_policy>( *domain->matElemList, [=] (int zidx) {
      work[zidx] = Real_t(0.) ;
      }
      ) ;

  CalcEnergyForElems(p_new, e_new, q_new, bvc, pbvc,
      p_old, domain->e, domain->q, compression, compHalfStep,
      vnewc, work, delvc, pmin,
      p_cut, e_cut, q_cut, emin,
      domain->qq, domain->ql, rho0, eosvmax,
      domain->matElemList, numElem);

  RAJA::forall<mat_exec_policy>( *domain->matElemList, [=] (int zidx) {
      domain->p[zidx] = p_new[zidx] ;
      domain->e[zidx] = e_new[zidx] ;
      domain->q[zidx] = q_new[zidx] ;
      }
      ) ;

  CalcSoundSpeedForElems(domain->matElemList, domain->ss,
      vnewc, rho0, e_new, p_new,
      pbvc, bvc, ss4o3) ;

  elemMemPool.release(&pbvc) ;
  elemMemPool.release(&bvc) ;
  elemMemPool.release(&q_new) ;
  elemMemPool.release(&e_new) ;
  elemMemPool.release(&p_new) ;
  elemMemPool.release(&work) ;
  elemMemPool.release(&compHalfStep) ;
  elemMemPool.release(&compression) ;
  elemMemPool.release(&p_old) ;
}

void ApplyMaterialPropertiesForElems(Domain *domain)
{
  Index_t numElem = domain->numElem ;

  if (numElem != 0) {

    Real_t eosvmin = domain->eosvmin ;
    Real_t eosvmax = domain->eosvmax ;

    Real_p vnewc = elemMemPool.allocate(numElem) ;

    RAJA::forall<mat_exec_policy>( *domain->matElemList, [=] (int zn) {
        vnewc[zn] = domain->vnew[zn] ;

        if (eosvmin != Real_t(0.)) {
        if (vnewc[zn] < eosvmin) {
        vnewc[zn] = eosvmin ;
        }
        }

        if (eosvmax != Real_t(0.)) {
        if (vnewc[zn] > eosvmax) {
        vnewc[zn] = eosvmax ;
        }
        }

        }
        ) ;

    RAJA::ReduceMin<reduce_policy, Real_t> minvol(Real_t(1.0e+20));

    RAJA::forall<mat_exec_policy>( *domain->matElemList, [=] (int zn) {
        Real_t vc = domain->v[zn] ;
        if (eosvmin != Real_t(0.)) {
        if (vc < eosvmin) {
        vc = eosvmin ;
        }
        }
        if (eosvmax != Real_t(0.)) {
        if (vc > eosvmax) {
        vc = eosvmax ;
        }
        }

        minvol.min(vc);
        }
        ) ;

    if ( Real_t(minvol) <= Real_t(0.) ) {
      exit(VolumeError) ;
    }

    EvalEOSForElems(domain, vnewc, numElem);

    elemMemPool.release(&vnewc) ;
  }
}

void UpdateVolumesForElems(RAJA::IndexSet *domElemList,
    Real_p vnew, Real_p v, Real_t v_cut)
{
  RAJA::forall<elem_exec_policy>( *domElemList, [=] (int i) {
      Real_t tmpV = vnew[i] ;

      if ( FABS(tmpV - Real_t(1.0)) < v_cut )
      tmpV = Real_t(1.0) ;

      v[i] = tmpV ;
      }
      ) ;

  return ;
}

void LagrangeElements(Domain *domain, Index_t numElem)
{

  domain->vnew = elemMemPool.allocate(numElem) ;

  CalcLagrangeElements(domain) ;

  CalcQForElems(domain) ;

  ApplyMaterialPropertiesForElems(domain) ;

  UpdateVolumesForElems(domain->domElemList,
      domain->vnew, domain->v, domain->v_cut) ;

  elemMemPool.release(&domain->vnew) ;
}

void CalcCourantConstraintForElems(RAJA::IndexSet *matElemList, Real_p ss,
    Real_p vdov, Real_p arealg,
    Real_t qqc, Real_t *dtcourant)
{
  RAJA::ReduceMin<reduce_policy, Real_t> dtcourantLoc(Real_t(1.0e+20)) ;
  Real_t qqc2 = Real_t(64.0) * qqc * qqc ;

  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int indx) {
      Real_t dtf = ss[indx] * ss[indx] ;

      if ( vdov[indx] < Real_t(0.) ) {
      dtf += qqc2 * arealg[indx] * arealg[indx] * vdov[indx] * vdov[indx] ;
      }

      Real_t dtf_cmp = (vdov[indx] != Real_t(0.))
      ? arealg[indx] / SQRT(dtf) : Real_t(1.0e+20) ;

      dtcourantLoc.min(dtf_cmp) ;
      } ) ;

  Real_t result = Real_t(dtcourantLoc);
  if (result < Real_t(1.0e+20)) {
    *dtcourant = result ;
  }

  return ;
}
#if 0
void CalcHydroConstraintForElems(RAJA::IndexSet *matElemList, Real_p vdov,
    Real_t dvovmax, Real_t *dthydro)
{
  RAJA::ReduceMin<reduce_policy, Real_t> dthydroLoc(Real_t(1.0e+20)) ;

  RAJA::forall<mat_exec_policy>( *matElemList, [=] (int indx) {

      Real_t dtvov_cmp = (vdov[indx] != Real_t(0.))
      ? (dvovmax / (FABS(vdov[indx])+Real_t(1.e-20)))
      : Real_t(1.0e+10) ;

      dthydroLoc.min(dtvov_cmp) ;
      } ) ;

  Real_t result = Real_t(dthydroLoc);
  if (result < Real_t(1.0e+20)) {
    *dthydro = result ;
  }

  return ;
}
#endif
void CalcTimeConstraintsForElems(Domain *domain) {

  CalcCourantConstraintForElems(domain->matElemList, domain->ss,
      domain->vdov, domain->arealg,
      domain->qqc, &domain->dtcourant) ;

  CalcHydroConstraintForElems(domain->matElemList, domain->vdov,
      domain->dvovmax, &domain->dthydro) ;
}

void LagrangeLeapFrog(Domain *domain)
{

  LagrangeNodal(domain);

  LagrangeElements(domain, domain->numElem);

  if (domain->dtfixed <= Real_t(0.0)) {
    CalcTimeConstraintsForElems(domain);
  }
}

int main(int argc, char *argv[])
{

  RAJA::Timer timer_main;
  RAJA::Timer timer_cycle;

  timer_main.start("timer_main");

  Real_t tx, ty, tz ;
  Index_t nidx, zidx ;
  int maxIter = 1024*1024 ;

  Domain* domain = Allocate<Domain>(1);

  Index_t edgeElems = lulesh_edge_elems ;

  for (int i=1; i<argc; ++i) {
    if (strcmp(argv[i], "-p") == 0) {
      show_run_progress = true ;
    }
    else if (strcmp(argv[i], "-i") == 0) {
      if ((i+1 < argc) && isdigit(argv[i+1][0])) {
        maxIter = atoi(argv[i+1]) ;
        ++i;
      }
      else {
        printf("Iteration (-i) option has bad argument -- ignoring\n") ;
      }
    }
    else if (strcmp(argv[i], "-s") == 0) {
      if ((i+1 < argc) && isdigit(argv[i+1][0])) {
        edgeElems = atoi(argv[i+1]) ;
        ++i;
      }
      else {
        printf("Size (-s) option has bad argument -- ignoring\n") ;
      }
    }
  }

  Index_t edgeNodes = edgeElems+1 ;

  Index_p perm = 0 ;
  Index_p iperm = 0 ;

  printf("LULESH parallel run parameters:\n");
  printf("\t stop time = %e\n", double(lulesh_stop_time)) ;
  if ( lulesh_time_step > 0 ) {
    printf("\t Fixed time step = %e\n", double(lulesh_time_step)) ;
  } else {
    printf("\t CFL-controlled: initial time step = %e\n",
        double(-lulesh_time_step)) ;
  }
  printf("\t Mesh size = %i x %i x %i\n",
      edgeElems, edgeElems, edgeElems) ;

  switch (lulesh_tiling_mode) {
    case Canonical:
      {
        printf("\t Tiling mode is 'Canonical'\n");

        break;
      }
    case Tiled_Index:
      {
        printf("\t Tiling mode is 'Tiled_Index'\n");

        printf("must have OMP_FINE_SYNC defined for this tiling mode, at present\n") ;
        exit(-1) ;

        break;
      }
    case Tiled_Order:
      {
        printf("\t Tiling mode is 'Tiled_Order'\n");

        printf("must have OMP_FINE_SYNC defined for this tiling mode, at present\n") ;
        exit(-1) ;

        break;
      }
    case Tiled_LockFree:
      {
        printf("\t Tiling mode is 'Lock-free chunk'\n");
        if ( !(std::is_same<Segment_Exec, RAJA::seq_exec>::value ||
              std::is_same<Segment_Exec, RAJA::simd_exec>::value) ) {
          printf("Cannot have inner parallelism for this tiling mode\n") ;
          exit(-1) ;
        }
        break;
      }
    case Tiled_LockFreeColor:
      {
        printf("\t Tiling mode is 'Lock-free color'\n");
        if ( !std::is_same<Hybrid_Seg_Iter, RAJA::seq_segit>::value ) {
          printf("Cannot have outer parallelism for this tiling mode\n") ;
          exit(-1) ;
        }
        break;
      }
    case Tiled_LockFreeColorSIMD:
      {
        printf("\t Tiling mode is 'Lock-free color SIMD'\n");
        if ( !std::is_same<Hybrid_Seg_Iter, RAJA::seq_segit>::value ) {
          printf("Cannot have outer parallelism for this tiling mode\n") ;
          exit(-1) ;
        }
        break;
      }
    default :
      {
        printf("Unknown tiling mode!!!\n");
      }
  }

  if (lulesh_tiling_mode == Tiled_Index ||
      lulesh_tiling_mode == Tiled_Order) {
    printf("\t Mesh tiling = %i x %i x %i\n",
        lulesh_xtile, lulesh_ytile, lulesh_ztile) ;
  }
  domain->sizeX = edgeElems ;
  domain->sizeY = edgeElems ;
  domain->sizeZ = edgeElems ;
  domain->numElem = edgeElems*edgeElems*edgeElems ;

  domain->numNode = edgeNodes*edgeNodes*edgeNodes ;

  Index_t domElems = domain->numElem ;
  Index_t domNodes = domain->numNode ;

  domain->nodelist = Allocate<Index_t>(8*domElems) ;

  nidx = 0 ;
  zidx = 0 ;
  for (Index_t plane=0; plane<edgeElems; ++plane) {
    for (Index_t row=0; row<edgeElems; ++row) {
      for (Index_t col=0; col<edgeElems; ++col) {
        Index_p localNode = &domain->nodelist[8*zidx] ;
        localNode[0] = nidx ;
        localNode[1] = nidx + 1 ;
        localNode[2] = nidx + edgeNodes + 1 ;
        localNode[3] = nidx + edgeNodes ;
        localNode[4] = nidx + edgeNodes*edgeNodes ;
        localNode[5] = nidx + edgeNodes*edgeNodes + 1 ;
        localNode[6] = nidx + edgeNodes*edgeNodes + edgeNodes + 1 ;
        localNode[7] = nidx + edgeNodes*edgeNodes + edgeNodes ;
        ++zidx ;
        ++nidx ;
      }
      ++nidx ;
    }
    nidx += edgeNodes ;
  }

  domain->domNodeList = new RAJA::IndexSet() ;
  domain->domNodeList->push_back( RAJA::RangeSegment(0, domNodes) ) ;

  domain->domElemList = new RAJA::IndexSet() ;
  domain->matElemList = new RAJA::IndexSet() ;

  const Index_t xtile = lulesh_xtile ;
  const Index_t ytile = lulesh_ytile ;
  const Index_t ztile = lulesh_ztile ;

  switch (lulesh_tiling_mode) {

    case Canonical:
      {
        domain->domElemList->push_back( RAJA::RangeSegment(0, domElems) );

        domain->matElemList->push_back( RAJA::RangeSegment(0, domElems) ) ;
      }
      break ;

    case Tiled_Index:
      {
        for (Index_t zt = 0; zt < ztile; ++zt) {
          for (Index_t yt = 0; yt < ytile; ++yt) {
            for (Index_t xt = 0; xt < xtile; ++xt) {
              Index_t xbegin = edgeElems*( xt )/xtile ;
              Index_t xend = edgeElems*(xt+1)/xtile ;
              Index_t ybegin = edgeElems*( yt )/ytile ;
              Index_t yend = edgeElems*(yt+1)/ytile ;
              Index_t zbegin = edgeElems*( zt )/ztile ;
              Index_t zend = edgeElems*(zt+1)/ztile ;
              Index_t tileSize =
                (xend - xbegin)*(yend-ybegin)*(zend-zbegin) ;
              Index_t tileIdx[tileSize] ;
              Index_t idx = 0 ;

              for (Index_t plane = zbegin; plane<zend; ++plane) {
                for (Index_t row = ybegin; row<yend; ++row) {
                  for (Index_t col = xbegin; col<xend; ++col) {
                    tileIdx[idx++] =
                      (plane*edgeElems + row)*edgeElems + col ;
                  }
                }
              }
              domain->domElemList->push_back( RAJA::ListSegment(tileIdx, tileSize) );
              domain->matElemList->push_back( RAJA::ListSegment(tileIdx, tileSize) );
            }
          }
        }
      }
      break ;

    case Tiled_Order:
      {
        Index_t idx = 0 ;
        perm = Allocate<Index_t>(domElems) ;
        iperm = Allocate<Index_t>(domElems) ;
        Index_t tileBegin = 0 ;
        for (Index_t zt = 0; zt < ztile; ++zt) {
          for (Index_t yt = 0; yt < ytile; ++yt) {
            for (Index_t xt = 0; xt < xtile; ++xt) {
              Index_t xbegin = edgeElems*( xt )/xtile ;
              Index_t xend = edgeElems*(xt+1)/xtile ;
              Index_t ybegin = edgeElems*( yt )/ytile ;
              Index_t yend = edgeElems*(yt+1)/ytile ;
              Index_t zbegin = edgeElems*( zt )/ztile ;
              Index_t zend = edgeElems*(zt+1)/ztile ;
              Index_t tileSize =
                (xend - xbegin)*(yend-ybegin)*(zend-zbegin) ;

              for (Index_t plane = zbegin; plane<zend; ++plane) {
                for (Index_t row = ybegin; row<yend; ++row) {
                  for (Index_t col = xbegin; col<xend; ++col) {
                    perm[idx] =
                      (plane*edgeElems + row)*edgeElems + col ;
                    iperm[perm[idx]] = idx ;
                    ++idx ;
                  }
                }
              }
              Index_t tileEnd = tileBegin + tileSize ;
              domain->domElemList->push_back( RAJA::RangeSegment(tileBegin, tileEnd) );
              domain->matElemList->push_back( RAJA::RangeSegment(tileBegin, tileEnd) );
              tileBegin = tileEnd ;
            }
          }
        }
      }
      break ;

    case Tiled_LockFree:
      {
        buildLockFreeBlockIndexset( *domain->domElemList,
            edgeElems, edgeElems, edgeElems) ;

        buildLockFreeBlockIndexset ( *domain->matElemList,
            edgeElems, edgeElems, edgeElems) ;
      }
      break;

    case Tiled_LockFreeColor:
      {

        buildLockFreeColorIndexset( *domain->domElemList,
            domain->nodelist, domElems, 8, domNodes) ;

        buildLockFreeColorIndexset ( *domain->matElemList,
            domain->nodelist, domElems, 8, domNodes) ;
      }
      break;

    case Tiled_LockFreeColorSIMD:
      {
        perm = Allocate<Index_t>(domElems) ;
        iperm = Allocate<Index_t>(domElems) ;

        buildLockFreeColorIndexset( *domain->domElemList,
            domain->nodelist, domElems, 8, domNodes,
            perm, iperm) ;

        buildLockFreeColorIndexset ( *domain->matElemList,
            domain->nodelist, domElems, 8, domNodes,
            perm, iperm) ;
      }
      break;

    default :
      {
        printf("Only Tiled_LockFree or Canonical is implemented!!!\n");
        exit(-1) ;
      }
  }

  domain->symmX = new RAJA::IndexSet() ;
  {
    Index_t *nset = new Index_t[edgeNodes*edgeNodes] ;
    Index_t nidx = 0 ;
    for (Index_t i=0; i<edgeNodes; ++i) {
      Index_t planeInc = i*edgeNodes*edgeNodes ;
      for (Index_t j=0; j<edgeNodes; ++j) {
        nset[nidx++] = planeInc + j*edgeNodes ;
      }
    }
    domain->symmX->push_back( RAJA::ListSegment(nset, edgeNodes*edgeNodes) );
    delete [] nset ;
  }

  domain->symmY = new RAJA::IndexSet() ;
  {
    Index_t *nset = new Index_t[edgeNodes*edgeNodes] ;
    Index_t nidx = 0 ;
    for (Index_t i=0; i<edgeNodes; ++i) {
      Index_t planeInc = i*edgeNodes*edgeNodes ;
      for (Index_t j=0; j<edgeNodes; ++j) {
        nset[nidx++] = planeInc + j ;
      }
    }
    domain->symmY->push_back( RAJA::ListSegment(nset, edgeNodes*edgeNodes) );
    delete [] nset ;
  }

  domain->symmZ = new RAJA::IndexSet() ;
  {
    domain->symmZ->push_back( RAJA::RangeSegment(0, edgeNodes*edgeNodes) );
  }
  domain->lxim =
    AllocateTouch<elem_exec_policy, Index_t>(domain->domElemList, domElems) ;
  domain->lxip =
    AllocateTouch<elem_exec_policy, Index_t>(domain->domElemList, domElems) ;
  domain->letam =
    AllocateTouch<elem_exec_policy, Index_t>(domain->domElemList, domElems) ;
  domain->letap =
    AllocateTouch<elem_exec_policy, Index_t>(domain->domElemList, domElems) ;
  domain->lzetam =
    AllocateTouch<elem_exec_policy, Index_t>(domain->domElemList, domElems) ;
  domain->lzetap =
    AllocateTouch<elem_exec_policy, Index_t>(domain->domElemList, domElems) ;

  domain->elemBC =
    AllocateTouch<elem_exec_policy, Int_t>(domain->domElemList, domElems) ;

  domain->volo =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;

  domain->elemMass =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;

  domain->x =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;
  domain->y =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;
  domain->z =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;

  domain->xd =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;
  domain->yd =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;
  domain->zd =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;

  domain->xdd =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;
  domain->ydd =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;
  domain->zdd =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;

  domain->fx =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;
  domain->fy =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;
  domain->fz =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;

  domain->nodalMass =
    AllocateTouch<node_exec_policy, Real_t>(domain->domNodeList, domNodes) ;

  nidx = 0 ;
  tz = Real_t(0.) ;
  for (Index_t plane=0; plane<edgeNodes; ++plane) {
    ty = Real_t(0.) ;
    for (Index_t row=0; row<edgeNodes; ++row) {
      tx = Real_t(0.) ;
      for (Index_t col=0; col<edgeNodes; ++col) {
        domain->x[nidx] = tx ;
        domain->y[nidx] = ty ;
        domain->z[nidx] = tz ;
        ++nidx ;

        tx = Real_t(1.125)*Real_t(col+1)/Real_t(edgeElems) ;
      }

      ty = Real_t(1.125)*Real_t(row+1)/Real_t(edgeElems) ;
    }

    tz = Real_t(1.125)*Real_t(plane+1)/Real_t(edgeElems) ;
  }

  domain->dtfixed = Real_t(lulesh_time_step) ;
  domain->deltatime = ((domain->dtfixed < Real_t(0.0)) ?
      -domain->dtfixed : domain->dtfixed) ;
  domain->deltatimemultlb = Real_t(1.1) ;
  domain->deltatimemultub = Real_t(1.2) ;
  domain->stoptime = Real_t(lulesh_stop_time) ;
  domain->dtcourant = Real_t(1.0e+20) ;
  domain->dthydro = Real_t(1.0e+20) ;
  domain->dtmax = Real_t(1.0e-2) ;
  domain->time = Real_t(0.) ;
  domain->cycle = 0 ;

  domain->e_cut = Real_t(1.0e-7) ;
  domain->p_cut = Real_t(1.0e-7) ;
  domain->q_cut = Real_t(1.0e-7) ;
  domain->u_cut = Real_t(1.0e-7) ;
  domain->v_cut = Real_t(1.0e-10) ;

  domain->hgcoef = Real_t(3.0) ;
  domain->ss4o3 = Real_t(4.0)/Real_t(3.0) ;

  domain->qstop = Real_t(1.0e+12) ;
  domain->monoq_max_slope = Real_t(1.0) ;
  domain->monoq_limiter_mult = Real_t(2.0) ;
  domain->qlc_monoq = Real_t(0.5) ;
  domain->qqc_monoq = Real_t(2.0)/Real_t(3.0) ;
  domain->qqc = Real_t(2.0) ;

  domain->pmin = Real_t(0.) ;
  domain->emin = Real_t(-1.0e+15) ;

  domain->dvovmax = Real_t(0.1) ;

  domain->eosvmax = Real_t(1.0e+9) ;
  domain->eosvmin = Real_t(1.0e-9) ;

  domain->refdens = Real_t(1.0) ;

  for (Index_t i=0; i<domElems; ++i) {
    Real_t x_local[8], y_local[8], z_local[8] ;
    Index_p elemToNode = &domain->nodelist[8*i] ;
    for( Index_t lnode=0 ; lnode<8 ; ++lnode )
    {
      Index_t gnode = elemToNode[lnode];
      x_local[lnode] = domain->x[gnode];
      y_local[lnode] = domain->y[gnode];
      z_local[lnode] = domain->z[gnode];
    }

    Real_t volume = CalcElemVolume(x_local, y_local, z_local );
    domain->volo[i] = volume ;
    domain->elemMass[i] = volume ;
    for (Index_t j=0; j<8; ++j) {
      Index_t idx = elemToNode[j] ;
      domain->nodalMass[idx] += volume / Real_t(8.0) ;
    }
  }

  domain->lxim[0] = 0 ;
  for (Index_t i=1; i<domElems; ++i) {
    domain->lxim[i] = i-1 ;
    domain->lxip[i-1] = i ;
  }
  domain->lxip[domElems-1] = domElems-1 ;

  for (Index_t i=0; i<edgeElems; ++i) {
    domain->letam[i] = i ;
    domain->letap[domElems-edgeElems+i] = domElems-edgeElems+i ;
  }
  for (Index_t i=edgeElems; i<domElems; ++i) {
    domain->letam[i] = i-edgeElems ;
    domain->letap[i-edgeElems] = i ;
  }

  for (Index_t i=0; i<edgeElems*edgeElems; ++i) {
    domain->lzetam[i] = i ;
    domain->lzetap[domElems-edgeElems*edgeElems+i] = domElems-edgeElems*edgeElems+i ;
  }
  for (Index_t i=edgeElems*edgeElems; i<domElems; ++i) {
    domain->lzetam[i] = i - edgeElems*edgeElems ;
    domain->lzetap[i-edgeElems*edgeElems] = i ;
  }

  for (Index_t i=0; i<domElems; ++i) {
    domain->elemBC[i] = 0 ;
  }

  for (Index_t i=0; i<edgeElems; ++i) {
    Index_t planeInc = i*edgeElems*edgeElems ;
    Index_t rowInc = i*edgeElems ;
    for (Index_t j=0; j<edgeElems; ++j) {
      domain->elemBC[planeInc+j*edgeElems] |= 0x001 ;
      domain->elemBC[planeInc+j*edgeElems+edgeElems-1] |= 0x008 ;
      domain->elemBC[planeInc+j] |= 0x010 ;
      domain->elemBC[planeInc+j+edgeElems*edgeElems-edgeElems] |= 0x080 ;
      domain->elemBC[rowInc+j] |= 0x100 ;
      domain->elemBC[rowInc+j+domElems-edgeElems*edgeElems] |= 0x800 ;
    }
  }

  if (lulesh_tiling_mode == Tiled_Order ||
      lulesh_tiling_mode == Tiled_LockFreeColorSIMD) {

    {
      Index_t tmp[8*domElems] ;
      for (Index_t i=0; i<domElems; ++i) {
        for (Index_t j=0; j<8; ++j) {
          tmp[i*8+j] = domain->nodelist[perm[i]*8+j] ;
        }
      }
      for (Index_t i=0; i<8*domElems; ++i) {
        domain->nodelist[i] = tmp[i] ;
      }
    }

    {
      Real_t tmp[domElems] ;
      for (Index_t i=0; i<domElems; ++i) {
        tmp[i] = domain->volo[perm[i]] ;
      }
      for (Index_t i=0; i<domElems; ++i) {
        domain->volo[i] = tmp[i] ;
      }
    }

    {
      Real_t tmp[domElems] ;
      for (Index_t i=0; i<domElems; ++i) {
        tmp[i] = domain->elemMass[perm[i]] ;
      }
      for (Index_t i=0; i<domElems; ++i) {
        domain->elemMass[i] = tmp[i] ;
      }
    }

    {
      Index_t tmp[6*domElems] ;
      for (Index_t i=0; i<domElems; ++i) {
        tmp[i*6+0] = iperm[domain->lxim[perm[i]]] ;
        tmp[i*6+1] = iperm[domain->lxip[perm[i]]] ;
        tmp[i*6+2] = iperm[domain->letam[perm[i]]] ;
        tmp[i*6+3] = iperm[domain->letap[perm[i]]] ;
        tmp[i*6+4] = iperm[domain->lzetam[perm[i]]] ;
        tmp[i*6+5] = iperm[domain->lzetap[perm[i]]] ;
      }
      for (Index_t i=0; i<domElems; ++i) {
        domain->lxim[i] = tmp[i*6+0] ;
        domain->lxip[i] = tmp[i*6+1] ;
        domain->letam[i] = tmp[i*6+2] ;
        domain->letap[i] = tmp[i*6+3] ;
        domain->lzetam[i] = tmp[i*6+4] ;
        domain->lzetap[i] = tmp[i*6+5] ;
      }
    }

    {
      Int_t tmp[domElems] ;
      for (Index_t i=0; i<domElems; ++i) {
        tmp[i] = domain->elemBC[perm[i]] ;
      }
      for (Index_t i=0; i<domElems; ++i) {
        domain->elemBC[i] = tmp[i] ;
      }
    }
    Release(&iperm) ;
    Release(&perm) ;
  }

  domain->e =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;
  domain->p =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;

  domain->q =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;
  domain->ql =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;
  domain->qq =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;

  domain->v =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;
  domain->delv =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;
  domain->vdov =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;

  domain->arealg =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;

  domain->ss =
    AllocateTouch<elem_exec_policy, Real_t>(domain->domElemList, domElems) ;

  for (Index_t i=0; i<domElems; ++i) {
    domain->v[i] = Real_t(1.0) ;

  }

  domain->e[0] = Real_t(3.948746e+7) ;
  timer_cycle.start("timer_cycle");
  while((domain->time < domain->stoptime) && (domain->cycle < maxIter)) {
    TimeIncrement(domain) ;
    LagrangeLeapFrog(domain) ;

    if ( show_run_progress ) {
      printf("cycle = %d, time = %e, dt=%e\n",
          int(domain->cycle),double(domain->time), double(domain->deltatime) ) ;
    }
  }
  timer_cycle.stop("timer_cycle");

  timer_main.stop("timer_main");

  printf("Total Cycle Time (sec) = %Lf\n", timer_cycle.elapsed() );
  printf("Total main Time (sec) = %Lf\n", timer_main.elapsed() );

  Release(&domain) ;

  return 0 ;
}
