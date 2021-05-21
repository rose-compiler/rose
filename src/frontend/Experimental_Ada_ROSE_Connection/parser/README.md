# dot_asis

Parses an Ada unit and produces textual output, GraphViz Dot graph output, and a C-compatible record structure.

Implemented using AdaCore gnat-gpl-2017 compiler and ASIS library.

To run on Linux:
- Install GNAT and ASIS
- Clone this repo
- Run the build and run script

To install GNAT and ASIS:
- Download GNAT and ASIS:
  - Go to http://libre.adacore.com/download/
  - Select "Free Software or Academic Development"
  - Click "Build Your Download Package"
  - On the next page:
    - Select your platform
    - Expand "> GNAT Ada 2017"
    - Select README.txt
    - Select the compiler tar or zip file
    - Expand "> Sources"
    - Select asis-gpl-2017-src.tar.gz
  - At the bottom of the page:
    - Select zip or tar
    - Click on "Download Selected Files"
  
This will download a file with a name like "AdaCore-Download-2017-08-24_1403.zip". The compiler takes up half a gig, and the adacore.com website can be very slow, so be patient.  

- Install GNAT:
  - Unzip the downloaded file.  
  - There will be a directory tree that contains "adagpl-2017/gnatgpl", with subdirs like:
    - "gnat-gpl-2017-x86_64-linux-bin"
    - "sources".
  - In gnat-gpl-2017-x86_64-linux-bin or similar:
    - Read "README"
    - Run "doinstall"
- Put GNAT on the path.  
  - All that is needed is to put <install_dir>/bin on the path, where <install_dir> is the location gnat-gpl-2017 was installed in.
  - Test that GNAT is there by running "gnatls -v".
  
- Install the ASIS library and source files in the GNAT installation, so that ASIS-using projects and source files can find it:
  - In sources
    - Unzip asis-gpl-2017-src.tar.gz into dir asis-gpl-2017-src
  - In sources/asis-gpl-2017-src:
    - Edit common.gpr
      - In package Compiler, Add "-fPIC" to both occurrences of 'for Default_Switches ("ada") use' (this enables generation of a shared client library later).
    - Read README
    - Run "module load gnat" or similiar to put GNAT on the path (see Install GNAT, above).
    - Run "make all install prefix=<install_dir>"
    
- Clone this dot_asis repo
- Run "module load gnat" or similiar to put GNAT on the path (see Install GNAT, above).
- In dot_asis/ada_main, run the build and run script:
  - Run "run_and_log do_ada_main.sh" to run and capture the results in a log file
    - Run "run_and_log" for help
  - Run "do_ada_main.sh" to run without logging to a file
- Success!
- To run the C program, in dot_asis/c_main, run "do_c_main.sh"
  - Run "do_ada_main.sh" first to build the library "do_c_main.sh" needs
  
