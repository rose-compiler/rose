# Overview

## Framework Code

### Compilation
Common unit testing framework code is defined in `UNITS/include/rose/tests/unitTests/common.h` and must be included in your test source file:

```C
#include <rose/tests/unitTests/common.h>
```

Note: Your test executable must be compiled with the appropriate header include search path:

```Makefile
AM_CPPFLAGS = -I$(top_srcdir)/tests/UnitTests/include/
```

### Linking

Unit test executables must link with `common.la` in their respective `Makefile.am`:

```Makefile
LDADD = $(top_builddir)/tests/UnitTests/libcommon.la
```

## Unit Tests

### Main Run Function: `RunUnitTests()`

Unit tests must be organized into their respective `namespace` and each `namespace` must have a generic run function:

```C
namespace MyXyzUnitTests {
  static void RunUnitTests() {
      HandlesDefaultParameter();
      HandlesSingleParameter();
      HandlesInvalidInput();
      ...
  }
}
```

