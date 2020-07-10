class DEF
   {
     public:
          bool ghi;

          void foo()
             {
            // This will have source position information, but the simple reference to the variable will not.
            // if (ghi == true)
            // This is because the expression will be a SgArrowOp in the subtree represented by "(this)->ghi" 
            // instead of "ghi" as a SgVarRefExp.
               if (ghi)
                  {
                  }
            // ghi = 42;
             }
   };

