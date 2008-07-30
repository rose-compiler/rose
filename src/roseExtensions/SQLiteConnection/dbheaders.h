#include <stdio.h>
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

class Row;

typedef std::vector <Row> Matrix;
