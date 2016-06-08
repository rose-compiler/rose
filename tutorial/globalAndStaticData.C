
#include "rose.h"

class Traversal : public SgSimpleProcessing
   {
     public:
       // location of where output data will be put
          ofstream *file;

       // constructor
          Traversal (ofstream *datfile);

      //! Required traversal function
          void visit (SgNode* node);

          void outputPositionInformation ( SgStatement* statement );
   };

Traversal::Traversal (ofstream *datfile)
   {
     assert(datfile != NULL);
     file = datfile;
   }

void
Traversal::outputPositionInformation ( SgStatement* statement )
   {
#if 0
     int lineNumber  = statement->get_file_info()->get_line();
     string fileName = statement->get_file_info()->get_filename();
#else
     int lineNumber  = statement->get_file_info()->get_raw_line();
     string fileName = statement->get_file_info()->get_raw_filename();
#endif
     printf ("Problem Variable Declaration at line = %4d file = %s \n",lineNumber,fileName.c_str());
     ROSE_ASSERT(file != NULL);
     (*file) << "Problem Variable Declaration at line = " << lineNumber << " file = " << fileName << std::endl;
   }


void
Traversal::visit(SgNode* node)
   {
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);

  // Look for variable declarations appearing in global scope!
  // if (variableDeclaration != NULL && isSgGlobal(variableDeclaration->get_parent()) != NULL)
     if (variableDeclaration != NULL)
        {
          SgInitializedNamePtrList::iterator i = variableDeclaration->get_variables().begin();
          while (i != variableDeclaration->get_variables().end())
             {
               SgInitializedName* initializedName = *i;

            // Check the type and see if it is a class (check for typedefs too)
               SgType* variableType = initializedName->get_type();

               SgClassType *classType = isSgClassType(variableType);
               if (classType != NULL)
                  {
                 // Now check if this is a global variable or an static class member
                    SgScopeStatement* scope = variableDeclaration->get_scope();
                    if (isSgGlobal(scope) != NULL)
                       {
                         printf ("Found a global variable defining a class \n");
                      // variableDeclaration->get_file_info()->display("global variable defining a class");
                         outputPositionInformation(variableDeclaration);
                       }

                    if (isSgClassDefinition(scope) != NULL)
                       {
                      // Now check if it is a static data member
                         if (variableDeclaration->get_declarationModifier().get_storageModifier().isStatic() == true)
                            {
                              printf ("Found a static data member defining a class \n");
                           // variableDeclaration->get_file_info()->display("static data member defining a class");
                              outputPositionInformation(variableDeclaration);
                            }
                       }
                  }

            // increment though the variables in the declaration (typically just one)
               i++;
             }
        }
   }

int
main ( int argc, char* argv[] )
   {
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     ofstream datfile ( "ProblemVariableDeclarations.data" , ios::out | ios::app );
     if ( datfile.good() == false )
        {
          printf ("File failed to open \n");
        }

  // datfile << "This is a test!" << std::endl;

#if 1
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

     Traversal traversal(&datfile);
     traversal.traverse(project,preorder);
#endif

     datfile.close();

#if 1
  // Run AST consistancy tests (so that we can test them while we analize KULL
     AstTests::runAllTests(const_cast<SgProject*>(project));
#else
     printf ("Skipped agressive (slow) internal consistancy tests! \n");
#endif



#if 0
     return 0;
#else
  // Output the source code and generate the object file so that 
  // we can test these parts of KULL processing as well
     if (project->get_verbose() > 0)
          printf ("Calling the backend() \n");

     int errorCode = 0;
     errorCode = backend(project);

  // DQ (12/3/2004): Test use of AST delete
  // DeleteSgTree(project);
     project = NULL;

  // DQ (7/7/2005): Only output the performance report if verbose is set (greater than zero)
     if (project->get_verbose() > 0)
        {
       // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
          AstPerformance::generateReport();
        }
     return errorCode;
#endif

   }
