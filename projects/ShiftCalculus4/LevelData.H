#ifndef _LEVELDATA_H_
#define _LEVELDATA_H_

#include "BoxLayout.H"
#include "BLIterator.H"
#include <map>
template  <class T=double, unsigned int C=1, unsigned char D=1, unsigned char E=1>
class LevelData 
{

public:
  LevelData();
  LevelData(const BoxLayout a_boxLayout, int a_ghostSize=0, int a_coarseningFactor=1);
  ~LevelData();
  
  Box getGhostBox(const Box& a_validBox, const int a_idir, const int& a_side);

  void define(const BoxLayout a_boxLayoutm, int a_ghostSize=0, int a_coarseningFactor=1);
  const RectMDArray<T, C, D, E>& operator[](int a_index) const;
  const RectMDArray<T, C, D, E>& operator[](Point& a_pt) const;
  RectMDArray<T, C, D, E>& operator[](int a_index);
  RectMDArray<T, C, D, E>& operator[](Point& a_pt);
  BoxLayout getBoxLayout() const {return m_boxLayout;};

  /** Fills in the ghost cells in the RMDAs. */
  void exchange();

  /** Copies data from from this object to a_dest. */
  void copyTo(LevelData& a_dest, bool a_copyGhost=false) const;

  Point getNeighbor(const Point& a_p, const Point& a_dir);
  int getGhostSize() const;
  void operator+=(const LevelData<T>& a_src);

  void setVal(const T & a_val);

private:
  BoxLayout m_boxLayout;
  vector<RectMDArray<T, C, D, E> > m_data;
  int m_ghostSize;
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
