#include <cassert>
#include "SPACE.H"
#include <math.h>

template<class T, unsigned int C, unsigned char D, unsigned char E> 
LevelData<T,C,D,E>::LevelData()
{
}


template<class T, unsigned int C, unsigned char D, unsigned char E> 
LevelData<T, C, D, E>::
~LevelData()
{
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
void
LevelData<T, C, D, E>::
define(const BoxLayout a_boxLayout, int a_ghostSize, int a_coarseningFactor)
{
  m_boxLayout = a_boxLayout;
  m_ghostSize = a_ghostSize;
  m_coarseningFactor = a_coarseningFactor;

  Point lc;
  Point hc;
  vector<Point> points = m_boxLayout.getPatchLocs();
  m_data.resize(points.size());
  for (vector<Point>::iterator it = points.begin(); it != points.end(); ++it)
    {
      Point p = *it;
      lc = p * BLOCKSIZE / m_coarseningFactor;
      hc = (p * BLOCKSIZE + getOnes() * (BLOCKSIZE-1)) / m_coarseningFactor;
      m_data[m_boxLayout.getPatchIndex(*it)] = RectMDArray<T,C,D,E>(Box(lc,hc).grow(m_ghostSize));
    }
};
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
  RectMDArray<bool> sourceBitmap = m_boxLayout.getBitmap();
  RectMDArray<bool> destBitmap = a_dest.getBoxLayout().getBitmap();
  BLIterator iterator(m_boxLayout);
  for (iterator = iterator.begin(); iterator != iterator.end(); ++iterator)
    {
      Point point = *iterator;
      if (destBitmap[point])
	{
	  RectMDArray<T, C, D, E>       & destRMDA   =  a_dest[point];
	  const RectMDArray<T, C, D, E> & sourceRMDA = (*this)[point];
          sourceRMDA.copyTo(destRMDA);
	}
    }
}

template<class T, unsigned int C, unsigned char D, unsigned char E> 
Box 
LevelData<T, C, D, E>::
getGhostBox(const Box& a_validBox, const int a_idir, const int& a_side)
{
  Box retval = a_validBox;
  Point growVec = getBasisV(a_idir);
  growVec *= m_ghostSize;
  retval.grow(growVec);
  if(a_side == 1)
    {
      retval.getLowCorner() [a_idir] = a_validBox.getHighCorner()[a_idir]+1;
      retval.getHighCorner()[a_idir] = a_validBox.getHighCorner()[a_idir]+m_ghostSize;
    }
  else
    {
      retval.getLowCorner() [a_idir] = a_validBox.getLowCorner()[a_idir]-m_ghostSize;
      retval.getHighCorner()[a_idir] = a_validBox.getLowCorner()[a_idir]-1;
     }
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
  //this geta  a fair bit weirder in higher dimensions
  assert(DIM <= 3);
  Point p = a_p;
  const RectMDArray<bool>& bitmap = m_boxLayout.getBitmap();
  Box bitMapBox = bitmap.getBox();
  //box without ghost cells

  Point shiftzero = getZeros();
  Box validBox = m_boxLayout[p];
  for(int idir = 0; idir < DIM; idir++)
    {
      //first exchange flaps
      for(int iside = -1; iside <= 1; iside+=2)
        {
          Point ishift = getBasisV(idir);
          ishift *= iside;


          //first fill in the interior ghost cells
          RectMDArray<T, C, D, E> & thisRMDA  =  (*this)[p];
          
          Point pshift = ishift;
          Point neighborPt = pshift + p;
          if(bitmap.getBox().contains(neighborPt) && bitmap[neighborPt])
            {
              RectMDArray<T, C, D, E> & neigRMDA  =  (*this)[neighborPt];
              Box neighborValid = m_boxLayout[neighborPt];

              //copy my valid into his ghost 
              thisRMDA.copyTo(neigRMDA,      validBox, shiftzero);
              //copy his valid into my ghost 
              neigRMDA.copyTo(thisRMDA, neighborValid, shiftzero);
            }
          else if (!bitmap.getBox().contains(neighborPt))
            {
              //now for the periodic bcs. 
              //shift the neighborPt to the other side of the domain
              Box neighborValid;
              Point shiftdom;
              Point neighPeriodic = neighborPt;
              getPeriodicFlapNeighborInfo(neighPeriodic, neighborValid, shiftdom, p, idir, iside); 
              if(bitmap[neighPeriodic])
                {
                  (*this)[neighPeriodic].copyTo(thisRMDA, neighborValid, shiftdom);
                }
            }

          //now exchange edges
          for(int jdir = 0; jdir < DIM; jdir++)
            {
              if(idir != jdir)
                {
                  for(int   jside= -1; jside <= 1; jside+=2)
                    {
                      Point jshift = getBasisV(jdir);
                      jshift *= jside;

                      pshift     = ishift + jshift;
                      neighborPt = pshift + p;
                      if(bitmap.getBox().contains(neighborPt) && bitmap[neighborPt])
                        {
                          RectMDArray<T, C, D, E> & neigRMDA  =  (*this)[neighborPt];
                          Box neighborValid = m_boxLayout[neighborPt];

                          //copy my valid into his ghost 
                          thisRMDA.copyTo(neigRMDA,      validBox, shiftzero);
                          //copy his valid into my ghost 
                          neigRMDA.copyTo(thisRMDA, neighborValid, shiftzero);
                        }
                      else if (!bitmap.getBox().contains(neighborPt))
                        {
                          //now for the periodic bcs. 
                          //shift the neighborPt to the other side of the domain
                          //this can be in a corner so it might involve two shifts
                          Box neighborValid;
                          Point shiftdom;
                          Point neighPeriodic = neighborPt;
                          getPeriodicEdgeNeighborInfo(neighPeriodic,  
                                                      neighborValid, 
                                                      shiftdom, p, 
                                                      idir, iside, 
                                                      jdir, jside); 
                          if(bitmap[neighPeriodic])
                            {
                              (*this)[neighPeriodic].copyTo(thisRMDA, neighborValid, shiftdom);
                            }
                        }

                      //now exchange corners (only happens in 3d)
                      for(int kdir = 0; kdir < DIM; kdir++)
                        {
                          if((kdir != jdir) && (kdir != idir))
                            {
                              assert(DIM==3);
                              for(int kside= -1; kside <= 1; kside+=2)
                                {
                                  Point kshift = getBasisV(kdir);
                                  kshift *= kside;
                                  pshift     = ishift + jshift + kshift;
                                  neighborPt = pshift + p;
                                  if(bitmap.getBox().contains(neighborPt) && bitmap[neighborPt])
                                    {
                                      RectMDArray<T, C, D, E> & neigRMDA  =  (*this)[neighborPt];
                                      Box neighborValid = m_boxLayout[neighborPt];

                                      //copy my valid into his ghost 
                                      thisRMDA.copyTo(neigRMDA,      validBox, shiftzero);
                                      //copy his valid into my ghost 
                                      neigRMDA.copyTo(thisRMDA, neighborValid, shiftzero);
                                    } //if inside domain
                                  else if (!bitmap.getBox().contains(neighborPt))
                                    {
                                      /**/
                                      //now for the periodic bcs. 
                                      //shift the neighborPt to the other side of the domain
                                      //this can be in a corner so it might involve two shifts
                                      Box neighborValid;
                                      Point shiftdom;
                                      Point neighPeriodic = neighborPt;
                                      getPeriodicCornerNeighborInfo(neighPeriodic, 
                                                                    neighborValid, 
                                                                    shiftdom, p, 
                                                                    idir, iside, 
                                                                    jdir, jside,
                                                                    kdir, kside);
                                                                    
                                      if(bitmap[neighPeriodic])
                                        {
                                          (*this)[neighPeriodic].copyTo(thisRMDA, neighborValid, shiftdom);
                                        }
                                      /**/
                                    } // if filling periodic bit
                                 } //loop over sides
                            } //if on corner
                        } //loop over kdir
                    } //loop over jside
                } //if idir != jdir
            } //loop over jdir
        } //loop over iside
    }//loop over idir
} // wow, that is a lot of curly braces
////
template<class T, unsigned int C, unsigned char D, unsigned char E> 
void 
LevelData<T, C, D, E>::
exchange()
{
  RectMDArray<bool> bitmap = m_boxLayout.getBitmap();
  Box bitMapBox = bitmap.getBox();
  for (Point p = bitMapBox.getLowCorner(); bitMapBox.notDone(p); bitMapBox.increment(p))
  {
    if (bitmap[p])
    {
      exchangeSingleBox(p);
    }
  }
}
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
  if (m_boxLayout.getDomain().coarsen(m_coarseningFactor).hasPoint(ghostLc))
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
  CH_TIME("LevelData::getNeighbor");
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

