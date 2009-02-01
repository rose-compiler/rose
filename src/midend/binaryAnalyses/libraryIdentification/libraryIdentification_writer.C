
// Support for writing a FLIRT data base from an existing AST.

#include <rose.h>

#include <libraryIdentification.h>

// Use the MD5 implementation that is in Linux.
// I don't need this byt Andreas will...
#include <openssl/md5.h>

// Function prototype looks like:
// unsigned char *MD5(const unsigned char *d, unsigned long n, unsigned char *md);

using namespace std;

void
LibraryIdentification::generateLibraryIdentificationDataBase( string databaseName, SgProject* project )
   {
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);

     printf ("Building LibraryIdentification database: %s from AST of project: %p \n",databaseName.c_str(),project);

     FindFunctions t;

     printf ("Traverse the AST to file functions \n");
     t.traverse(project,preorder);
     printf ("DONE: Traverse the AST to file functions \n");
   }


void
LibraryIdentification::FindFunctions::visit(SgNode* n)
   {
     static int counter = 0;
     if (isSgAsmFunctionDeclaration(n) != NULL)
        {
          SgUnsignedCharList s;
          FlattenAST t(s);
          printf ("Traverse the AST for this function to generate byte stream \n");
          t.traverse(n,preorder);
          printf ("DONE: Traverse the AST for this function to generate byte stream \n");

       // set_function_match( const library_handle & handle, const std::string & data );

       // TODO: 
       //    1) fill in the library_handle with the filename, etc.
       //    2) Compute the file offset from the instruction (is this easy).
       //    3) Get Andreas' class in place.

          library_handle handle;
       // ANDREAS_CLASS::set_function_match(handle,t.data);

          printf ("Function %d: ",counter++);
          for (size_t i=0; i < s.size(); i++)
             {
               if (isprint(s[i]))
                    printf("%c",s[i]);
                 else
                    printf(".");
             }
          printf("\n");
        }
   }

void
LibraryIdentification::FlattenAST::visit(SgNode* n)
   {
     SgAsmInstruction* asmInstruction = isSgAsmInstruction(n);
     if (asmInstruction != NULL)
        {
          SgUnsignedCharList opCodeString = asmInstruction->get_raw_bytes();
          for (size_t i=0; i < opCodeString.size(); i++)
             {
               data.push_back(opCodeString[i]);
             }
        }
   }

