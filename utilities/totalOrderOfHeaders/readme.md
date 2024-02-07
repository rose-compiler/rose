Given an input path containing *.json files: 

input .json file has a format of 

 [["fullpath1", "name1", "0"],["fullpath2", "name2","1"],["fullpath3", "name3","1"], ...] 

The tool, totalOrderGeneration, will Extract header full paths to creaet a partial order for each file, 
 Then build a total order of the full paths.
 And finally write #include <headerName> or #include "headerName", depending on the last flag 1 or 0.

To build the tool
* make


To install the tool into $ROSE_HOME/bin
* make install


Run its builtin regression tests
* make test

Run its regression tests using .json files within json folder
* make check


Show help messages

```
./totalOrderGeneration --help
Usage: totalOrderGeneration [options]
Options:
  --help        Show this help message
  --test        Run the builtin test mode
  --abortOnCycle  Disable cycle removal, abort and issue error messages when encountering first cyclic order in header files
  --verbose     Run the program in a verbose mode
  --input=path  Specify the input path to search for input .json files recursively
  --output=filename  Specify the output header file's name, default is total_header.hh
  --include-vxworks-header In the generated header file, prepend #include "vxWorks.h"

```

Run the tool processing all .json files under a given path
*  totalOrderGeneration --input=jsonFiles  # assuming .json files are in a folder named jsonFiles


 In the end, the headers will be written into a file named:
    total_header.hh (default name)
    or specified file name using --output=???.hh


insertHeader2AllCpp.sh total_header.hpp
* a script to prepend #include "total_header.hpp" to all .cpp files under the current path
