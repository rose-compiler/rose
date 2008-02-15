#include "mapleSupport.h"

using namespace std;

namespace MapleRoseInterface
   {
  // The Maple server object (is there a better name for this)
     MKernelVector kv;

  /* ALGEB is the C data type for representing Maple data structures */
     ALGEB mapleResult;

  // Note about "M_DECL", it is defined to be "" on all machines but Windows
  // I have left it, but it could very well be removed later :-).

  /* Callback used for directing result output */
  // static void M_DECL textCallBack( void *data, int tag, char *output );
     void   M_DECL textCallBack( void *data, int tag, char *output );

  // All other Maple call back functions are also implemented but are not so useful
     void   M_DECL errorCallBack( void *data, M_INT offset, char *msg );
     void   M_DECL statusCallBack( void *data, long kilobytesUsed, long kilobytesAlloc, double cpuTime );
     char * M_DECL readLineCallBack( void *data, M_BOOL debug );
     M_BOOL M_DECL redirectCallBack( void *data, char *name, char *mode );
     char * M_DECL streamCallBack( void *data, char *name, M_INT nargs, char **args );
     M_BOOL M_DECL queryInterrupt( void *data );
     char * M_DECL callBackCallBack( void *data, char *output );

     string convertMapleTagsToString(int tag);

  // This is the string we use for the results passed back from Maple
     string mapleOutputString;
   }

/* Callback used for directing result output */
void M_DECL MapleRoseInterface::textCallBack( void *data, int tag, char *output )
   {
  // Note: In the displayed output the ";" is dropped (I think this is another Maple OpenMaple API bug)
  // printf("textCallBack: tag = %d (%s) output = %s (data = %p)\n",tag,convertMapleTagsToString(tag).c_str(),output,data);
     mapleOutputString = output;
   }

void M_DECL MapleRoseInterface::errorCallBack( void *data, M_INT offset, char *msg )
   {
     printf("errorCallBack: %s (data = %p)\n",msg,data);
   }

void   M_DECL MapleRoseInterface::statusCallBack( void *data, long kilobytesUsed, long kilobytesAlloc, double cpuTime )
   {
     printf("statusCallBack: kilobytesUsed = %d kilobytesAlloc = %d cpuTime = %f (data = %p) \n",kilobytesUsed,kilobytesAlloc,cpuTime,data);
   }

char * M_DECL MapleRoseInterface::readLineCallBack( void *data, M_BOOL debug )
   {
     printf("readLineCallBack: debug = %s (data = %p) \n",(debug) ? "true" : "false",data);
     return NULL;
   }

M_BOOL M_DECL MapleRoseInterface::redirectCallBack( void *data, char *name, char *mode )
   {
     printf("redirectCallBack: name = %s (data = %p) (mode = %s) \n",name,data,mode);
     return false;
   }

char * M_DECL MapleRoseInterface::streamCallBack( void *data, char *name, M_INT nargs, char **args )
   {
     printf("streamCallBack: name = %s (data = %p) \n",name,data);
     return NULL;
   }

M_BOOL M_DECL MapleRoseInterface::queryInterrupt( void *data )
   {
     printf("queryInterrupt: (data = %p) \n",data);
     return false;
   }

char * M_DECL MapleRoseInterface::callBackCallBack( void *data, char *output )
   {
     printf("callBackCallBack: %s (data = %p) \n",output,data);
     return NULL;
   }

string
MapleRoseInterface::convertMapleTagsToString(int tag)
   {
  // Convert Maple tag values into strings so that we can provide 
  // clearer output messages.

     string s;
     switch (tag)
        {
          case MAPLE_TEXT_DIAG:    s = "MAPLE_TEXT_DIAG";    break;
          case MAPLE_TEXT_MISC:    s = "MAPLE_TEXT_MISC";    break;
          case MAPLE_TEXT_OUTPUT:  s = "MAPLE_TEXT_OUTPUT";  break;
          case MAPLE_TEXT_QUIT:    s = "MAPLE_TEXT_QUIT";    break;
          case MAPLE_TEXT_WARNING: s = "MAPLE_TEXT_WARNING"; break;
          case MAPLE_TEXT_ERROR:   s = "MAPLE_TEXT_ERROR";   break;
          case MAPLE_TEXT_STATUS:  s = "MAPLE_TEXT_STATUS";  break;
          case MAPLE_TEXT_PRETTY:  s = "MAPLE_TEXT_PRETTY";  break;
          case MAPLE_TEXT_HELP:    s = "MAPLE_TEXT_HELP";    break;
          case MAPLE_TEXT_DEBUG:   s = "MAPLE_TEXT_DEBUG";   break;
          default:
             {
               printf ("Error: undefined Maple tag value = %d \n",tag);
               exit(1);
             }           
        }

     return s;
   }

void
MapleRoseInterface::startupMaple ( int argc, char *argv[] )
   {
  // How long of an error message buffer should this be?
     char err[2048];

  /* Maple kernel vector - used to start and make calls to Maple. */
  /* All OpenMaple function calls take kv as an argument. */
  // MKernelVector kv;

  // DQ (10/3/2005): Since we pass a reference to this data structure for later
  // use after startupMaple, maybe we should make this a static variable?
#if 1
     static MCallBackVectorDesc cb = { textCallBack,
                                     0,   /* errorCallBack not used */
                                     0,   /* statusCallBack not used */
                                     0,   /* readLineCallBack not used */
                                     0,   /* redirectCallBack not used */
                                     0,   /* streamCallBack not used */
                                     0,   /* queryInterrupt not used */
                                     0    /* callBackCallBack not used */
                                   };
#else
     static MCallBackVectorDesc cb = { textCallBack,    /* textCallBack used */
                                       errorCallBack,   /* errorCallBack used */
                                       statusCallBack,  /* statusCallBack used */
                                       readLineCallBack,/* readLineCallBack used */
                                       redirectCallBack,/* redirectCallBack used */
                                       streamCallBack,  /* streamCallBack used */
                                       queryInterrupt,  /* queryInterrupt used */
                                       callBackCallBack /* callBackCallBack used */
                                     };
#endif

  /* ALGEB is the C data type for representing Maple data structures */
  // ALGEB r;

#if 0
  // DQ (9/30/2005): This fixes a bug in the API for Maple (v9.0).  The bug is 
  // that Maple can't handle the value of argv[0] to be an empty string once 
  // the basename has been stripped off.  For now we just warn of this problem
  // and let the Maple function "StartMaple" fail directly.  We could fixup
  // argv[0] if this continues to be a poblem in Maple version 10.
     if (strlen(argv[0]) == strlen((char*)basename(argv[0])))
        {
          printf ("Error: Specify application using maple using \"./\" prefix as in ./%s (bug in Maple) \n",argv[0]);
        }
#endif

  /* Initialize Maple */
     printf ("Calling startMaple ... \n");
     if ( ( kv = StartMaple(argc, argv, &cb, NULL, NULL, err) ) == NULL )
        {
          printf( "Could not start Maple, %s\n", err );
          exit(1);
        }

     printf ("Finished with Maple server setup! \n");
   }

void
MapleRoseInterface::stopMaple ()
   {
     StopMaple(kv);

     printf ("Maple server terminated (no further use of Maple is possible... (total Maple invocations = %d) \n",mapleEquationCounter);
   }

static string wrapCommandString ( string inputCommand )
   {
  // Only commands wrapped in "diff" appear to work through the C/C++ to Maple interface!
  // return "simplify(" + inputCommand + ");";
#if 0
  // string prefix  = "writeto(\"/home/dquinlan2/ROSE/LINUX-3.3.2/projects/programModeling/mapleOutput.data\"); \n";
  // string postfix = "writeto(terminal); \n";
  // string prefix;
  // string postfix = "save statement_equation, \"/home/dquinlan2/ROSE/LINUX-3.3.2/projects/programModeling/mapleOutput.data\"; \n";
  // string postfix = "latex(\"/home/dquinlan2/ROSE/LINUX-3.3.2/projects/programModeling/mapleOutput.data\"); print(statement_equation); \n";
  // string postfix = "latex(statement_equation); \n";
  // string postfix = "appendto(\"/home/dquinlan2/ROSE/LINUX-3.3.2/projects/programModeling/mapleOutput.data\"); latex(statement_equation); \n";
  // string postfix = "appendto(\"/home/dquinlan2/ROSE/LINUX-3.3.2/projects/programModeling/mapleOutput2.data\"); \n";
     string prefix = "appendto(\"/home/dquinlan2/ROSE/LINUX-3.3.2/projects/programModeling/mapleOutput2.data\"); \n";
     string postfix = "latex(statement_equation); \n";
#else
     string prefix;
     string postfix;
#endif

     return prefix + inputCommand + postfix;
   }

string
MapleRoseInterface::evaluateMapleStatement ( string command )
   {
  // This is the function that executes a Maple command
  // printf( "Compute a Maple command (EvalMapleStatement): [%s] \n",command.c_str());

  // Invocation counter (useful for debugging)
     MapleRoseInterface::mapleEquationCounter++;

  // printf( "Compute a Maple command (MapleEval) \n");
  // char* commandCstring = strdup(command.c_str());
     char* commandCstring = strdup(wrapCommandString(command).c_str());
     mapleResult = EvalMapleStatement( kv, commandCstring );

#if 0
     printf ("Leaving MapleRoseInterface::evaluateMapleStatement (\n%s\n) \n",command.c_str());
     printf ("                            mapleOutputString (\n%s\n) \n",mapleOutputString.c_str());
#endif

     return mapleOutputString;
   }


