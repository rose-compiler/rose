#ifndef _LEVELDATA_H_
#define _LEVELDATA_H_

#include "BoxLayout.H"
#include "BLIterator.H"
#include "CH_Timer.H"
#include <map>

/// Structure which holds all of the data in a computational domain for a given level of refinement. 
/**
  Contains a BoxLayout which defines the valid patches of the domain. Each patch is represented by a RectMDArray which holds the actual data. 
*/
#pragma DSL keyword
template  <class T=double, unsigned int C=1, unsigned char D=1, unsigned char E=1>
class LevelData 
{

public:
  /// Default Constructor
  LevelData();

  /// Construct a LevelData from a BoxLayout
  LevelData(const BoxLayout a_boxLayout, int a_ghostSize=0, int a_coarseningFactor=1);

  /// Destructor
  ~LevelData();
  
  /// Returns a Box which contains the ghost cells of a_validBox based on m_ghostSize on a particular side and direction. 
  /**
    \param a_validBox Box whose ghost cells are desired
    \param a_idir Integer representing the direction of the desired ghost cells with respect to a_validBox. e.g. if a_idir=0, getGhostBox() will return the ghost box which is adjacent to a_validBox in the 0 direction.
    \param a_side Integer which defines if the returned GhostBox is adjacent to a_validBox in the positive or negative a_idir direction. a_side = 1 defines positive, all other values signify negative.
    \bug Does not return "corner" regions of the GhostBox. Unsure if this is intentional.
  */
  Box getGhostBox(const Box& a_validBox, const int a_idir, const int& a_side);

  void define(const BoxLayout a_boxLayoutm, int a_ghostSize=0, int a_coarseningFactor=1);

  /// Index into a const LevelData using an integer. Returns the associated RectMDArray.
  const RectMDArray<T, C, D, E>& operator[](int a_index) const;

  /// Index into a const LevelData using a Point in bit-space. Used by the iterator. 
  const RectMDArray<T, C, D, E>& operator[](Point& a_pt) const;

  RectMDArray<T, C, D, E>& operator[](int a_index);

  RectMDArray<T, C, D, E>& operator[](Point& a_pt);

  BoxLayout getBoxLayout() const {return m_boxLayout;};

  /// Fills in the ghost cells of the RectMDArrays. 
  /** 
    Periodicity is assumed. Any ghost cells which are not within the domain are unchanged. 
  */
  void exchange();

  /** Copies data from from this object to a_dest. */
  void copyTo(LevelData& a_dest, bool a_copyGhost=false) const;

  Point getNeighbor(const Point& a_p, const Point& a_dir);

  /// Returns the number of ghost cells in the ghost box. 
  int getGhostSize() const;

  ///\todo Not yet implemented. Unsure what is intended given the signature. 
  void operator+=(const LevelData<T>& a_src);

  /// Sets all values of each RectMDArray in *this to a_val. 
  void setVal(const T & a_val);

  void print() {
    cout << "GhostSize: " << m_ghostSize << endl;
    cout << "CoarseningFactor: " << m_coarseningFactor << endl << endl;
    cout << "BoxLayout: " << endl;
    m_boxLayout.print();
  }

private:
  BoxLayout m_boxLayout;
  vector<RectMDArray<T, C, D, E> > m_data;
  int m_ghostSize; ///< Number of cells in the ghost region. 
  int m_coarseningFactor;

  /** The assignment operator and the copy constructor is declared
      private to prevent deep copies and passing LevelData as a value. */
  LevelData(const LevelData<T>& a_levelData);
  LevelData& operator=(const LevelData<T>& a_levelData);
  /** Computes a_lhs mod a_rhs. Also works for negative numbers. */
  T mod(T& a_lhs, T& a_rhs) const;

  void generateBoxes(const Box& a_box, const Point& a_point, Box& a_ghostCells, Box& a_neighborBox);

  //the joys of box calculus in a a periodic domain
  void
  getPeriodicFlapNeighborInfo(Point & a_neighborPt, 
                              Box   & a_neighborValid, 
                              Point & a_shiftdom, 
                              const Point & a_p,
                              const int & a_idir,
                              const int & a_iside) ;

  void 
  getPeriodicEdgeNeighborInfo(Point & a_neighborPt, 
                              Box   & a_neighborValid, 
                              Point & a_shiftdom, 
                              const Point & a_p,
                              const int & a_idir,
                              const int & a_iside,
                              const int & a_jdir,
                              const int & a_jside) ;


  void 
  getPeriodicCornerNeighborInfo(Point & a_neighborPt, 
                                Box   & a_neighborValid, 
                                Point & a_shiftdom, 
                                const Point & a_p,
                                const int & a_idir,
                                const int & a_iside,
                                const int & a_jdir,
                                const int & a_jside,
                                const int & a_kdir,
                                const int & a_kside);

  void exchangeSingleBox(const Point& a_p);
};
#include "LevelDataImplem.H"

#endif // _LEVELDATA_H_
