/* Test case contributed by Jeff Keasler
 * 1/26/2009
 *
 * */
#include <vector>

void foo()
{
  class std::vector< int  , class std::allocator< int  >  > v(10,7);
  v . push_back(0);
  for (std::vector< int ,class std::allocator< int  > > ::iterator i = v . begin();  * i != 0; i ++ ) {
  }
}
