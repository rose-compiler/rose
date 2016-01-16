#ifndef _BLIterator_H_
#define _BLIterator_H_
#include  "BoxLayout.H"

///Iterator for the BoxLayout class. Used to iterate through the Points in bit space associated with patches of the domain.
/**
  Used to iterate through the Points in bit space associated with patches of the domain.\n\n
  Example usage: \n
  @code
  BoxLayout layout;
  // ... layout is constructed ...
  Point pi; Box bi;
  for (BLIterator iter(layout); iter != iter.end(); ++iter) {
  // access current Point:
  pi = (*iter);
  // access current Patch:
  bi = layout[*iter];
  // ... do things ...
  }
  @endcode
*/
class BLIterator
{
public:
  /// Default Constructor
  BLIterator();
  /// Create a BLIterator from a BoxLayout
  BLIterator(const BoxLayout& a_boxLayout);

  ///Destructor
  ~BLIterator();

  /// Definition of increment operator
  BLIterator& operator++();
  
  /// Definition of "*" operator
  Point& operator*();

  /// Returns the linear index of the point in m_boxLayout associated with *this.
  int operator()(){return m_boxLayout.getPatchIndex(*(*this));};

  /// Equality test for BLIterator objects
  bool operator!=(BLIterator a_other);
  /// Returns a BLIterator pointing to the first Point in m_boxLayout.
  BLIterator begin();

  /// Returns a BLIterator pointing to a Point outside m_boxLayout.
  /**
    This method should only be used to check if we're done in a for-loop.
  */
  BLIterator end();

private:

  /// Create a BLIterator from a BoxLayout starting from a user defined Point in bit-space.
  /**
    Only used internally by end(), and does not create a legitimate BLIterator. Possibly unnecessary.
  */
  BLIterator(const BoxLayout& a_boxLayout, const Point& a_point);

  BoxLayout m_boxLayout; ///< BoxLayout object associated with *this
  Point m_data; ///< Current iterant. Data is a Point in bit-space representing (the lowCorner of) a patch in the BoxLayout domain. 
  std::vector<Point>::iterator m_vectorIterator; ///< An object used to iterate through a vector<Point> object.
};
#endif // _BLIterator_H_
