// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          github.com:matzke1/sawyer.




#include <Sawyer/SmallObject.h>
#include <Sawyer/Sawyer.h>

namespace Sawyer {
SAWYER_EXPORT SynchronizedPoolAllocator SmallObject::allocator_;
} // namespace
