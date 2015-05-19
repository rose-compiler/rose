#include "BLIterator.H"

BLIterator::BLIterator()
{
}

BLIterator::BLIterator(const BoxLayout& a_boxLayout)
{
  m_boxLayout = a_boxLayout;
  m_data = getOnes()*-1;
  begin();
}

BLIterator::BLIterator(const BoxLayout& a_boxLayout, const Point& a_point)
{
  m_boxLayout = a_boxLayout;
  m_data = a_point;
}

BLIterator::~BLIterator()
{
}

BLIterator& BLIterator::operator++()
{

  ++m_vectorIterator;

  if (m_vectorIterator != m_boxLayout.getPatchLocs().end())
    {
      m_data = *m_vectorIterator;
      return *this;
    }
  m_data = getOnes()*-1;
  return *this;
}

Point& BLIterator::operator*()
{
  return m_data;
}

bool BLIterator::operator!=(BLIterator a_other)
{
  return !(m_data == *a_other);
}

BLIterator BLIterator::begin()
 {
  m_vectorIterator = m_boxLayout.getPatchLocs().begin();

  if (m_vectorIterator != m_boxLayout.getPatchLocs().end())
   {
     m_data = *m_vectorIterator;
   }

  return *this;
 }

BLIterator BLIterator::end()
 {
   //don't change member data here!)
   return BLIterator(m_boxLayout, getOnes()*-1);
 }

