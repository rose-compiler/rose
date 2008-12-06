#ifdef ROSE_MPI
  #include <mpi.h>
#endif

#include "rose.h"
#include "compass.h"

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
     return 0;
   }
