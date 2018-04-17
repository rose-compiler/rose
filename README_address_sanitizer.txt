
Address Sanitizer is a tool for memory checking that is implemented as a dynamic 
analysis and for which the instrumentation can be triggered uisng compiler options 
in GNU (version 4.8 and later) and Clang (version 3.1 and greater).

The use of this suppport has been implemented as a configure option (--enable-sanitizer)
in ROSE.

One way to find where I have made modifications to the ROSE Makefile specific to the 
Address Sanitizer is to grep on the use of the AM_CONDITIONAL "ROSE_USE_SANITIZER".

Experiences on use with "make check" (53,000 tests) are:
   1) Fixed a bug in the rose_attributes_list.C file (required use of array delete).
   2) Fixed one bug in the Fortran support for where tokens are allocated and deleted 
      (must use malloc and free because tokens are freed from a file compiled using the C 
      compiler (token.c)).
   3) Fortran support with address sanitizer is flacky due to the JVM and maybe the early
      implementation of address sanitizer in GNU 4.8.3).  Less reliable with greater
      parallelism using to compile tests codes, plus either failes clearly in the JVM, or 
      address sanitizer can't report informaion about the problem: 
          "AddressSanitizer can not provide additional info.".  
      All failing files will
      compile the second time, there are not reproducable failures of address sanitizer on 
      any specific files.

   4) Fixed a few related bugs in roseTests/astProcessingTests/astTraversalTest.C (nicely caught by address sanitizer).

   5) All of the Java tests in roseTests/astSnippetTests appear to fail (though address sanitizer 
      is also unable to report any specific information as well).  These are now skipped when testing
      using the new --enable-sanitizer configure option in ROSE.

   6) The Java_tests directory is excluded from address sanitizer testing (the JVM does not play well 
      with the GNU 4.8 implementation of Address Sanitizer).  These are now skipped when testing
      using the new --enable-sanitizer configure option in ROSE.

   7) The tests in RoseExample_tests are excluded from the Address Sanitizer testing, but can be 
      run directly within that directory.  Several fail:
     testRoseHeaders_01.C
     testRoseHeaders_02.C
     testRoseHeaders_03.C
     testRoseHeaders_04.C
     testRoseHeaders_06.C

     All errors reported by address sanitizer appear to be in the same place within EDG at:
          i_copy_constant_full /home/quinlan1/ROSE/git_rose_upgrade_to_edg_411/src/frontend/CxxFrontend/EDG/EDG_4.9/src/il.c:5696

     Since this is in an older version of EDG (4.9 instead of EDG 4.12), we can wait a bit and 
     gather results from EDG 4.12 if there is even an issue there.  These are now skipped when testing
      using the new --enable-sanitizer configure option in ROSE.

   8) When using the address sanitizer we fail the testing in STL_tests, however, I can't see the output.
      Need to work with Markus to figure out how to get the output from these tests. These are now skipped 
      when testing using the new --enable-sanitizer configure option in ROSE (this is temporary).

   9) Skipped testing of virtualCFG_tests directory due to Fortran code and proglems with too much 
      parallelism being used with the combination of the JVM and the address sanitizer.  These are 
      now skipped when testing using the new --enable-sanitizer configure option in ROSE.

  10) 3-4 tests fail in the directory: tests/nonsmoke/functional/CompilerOptionsTests/testGenerateSourceFileNames
      These are all the same issue:
     ==9813== ERROR: AddressSanitizer: alloc-dealloc-mismatch (malloc vs operator delete) on 0x600400be4810
         #0 0x2b5fa646ba5a in operator delete(void*) /nfs/casc/overture/ROSE/opt/rhel7/x86_64/gcc/4.8.3/mpc/1.0/mpfr/3.1.2/gmp/5.1.2/workspace/gcc-4.8.3/libsanitizer/asan/asan_new_delete.cc:57
         #1 0x2b5fac9fb351 in Rose::BinaryAnalysis::AsmUnparser::StaticDataRawBytes::operator()(bool, Rose::BinaryAnalysis::AsmUnparser::UnparserCallback::StaticDataArgs const&) /home/quinlan1/ROSE/git_rose_upgrade_to_edg_411/src/backend/asmUnparser/AsmUnparser.C:932

     I will let Robb look into this issue, since it is binary analysis specific (unparsing ASM).


   11) Tests failing in directory:
          tests/nonsmoke/functional/roseTests/programAnalysisTests/staticInterproceduralSlicingTests
       Skipping this directory.  Fixed makefile to report location of errors. All errros appear to be the same one in 
       the staticInterproceduralSlicing.C file.  These are now skipped when testing using the new --enable-sanitizer 
       configure option in ROSE (these need to be fixed).

   12) The BinaryAnalysis test have a number of failing tests (94 pass and 55 fail).
     testElfStrtab [err]: ==39677== ERROR: AddressSanitizer: alloc-dealloc-mismatch (malloc vs operator delete) on 0x600400643e50
     testElfStrtab [err]:     #0 0x7f13eaea1a5a in operator delete(void*) /nfs/casc/overture/ROSE/opt/rhel7/x86_64/gcc/4.8.3/mpc/1.0/mpfr/3.1.2/gmp/5.1.2/workspace/gcc-4.8.3/libsanitizer/asan/asan_new_delete.cc:57
     testElfStrtab [err]:     #1 0x7f13e2862351 in Rose::BinaryAnalysis::AsmUnparser::StaticDataRawBytes::operator()(bool, Rose::BinaryAnalysis::AsmUnparser::UnparserCallback::StaticDataArgs const&) /home/quinlan1/ROSE/git_rose_upgrade_to_edg_411/src/backend/asmUnparser/AsmUnparser.C:932

       Note that this is the same problem as in point #10 (above).  Also all fialing tests appear to be the same.



