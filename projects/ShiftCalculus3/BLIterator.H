#ifndef _BLIterator_H_
#define _BLIterator_H_
#include  "BoxLayout.H"

/** Iterator for the BoxLayout class.

    Example usage:
    BLIterator iterator; 
    for (iterator = iterator.begin(); iterator != iterator.end(); ++iterator) */

class BLIterator
{
public:
  BLIterator();
  BLIterator(const BoxLayout& a_boxLayout);
  BLIterator(const BoxLayout& a_boxLayout, const Point& a_point);
  ~BLIterator();

  BLIterator& operator++();
  Point& operator*();
  int operator()(){return m_boxLayout.getPatchIndex(*(*this));};
  bool operator!=(BLIterator a_other);
    /** Returns a BLIterator pointing to the first Point in m_boxLayout. */
  BLIterator begin();

  /** Returns a BLIterator pointing to a Point outside m_boxLayout. This
      method should only be used to check if we're done in a for-loop. */
  BLIterator end();

private:
  BoxLayout m_boxLayout;
  Point m_data;
  std::vector<Point>::iterator m_vectorIterator;
};
#endif // _BLIterator_H_
