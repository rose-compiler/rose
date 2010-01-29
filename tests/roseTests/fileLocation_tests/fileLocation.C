// This code tests the classification of files.
// Files are classified at either unknown, user, 
// or system (system includes system libaries).

// The support for this work is in ROSE/src/util/stringSupport/FileNameClassifier.C


#include "rose.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#include <libgen.h>             /* basename(), dirame()               */

using namespace std;
using namespace StringUtility;

bool
isLink( const string & name )
   {
  // In oorder to evaluate if this is a link we can't just check the file directly, 
  // since the file might be part of a directory that is linked.  So we have to
  // have to check each part of the whole absolute path to see if a link was used.
  // This detail make this function more complicated.

     struct stat info;

     string fileNameWithPath = name;
     char c_version[PATH_MAX];
     ROSE_ASSERT (fileNameWithPath.size() + 1 < PATH_MAX);

     while (fileNameWithPath != "/" && fileNameWithPath != ".")
        {
          strcpy(c_version, fileNameWithPath.c_str());

       // string directoryName = dirname(name.c_str());
          string directoryName = dirname(c_version);

       // printf ("directoryName = %s \n",directoryName.c_str());

          fileNameWithPath = directoryName;
        }


#if 0
     char fn[]="temp.file";
     char ln[]="temp.link";
     int file_descriptor;

     if ((file_descriptor = creat(fn, S_IWUSR)) < 0)
          perror("creat() error");
       else
        {
          close(file_descriptor);
          if (link(fn, ln) != 0)
               perror("link() error");
            else
             {
               if (lstat(ln, &info) != 0)
                    perror("lstat() error");
                 else
                  {
                    puts("lstat() returned:");
                    printf("  inode:   %d\n",   (int) info.st_ino);
                    printf(" dev id:   %d\n",   (int) info.st_dev);
                    printf("   mode:   %08x\n",       info.st_mode);
                    printf("  links:   %d\n",         info.st_nlink);
                    printf("    uid:   %d\n",   (int) info.st_uid);
                    printf("    gid:   %d\n",   (int) info.st_gid);
                  }
               unlink(ln);
             }
          unlink(fn);
        }
#else
     printf ("testing for link: nake = %s \n",name.c_str());

     if (lstat(name.c_str(), &info) != 0)
        {
          perror("lstat() error");
        }
       else
        {
          puts("lstat() returned:");
          printf("  inode:   %d\n",   (int) info.st_ino);
          printf(" dev id:   %d\n",   (int) info.st_dev);
          printf("   mode:   %08x\n",       info.st_mode);
          printf("  links:   %zu\n",        info.st_nlink);
          printf("    uid:   %d\n",   (int) info.st_uid);
          printf("    gid:   %d\n",   (int) info.st_gid);
        }
#endif

  // The minimum is to have a single link for a normal file.
     bool isALink = info.st_nlink > 1;

     return isALink;
   }


bool
islinkOrPartOfLinkedDirectory( const string & fileName )
   {
  // In oorder to evaluate if this is a link we can't just check the file directly, 
  // since the file might be part of a directory that is linked.  So we have to
  // have to check each part of the whole absolute path to see if a link was used.
  // This detail make this function more complicated.

     struct stat info;

     string fileNameWithPath = fileName;
     char c_version[PATH_MAX];
     ROSE_ASSERT (fileNameWithPath.size() + 1 < PATH_MAX);

     bool directoryIsLink = false;
     bool fileIsLink = false;

     while (fileNameWithPath != "/" && directoryIsLink == false)
        {
          strcpy(c_version, fileNameWithPath.c_str());

       // I think that the interface to dirname() requires a char array.
       // string directoryName = dirname(name.c_str());
          string directoryName = dirname(c_version);

          printf ("directoryName = %s \n",directoryName.c_str());

          fileNameWithPath = directoryName;

          directoryIsLink = isLink(directoryName);
        }

     if (directoryIsLink == false)
        {
          fileIsLink = isLink(fileName);
        }
       else
        {
       // If the directory is a link then the file is indirectly a linked file (for our purposes in filename classification)
          fileIsLink = true;
        }

     return fileIsLink;

  // The minimum is to have a single link for a normal file.
     bool isALink = info.st_nlink > 1;

     return isALink;
   }



class visitorTraversal : public AstSimpleProcessing
   {
     string previousFilename;

     public:
          virtual void visit(SgNode* n);
   };

#if 0
           enum FileNameLocation { FILENAME_LOCATION_UNKNOWN, 
                                   FILENAME_LOCATION_USER,    
                                   FILENAME_LOCATION_LIBRARY };
           
           /* Files can be classified as being part of one of these
            * libraries: Unknown, it isn't a library - it's part of
            * the user application, or any of the libraries that the
            * enum values imply, this list will likely be added to
            * over time */
           enum FileNameLibrary { FILENAME_LIBRARY_UNKNOWN,
                                  FILENAME_LIBRARY_USER,
                                  FILENAME_LIBRARY_C,
                                  FILENAME_LIBRARY_STDCXX,
                                  FILENAME_LIBRARY_LINUX,
                                  FILENAME_LIBRARY_GCC,
                                  FILENAME_LIBRARY_BOOST,
                                  FILENAME_LIBRARY_ROSE };
#endif

void
display ( const StringUtility::FileNameLocation & X, const string & label = "" )
   {
     printf ("In display(FileNameLocation): label = %s \n",label.c_str());
     string classification = "";
     switch (X)
        {
          case FILENAME_LOCATION_UNKNOWN: classification = "unknown"; break;
          case FILENAME_LOCATION_USER: classification    = "user";    break;
          case FILENAME_LOCATION_LIBRARY: classification = "library"; break;

          default:
             {
               printf ("Error: undefined classification X = %d \n",X);
               ROSE_ASSERT(false);
             }
        }

     printf ("file type classification = %s \n",classification.c_str());

  // return classification;
   }

void
display ( const StringUtility::FileNameLibrary & X, const string & label = "" )
   {
     printf ("In display(FileNameLibrary): label = %s \n",label.c_str());
     string classification = "";
     switch (X)
        {
          case FILENAME_LIBRARY_UNKNOWN: classification = "unknown"; break;
          case FILENAME_LIBRARY_USER:    classification    = "user";    break;
          case FILENAME_LIBRARY_C:       classification = "library"; break;
          case FILENAME_LIBRARY_STDCXX:       classification = "library STDCXX"; break;
          case FILENAME_LIBRARY_LINUX:       classification = "library LINUX"; break;
          case FILENAME_LIBRARY_GCC:       classification = "library GCC"; break;
          case FILENAME_LIBRARY_BOOST:       classification = "library BOOST"; break;
          case FILENAME_LIBRARY_ROSE:       classification = "library ROSE"; break;

          default:
             {
               printf ("Error: undefined library classification X = %d \n",X);
               ROSE_ASSERT(false);
             }
        }

     printf ("library classification = %s \n",classification.c_str());

  // return classification;
   }



void 
visitorTraversal::visit(SgNode* n)
   {
     SgStatement* statement = isSgStatement(n);
     if (statement != NULL)
        {
          string filename = statement->get_file_info()->get_filename();

       // Skip the case of compiler generated Sg_File_Info objects.
          if (previousFilename != filename && filename != "compilerGenerated")
             {
               printf ("\n\nfilename = %s statement = %s \n",filename.c_str(),n->class_name().c_str());

               FileNameClassification classification;
#if 1
            // string sourceDir = "/home/dquinlan/ROSE/roseCompileTree-g++4.2.2/developersScratchSpace/Dan/fileLocation_tests";

            // This causes the path edit distance to be: 4
               //string sourceDir = "/home/dquinlan/ROSE/svn-rose";
				string sourceDir = "/home/hou1/rose/rose";

            // This causes the path edit distance to be: 0
            // string sourceDir = "/home/dquinlan/ROSE";

               classification = classifyFileName(filename,sourceDir);
#else
               string home = "/home/dquinlan/";
               string sourceDir = home + "ROSE/svn-rose/";
               classification = classifyFileName("/usr/include/stdio.h",sourceDir);
#endif

               FileNameLocation fileTypeClassification = classification.getLocation();
               FileNameLibrary  libraryClassification  = classification.getLibrary();
               int pathEditDistance = classification.getDistanceFromSourceDirectory();
               
               printf ("fileTypeClassification = %d \n",fileTypeClassification);
               display(fileTypeClassification,"Display fileTypeClassification");
               printf ("libraryClassification  = %d \n",libraryClassification);
               display(libraryClassification,"Display libraryClassification");
               printf ("pathEditDistance       = %d \n",pathEditDistance);

               ROSE_ASSERT(isLink(filename) == false);
             }

          previousFilename = filename;
        }
   }

int
main(int argc, char * argv[])
   {
     SgProject *project = frontend (argc, argv);

     visitorTraversal myvisitor;
     myvisitor.traverse(project,preorder);

     return backend(project);
   }


