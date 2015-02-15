template <class T> RectMDArray<T>::RectMDArray()
{
  m_isDefined = false;
};
template <class T> void RectMDArray<T>::define(const Box& a_box)
{
  if (!m_isDefined)
    {
      m_box=a_box;
      m_data = new T[a_box.sizeOf()];
      m_isDefined=true;
    }
  else
    {
      m_box = a_box;
      delete [] m_data;
      m_data = new T[a_box.sizeOf()];
      m_isDefined=true;
    }
};
template <class T> RectMDArray<T>::RectMDArray(const Box& a_box) 
{
  m_box=a_box;
  m_data = new T[a_box.sizeOf()];
  m_isDefined=true;
};
template <class T> RectMDArray<T>::~RectMDArray()
{
  if (m_isDefined)
    {
      delete [] m_data;
    }
};
template <class T> RectMDArray<T>::RectMDArray(const RectMDArray<T>& a_srcArray)
{
  if (a_srcArray.m_isDefined)
    {
      m_box = a_srcArray.m_box;
      m_isDefined = true;
      m_data = new T[m_box.sizeOf()];
      for (int k = 0;k < m_box.sizeOf() ;k++)
        {
          m_data[k] = a_srcArray.m_data[k];
        }
    }
};
template <class T> RectMDArray<T>& RectMDArray<T>::operator=(const RectMDArray<T>& a_srcArray)
{
#if 0
  if (m_isDefined)
    {
      m_isDefined = false;
      delete [] m_data;
      m_box = Box();
    }
// This is a problem for ROSE (comment out for now).
  if (a_srcArray.m_isDefined)
    {
      m_box = a_srcArray.m_box;
      m_isDefined = true;
      m_data = new T[m_box.sizeOf()];
      for (int k = 0;k < m_box.sizeOf() ;k++)
        {
          m_data[k] = a_srcArray.m_data[k];
        }
    }
#endif
  return *this;
};
template <class T> void RectMDArray<T>::setVal(const T& a_val) const
{
  if (m_isDefined)
    {
      for (int k = 0; k < m_box.sizeOf();k++)
        {
          m_data[k] = a_val;
        }
    }
};
template <class T> void RectMDArray<T>::copyTo(const RectMDArray<T>& a_dest) const
{
  Box bxInt = m_box*a_dest.m_box;
  for (Point pt=bxInt.getLowCorner();bxInt.notDone(pt);bxInt.increment(pt))
    {
      a_dest[pt] = (*this)[pt];
    }
};    
template <class T> T& RectMDArray<T>::operator[](const Point& a_iv) const
{
  assert(m_isDefined);
  int k = m_box.getIndex(a_iv);
  assert(k < m_box.sizeOf());
   assert(k >= 0);
  return m_data[k];
};
template <class T> T* RectMDArray<T>::getPointer() const
{
  // assert(m_isDefined);
  return m_data;
};

template <class T> void RectMDArray<T>::print()
{
  if (m_isDefined)
    {
      int i=0;
      for (int k = 0; k < m_box.sizeOf();k++)
        {
          cout << m_data[k] << " ";
	  i++;
	  if(i==BLOCKSIZE)
	    {
	      cout << endl;
	      i=0;
	    }
        }
    }
};
