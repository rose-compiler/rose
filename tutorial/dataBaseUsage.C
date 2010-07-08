// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"

using namespace std;

// DQ (9/9/2005): Don't include the database by default
// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
//#ifdef HAVE_MYSQL
   #include "GlobalDatabaseConnectionMYSQL.h"
#endif

int main( int argc, char * argv[] ) 
   {
// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
     //#ifdef HAVE_MYSQL
  // Build the Data base
     GlobalDatabaseConnection *gDB;
     gDB = new GlobalDatabaseConnection( "functionNameDataBase" );
     gDB->initialize();
     string command = "";
     command = command + "CREATE TABLE Functions ( name TEXT, counter );";

     Query *q = gDB->getQuery();
     q->set( command );
     q->execute();

     if ( q->success() != 0 )
          cout << "Error creating schema: " << q->error() << "\n";
  // Alternative syntax, but does not permit access to error messages and exit codes
  // gDB->execute(command.c_str());
#endif

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Run internal consistency tests on AST
     AstTests::runAllTests(project);
     
  // Build a list of functions within the AST
     Rose_STL_Container<SgNode*> functionDeclarationList = 
          NodeQuery::querySubTree (project,V_SgFunctionDeclaration);

     int counter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); 
                i != functionDeclarationList.end(); i++)
        {
       // Build a pointer to the current type so that we can call 
       // the get_name() member function.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT(functionDeclaration != NULL);
          
          SgName func_name = functionDeclaration->get_name();
      // Skip builtin functions for shorter output, Liao 4/28/2008   
          if (func_name.getString().find("__builtin",0)==0)
            continue;

       // output the function number and the name of the function
          printf ("function name #%d is %s at line %d \n",
               counter++,func_name.str(),
               functionDeclaration->get_file_info()->get_line());

          string functionName = functionDeclaration->get_qualified_name().str();

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
	  //#ifdef HAVE_MYSQL
          command = "INSERT INTO Functions values(\"" + functionName + "\"," + 
                  StringUtility::numberToString(counter) + ");";
       // Alternative interface
       // q->set( command );
       // cout << "Executing: " << q->preview() << "\n";
       // q->execute();
          gDB->execute(command.c_str());
#endif
        }

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
     //#ifdef HAVE_MYSQL
     command = "SELECT * from Functions;";

  // Alternative Interface (using query objects)
  // q << command;
     q->set(command);
     cout << "Executing: " << q->preview() << "\n";

  // execute and return result (alternative usage: "gDB->select()")
     Result *res = q->store();
     if ( q->success() != 0 )
          cout << "Error reading values: " << q->error() << "\n";
     else
      {
     // Read the table returned from the query
     // res->showResult();
        for ( Result::iterator i = res->begin(); i != res->end(); i++ )
           {
          // Alternative syntax is possible: "Row r = *i;"
             string functionName = (*i)[0].get_string();
             int counter = (*i)[1];
             printf ("functionName = %s counter = %d \n",functionName.c_str(),counter);
           }
      }

     gDB->shutdown();
#else
     printf ("Program compiled without data base connection support (add using ROSE configure option) \n");
#endif

     return 0;
   }

