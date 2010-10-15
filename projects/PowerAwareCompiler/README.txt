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
