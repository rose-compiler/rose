//#include "CH_Timer.H"
#include "SPACE.H"
#include <cstdlib>
#include <functional>
#include <algorithm>

template <class T, unsigned int C, unsigned char D, unsigned char E> RectMDArray<T,C,D,E>::RectMDArray():m_rawPtr(NULL)
{
};
template <class T, unsigned int C, unsigned char D, unsigned char E> void RectMDArray<T,C,D,E>::define(const Box& a_box)
{
  m_box=a_box;
  if(reportMemory)
    {
      memory+=dataSize()*sizeof(T);
      std::cout<<memory/(1024)<<"__"<<std::endl;
    }

// DQ (2/14/2015): commented out due to problem with C++11 support in ROSE.
  m_data=std::shared_ptr<T>(new T [dataSize()], [](T* p) { delete[] p;});

  m_rawPtr = m_data.get();
}
template <class T, unsigned int C, unsigned char D, unsigned char E> RectMDArray<T,C,D,E>::RectMDArray(const Box& a_box) 
{
  define(a_box);
};
template <class T, unsigned int C, unsigned char D, unsigned char E> RectMDArray<T,C,D,E>::~RectMDArray()
{
  if(reportMemory && m_data.unique())
    {
      memory-=dataSize()*sizeof(T);
      std::cout<<memory/(1024)<<"__"<<std::endl;
    }
};
template <class T, unsigned int C, unsigned char D, unsigned char E> RectMDArray<T,C,D,E>::RectMDArray(const RectMDArray<T,C,D,E>& a_srcArray)
{
  define(a_srcArray.m_box);
  for (int k = 0;k < dataSize() ;k++)
    {
      m_rawPtr[k] = a_srcArray.m_rawPtr[k];
    }
};

template <class T, unsigned int C, unsigned char D, unsigned char E> RectMDArray<T,C,D,E>::RectMDArray(RectMDArray<T,C,D,E>&& a_srcArray):m_data(a_srcArray.m_data), m_rawPtr(a_srcArray.m_rawPtr),m_box(a_srcArray.m_box)
{
};


template <class T, unsigned int C, unsigned char D, unsigned char E> 
RectMDArray<T,C,D,E>::RectMDArray(std::shared_ptr<T> a_data, T* a_ptr, const Box& a_box):m_data(a_data),m_rawPtr(a_ptr),m_box(a_box)
{

};

template <class T, unsigned int C, unsigned char D, unsigned char E> RectMDArray<T,C,D,E>& RectMDArray<T,C,D,E>::operator=(const RectMDArray<T,C,D,E>& a_srcArray)
{
  define(a_srcArray.m_box);
  for (int k = 0;k < dataSize() ;k++)
    {
      m_rawPtr[k] = a_srcArray.m_rawPtr[k];
    }
  return *this;
};
template <class T, unsigned int C, unsigned char D, unsigned char E> bool RectMDArray<T,C,D,E>::defined() const
{
  return bool(m_data);
}
template <class T, unsigned int C, unsigned char D, unsigned char E> void RectMDArray<T,C,D,E>::setVal(const T& a_val) const
{
  //CH_TIME("setval");
  if (m_data)
    {
      for (int k = 0; k < dataSize();k++)
        {
          m_rawPtr[k] = a_val;
        }
    }
};
template <class T, unsigned int C, unsigned char D, unsigned char E> void RectMDArray<T,C,D,E>::copyTo(RectMDArray<T,C,D,E>& a_dest) const
{
  static_assert(D==1 && E==1,"copyto only defined for vector RectMDArray");  
  Box bxInt = m_box & a_dest.m_box;
  for (Point pt=bxInt.getLowCorner();bxInt.notDone(pt);bxInt.increment(pt))
    {
      for(unsigned int i=0; i<C; i++)
	a_dest(pt,i) = getConst(pt,i);
    }
};    

template <class T, unsigned int C, unsigned char D, unsigned char E> void RectMDArray<T,C,D,E>::copyTo(RectMDArray<T,C,D,E> & a_dest,
                                                                                                       const  Box           & a_validBoxSrc, 
                                                                                                       const  Point         & a_shift) const
{
  static_assert(D==1 && E==1,"copyto only defined for vector RectMDArray");  

  //shift is defined as a_src.box().getLowCorner() - a_dst.getLowCorner();
  //if you want them to line up exactly (typically you do not)
  //it is  how far you have to move dst to get it into the source index space

  Box bx = a_dest.getBox();
  bx = bx.shift(a_shift);
  bx = bx & a_validBoxSrc;
    
  for (Point pt=bx.getLowCorner();bx.notDone(pt);bx.increment(pt))
    {
      Point dstPoint = pt - a_shift;
      for(unsigned int i=0; i<C; i++)
        {
          a_dest(dstPoint,i) = getConst(pt,i);
        }
    }
}   
 
template <class T, unsigned int C, unsigned char D, unsigned char E> void RectMDArray<T,C,D,E>::copyTo(RectMDArray<T,C,D,E>& a_dest, const Point& a_shift) const
{
  Box bxInt = m_box & a_dest.m_box;
  for (Point pt=bxInt.getLowCorner();bxInt.notDone(pt);bxInt.increment(pt))
    {
      static_assert(D==1 && E==1,"copyto only defined for vector RectMDArray");  
      for(unsigned int i=0; i<C; i++)
        {
          a_dest(pt,i) = getConst(pt+a_shift,i);
        }
    }
}
template <class T, unsigned int C, unsigned char D, unsigned char E> 
RectMDArray<T,C,D,E>& RectMDArray<T,C,D,E>::operator*=(T scale)
{
  const unsigned int n = dataSize();
  for(unsigned int i=0; i<n; i++)
    {
      m_rawPtr[i]*=scale;
    }
  return *this;
}

template <class T, unsigned int C, unsigned char D, unsigned char E> 
RectMDArray<T,C,D,E>& RectMDArray<T,C,D,E>::axby(T a, const RectMDArray<T,C,D,E>& x, T b, const RectMDArray<T,C,D,E>& y)
{
  if(!(m_box == x.m_box) || !(m_box == y.m_box))
    {
      std::cerr<<"axby not implemented for box intersection yet"<<std::endl;
      abort();
    }
  const unsigned int n = dataSize();
  for(unsigned int i=0; i<n; i++)
    {
      m_rawPtr[i]+= a*x.m_rawPtr[i]+b*y.m_rawPtr[i];
    }
  return *this;
}

template <class T, unsigned int C, unsigned char D, unsigned char E>
RectMDArray<T,C,D,E>& RectMDArray<T,C,D,E>::plus(T scale, const RectMDArray<T,C,D,E>& a_rhs)
{
  if(!(m_box == a_rhs.m_box))
    {
      std::cerr<<"plus not implemented for box intersection yet"<<std::endl;
      abort();
    }
  const unsigned int n = dataSize();
  for(unsigned int i=0; i<n; i++)
    {
      m_rawPtr[i]+=scale*a_rhs.m_rawPtr[i];
    }
  return *this;
}

template <class T, unsigned int C, unsigned char D, unsigned char E>
RectMDArray<T,C,D,E>& RectMDArray<T,C,D,E>::operator*=(const RectMDArray<T,C,D,E>& a_rhs)
{
  if(!(m_box == a_rhs.m_box))
    {
      std::cerr<<"operator*= not implemented for box intersection yet"<<std::endl;
      abort();
    }
  const unsigned int n = dataSize();
  for(unsigned int i=0; i<n; i++)
    {
      m_rawPtr[i]*=a_rhs.m_rawPtr[i];
    }
  return *this;
}

template <class T, unsigned int C, unsigned char D, unsigned char E> 
RectMDArray<T,C,D,E>& RectMDArray<T,C,D,E>::plus(const RectMDArray<T,C,D,E>& a_rhs)
{
  if(m_box == a_rhs.m_box)
    {
      const unsigned int n = dataSize();
      for(unsigned int i=0; i<n; i++)
	{
	  m_rawPtr[i]+=a_rhs.m_rawPtr[i];
	}
    }
  else
    {
      Box unibox = m_box & a_rhs.m_box;
      Point lo = unibox.getLowCorner(); Point hi=unibox.getHighCorner(); hi[0]=lo[0];
      Box crossBox(lo, hi);
      unsigned int npencil = unibox.size(0);
      for(unsigned char e=0; e<E; e++)
	for(unsigned char d=0; d<D; d++)
	  for(unsigned int c=0; c<C; c++)
	    {
	      T* a = m_rawPtr+m_box.sizeOf()*c+m_box.sizeOf()*C*d+m_box.sizeOf()*C*D*e;
	      const T* b = a_rhs.m_rawPtr+a_rhs.m_box.sizeOf()*c+a_rhs.m_box.sizeOf()*C*d+a_rhs.m_box.sizeOf()*C*D*e;
	      for(Point pt(lo); unibox.notDone(pt); unibox.increment(pt))
		{
		  T* ap = a+m_box.getIndex(pt);
		  const T* bp = b+a_rhs.m_box.getIndex(pt);
		  for(unsigned int i=0; i<npencil; i++,ap++,bp++)
		    {
		      *ap += *bp;
		    }
		}
	    }
    }
  return *this;
}

template <class T, unsigned int C, unsigned char D, unsigned char E> T& RectMDArray<T,C,D,E>::operator[](const Point& a_iv)
{
   static_assert(C==1 && D==1 && E==1,"operator[] only defined for scalar RectMDArray");  
   return get(a_iv);
}
template <class T, unsigned int C, unsigned char D, unsigned char E> const T& RectMDArray<T,C,D,E>::operator[](const Point& a_iv) const
{
  static_assert(C==1 && D==1 && E==1,"operator[] only defined for scalar RectMDArray");  
  return getConst(a_iv);
}

template <class T, unsigned int C, unsigned char D, unsigned char E> T& RectMDArray<T,C,D,E>::operator()(const Point& a_iv, unsigned int a_comp)
{
  static_assert(D==1 && E==1,"operator() only defined for vector RectMDArray");  
  return get(a_iv, a_comp);
}
template <class T, unsigned int C, unsigned char D, unsigned char E> const T& RectMDArray<T,C,D,E>::operator()(const Point& a_iv, unsigned int a_comp) const
{

  static_assert(D==1 && E==1,"operator() only defined for vector RectMDArray");  
  return getConst(a_iv, a_comp);
}
template <class T, unsigned int C, unsigned char D, unsigned char E> T& RectMDArray<T,C,D,E>::operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d)
{
  static_assert(E==1,"operator() only defined up to D component");  
  return get2(a_iv, a_comp,a_d);
};
template <class T, unsigned int C, unsigned char D, unsigned char E> const T& RectMDArray<T,C,D,E>::operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d) const
{

  static_assert(E==1,"operator() only defined up to D component");  
  return getConst2(a_iv, a_comp,a_d);
};

template <class T, unsigned int C, unsigned char D, unsigned char E> T& RectMDArray<T,C,D,E>::operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e)
{  
  return get3(a_iv, a_comp,a_d, a_e);
};
template <class T, unsigned int C, unsigned char D, unsigned char E> const T& RectMDArray<T,C,D,E>::operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e) const
{ 
  return getConst3(a_iv, a_comp,a_d,a_e);
};



template <class T, unsigned int C, unsigned char D, unsigned char E> 
T& RectMDArray<T,C,D,E>::get(const Point& a_iv, unsigned int a_comp)
{

  // CH_TIME("index");
  int m=m_box.sizeOf();
  int k = m_box.getIndex(a_iv);
  assert(k < m);
  assert(k >= 0);
  return m_rawPtr[k+m*a_comp];
};
template <class T, unsigned int C, unsigned char D, unsigned char E> 
T& RectMDArray<T,C,D,E>::get2(const Point& a_iv, unsigned int a_comp, unsigned char a_d)
{

  // CH_TIME("index");
  int m=m_box.sizeOf();
  int k = m_box.getIndex(a_iv);
  assert(k < m);
  assert(k >= 0);
  return m_rawPtr[k+m*a_comp+a_d*m*C];
};

template <class T, unsigned int C, unsigned char D, unsigned char E> 
T& RectMDArray<T,C,D,E>::get3(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e)
{

  // CH_TIME("index");
  int m=m_box.sizeOf();
  int k = m_box.getIndex(a_iv);
  assert(k < m);
  assert(k >= 0);
  return m_rawPtr[k+m*a_comp+a_d*m*C+a_e*m*C*D];
};

template <class T, unsigned int C, unsigned char D, unsigned char E> 
const T& RectMDArray<T,C,D,E>::getConst(const Point& a_iv, unsigned int a_comp) const
{

  // CH_TIME("index");
  int m=m_box.sizeOf();
  int k = m_box.getIndex(a_iv);
  assert(k < m);
  assert(k >= 0);
  return m_rawPtr[k+m*a_comp];
};
template <class T, unsigned int C, unsigned char D, unsigned char E> 
const T& RectMDArray<T,C,D,E>::getConst2(const Point& a_iv, unsigned int a_comp, unsigned char a_d) const
{

  // CH_TIME("index");
  int m=m_box.sizeOf();
  int k = m_box.getIndex(a_iv);
  assert(k < m);
  assert(k >= 0);
  return m_rawPtr[k+m*a_comp+a_d*m*C];
};

template <class T, unsigned int C, unsigned char D, unsigned char E> 
const T& RectMDArray<T,C,D,E>::getConst3(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e) const
{

  // CH_TIME("index");
  int m=m_box.sizeOf();
  int k = m_box.getIndex(a_iv);
  assert(k < m);
  assert(k >= 0);
  return m_rawPtr[k+m*a_comp+a_d*m*C+a_e*m*C*D];
};


template <class T, unsigned int C, unsigned char D, unsigned char E>
inline T& RectMDArray<T,C,D,E>::operator[](int a_index) const
{ 
  assert((a_index>=0) && ( a_index < dataSize())); 
  return m_rawPtr[a_index];
}


template <class T, unsigned int C, unsigned char D, unsigned char E> void RectMDArray<T,C,D,E>::print()
{
  if (m_data)
    {
      int i=0;
      for (int k = 0; k < dataSize();k++)
        {
          cout << m_rawPtr[k] << " ";
	  i++;
	  if(i==BLOCKSIZE)
	    {
	      cout << endl;
	      i=0;
	    }
        }
    }
};

template <class T, unsigned int C, unsigned char D, unsigned char E>
inline size_t RectMDArray<T,C,D,E>::dataSize() const {return m_box.sizeOf()*C*D*E;}


template<class T, unsigned int C0, unsigned int C1>
RectMDArray<T,C1> slice(RectMDArray<T,C0>& a_original, const Interval& a_interval)
{
  const Box& b=a_original.getBox();
  RectMDArray<T,C1> rtn(a_original.m_sliceData(), a_original.m_slicePtr()+b.sizeOf()*a_interval.low, b);
  return rtn; // will invoke the move constructor, thus carry the pointer values, not a deep copy of the rtn object
}

template<class T, unsigned int C0, unsigned int C1>
const RectMDArray<T,C1> slice(const RectMDArray<T,C0>& a_original, const Interval& a_interval)
{
  const Box& b=a_original.getBox();
  RectMDArray<T,C0>* src = const_cast<RectMDArray<T,C0>*>(&a_original);
  const RectMDArray<T,C1> rtn(src->m_sliceData(), src->m_slicePtr()+b.sizeOf()*a_interval.low, b);
  return rtn; // will invoke the move constructor, thus carry the pointer values, not a deep copy of the rtn object
}

template<class T, unsigned int C, unsigned char D0, unsigned char D1>
RectMDArray<T,C,D1> slice(RectMDArray<T,C,D0>& a_original, const Interval& a_interval)
{
  const Box& b=a_original.getBox();
  RectMDArray<T,C,D1> rtn(a_original.m_sliceData(), a_original.m_slicePtr()+b.sizeOf()*C*a_interval.low,b);
  return rtn;
}

template<class T, unsigned int C, unsigned char D0, unsigned char D1>
const RectMDArray<T,C,D1> slice(const RectMDArray<T,C,D0>& a_original, const Interval& a_interval)
{
  const Box& b=a_original.getBox();
  RectMDArray<T,C,D0>* src = const_cast<RectMDArray<T,C,D0>*>(&a_original);
  const RectMDArray<T,C,D1> rtn(src->m_sliceData(), src->m_slicePtr()+b.sizeOf()*C*a_interval.low, b);
  return rtn; // will invoke the move constructor, thus carry the pointer values, not a deep copy of the rtn object
}


template<class T, unsigned int C, unsigned char D, unsigned char E0, unsigned char E1>
RectMDArray<T,C,D,E1> slice(RectMDArray<T,C,D,E0>& a_original, const Interval& a_interval)
{
  const Box& b=a_original.getBox();
  RectMDArray<T,C,D,E1> rtn(a_original.m_sliceData(), a_original.m_slicePtr()+b.sizeOf()*C*D*a_interval.low,b);
  return rtn;
}

template<class T, unsigned int C, unsigned char D, unsigned char E0, unsigned char E1>
const RectMDArray<T,C,D,E1> slice(const RectMDArray<T,C,D,E0>& a_original, const Interval& a_interval)
{
  const Box& b=a_original.getBox();
  RectMDArray<T,C,D,E0>* src = const_cast<RectMDArray<T,C,D,E0>*>(&a_original);
  const RectMDArray<T,C,D,E1> rtn(src->m_sliceData(), src->m_slicePtr()+b.sizeOf()*C*D*a_interval.low,b);
  return rtn;
}

/// pointwise operator

template<class T, unsigned int Cdest, unsigned int Csrc, typename Func> 
void forall(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src, const Func& F, const Box& a_box)
{
  std::function<T&(const Point&, unsigned int)> dest = std::bind(&RectMDArray<T,Cdest>::get,&a_dest,std::placeholders::_1,std::placeholders::_2);
  std::function<const T&(const Point&, unsigned int)> src = std::bind(&RectMDArray<T,Csrc>::getConst,&a_src, std::placeholders::_1, std::placeholders::_2);   
  std::function<T&(unsigned int)> d1;
  std::function<const T&(unsigned int)> s1;
  Tensor<T,Cdest> d(d1);
  CTensor<T,Csrc> s(s1); 
  for ( Point pt = a_box.getLowCorner();a_box.notDone(pt);a_box.increment(pt))
    {
      d1 = std::bind(dest,pt,std::placeholders::_1);
      d  = d1;
      s1 = std::bind(src,pt, std::placeholders::_1);
      s  = s1;
      F(d,s);
    }
}

template<class T, unsigned int C, unsigned char D, unsigned char E, typename Func> 
void forall(RectMDArray<T,C>& a_dest, const RectMDArray<T,C,D,E>& a_src, const Func& F, const Box& a_box)
{
  std::function<T&(const Point&, unsigned int)> dest = std::bind(&RectMDArray<T,C>::get,&a_dest,std::placeholders::_1,std::placeholders::_2);
  std::function<const T&(const Point&, unsigned int, unsigned char, unsigned char)> src = std::bind(&RectMDArray<T,C,D,E>::getConst3,&a_src,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4);   
  std::function<T&(unsigned int)> d1;
  std::function<const T&(unsigned int, unsigned char, unsigned char)> s1;
  Tensor<T,C> d(d1);
  CTensor<T,C,D,E> s(s1); 
  for ( Point pt = a_box.getLowCorner();a_box.notDone(pt);a_box.increment(pt))
    {
      d1 = std::bind(dest,pt,std::placeholders::_1);
      d  = d1;
      s1 = std::bind(src,pt,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
      s  = s1;
      F(d,s);
    }
}

template<class T, unsigned int Cdest, unsigned int Csrc, unsigned char D, typename Func> 
void forall(RectMDArray<T,Cdest,D>& a_dest, const RectMDArray<T,Csrc>& a_src, const Func& F, const Box& a_box)
{
  std::function<T&(const Point&, unsigned int,unsigned char)> dest = std::bind(&RectMDArray<T,Cdest,D>::get2,&a_dest);
  std::function<const T&(const Point&, unsigned int)> src = std::bind(&RectMDArray<T,Csrc>::getConst,&a_src);   
  std::function<T&(unsigned int, unsigned char)> d1;
  std::function<const T&(unsigned int)> s1;
  Tensor<T,Cdest,D> d(d1);
  CTensor<T,Csrc> s(s1); 
  for ( Point pt = a_box.getLowCorner();a_box.notDone(pt);a_box.increment(pt))
    {
      d1 = std::bind(dest,pt);
      d  = d1;
      s1 = std::bind(src,pt);
      s  = s1;
      F(d,s);
    }
}

template<class T, unsigned int C, unsigned char D, typename Func> 
void forall(RectMDArray<T>& a_dest, const RectMDArray<T,C,D>& a_src, const Func& F, const Box& a_box)
{

  std::function<const T&(const Point&, unsigned int, unsigned char)> src = std::bind(&RectMDArray<T,C,D>::getConst2,&a_src,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);   
  std::function<const T&(unsigned int, unsigned char)> s1;
  CTensor<T,C,D> s(s1); 
  for ( Point pt = a_box.getLowCorner();a_box.notDone(pt);a_box.increment(pt))
    {
      s1 = std::bind(src,pt,std::placeholders::_1,std::placeholders::_2);
      s  = s1;
      F(a_dest.get(pt),s);
    }
}



// /// pointwise operator in-place
template<class T, typename Func> 
void forall(RectMDArray<T>& a_dest, const Func& F, const Box& a_box)
{
  for (Point pt = a_box.getLowCorner();a_box.notDone(pt);a_box.increment(pt))
    {
      F(a_dest.get(pt));
    }
}

// /// pointwise operator out-of-place for just a scalar holder.  skip all the tensor mojo
template<class T, unsigned int C, typename Func> 
void forall(RectMDArray<T,C>& a_dest, const RectMDArray<T,C>& a_src, const Func& F, const Box& a_box)
{
  for (Point pt = a_box.getLowCorner();a_box.notDone(pt);a_box.increment(pt))
    {
      F(a_dest.get(pt),a_src.getConst(pt));
    }
}

// /// pointwise operator out-of-place for just a scalar holder.  skip all the tensor mojo
/// but here we add striding mojo
template<class T, unsigned int C, typename Func> 
void forall_stride(RectMDArray<T,C>& a_dest, const RectMDArray<T,C>& a_src, const Func& F, const Box& a_box,
                  const int& a_stride, const int & a_start)
{
  Point pstart = getUnitv(0); 
  assert(a_stride >= 1);
  assert(a_start  >= 0);

  pstart *= a_stride;
  pstart = a_box.getLowCorner() + pstart;
  Point pt = pstart;
  while(a_box.notDone(pt))
    {
      F(a_dest.get(pt),a_src.getConst(pt));

      for(int iinc = 0; iinc < a_stride; iinc++)
        {
          a_box.increment(pt);
        }
    }
}

// /// scalar
template<class T, typename Func> 
T forall_max_scal(RectMDArray<T>& a_dest, const RectMDArray<T>& a_src, const Func& F, const Box& a_box)
{
  T maxval = 0;
  for (Point pt = a_box.getLowCorner();a_box.notDone(pt);a_box.increment(pt))
    {
      T locval = F(a_dest.get(pt), a_src.getConst(pt));
      maxval = std::max(maxval, locval);
    }
  return maxval;
}

/// pair pointwise operator with max reduction

template<class T, unsigned int Cdest, unsigned int Csrc, typename Func> 
T forall_max(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src,const Func& F, const Box& a_box)
{
  std::function<T&(const Point&, unsigned int)> dest = std::bind(&RectMDArray<T,Cdest>::get,&a_dest,std::placeholders::_1,std::placeholders::_2);
  std::function<const T&(const Point&, unsigned int)> src = std::bind(&RectMDArray<T,Csrc>::getConst,&a_src,std::placeholders::_1,std::placeholders::_2);
  Point pt = a_box.getLowCorner();  a_box.increment(pt);    
  std::function<T&(unsigned int)> d1 = std::bind(dest,pt,std::placeholders::_1);
  std::function<const T&(unsigned int)> s1 = std::bind(src,pt,std::placeholders::_1);
  Tensor<T,Cdest> d(d1);
  CTensor<T,Csrc>  s(s1); 
  T tmax = F(d,s);
  for (;a_box.notDone(pt);a_box.increment(pt))
    {
      d1=std::bind(dest,pt,std::placeholders::_1);
      d = d1;
      s1 = std::bind(src,pt,std::placeholders::_1);
      s = s1;
      tmax = std::max(tmax,F(d,s));
    }
  return tmax;
}

