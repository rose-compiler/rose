class GrainularitySpecification
   {
  // This class is used to specify the grainularity of copies of the AST into which we seed security flaws.

     public:
         enum GranularityLevelEnum
            {
              e_unknown    = 0,
              e_expression = 1,
              e_statement  = 2,
              e_function   = 3,
              e_class      = 4,
              e_file       = 5,
              LAST_ENUM_VALUE
            };

      // Specify the kind of grainularity for new code to be generated at
         GranularityLevelEnum grainularityLevel;

      // Select all levels of grainularity (mostly for testing)
         bool testAllLevels;

      // If this is an e_expression, e_statement, or e_class; specify the depth of the vulnerability 
      // for a copy.  For example, if the vulnerability is in an expression, e_statement might be
      // specified and the enclosingScopeDepth set to 2 meaning that levels up in the AST containing
      // the vulnerability would define the subtree to be used to build a copy.  That copy would then
      // be inserted after the original subtree (subtrees can not be selected where this would not be
      // possible (e.g. a for loop can only have a single loop body, etc.).  The details of the 
      // implementation languge enforce a number of constraints that must be obeyed.
         int enclosingScopeDepth;

         GrainularitySpecification();
        ~GrainularitySpecification();

         void set_enclosingScopeDepth( int n );
         int get_enclosingScopeDepth();

         void set_testAllLevels( bool t );
         bool get_testAllLevels();

         void set_grainularityLevel( GranularityLevelEnum t );
         GranularityLevelEnum get_grainularityLevel();
   };



