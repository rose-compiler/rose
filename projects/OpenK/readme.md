OpenK

An ongoing project to explore knowledge-driven HPC analysis and optimization. 
We use the standard and toolchain used by OWL to formally model the concepts and relations in HPC domains, including programs, hardware, analysis and optimization, etc.

To enable make check rules to run prolog queries
* configure with --with-swi-prolog=/your/pathto/swipl if swipl cannot be automatically detected in your path

For latest documentation:
* https://en.wikibooks.org/wiki/ROSE_Compiler_Framework/OpenK 

3. Change to `rosePrgKnowledgeBuilder/` and run `make`
4. Check `test/npb2.3-test/run_test.py` to see how to run the program.

## Directory

+ ontology/ - owl files.
+ projects/ - contains independent projects or programs.
  canonicalloop/ - using Prolog query to find canonical loops
  cfg_test/ - using Prolog to generate control flow graph
  rose_canonicalloop/ - classic AST-based canonical loop detection, used for comparison
  staticCFG_tests/- classic AST-based control flow graph generation, used for comparison
+ test/ - test bench and run test script.
+ rosePrgKnowledgeBuilder/ - the knowledge generator using ROSE frontend to parse the C code and build knowledge base.

