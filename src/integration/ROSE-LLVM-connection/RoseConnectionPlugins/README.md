# ROSE-LLVM-CONNECTION

## Description

- This demostrates how to connect ROSE and Clang/LLVM together to call ROSE features within Clang and LLVM plugins.

- The connection requires two components: Clang-ROSE Plugin and LLVM-ROSE pass plugin.

- The simplest implementation of this proceeds the following steps and behaves like the ROSE identityTranslator
  1.  Clang-ROSE plugin calls ROSE frontend to process the input file with "-rose:skipfinalCompileStep" option. 
  2.  The pointer to SgProject object is stored as global variable, roseProject. This pointer is shared between Clang plugin and LLVM pass plugin.
  3.  The LLVM-ROSE Pass Plugin can obtain the pointer to SgProject, created in Clang-ROSE plugin, and call ROSE backend afterward to generate unparsed code.  
  4. Other support such as static analysis in Clang or analysis and optimization in LLVM can be triggered through Clang plugin and LLVM pass plugin support.

## Software Prerequisites 

- Clang/LLVM 14.0
- Boost (built with LLVM), version 1.73 is used in development
- CMake 3.17.4 ( to build LLVM-ROSE-Plugin)


## Building Clang-ROSE plugin  & LLVM-ROSE Pass Plugin

1. Building ROSE
  - EDG source code required to build EDG binary
  - Config ROSE with Clang and Clang++
  - Example config command: ${ROSE_SRC}/configure --prefix=${ROSE_INSTALL_TREE} --enable-languages=c,c++ --with-boost=${BOOST_ROOT} --enable-boost-version-check=false --with-CXX_DEBUG=-g --with-C_OPTIMIZE=-O0 --with-CXX_OPTIMIZE=-O0 CC=${PATH_TO_CLANG} CXX=${PATH_TO_CLANG++}
2. Building Plugins 
  - mkdir build; cd build
  - cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DROSE_ROOT=${ROSE_INSTALL_TREE} -DBoost_ROOT=${BOOST_ROOT} ${PATH_TO_ROSE-LLVM-CONNECTION-SRC}
  - make

3. libRoseConnectionClangPlugin.so and libRoseConnectionLLVMPassPlugin.so will be created in build directory.


## Running with Clang-ROSE Plugin

- with clang driver: clang -Xclang -load -Xclang libRoseConnectionClangPlugin.so ${INPUT_CODE}
- Using the cc1 command line: clang -cc1 -load libRoseConnectionClangPlugin.so -plugin ROSE test.c
- Example command line: clang  -O0  -Xclang -load -Xclang libRoseConnectionClangPlugin.so test.c -c

## Running with LLVM-ROSE Pass Plugin

- LLVM-ROSE pass plugin depends on the pointer generated from Clang-ROSE plugin.  It cannot be executed standalone.
- with clang river: clang -O1  -Xclang -load -Xclang libRoseConnectionClangPlugin.so  -fpass-plugin=libRoseConnectionLLVMPassPlugin.so ${INPUT_CODE}
- Using LLVM's opt tool is not valid because this LLVM pass requires sgproject generated from the Clang plugin.
- Example command line: clang  -O0  -Xclang -load -Xclang libRoseConnectionClangPlugin.so  -fpass-plugin=libRoseConnectionLLVMPassPlugin.so test.c -c -g
