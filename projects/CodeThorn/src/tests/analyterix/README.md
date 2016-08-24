Analyterix test infrastructure
==============================
 * The Analyterix tests are executed during `make check` in the codethorn build directory.
   * They can be skipped by providing `skip-analyterix=1` as argument to make check (`make check skip-analyterix=1`).
   * If `clean-only=1` is provided as an argument to make check (`make check clean-only=1`) then temporary test/ output files are cleaned up only (and no Analyterix tests are executed).
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
   * `<X>.cmpmd`  
     Various comparision mode files which specify the comparision mode when comparing an output file `<X>.<Y>` with the corresponding reference file `<X>.ref`. Each of these files has to contain `equal`, `supersetOrEqual`, or `subsetOrEqual`. The comparision mode can therefore be specified for each reference file separately. If there is no comparision mode file for a reference file, then the default comparision mode `equal` is used. If one of the comparision modes `subsetOrEqual` or `supersetOrEqual` is specified for a reference file then the test passes if the files are equal **or** if the lines of the output file are a superset/ subset of the lines of the reference file. 
     * **Example**  
      Reference file `deadcode.ref` contains:
        ~~~
        2,i = 1;
        3,j = 3;
        ~~~
      Comparision mode file `deadcode.cmpmd` contains:
        ~~~
        subsetOrEqual
        ~~~ 
        * If output file `deadcode.csv` contains:
          ~~~       
          2,i = 1;
          ~~~
    
          ==> Output is subset of reference: Test passes.
    
        * If output file `deadcode.csv` contains the following instead:
          ~~~
          2,i = 1;
          4,k = 2;
          ~~~
          ==> Output is neither subset of (nor superset of) nor equal to reference: test fails.
    
        * If output file `deadcode.csv` contains the following instead:
          ~~~
          2,i = 1;
          3,j = 3;
          ~~~
         ==> Output and reference are equal: test passes.
    
        * If output file `deadcode.csv` contains the following instead:
          ~~~
          2,i = 1;
          3,j = 3;
          4,k = 2;
          ~~~
         ==> Output is superset of reference: test fails.
