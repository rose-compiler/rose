# ROSE Compiler
	
ROSE is an open source compiler infrastructure to build source-to-source program transformation and analysis tools for large-scale Fortran (77/95/2003), C (up to C23), C++ (up to C++17), OpenMP, Cuda, and UPC applications. ROSE also has support for Java (7), Ada (95), and the analysis of binary files which use the ARM, AMD64 (x86_64), Intel x86, MIPS, Motorola 68k, PowerPC (32- and 64-bit), Java byte code, or CIL instruction sets. The intended users of ROSE could be either experienced compiler researchers or library and tool developers who may have minimal compiler experience. ROSE is particularly well suited for building custom tools for static analysis, program optimization, arbitrary program transformation, domain-specific optimizations, complex loop optimizations, performance analysis, and cyber-security.

http://www.rosecompiler.org/

# Why Use ROSE

ROSE is not grep, sed, LLVM, or a Perl script. A ROSE Tool uses the ROSE compiler-based infrastructure to parse code into a complete Abstract Syntax Tree (AST). The AST contains all the syntax and semantic information in the original code, and has a rich API supporting sophisticated analysis and transformations. The ROSE Tool queries the AST and reports on and/or changes the AST, then may emit new code from the AST. All ROSE Tools can thus precisely replicate the parsing and semantic analysis behaviour of multiple compiler languages, vendors, and versions. New ROSE Tools can quickly be created by customers or by the ROSE Team. ROSE is open-source, and is portable across a large and expanding range of platforms. ROSE Tools can process large code bases, and the ROSE infrastructure and ROSE Tool collection are continuously upgraded and extended by the LLNL ROSE Team and outside contributors.

# Installation Instructions
From the source tree run `./build`. Then navigate to your build tree and run configure and make.
```
../src/configure --prefix=/path/for/ROSE/install \
                 --enable-languages=c,c++ \
                 --with-boost=/path/to/boost/install
make -j${NUM_PROCESSORS}
make install -j${NUM_PROCESSORS}
make check -j${NUM_PROCESSORS}
```

For Ubuntu 18.04, we have experimental support for installating ROSE pre-built binaries packages using apt-get
```
sudo apt-get install software-properties-common
sudo add-apt-repository ppa:rosecompiler/rose-development # Replace rose-development with rose-stable for release version
sudo apt-get install rose
sudo apt-get install rose-tools # Optional: Installs ROSE tools in addition to ROSE Core
```

For full install instructions go to https://github.com/rose-compiler/rose/wiki/How-to-Set-Up-ROSE

# Supported Compilers
When ROSE is configured with c/c++ support it is required to download an EDG binary. Currently we generate these for GCC 7 through 12. The support for 11 and 12 is in progress, so for fully supported versions use GCC 7 through 10.

When not using c/c++ support GCC 7 through 13 are supported.

# ROSE Directories 
* `src`: all source code for ROSE
* `tools`: usable feature complete tools
* `projects`: in development and incomplete tools
* `tutorial`: exmples of ROSE features
* `docs`: files for bulding the documentation

# Documentation
For more information about ROSE and how to use it visit the github wiki at https://github.com/rose-compiler/rose/wiki

The ROSE API can be found at http://doxygen.rosecompiler.org. The API can also be made locally by going to `cd $ROSE_BUILD/docs/Rose` and running `make doxygen_docs`. The html pages can then be found in `${ROSE_BUILD}/docs/Rose/ROSE_WebPages` and can be easily viewed by pointing your browser at `${ROSE_BUILD}/docs/Rose/ROSE_WebPages`.
