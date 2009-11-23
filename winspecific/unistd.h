#ifdef __MINGW32__

#  include_next <unistd.h>

#else

#ifdef _MSC_VER 
#  include <io.h>
#  include <process.h>
#endif

#endif
