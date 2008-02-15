// Notes on the Composition of transformations:

// The interaction of transformations forms an important part of the specification of more complex
// transformation (e.g. A++ transformations (A++) and P++ modifications of A++ transformations
// (P++(A++)).  The ability to express the parallelization of transformations as an operator on a
// set of transformations is a powerful concept.  Other examples include performance
// instrumentation, error checking, etc.

class TransformationBaseClass
   {
  // true if the transformation was performed
     bool processed () = 0;

  // gather data representing the transformation (typically collections of strings)
     void gatherData() = 0;

  // Process the strings to form the AST fragments that will be used to modify the AST
     void processData () = 0;

  // Edit the AST using the AST fragments
     void modifyAST () = 0;
   };

class X_Transformation : public TransformationBaseClass
   {
     static X_Transformation transform(SgNode*);
     bool processed();
     void gatherData();
     void processData();
     void modifyAST();
   };

class Y_Transformation : public TransformationBaseClass
   {
     static X_Transformation transform(SgNode*);
     bool processed();
     void gatherData();
     void processData();
     void modifyAST();
   };

class XofY_Transformation : public TransformationBaseClass,
                            public X_Transformation,
                            public Y_Transformation
   {
  // Forms X(Y) depending upon implementation
     static XY_Transformation transform(SgNode*);
     bool processed()
        {
          return X_Transformation::processed() && Y_Transformation::processed();
        }
     void gatherData()
        { 
          X_Transformation::gatherData();
          Y_Transformation::gatherData();
       // Place X(Y) specific code here
        }
     void processData()
        {
          X_Transformation::processData();
          Y_Transformation::processData();
        }
     void modifyAST();
   };


                    X_Transformation X = ArrayStatementTransformation::transform(astNode);

                    if (X.processed() == true)
                       {
                         printf ("Transformation succeeded! \n");
                       }
                      else
                       {
                         printf ("No Transformation Done (not required)! \n");
                       }

OR

                    ArrayStatementTransformation X = ArrayStatementTransformation::gatherInfo(astNode);

                    if (X.processed() == true)
                       {
                         printf ("Transformation need to be doen! \n");
                         X.processPart1();
                         X.processPart2();
                         X.processPart3();
                       }
                      else
                       {
                         printf ("No Transformation Done! \n");
                       }


