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


Run the tool processing all .json files under a given path
*  totalOrderGeneration .  # assuming .json files are in current path


 In the end, the headers will be written into a file named:
    total_header.hh


insertHeader2AllCpp.sh 
* a script to prepend #include "total_header.hpp" to all .cpp files under the current path
