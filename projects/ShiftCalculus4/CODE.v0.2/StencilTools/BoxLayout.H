#ifndef _BOXLAYOUT_H_
#define _BOXLAYOUT_H_

#include "SPACE.H"
#include "RectMDArray.H"
#include "Box.H"
#include "Point.H"
#include "PowerItoI.H"
#include <iostream>
#include <vector>
#include <map>
#include <memory>
using namespace std;
//yusing namespace std::tr1;
class BLIterator;

/// An organization of Boxes that represents the computational domain.
class BoxLayout {

public:
  /// Default Constructor
  BoxLayout();

  /// Construct a BoxLayout from an integer and a vector of Points.
  /**
    \param a_M Desired value of m_blockPower. Each direction in the domain will contain Pow(2,a_M) blocks. 
    \param a_points Vector of Point objects. Each Point is in bit space and represents the lowCorner of a patch in the domain.
  */
  BoxLayout(int a_M, const vector<Point>& a_points);

  /// Construct a BoxLayout with (2^a_M)^DIM boxes of equal size. The domain is all of *this.m_domain
  
BoxLayout(int a_M);

  /// Destructor
  ~BoxLayout();
 
  /// Return a Boolean, scalar valued RectMDArray which defines the computational domain.
  const RectMDArray<bool>& getBitmap() const;

  /// Return a Box object which encloses the entire computational domain. 
  const Box& getDomain() const;

  /// Return a Box object which encloses the entire computational domain. 
  const Box& getBitBox() const{return m_bitbox;};

  /// Returns the linear dimension of a single Box. 
  int getBoxsize() const{return Power(2,m_blocksize);};

  /// Returns a coarsened version of the present BoxLayout.
  BoxLayout coarsen(unsigned int a_numLevels=1) const;

  /// Determines whether the current BL can be coarsened. 
  bool canCoarsen(unsigned int a_numLevels=1) const;

  /// Returns a BL that covers the same region. 
  BoxLayout deepCoarsen(unsigned int a_numLevels=1) const;

  /// Determines whether the current BL can be coarsened. 
  bool sameBLI(const BoxLayout& a_bli) const;

  /// Returns a Box representing the patch corresponding to this point in the bitmap.
  /**
    *bi, where bi is a BoxIterator, is an appropriate argument. Returns the Box for which a_pt maps to the lowCorner.
    \param a_pt A Point in the bit space.
  */
  inline Box operator[](const Point& a_pt) const
  {
    Point lc   = a_pt * Power(2,m_blocksize);
    Point incr = getOnes()*(Power(2,m_blocksize)-1);
    Point hc   = lc + incr;
    return Box(lc,hc);
  };

  /// Return the linear index of the patch whose lowCorner is a_point. 
  /**
    The returned index is associated with the patch that has a_point as the lowCorner. If a_point is not associated with a patch in the domain, -1 is returned. 
    \param a_point A Point in the bitbox space (not in the physical domain)
  */
  int getPatchIndex(const Point& a_point) const;

  // REDUNDANT
  //****************************************************************************************************************
  // int getPatchIndexFromBitpoint(const Point& a_point) const;
  //****************************************************************************************************************

  /// Returns the low corner (in physical space) of the patch specified by a_point (in bit space). 
  /**
    \param a_point Point in bitspace representing the low corner of the desired patch. 
  */ 
  Point getCornerFromBitpoint(const Point& a_point);

  /// Check if a neighboring patch is in the domain. 
  /**
    Returns true if neighboring patch is in the domain. Returns false if neighboring patch is not in the domain (either because it has a FALSE bitmap value or because it is not within the bitbox)
    \param a_p Point in bit space
    \param a_dir Direction of neighbor relative to a_p
    \param a_face Integer representing orientation of neighbor. a_face = 0 represents -a_dir direction, otherwise direction is +a_dir
  */
  bool neighborExists(const Point& a_p, const int a_dir, const int a_face);

  /// Returns a vector of Points in bit space representing the patches that make up the domain.
  /**
    Each Point in the output is the lowCorner of the Box that represents the patch in the domain. 
  */
  vector<Point>& getPatchLocs() const;

  /// Returns the number of patches in the domain.
  int size(){return m_getPatches->size();};

  /// Outputs the Point associated with the neighboring patch to a_p in the direction a_dir.
  /*
    This function returns the point a_p + a_dir, assuming that m_bitbox is periodic. Despite the function name, the output patch will not, in general, be adjacent to the reference patch. There is also no guarantee that the output patch is a member of the domain (i.e. the output may have a bitmap value of FALSE)
    \param a_p Point representing the reference patch
    \param a_dir Point representing the direction of the desired neighbor.
  **/ 
  Point getNeighbor(const Point& a_p, const Point& a_dir);
  
  // UNUSED
  // //there has to be a better way of doing this
  // Point getCorner(const int index);

  ///\bug Determine the purpose of this function. It appears unfinished.
  Box shift(const Box& a_bx,const Point& a_pt);

  /// Return m_blockPower
  int getBlockPower() const
  {
    return m_blockPower;
  }

  /// Prints information about *this to the command line.
  void print() const {
    cout << "Domain: ";
    m_domain.print();
    cout << "Bitbox: ";
    m_bitbox.print();
    cout << "blockPower: " << m_blockPower << endl;
    cout << "Bitmap: " << endl;
    if (m_bitmap) {
      (*m_bitmap).print();
    }
  }
private:
//  BoxLayout* m_boxLayoutPtr;
  Box m_domain; ///< Box representing the physical domain. A refinement of m_bitbox by Power(2,m_blocksize)
  Box m_bitbox; ///< Box representing an array of blocks. Each Point in m_bitbox corresponds to a block. 
  int m_blockPower; ///< Determines the number N of blocks in any direction. N = 2^m_blockPower
  int m_blocksize; ///< Size of a single block = 2^m_blocksize. MINBLOCKSIZE_EXP<= m_blocksize <= MAXBLOCKSIZE_EXP.
  shared_ptr<RectMDArray<bool>> m_bitmap; ///< Boolean, scalar valued RectMDArray. Defines which Boxes are members of the domain.
  shared_ptr<vector<Point>> m_patchlocs; ///< Vector of Points each of which is associated with a block in the domain.
  shared_ptr<map<Point, int > > m_getPatches; ///< Maps Points in m_patchlocs to an index. 
friend ostream& operator<<(ostream& os, const BoxLayout& a_bl);
};

#endif // _BOXLAYOUT_H_
