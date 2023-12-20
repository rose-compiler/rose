sawyer
======

      log[INFO] <<"Sawyer is an event logger using C++ idioms\n";

Features
--------

+ *Familiar API:* Sawyer uses C++ paradigms for printing messages,
  namely the STL `std::ostream` insertion operators, manipulators, etc. 

+ *Ease of use:* All STL `std::ostream` insertion operators (`<<`) and
   manipulators work as expected, so no need to write any new functions
   for logging complex objects.

+ *Type safety:* Sawyer avoids the use of C `printf`-like varargs
  functions.

+ *Consistent output:* Messages have a consistent look, which by
  default includes the name of the program, process ID, software
  component, importance level, and optional color. The look of this
  prefix area can be controlled by the programmer.

+ *Multiple backends:* Sawyer supports multiple backends which can
  be extended by the programmer. Possibilities include colored output
  to the terminal, output to a structured file, output to a database,
  etc. (Currently only text-based backends are implemented. [2013-11-01])

+ *Partial messages:* Sawyer is able to emit messages incrementally
  as the data becomes available.  This is akin to C++ `std::cerr`
  output.

+ *Readable output:* Sawyer output is readable even when multiple
  messages are being created incrementally.  Colored text can be used
  to visually distinguish between different importance levels (e.g.,
  errors versus warnings).

+ *Granular output:* Sawyer supports multiple messaging facilities
  at once so that each software component can be configured
  individually at whatever granularity is desired: program, file,
  namespace, class, method, function, whatever.

+ *Run-time controls:* Sawyer has a simple language for enabling and
  disabling collections of error streams to provide a consistent user
  API for things like command-line switches and configuration
  files. Using these controls is entirely optional.

+ *Efficient:* Sawyer message output can be written in such a way that
  the right hand size of `<<` operators is not evaluated when the
  message stream is in a disabled state.

