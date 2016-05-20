#include <cassert>
#include "SPACE.H"
#include "WriteRectMDArray.H"
#include <math.h>

// Default Constructor
template<class T, unsigned int C, unsigned char D, unsigned char E> 
LevelData<T,C,D,E>::LevelData()
{
}
// Destructor
template<class T, unsigned int C, unsigned char D, unsigned char E> 
LevelData<T, C, D, E>::
~LevelData()
{
}

// Define a non-trivial LevelData. Used by constructor. 
template<class T, unsigned int C, unsigned char D, unsigned char E> 
void
LevelData<T, C, D, E>::
define(const BoxLayout a_boxLayout, int a_ghostSize, int a_coarseningFactor)
{
  m_boxLayout = a_boxLayout;
  m_ghostSize = a_ghostSize;
  m_coarseningFactor = a_coarseningFactor;
  assert(a_coarseningFactor == 1);
  int boxsize = a_boxLayout.getBoxsize();

  Point lc;
  Point hc;
  vector<Point> points = m_boxLayout.getPatchLocs();
  m_data.resize(points.size());
  for (vector<Point>::iterator it = points.begin(); it != points.end(); ++it)
    {
      Point p = *it;
      lc = p * boxsize / m_coarseningFactor;
      hc = (p * boxsize + getOnes() * (boxsize-1)) / m_coarseningFactor;
      m_data[m_boxLayout.getPatchIndex(*it)] = RectMDArray<T,C,D,E>(Box(lc,hc).grow(m_ghostSize));
    }
};

// Build a LevelData from a box layout and other parameters. Calls define()
template<class T, unsigned int C, unsigned char D, unsigned char E> 
LevelData<T, C, D, E>::
LevelData(const BoxLayout a_boxLayout,int a_ghostSize,int a_coarseningFactor)
{
  (*this).define(a_boxLayout,a_ghostSize, a_coarseningFactor);
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
const RectMDArray<T, C, D, E>&
LevelData<T, C, D, E>::
operator[](int a_index) const
{
  return m_data[a_index];
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
const RectMDArray<T, C, D, E>&
LevelData<T, C, D, E>::
operator[](Point& a_pt) const
{
  ///We are assuming only the iterator will be used to index.
  ///We do the validation in the iterator, so we dont need it here.
  return m_data[m_boxLayout.getPatchIndex(a_pt)];
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
RectMDArray<T, C, D, E>&
LevelData<T, C, D, E>::
operator[](int a_index)
{
  return m_data[a_index];
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
RectMDArray<T, C, D, E>&
LevelData<T, C, D, E>::
operator[](Point& a_pt)
{
  ///We are assuming only the iterator will be used to index.
  ///We do the validation in the iterator, so we dont need it here.
  return m_data[m_boxLayout.getPatchIndex(a_pt)];
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
int 
LevelData<T, C, D, E>::
getGhostSize() const
{
  return m_ghostSize;
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
T 
LevelData<T, C, D, E>::
mod(T& a_lhs, T& a_rhs) const
{
  return fmod(fmod(a_lhs, a_rhs) + a_rhs, a_rhs);
    //  return (a_lhs%a_rhs + a_rhs) % a_rhs;
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
void 
LevelData<T, C, D, E>::
setVal(const T & a_val)
{
  BLIterator iterator(m_boxLayout);
  for (iterator = iterator.begin(); iterator != iterator.end(); ++iterator)
    {
      Point point = *iterator;
      RectMDArray<T, C, D, E> & thisRMDA = (*this)[point];
      thisRMDA.setVal(a_val);
    }
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
void 
LevelData<T, C, D, E>::
copyTo(LevelData& a_dest, bool a_copyGhost) const
{
  CH_TIMERS("LevelData::copyTo");
  assert(m_boxLayout.getDomain()==a_dest.m_boxLayout.getDomain());
  if (m_boxLayout.sameBLI(a_dest.m_boxLayout))
    {
      const RectMDArray<bool>& sourceBitmap = m_boxLayout.getBitmap();
      const RectMDArray<bool>& destBitmap = a_dest.getBoxLayout().getBitmap();
      BLIterator iterator(m_boxLayout);
      Point no_shift(getZeros());
      for (iterator = iterator.begin(); iterator != iterator.end(); ++iterator)
        {
          Point point = *iterator;
          if (destBitmap[point]) {
            
            RectMDArray<T, C, D, E>       & destRMDA   =  a_dest[point];
            const RectMDArray<T, C, D, E> & sourceRMDA = (*this)[point];
            if (a_copyGhost) {
              sourceRMDA.copyTo(destRMDA);
            } else {
              sourceRMDA.copyTo(destRMDA,m_boxLayout[point],no_shift);
              
            }
          }
        }
    }
  // Iterate over dest. Copy from unique source if it exists.
  else if (m_boxLayout.getBlockPower() < a_dest.m_boxLayout.getBlockPower())
    {
      int refratio = m_boxLayout.getBoxsize()/a_dest.m_boxLayout.getBoxsize();
      const RectMDArray<bool>& sourceBitmap = m_boxLayout.getBitmap();
      const RectMDArray<bool>& destBitmap = a_dest.getBoxLayout().getBitmap();
      //cout << a_dest.m_boxLayout << endl;
      BLIterator iterator(a_dest.m_boxLayout);
      Point no_shift(getZeros());
      //cout << "looping over destination, refratio = "<< refratio << endl;
      for (iterator = iterator.begin(); iterator != iterator.end(); ++iterator)
        {
          Point point = *iterator;
          Point pointSrc = point / refratio;
          if (sourceBitmap[pointSrc])
              {
               
                
                RectMDArray<T, C, D, E>       & destRMDA   =  a_dest[point];
                const RectMDArray<T, C, D, E> & sourceRMDA = (*this)[pointSrc];
                if (a_copyGhost) {
                  sourceRMDA.copyTo(destRMDA);
                } else {
                  sourceRMDA.copyTo(destRMDA,m_boxLayout[pointSrc],no_shift);
                  //string foo = MDWrite(destRMDA);
                  //cout << "source = " << pointSrc << ", dest = " << point << endl;
                }
              }
        }
      // cout << endl;
    }
  // Iterate over src. Copy to unique dest if it exists. 
  else
    {
      int refratio = a_dest.m_boxLayout.getBoxsize()/m_boxLayout.getBoxsize();
      const RectMDArray<bool>& sourceBitmap = m_boxLayout.getBitmap();
      const RectMDArray<bool>& destBitmap = a_dest.getBoxLayout().getBitmap();
      BLIterator iterator(m_boxLayout);
      Point no_shift(getZeros());
      // cout << "looping over source, refratio = "<< refratio << endl;
      for (iterator = iterator.begin(); iterator != iterator.end(); ++iterator)
        {
          Point point = *iterator;
          Point pointDst = point / refratio;
         
          if (destBitmap[pointDst])
            {
              //cout << "source = " << point << ", dest = " << pointDst << endl;
              RectMDArray<T, C, D, E>       & destRMDA   =  a_dest[pointDst];
              const RectMDArray<T, C, D, E> & sourceRMDA = (*this)[point];
              if (a_copyGhost) {
                sourceRMDA.copyTo(destRMDA);
              } else {
                sourceRMDA.copyTo(destRMDA,m_boxLayout[point],no_shift);
              }
            }
        }
      // cout << endl;
    }
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
Box 
LevelData<T, C, D, E>::
getGhostBox(const Box& a_validBox, const int a_idir, const int& a_side)
{
  // Box retval = a_validBox;
  Point hi, lo;
  Point growVec = getUnitv(a_idir);
  growVec *= m_ghostSize;
  // retval.grow(growVec);
  if(a_side == 1)
    {
      hi = a_validBox.getHighCorner() + growVec;
      lo = a_validBox.getLowCorner() + getUnitv(a_idir)*a_validBox.size(a_idir);
      // retval.getLowCorner() [a_idir] = a_validBox.getHighCorner()[a_idir]+1;
      // retval.getHighCorner()[a_idir] = a_validBox.getHighCorner()[a_idir]+m_ghostSize;
    }
  else
    {
      lo = a_validBox.getLowCorner() - growVec;
      hi = a_validBox.getHighCorner() - getUnitv(a_idir)*a_validBox.size(a_idir);
      // retval.getLowCorner() [a_idir] = a_validBox.getLowCorner()[a_idir]-m_ghostSize;
      // retval.getHighCorner()[a_idir] = a_validBox.getLowCorner()[a_idir]-1;
     }
  Box retval = Box(lo,hi);
  return retval;
}
//////
template<class T, unsigned int C, unsigned char D, unsigned char E> 
void 
LevelData<T, C, D, E>::
getPeriodicEdgeNeighborInfo(Point & a_neighborPt, 
                            Box   & a_neighborValid, 
                            Point & a_shiftdom, 
                            const Point & a_p,
                            const int & a_idir,
                            const int & a_iside, 
                            const int & a_jdir,
                            const int & a_jside) 
{
  const RectMDArray<bool>& bitmap = m_boxLayout.getBitmap();
  //if one direction shift is still in the domain, only shift in one direction
  Point pshifti =  getUnitv(a_idir);
  Point pshiftj =  getUnitv(a_jdir);
  pshifti *= a_iside;
  pshiftj *= a_jside;
  pshifti += a_p;
  pshiftj += a_p;
  if(bitmap.getBox().contains(pshifti))
    {
      //only shifting in the j direction
      int domLen = m_boxLayout.getDomain().size(a_jdir);
      int bitmaplen = bitmap.getBox().size(a_jdir);
      int ishift = -(bitmaplen-1)*a_jside;
      a_neighborPt = pshifti; //starting from a slightly different point than the flap
      a_neighborPt[a_jdir] += ishift;
      a_shiftdom = getUnitv(a_jdir);
      a_shiftdom *= -a_jside*domLen;
    }
  else if(bitmap.getBox().contains(pshiftj))
    {
      //only shifting in the i direction
      int domLen = m_boxLayout.getDomain().size(a_idir);
      int bitmaplen = bitmap.getBox().size(a_idir);
      int ishift = -(bitmaplen-1)*a_iside;
      a_neighborPt = pshiftj; //starting from a slightly different point than the flap
      a_neighborPt[a_idir] += ishift;
      a_shiftdom = getUnitv(a_idir);
      a_shiftdom *= -a_iside*domLen;
    }
  else
    {
      //truly an edge.  shifting both ways
      int domLeni = m_boxLayout.getDomain().size(a_idir);
      int domLenj = m_boxLayout.getDomain().size(a_jdir);
      int bitmapleni = bitmap.getBox().size(a_idir);
      int bitmaplenj = bitmap.getBox().size(a_jdir);
      int ishift = -(bitmapleni-1)*a_iside;
      int jshift = -(bitmaplenj-1)*a_jside;
      a_neighborPt = a_p;
      a_neighborPt[a_idir] += ishift;
      a_neighborPt[a_jdir] += jshift;
      Point shiftdomi = getUnitv(a_idir);
      Point shiftdomj = getUnitv(a_jdir);
      shiftdomi *= -a_iside*domLeni;
      shiftdomj *= -a_jside*domLenj;
      a_shiftdom = shiftdomi + shiftdomj;
    }
  //this can be false if we are doing AMR (not all points have grids
  if(bitmap[a_neighborPt])
    {
      a_neighborValid = m_boxLayout[a_neighborPt];
    }
}
//////
template<class T, unsigned int C, unsigned char D, unsigned char E> 
void 
LevelData<T, C, D, E>::
getPeriodicCornerNeighborInfo(Point & a_neighborPt, 
                              Box   & a_neighborValid, 
                              Point & a_shiftdom, 
                              const Point & a_p,
                              const int & a_idir,
                              const int & a_iside, 
                              const int & a_jdir,
                              const int & a_jside, 
                              const int & a_kdir,
                              const int & a_kside)
{
  const RectMDArray<bool>& bitmap = m_boxLayout.getBitmap();
  //if one direction shift is still in the domain, only shift in one direction
  Point pshifti =  getUnitv(a_idir);
  Point pshiftj =  getUnitv(a_jdir);
  Point pshiftk =  getUnitv(a_kdir);
  pshifti *= a_iside;
  pshiftj *= a_jside;
  pshiftk *= a_kside;


  Point ppi = a_p +pshifti; 
  Point ppj = a_p +pshiftj; 
  Point ppk = a_p +pshiftk; 
  //this could probably be done more elegantly but I am losing patience
  if((bitmap.getBox().contains(ppi)) && (bitmap.getBox().contains(ppk)))
    {
      //only shifting in the j direction
      int domLen = m_boxLayout.getDomain().size(a_jdir);
      int bitmaplen = bitmap.getBox().size(a_jdir);
      int jshift = -(bitmaplen-1)*a_jside;
      a_neighborPt = a_p + pshifti + pshiftk; //starting from a slightly different point than the flap
      a_neighborPt[a_jdir] += jshift;
      a_shiftdom = getUnitv(a_jdir);
      a_shiftdom *= -a_jside*domLen;
    }
  else if(bitmap.getBox().contains(ppj) && bitmap.getBox().contains(ppk))
    {
      //only shifting in the i direction
      int domLen = m_boxLayout.getDomain().size(a_idir);
      int bitmaplen = bitmap.getBox().size(a_idir);
      int ishift = -(bitmaplen-1)*a_iside;
      a_neighborPt = a_p + pshiftj + pshiftk; //starting from a slightly different point than the flap
      a_neighborPt[a_idir] += ishift;
      a_shiftdom = getUnitv(a_idir);
      a_shiftdom *= -a_iside*domLen;
    }
  else if(bitmap.getBox().contains(ppj) && bitmap.getBox().contains(ppi))
    {
      //only shifting in the i direction
      int domLen = m_boxLayout.getDomain().size(a_kdir);
      int bitmaplen = bitmap.getBox().size(a_kdir);
      int ishift = -(bitmaplen-1)*a_kside;
      a_neighborPt = a_p + pshifti + pshiftj; //starting from a slightly different point than the flap
      a_neighborPt[a_kdir] += ishift;
      a_shiftdom = getUnitv(a_kdir);
      a_shiftdom *= -a_iside*domLen;
    }
  else if (bitmap.getBox().contains(ppk))
    {
      //truly an edge.  shifting two  ways
      int domLeni = m_boxLayout.getDomain().size(a_idir);
      int domLenj = m_boxLayout.getDomain().size(a_jdir);
      int bitmapleni = bitmap.getBox().size(a_idir);
      int bitmaplenj = bitmap.getBox().size(a_jdir);
      int ishift = -(bitmapleni-1)*a_iside;
      int jshift = -(bitmaplenj-1)*a_jside;
      a_neighborPt = a_p + pshiftk;
      a_neighborPt[a_idir] += ishift;
      a_neighborPt[a_jdir] += jshift;
      Point shiftdomi = getUnitv(a_idir);
      Point shiftdomj = getUnitv(a_jdir);
      shiftdomi *= -a_iside*domLeni;
      shiftdomj *= -a_jside*domLenj;
      a_shiftdom = shiftdomi + shiftdomj;
    }
  else if(bitmap.getBox().contains(ppi))
    {
      //truly an edge.  shifting two  ways
      int domLenk = m_boxLayout.getDomain().size(a_kdir);
      int domLenj = m_boxLayout.getDomain().size(a_jdir);
      int bitmaplenk = bitmap.getBox().size(a_kdir);
      int bitmaplenj = bitmap.getBox().size(a_jdir);
      int kshift = -(bitmaplenk-1)*a_kside;
      int jshift = -(bitmaplenj-1)*a_jside;
      a_neighborPt = a_p + pshifti;
      a_neighborPt[a_kdir] += kshift;
      a_neighborPt[a_jdir] += jshift;
      Point shiftdomk = getUnitv(a_kdir);
      Point shiftdomj = getUnitv(a_jdir);
      shiftdomk *= -a_kside*domLenk;
      shiftdomj *= -a_jside*domLenj;
      a_shiftdom = shiftdomk + shiftdomj;
    }
  else if(bitmap.getBox().contains(ppj))
    {
      //truly an edge.  shifting two  ways
      int domLenk = m_boxLayout.getDomain().size(a_kdir);
      int domLeni = m_boxLayout.getDomain().size(a_idir);
      int bitmaplenk = bitmap.getBox().size(a_kdir);
      int bitmapleni = bitmap.getBox().size(a_idir);
      int kshift = -(bitmaplenk-1)*a_kside;
      int ishift = -(bitmapleni-1)*a_iside;
      a_neighborPt = a_p + pshiftj;
      a_neighborPt[a_kdir] += kshift;
      a_neighborPt[a_idir] += ishift;
      Point shiftdomk = getUnitv(a_kdir);
      Point shiftdomi = getUnitv(a_idir);
      shiftdomk *= -a_kside*domLenk;
      shiftdomi *= -a_iside*domLeni;
      a_shiftdom = shiftdomk + shiftdomi;
    }
  else
    {
      //truly a corner point
      int domLeni = m_boxLayout.getDomain().size(a_idir);
      int domLenj = m_boxLayout.getDomain().size(a_jdir);
      int domLenk = m_boxLayout.getDomain().size(a_kdir);
      int bitmapleni = bitmap.getBox().size(a_idir);
      int bitmaplenj = bitmap.getBox().size(a_jdir);
      int bitmaplenk = bitmap.getBox().size(a_kdir);
      int ishift = -(bitmapleni-1)*a_iside;
      int jshift = -(bitmaplenj-1)*a_jside;
      int kshift = -(bitmaplenk-1)*a_kside;
      a_neighborPt = a_p;
      a_neighborPt[a_idir] += ishift;
      a_neighborPt[a_jdir] += jshift;
      a_neighborPt[a_kdir] += kshift;
      Point shiftdomi = getUnitv(a_idir);
      Point shiftdomj = getUnitv(a_jdir);
      Point shiftdomk = getUnitv(a_kdir);
      shiftdomi *= -a_iside*domLeni;
      shiftdomj *= -a_jside*domLenj;
      shiftdomk *= -a_kside*domLenk;
      a_shiftdom = shiftdomi + shiftdomj + shiftdomk;
    }
  //this can be false if we are doing AMR (not all points have grids)
  if(bitmap[a_neighborPt])
    {
      a_neighborValid = m_boxLayout[a_neighborPt];
    }
}
//////
template<class T, unsigned int C, unsigned char D, unsigned char E> 
void 
LevelData<T, C, D, E>::
getPeriodicFlapNeighborInfo(Point & a_neighborPt, 
                            Box   & a_neighborValid, 
                            Point & a_shiftdom, 
                            const Point & a_p,
                            const int & a_idir,
                            const int & a_iside) 
{
  const RectMDArray<bool>& bitmap = m_boxLayout.getBitmap();
  int bitmaplen = bitmap.getBox().size(a_idir);
  int ishift = -(bitmaplen-1)*a_iside;
  a_neighborPt = a_p;
  a_neighborPt[a_idir] += ishift;

  //this can be false if we are doing AMR (not all points have grids
  if(bitmap[a_neighborPt])
    {
      a_neighborValid = m_boxLayout[a_neighborPt];
      int domLen = m_boxLayout.getDomain().size(a_idir);
      a_shiftdom = getUnitv(a_idir);
      a_shiftdom *= -a_iside*domLen;
    }
}

//////
template<class T, unsigned int C, unsigned char D, unsigned char E> 
void 
LevelData<T, C, D, E>::
exchangeSingleBox(const Point& a_p)
{
  int kDest = m_boxLayout.getPatchIndex(a_p);
  RectMDArray<double, C, D, E>& RMDADest = m_data[kDest];
  Box bxKernel(getOnes()*(-1),getOnes());
  Point noshift = getZeros();
  for (Point pt=bxKernel.getLowCorner();bxKernel.notDone(pt);bxKernel.increment(pt))
    {
      if (m_boxLayout.getBitBox().contains(pt+a_p))
        {
          if (m_boxLayout.getBitmap()[pt+a_p])
            {
              int kSrc = m_boxLayout.getPatchIndex(pt + a_p);
              RectMDArray<double, C, D, E>& RMDASrc = m_data[kSrc];
              // copy contents of neighbor into the ghost region.
              if (kSrc != kDest)
                {
                  RectMDArray<double, C, D, E>&  RMDASrc = m_data[kSrc];
                  RMDASrc.copyTo(RMDADest,m_boxLayout[pt+a_p],noshift);
                }
            }
        }
      else
        {
          Point shiftedPt = m_boxLayout.getBitmap().getBox().mod(pt + a_p);
          
          if (m_boxLayout.getBitBox().contains(shiftedPt))
            {
              int n1D = m_boxLayout.getBoxsize();
              int kSrc = m_boxLayout.getPatchIndex(shiftedPt);
              Point patchShift = (pt + a_p - shiftedPt)*(-n1D);
              RectMDArray<double, C, D, E>& RMDASrc = m_data[kSrc];
              RMDASrc.copyTo(RMDADest,m_boxLayout[shiftedPt],patchShift);
            }
        }
    }
};
////
template<class T, unsigned int C, unsigned char D, unsigned char E> 
void 
LevelData<T, C, D, E>::
exchange()
{
  CH_TIMERS("LevelData::exchange");
  RectMDArray<bool> bitmap = m_boxLayout.getBitmap();
  Box bitMapBox = bitmap.getBox();
  for (Point p = bitMapBox.getLowCorner(); bitMapBox.notDone(p); bitMapBox.increment(p))
  {
    if (bitmap[p])
    {
      exchangeSingleBox(p);
    }
  }
};
////
template<class T, unsigned int C, unsigned char D, unsigned char E> 
void 
LevelData<T, C, D, E>::
generateBoxes(const Box& a_box, const Point& a_dir, Box& a_ghostCells, Box& a_neighborBox)
{
  Point actualLc = a_box.getLowCorner();
  Point actualHc = a_box.getHighCorner();

  // we create the box to fill in
  int ghostLcCoord[DIM];
  int ghostHcCoord[DIM];
  for (int i = 0; i < DIM; i++)
    {
      if (a_dir[i] < 0)
	{
	  ghostLcCoord[i] = actualLc[i] - (m_ghostSize);
	  ghostHcCoord[i] = actualHc[i] - (BLOCKSIZE / m_coarseningFactor);
	}
      else if (a_dir[i] > 0)
	{
	  ghostLcCoord[i] = actualLc[i] + (BLOCKSIZE / m_coarseningFactor);
	  ghostHcCoord[i] = actualHc[i] + (m_ghostSize);
	}
      else
	{
	  ghostLcCoord[i] = actualLc[i];
	  ghostHcCoord[i] = actualHc[i];
	}
    }
  Point ghostLc(ghostLcCoord);
  Point ghostHc(ghostHcCoord);
  a_ghostCells = Box(ghostLc, ghostHc);
  
  // we create the box to copy from
  Box neighborBox;
  if (m_boxLayout.getDomain().coarsen(m_coarseningFactor).contains(ghostLc))
    {
      a_neighborBox = a_ghostCells;
    }
  else
    {
      int copyLcCoord[DIM];
      int copyHcCoord[DIM];
      Point domainLc = m_boxLayout.getDomain().getLowCorner();
      Point domainHc = m_boxLayout.getDomain().getHighCorner();
      for (int i = 0; i < DIM; i++)
	{
	  T one = T(1);
	  T lcLHS = (T) ghostLc[i];
	  T lcRHS = (T) ((domainHc[i] - domainLc[i])/m_coarseningFactor) + one;
	  T hcLHS = (T) ghostHc[i];
	  T hcRHS = (T) ((domainHc[i] - domainLc[i])/m_coarseningFactor) + one;
	  copyLcCoord[i] = mod(lcLHS, lcRHS);
	  copyHcCoord[i] = mod(hcLHS, hcRHS);
	  
	}
      a_neighborBox = Box(Point(copyLcCoord), Point(copyHcCoord));
    }
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
Point
LevelData<T, C, D, E>::
getNeighbor(const Point& a_p, const Point& a_dir)
{
  CH_TIMERS("LevelData::getNeighbor");
  int patch[DIM];
  RectMDArray<bool> bitmap = m_boxLayout.getBitmap();
  Box bitBox = bitmap.getBox();
  Point lc = bitBox.getLowCorner();
  Point hc = bitBox.getHighCorner();
  
  for (int i = 0; i < DIM; i++)
    {
      T one = T(1);
      T leftSide = (T) a_p[i] + a_dir[i];
      T rightSide = (T) hc[i] - lc[i] + one;
      assert(rightSide != 0);
      patch[i] = mod(leftSide, rightSide);
    }
  
  return Point(patch);
}

template<class T, unsigned int C, typename Func>
void forall(LevelData<T,C>& a_lhs, const LevelData<T,C>& a_rhs, const Func& F)
{
  const BoxLayout& layout = a_lhs.getBoxLayout();
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      F(a_lhs[*blit], a_rhs[*blit]);
    }
}
template<class T, unsigned int C, typename Func>
void forall(LevelData<T,C>& a_lhs, const LevelData<T,C>& a_rhs0, const LevelData<T,C>& a_rhs1, const Func& F)
{
  const BoxLayout& layout = a_lhs.getBoxLayout();
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      F(a_lhs[*blit], a_rhs0[*blit],a_rhs1[*blit]);
    }
}

template<class T, unsigned int C, typename Func>
void forall(LevelData<T,C>& a_lhs, const LevelData<T,C>& a_rhs0, const LevelData<T,C>& a_rhs1,
       const LevelData<T,C>& a_rhs2, const Func& F)
{
  const BoxLayout& layout = a_lhs.getBoxLayout();
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      F(a_lhs[*blit], a_rhs0[*blit],a_rhs1[*blit],a_rhs2[*blit]);
    }
}
