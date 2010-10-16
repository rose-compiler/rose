                              THIRFY

This directory represents research work supporting the Thrifty project (funded by DOE).

This ROSE project defines a specialized compiler that supports power based optimizations.
Performance optimization will follow, but the initial focus on on power usage.


Work Flow:
   1) The ROSE translator introduces power optimizing transformations on an input code,
      the result of the translation is a ROSE transformed code.  
   2) The ROSE transformed code (source code) is fed to the GCC MIPs cross compiler to 
      build a modified executable.
   3) The modified executable is then executed on the Thrify simulator.




Details about using ROSE:
   1) The ROSE configure command must use the option:
         "--with-alternate_backend_C_compiler=<your path>/sescutils-ins/bin/gcc"
      To support this run "ln -s mipseb-linux-gcc gcc" in the "<your path>/sescutils-ins/bin" 
      directory.
   2) Run "make" as usual after configure is finished.

   3) Test the use of ROSE with the Thrifty Simulator by running "make check"
      in the compile tree's "projects/PowerAwareCompiler" directory.

   4) It should also be possible to run "make check" in the top level directory for 
      all of ROSE.

