

Expected stages of Ada support:

1) Processing through GNAT
2) Generation of flat C/C++ data structure for all ASIS IR nodes.
3) Traversal over flat C/C++ data structure of ASIS IR to build ROSE
   Untyped IR nodes (with references to ASIS IR nodes via attributes mechanism).
4) Assemble of ROSE untyped IR nodes into Untyped AST
5) Construction of ROSE AST (using ROSE IR nodes (not the untyped kind)
    a) construct type system
    b) build new IR nodes as needed
6) Build language specific unparser and test using ROSE AST
7) Connect to program analysis infrastructure
    a) test using internal analysis
    b) test with existing language independent tools
8) Test using internal test codes defining regression test suite.
9) Test using target applications (e.g. from sponsor).
10) Test against Ada compiler test suit (for Ada this is ACVC).
