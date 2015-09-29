#include <vector>

#include "boost/shared_ptr.hpp"

#include "boost/foreach.hpp"

class X;

void foobar() 
   {
     int num = 0;
     std::vector<boost::shared_ptr<X> > *X_ptr = 0L;

     BOOST_FOREACH(boost::shared_ptr<X> procInfo, *X_ptr) 
        {
          num++;
        }
   }


