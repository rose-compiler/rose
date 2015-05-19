#ifndef _BOXLAYOUT_H_
#define _BOXLAYOUT_H_

#include "SPACE.H"
#include "RectMDArray.H"
#include "Box.H"
#include "Point.H"
#include <vector>
#include <map>
#include <memory>

using namespace std;
class BLIterator;
class BoxLayout {

public:
  BoxLayout();
  BoxLayout(int a_M, const vector<Point>& a_points);

  //generate (2^a_M)^DIM boxes of equal size
  BoxLayout(int a_M);
  ~BoxLayout();
 
  const RectMDArray<bool>& getBitmap() const;

  // fetch the low corner from the lowest corner box and high corner from the highest corner box
  const Box& getDomain() const;
  /// returns the patch corresponding to this point in the bitmap. *bi, where bi is a 
  /// BoxIterator, is an appropriate argument.
  inline Box operator[](const Point& a_pt) const
  {
    Point lc   = a_pt * BLOCKSIZE;
    Point incr = getOnes()*(BLOCKSIZE-1);
    Point hc   = lc + incr;
    return Box(lc,hc);
  };
  int getPatchIndex(const Point& a_point) const;
  int getPatchIndexFromBitpoint(const Point& a_point) const;
  Point getCornerFromBitpoint(const Point& a_point);
  bool neighborExists(const Point& a_p, const int a_dir, const int a_face);
  vector<Point>& getPatchLocs() const;
  int size(){return m_getPatches->size();};
  Point getNeighbor(const Point& a_p, const Point& a_dir);
  //there has to be a better way of doing this
  Point getCorner(const int index);
  Box shift(const Box& a_bx,const Point& a_pt);
  int getBlockPower() const
  {
    return m_blockPower;
  }
private:
//  BoxLayout* m_boxLayoutPtr;
  Box m_domain;
  Box m_bitbox;
  int m_blockPower;
  shared_ptr<RectMDArray<bool>> m_bitmap;
  shared_ptr<vector<Point>> m_patchlocs;
  shared_ptr<map<Point, int >> m_getPatches;
};

#endif // _BOXLAYOUT_H_
