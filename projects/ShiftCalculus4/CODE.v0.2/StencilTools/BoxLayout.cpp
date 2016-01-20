#include "BoxLayout.H"
#include "PowerItoI.H"
#include <assert.h>
//MAXBLOCKSIZE_EXP, MINBLOCKSIZE_EXP are variables set in the makefile

BoxLayout::BoxLayout()
{};
BoxLayout::BoxLayout(int a_M, const vector<Point>& a_points)
{
  m_blockPower = a_M - MAXBLOCKSIZE_EXP;
  if (m_blockPower < 0) {m_blockPower = 0;};
  m_blocksize = a_M - m_blockPower;
  int N = Power(2,m_blockPower); //N = number of blocks in a direction
  m_bitbox = Box(getZeros(), getOnes()*(N-1)); 
  m_domain = m_bitbox.refine(Power(2,m_blocksize));
  m_bitmap = shared_ptr<RectMDArray<bool> > (new RectMDArray<bool>(m_bitbox));
  m_patchlocs = shared_ptr<vector<Point> > (new (vector<Point> ));
  m_getPatches = shared_ptr<map<Point,int > > (new (map<Point,int >) );
  m_bitmap->setVal(false); // Initialize m_bitmap as False everywhere. 
  *m_patchlocs = a_points;
  int counter = 0;
  // Iterate through all Points in a_points.
  // Set corresponding values in m_bitmap to True.
  // Store a related index in m_getPatches with key equal to the associated Point.
  for(auto it = m_patchlocs->begin(); it != m_patchlocs->end(); ++it)
  { 
    (*m_bitmap)[*it] = true; 
    int index = it - m_patchlocs->begin();
    (*m_getPatches)[*it] = index;
  }
};
BoxLayout::BoxLayout(int a_M)
{
  m_blockPower = a_M - MAXBLOCKSIZE_EXP;
  if (m_blockPower < 0) {m_blockPower = 0;};
  m_blocksize = a_M - m_blockPower;
  int N = Power(2,m_blockPower);
  m_bitbox = Box(getZeros(), getOnes()*(N-1));
  m_domain = m_bitbox.refine(Power(2,m_blocksize));
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
};
BoxLayout BoxLayout::coarsen(unsigned int a_numLevels) const
{
  //cout << "in coarsen" << endl;
  BoxLayout ret=*this;
  int coarseningFactor = Power(2,a_numLevels);
  ret.m_blocksize -= a_numLevels;
  ret.m_domain = m_bitbox.refine(Power(2,ret.m_blocksize));
  
  return ret;
};
bool BoxLayout::canCoarsen(unsigned int a_numLevels) const
{
  //cout << "in canCoarsen" << endl;
  int coarseningFactor = Power(2,a_numLevels);
  int fac = coarseningFactor*Power(2,MINBOXSIZE_EXP);
  return (Power(2,m_blocksize) /fac)*fac == Power(2,m_blocksize);
};
BoxLayout BoxLayout::deepCoarsen(unsigned int a_numLevels) const
{
  //cout << "in deepCoarsen" << endl;
  vector<Point > newPoints;
  RectMDArray<bool > tmp(*m_bitmap);
  tmp.setVal(false);
  int newTotExp = m_blockPower + m_blocksize - a_numLevels;
  int newBlockPower = newTotExp - MAXBLOCKSIZE_EXP;
  if (newBlockPower < 0) {newBlockPower = 0;};
  int newBlockSize = newTotExp - newBlockPower;
  int refRatio = Power(2,m_blockPower - newBlockPower);
  
  for (auto it = (*m_patchlocs).begin(); it != (*m_patchlocs).end(); ++it)
    {
      Point pt = (*it)/(refRatio); 
      if (!tmp[pt])
        {
          tmp[pt] = true;
          newPoints.push_back(pt);
          //cout << pt << endl;
        } 
    }
  BoxLayout ret(newTotExp,newPoints);
  assert(ret.m_blocksize == newTotExp - newBlockPower);
  assert(ret.m_domain == (ret.m_bitbox).refine(Power(2,ret.m_blocksize)));
  return(ret);
};  
const RectMDArray<bool>& BoxLayout::getBitmap() const
{
  return *m_bitmap;
};
int BoxLayout::getPatchIndex(const Point& a_point) const
{
  if ((*m_getPatches).count(a_point) == 0) {
    return -1;
  } else {
    return (*m_getPatches)[a_point];
  }

};
vector<Point>& BoxLayout::getPatchLocs() const
{
  return *m_patchlocs;
};
const Box& BoxLayout::getDomain() const
{
  return m_domain;
};
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
  //This loop does not effect the output at all... perhaps this function is incomplete?
  for (int dir = 0; dir < DIM;dir++)
  {
    tuplelow[dir] = mymod(ptlow[dir]-m_domain.getLowCorner()[dir],
      m_domain.getHighCorner()[dir] - m_domain.getLowCorner()[dir] + 1) + m_domain.getLowCorner()[dir];
    tuplehigh[dir] = mymod(pthigh[dir]-m_domain.getLowCorner()[dir],
      m_domain.getHighCorner()[dir] - m_domain.getLowCorner()[dir] + 1) + m_domain.getLowCorner()[dir];
  }
  Box ret(ptlow,pthigh);
  return ret;
};
// REDUNDANT
// int BoxLayout::getPatchIndexFromBitpoint(const Point& a_point) const
// {
//   Point corner=a_point*BLOCKSIZE;
//   return (*m_getPatches)[corner];
// }
Point BoxLayout::getCornerFromBitpoint(const Point& a_point)
{
  return a_point*Power(2,m_blocksize);
};
bool BoxLayout::neighborExists(const Point& a_p, const int a_dir, const int a_face)
{
  Point offset=getUnitv(a_dir);
  if(a_face==0)
    offset = offset*-1;
  return (*m_bitmap)[m_bitbox.mod(a_p+offset)];
};
bool BoxLayout::sameBLI(const BoxLayout& a_bl) const
{
  return (m_bitmap == a_bl.m_bitmap);
};
ostream& operator<<(ostream& os, const BoxLayout& a_bl)
{
  os << "Boxlayout:" << endl;
  os << "m_domain = " << a_bl.getDomain() << endl;
  os << "m_bitBox = " << a_bl.getBitBox() << endl;
  os << "m_blockPower = " << a_bl.m_blockPower << endl;
  os << "m_blocksize = " << a_bl.m_blocksize << endl;
  map<Point, int>& mymap = *(a_bl.m_getPatches);
  for (map<Point,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
    {
      Point pt = it-> first;
      int k = it->second;
      os << "Point in bitBox = " << pt << ", index = " << k << endl;
    }
  return os;
};
