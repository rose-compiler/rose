! The include file must exist or it is an error!
include 'fortran_foo_single_quote.h'
include "fortran_foo_double_quote.h"

! OFP appears to ignore CPP directives.
#include "cpp_foo.h"
#include <cpp_foobar.h>

end

