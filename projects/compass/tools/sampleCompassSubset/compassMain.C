#ifdef ROSE_MPI
  #include <mpi.h>
#endif

#include "rose.h"
#include "compass.h"

//Header file for the stat struct and fstat function
//to determine last file modification time
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <boost/lexical_cast.hpp>

#ifdef USE_QROSE
// This is part of incomplete GUI interface for Compass using QRose from Imperial.

#include "qrose.h"
#endif


#ifdef USE_QROSE
// This is part of incomplete GUI interface for Compass using QRose from Imperial.

// A QRose specific output object which provides the information required to support the QRose GUI.
class QRoseOutputObject
   : public Compass::OutputObject
   {
  // This output object adds the ability to collect the output violations so that they can be presented in the GUI after all runs are complete.

     public:
          QRoseOutputObject() {}

          virtual void addOutput(Compass::OutputViolationBase* theOutput)
             {
               ROSE_ASSERT( isSgLocatedNode(theOutput->getNode()) != NULL 
                   || isSgFile(theOutput->getNode()) != NULL );
               outputList.push_back(theOutput);
               printf ("In QRoseOutputObject::addOutput(): getString() = %s \n",theOutput->getString().c_str());
             }
   };
#endif


void buildCheckers( std::vector<const Compass::Checker*> & checkers, Compass::Parameters & params, 
		    Compass::OutputObject & output, SgProject* pr );

int main(int argc, char** argv)
   {
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer_main ("Compass performance (main): time (sec) = ",true);

     std::ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

     if (SgProject::get_verbose() > 0)
          printf ("In compassMain.C: main() \n");

     Rose_STL_Container<std::string> commandLineArray = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);

     Compass::commandLineProcessing(commandLineArray);

  // Read the Compass parameter file (contains input data for all checkers)
  // This has been moved ahead of the parsing of the AST so that it is more 
  // obvious when it is a problem.
     Compass::Parameters params(Compass::findParameterFile());

#ifdef ROSE_MPI
     // Initialize MPI if needed...
     // need to do this to make test cases pass with MPI. 
     /* setup MPI */
     MPI_Init(&argc, &argv);
     MPI_Comm_rank(MPI_COMM_WORLD, &Compass::my_rank);
     MPI_Comm_size(MPI_COMM_WORLD, &Compass::processes);
#endif

  // Use a modified commandline that inserts specific additional options
  // to the ROSE frontend to make use with Compass more appropriate.
  // SgProject* project = frontend(argc,argv);
     SgProject* project = frontend(commandLineArray);


#ifdef HAVE_SQLITE3

     std::vector<bool> was_modified;

     //Determine if any file has been modified since the last run. If so, rerun
     //the compass checkers. The first run will always run the compass checkers.
     for (int i = 0; i < project->numberOfFiles(); ++i)
     {
       // In each file find all declarations in global scope
       SgSourceFile* sageFile = isSgSourceFile(project->get_fileList()[i]);
       std::string filename   = sageFile->getFileName();

       struct stat file_info;
       
       if ( stat(filename.c_str(), &file_info) != 0 )
         {
           std::cerr << "Error: Can not determine last modification time of file " << filename 
                     << std::endl;
       }else{

         std::string last_modified = boost::lexical_cast<std::string>(file_info.st_mtime);
         try {

           /* Read in from database here */
           sqlite3x::sqlite3_command cmd(Compass::con, "SELECT last_modified from file_last_modified where filename=\""+filename+ "\"" );

           sqlite3x::sqlite3_reader r = cmd.executereader();


           while (r.read()) {
             std::string last_modified_in_db = r.getstring(0);
             was_modified.push_back( (last_modified_in_db == last_modified) ? false : true );

           }

         } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}

         //Update last modified time in database
         try{
           sqlite3x::sqlite3_command cmd(Compass::con,"DELETE from file_last_modified where filename=\""+filename+ "\"");
           cmd.executenonquery();
         } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}

         try{
           sqlite3x::sqlite3_command cmd(Compass::con,"INSERT into file_last_modified(filename, last_modified) VALUES(?,?)");
           cmd.bind(1,filename);
           cmd.bind(2,last_modified);

           cmd.executenonquery();
         } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}

                  
       };


     }

     //Continue processign iff at least one file was modified
     if( ( find(was_modified.begin(), was_modified.end(), true) != was_modified.end() )
         ||  (was_modified.size() == 0) )
     {
       //Delete violation entries that correspond to this file
       for (int i = 0; i < project->numberOfFiles(); ++i)
       {
         // In each file find all declarations in global scope
         SgSourceFile* sageFile = isSgSourceFile(project->get_fileList()[i]);
         std::string filename   = sageFile->getFileName();
         try{
           sqlite3x::sqlite3_command cmd(Compass::con,"DELETE from violations where filename=\""+filename+ "\"");
           cmd.executenonquery();
         } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}


       }
       //continue processing
     }else
       exit(0);




#endif


#if 0
     project->display("In Compass");
#endif

     std::vector<const Compass::Checker*> traversals;

#ifdef USE_QROSE
  // This is part of incomplete GUI interface for Compass using QRose from Imperial.

  // Both of these output object work, but one is more useful for QRose.
     QRoseOutputObject* output = new QRoseOutputObject();
#endif

     Compass::PrintingOutputObject output(std::cerr);
     
        {
       // Make this in a nested scope so that we can time the buildCheckers function
          TimingPerformance timer_build ("Compass performance (build checkers and run prerequisites): time (sec) = ",false);

          buildCheckers(traversals,params,output, project);
          for ( std::vector<const Compass::Checker*>::iterator itr = traversals.begin(); itr != traversals.end(); itr++ ) {
            ROSE_ASSERT (*itr);
            Compass::runPrereqs(*itr, project);
          }
        }

     TimingPerformance timer_checkers ("Compass performance (checkers only): time (sec) = ",false);

     std::vector<std::pair<std::string, std::string> > errors;
     for ( std::vector<const Compass::Checker*>::iterator itr = traversals.begin(); itr != traversals.end(); itr++ )
        {
          if ( (*itr) != NULL )
             {
               if (Compass::verboseSetting >= 0)
                    printf ("Running checker %s \n",(*itr)->checkerName.c_str());

               try
                  {
                    int spaceAvailable = 40;
                    std::string name = (*itr)->checkerName + ":";
                    int n = spaceAvailable - name.length();
                    //Liao, 4/3/2008, bug 82, negative value
                    if (n<0) n=0;
                    std::string spaces(n,' ');
                    TimingPerformance timer (name + spaces + " time (sec) = ",false);
                    (*itr)->run(params, &output);
                  }
               catch (const std::exception& e)
                  {
                    std::cerr << "error running checker : " << (*itr)->checkerName << " - reason: " << e.what() << std::endl;
                    errors.push_back(std::make_pair((*itr)->checkerName, e.what()));
                  }
             } // if( (*itr) != NULL )
            else
             {
               std::cerr << "Error: Traversal failed to initialize" << std::endl;
               return 1;
             } // else
        } // for()

  // Support for ToolGear
     if (Compass::UseToolGear == true)
        {
          Compass::outputTgui( Compass::tguiXML, traversals, &output );
        }

#ifdef HAVE_SQLITE3
     if (Compass::UseDbOutput == true)
        {
          Compass::outputDb( Compass::outputDbName, traversals, &output );
        }
#endif

  // Output errors specific to any checkers that didn't initialize properly
     if (!errors.empty())
        {
          std::cerr << "The following checkers failed due to internal errors:" << std::endl;
          std::vector<std::pair<std::string, std::string> >::iterator e_itr;
          for (e_itr = errors.begin(); e_itr != errors.end(); ++e_itr)
             {
               std::cerr << e_itr->first << ": " << e_itr->second << std::endl;
             }
        }

  // Just set the project, the report will be generated upon calling the destructor for "timer"
     timer_main.set_project(project);

#ifdef ROSE_MPI
  MPI_Finalize();
#endif
#if 1  // Liao, 2/26/2009, add this backend support to be more friendly to build systems
       // 
     return backend(project);
#else     
     return 0;
#endif     
   }
