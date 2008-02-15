#include <string>
using std::string;


class TauMidTraversal : public SgSimpleProcessing
{
  public:
  TauMidTraversal () {};
  ~TauMidTraversal () {};

  MiddleLevelRewrite::PlacementPositionEnum locationInScope;

  void visit ( SgNode* astNode );

};

void
TauMidTraversal::visit ( SgNode* astNode )
{
/*   SgStatement* statement = isSgStatement(astNode); */
/*   assert( statement != null ); */

  SgStatement *theStatement = isSgStatement( astNode );

  switch (astNode->variantT())
  {
      case V_SgGlobal:
      {
#if 0
        string theHeaderString("\n\
#ifdef USE_ROSE\n\
// If using a translator built using ROSE process the simpler tauProtos.h header \n\
// file instead of the more complex TAU.h header file (until ROSE is more robust) \n\
   #include \"tauProtos.h\"\n\n\
#else\n\
   #include <tauProfiler.h>\n\
#endif\n\
int abcdefg;");
#else
        string theHeaderString("\n\
#ifdef USE_ROSE\n\
// If using a translator built using ROSE process the simpler tauProtos.h header \n\
// file instead of the more complex TAU.h header file (until ROSE is more robust) \n\
   #include \"tauProtos.h\"\n\
#else\n\
   #include <tauProfiler.h>\n\
#endif");
#endif
        cout<<"inserting this string: "<<theHeaderString<<endl;

        MiddleLevelRewrite::insert(theStatement,theHeaderString,
                                   MidLevelCollectionTypedefs::StatementScope,
                                   MidLevelCollectionTypedefs::TopOfCurrentScope);
        cout<<"done."<<endl;
        break;
      } //end of global case

      case V_SgBasicBlock:
      {
        string tauInstumentationString = "TauTimer tauTimerInstrumentation;";

        printf ("At SgBasicBlock in %s: inserting this string: %s \n",
             theStatement->get_parent()->sage_class_name(),tauInstumentationString.c_str());

        MiddleLevelRewrite::insert(theStatement,tauInstumentationString,
                                   MidLevelCollectionTypedefs::StatementScope,
                                   MidLevelCollectionTypedefs::TopOfCurrentScope);
        break;
      }

     default: 
        {
#if 0
          printf ("Skipp instrumentation of everything else! \n");
#endif
        }
  }//end of switch statement
  
}//end of visit function


