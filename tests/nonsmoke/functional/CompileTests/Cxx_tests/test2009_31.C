/*
 *  Ellipse used with try-catch within a template function
 *  See bug 375
 *  https://outreach.scidac.gov/tracker/index.php?func=detail&aid=375&group_id=24&atid=185
 *  Liao, 10/29/2009
 *
 *  Distilled from 
 * #include <iostream>
 * using namespace std;
 * 
 * class Hello 
 * {
 *   void run()
 *   {
 *     cout<<"1";
 *   }
 * };
 * 
 * */
namespace std
{
  template<typename _CharT>
    class basic_ostream
    {
    };

  typedef basic_ostream<char> ostream;
  extern ostream cout;
}

template<typename _CharT>
  std::basic_ostream<_CharT> &
operator<<( std::basic_ostream<_CharT> & out, const char* __s )
{
  try {}
  catch(...)
  {}
  return out;
}

class TestClass
{
  public:
    void foo()
    {
      std::cout<<"1";
    };
};

