Analyterix test infrastructure
==============================

 * The tests are executed during `make check` in the codethorn directory.   
 * Every directory inside `<codethorn directory>/src/tests/analyterix/` is considered as a test.
 * The test-directory name should not contain spaces or special characters.
 * Every test-directory has to contain a subject file with the name `subject.C` that gets tested.
 * Optional files are:

   * `arguments`  
     Can contain additional arguments for analyterix. For example the analysis that should be performed.
    
   * `fails`  
     If existing the test has to fail to be OK. It is an error if the test should fail but passes. In the absence of this file the test has to pass to be OK.
    
   * `timeout`  
     If existing it has to contain a positive integer and analyterix will be aborted after the given number of seconds. Useful for tests which might lead to an endless loop.
    
   * `<X>.ref`  
     Various reference files which are compared with the corresponding output files. A output file corresponds to a reference file if the filenames without extension match. For example `a.ref` matches the output `a.C` or `b.ref` matches `b.csv`. The allowed output file extensions can be managed in `<codethorn directory>/src/scripts/runAnalyterixTest.sh`.
