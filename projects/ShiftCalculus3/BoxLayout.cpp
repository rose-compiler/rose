#include "BoxLayout.H"
#include "PowerItoI.H"
//BLOCKSIZE is a variable set in the makefile
BoxLayout::BoxLayout()
{
}

BoxLayout::BoxLayout(int a_M, const vector<Point>& a_points)
{
  m_blockPower = a_M;
  int N = Power(2,a_M);
  m_bitbox = Box(getZeros(), getOnes()*(N-1));
  m_domain = m_bitbox.refine(BLOCKSIZE);
  m_bitmap = shared_ptr<RectMDArray<bool> > (new RectMDArray<bool>(m_bitbox));
  m_patchlocs = shared_ptr<vector<Point> > (new (vector<Point> ));
  m_getPatches = shared_ptr<map<Point,int > > (new (map<Point,int >) );
  m_bitmap->setVal(false);
  *m_patchlocs = a_points;
  int counter = 0;
  for(vector<Point>::iterator it = m_patchlocs->begin(); it != m_patchlocs->end(); ++it)
  { 
    (*m_bitmap)[*it] = true;
    int index = it - m_patchlocs->begin();
    (*m_getPatches)[*it] = index;
  }
}
BoxLayout::BoxLayout(int a_M)
{
  m_blockPower = a_M;
  int N = Power(2,a_M);
  m_bitbox = Box(getZeros(), getOnes()*(N-1));
  m_domain = m_bitbox.refine(BLOCKSIZE);
  m_bitmap = shared_ptr<RectMDArray<bool> > (new RectMDArray<bool>(m_bitbox));
  m_patchlocs = shared_ptr<vector<Point> > (new (vector<Point> ));
  m_getPatches = shared_ptr<map<Point,int > > (new (map<Point,int >) );
  m_bitmap->setVal(true);
  for (Point pt = m_bitbox.getLowCorner();m_bitbox.notDone(pt);m_bitbox.increment(pt))
    {
      int k = m_bitbox.getIndex(pt);
      (*m_getPatches)[pt]=k;
      m_patchlocs->push_back(pt);
    }
};
BoxLayout::~BoxLayout()
{
}

const RectMDArray<bool>& BoxLayout::getBitmap() const
{
  return *m_bitmap;
}

int BoxLayout::getPatchIndex(const Point& a_point) const
{
  return (*m_getPatches)[a_point];
}

vector<Point>& BoxLayout::getPatchLocs() const
{
  return *m_patchlocs;
}

const Box& BoxLayout::getDomain() const
{
  return m_domain;
}
Point BoxLayout::getNeighbor(const Point& a_pt,const Point& a_dir)
{
  int tuple[DIM];
  Point ptplus = a_pt + a_dir;
  for (int dir = 0; dir < DIM;dir++)
    {
      tuple[dir] = mymod(ptplus[dir]-m_bitbox.getLowCorner()[dir],
                         m_bitbox.getHighCorner()[dir] 
                         - m_bitbox.getLowCorner()[dir] + 1) + 
        m_bitbox.getLowCorner()[dir];
    }
  Point ret(tuple);
  return ret;
};
Box BoxLayout::shift(const Box& a_bx,const Point& a_pt)
{
  
  int tuplelow[DIM];
  int tuplehigh[DIM];
  Point ptlow = a_bx.getLowCorner() + a_pt;
  Point pthigh = a_bx.getHighCorner() + a_pt;
  for (int dir = 0; dir < DIM;dir++)
    {
      
      tuplelow[dir] = mymod(ptlow[dir]-m_domain.getLowCorner()[dir],
                            m_domain.getHighCorner()[dir] 
                            - m_domain.getLowCorner()[dir] + 1) + 
        m_domain.getLowCorner()[dir];
      tuplehigh[dir] = mymod(pthigh[dir]-m_domain.getLowCorner()[dir],
                            m_domain.getHighCorner()[dir] 
                            - m_domain.getLowCorner()[dir] + 1) + 
        m_domain.getLowCorner()[dir];
    }
  Box ret(ptlow,pthigh);
  return ret;
};

int BoxLayout::getPatchIndexFromBitpoint(const Point& a_point) const
{
  Point corner=a_point*BLOCKSIZE;
  return (*m_getPatches)[corner];
}
Point BoxLayout::getCornerFromBitpoint(const Point& a_point)
{
  return a_point*BLOCKSIZE;
}
bool BoxLayout::neighborExists(const Point& a_p, const int a_dir, const int a_face)
{
  Point offset=getUnitv(a_dir);
  if(a_face==0)
    offset = offset*-1;
  return (*m_bitmap)[a_p+offset];
}
