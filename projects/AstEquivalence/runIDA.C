#include <rose.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include "string_functions.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

using namespace std;
using namespace boost::filesystem;
//using namespace boost;
using namespace Rose;

char *curl_easy_escape(const char *string, int inlength)
{
  size_t alloc = (inlength?(size_t)inlength:strlen(string))+1;
  char *ns;
  char *testing_ptr = NULL;
  unsigned char in; /* we need to treat the characters unsigned */
  size_t newlen = alloc;
  int strindex=0;
  size_t length;

  ns = (char*)malloc(alloc);
  if(!ns)
    return NULL;

  length = alloc-1;
  while(length--) {
    in = *string;

    /* Portable character check (remember EBCDIC). Do not use isalnum() because
       its behavior is altered by the current locale. */

    switch (in) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case 'a': case 'b': case 'c': case 'd': case 'e':
    case 'f': case 'g': case 'h': case 'i': case 'j':
    case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E':
    case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
      /* just copy this */
      ns[strindex++]=in;
      break;
    default:
      /* encode it */
      newlen += 2; /* the size grows with two, since this'll become a %XX */
      if(newlen > alloc) {
        alloc *= 2;
        testing_ptr = (char*)realloc(ns, alloc);
        if(!testing_ptr) {
          free( ns );
          return NULL;
        }
        else {
          ns = testing_ptr;
        }
      }

      snprintf(&ns[strindex], 4, "%%%02X", in);

      strindex+=3;
      break;
    }
    string++;
  }
  ns[strindex]=0; /* terminate it */
  return ns;
}


  string
escapeNewLineCharaters ( const string & X )
{
  int stringLength = X.length();

  char* curlEscaped = curl_easy_escape(X.c_str(),X.length());

  string returnString(curlEscaped);
  if(curlEscaped)
    free(curlEscaped);
  return returnString;
}

  
std::string
dotsToSlash(std::string )
{


};

void find_tsv_directories( const path & dir_path, std::list<string>& tsvDirectories )
{
//  std::cout << "looking in " << path.string();
  if ( !exists( dir_path ) ) return;

  directory_iterator end_itr; // default construction yields past-the-end
  for ( directory_iterator itr( dir_path );
      itr != end_itr;
      ++itr )
  {
    if ( is_directory(itr->status()) )
    {
      find_tsv_directories( itr->path(), tsvDirectories );
    }else
      tsvDirectories.push_back(itr->string());

  }

}

void rename_windows_directories( const path & dir_path )
{
//  std::cout << "looking in " << path.string();
  if ( !exists( dir_path ) ) return;

  directory_iterator end_itr; // default construction yields past-the-end
  for ( directory_iterator itr( dir_path );
      itr != end_itr;
      ++itr )
  {

    std::string currentDirElem;
    if ( is_directory(itr->status()) )
    {
      rename_windows_directories( itr->path() );
    }
    
    currentDirElem=itr->path().directory_string().substr(0,itr->path().directory_string().find_last_of("/"))+"/"+escapeNewLineCharaters(itr->path().leaf());



    if(currentDirElem != itr->path().string() )
    {
      path bef_path(*itr);
      path after_path(currentDirElem );
      rename(bef_path,after_path);

      std::cout << "Orignial string " <<   itr->path().string() << " changed to " << currentDirElem <<std::endl;

    }




  }

}


class DeleteAST : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

void
DeleteAST::visit(SgNode* node)
   {
     delete node;
   }

void DeleteSgTree( SgNode* root)
{
  DeleteAST deleteTree;
  deleteTree.traverse(root,postorder);
}



string
escapeNewLineCharatersBef ( const string & X )
{
  string returnString;
  int stringLength = X.length();

  for (int i=0; i < stringLength; i++)
  {
    if ( X[i] == ' ' )
    {
      returnString += "%20";
    }  else  if ( X[i] == '$' )
    returnString += "%24";
    else
      returnString += X[i];
  }

  return returnString;
}


int
main(int argc, char** argv)
   {
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!
	 std::string idaPath = IDA_PRO_PATH;
	 if(idaPath == "")
     {
	   std::cerr << "Error: It is required to configure ROSE with IDA Pro \n";
	   exit(1);
	 }

    std::list<string> tsvDirectories; 


    path dir_path(argv[1]); 
    
    find_tsv_directories(dir_path, tsvDirectories);


    std::cout << "We have " << tsvDirectories.size() << " files " << std::endl;


    for(std::list<string>::iterator iItr = tsvDirectories.begin();
        iItr != tsvDirectories.end(); ++iItr)
    {

      std::string filename = *iItr;
      std::cout << "now doing " << filename << std::endl;
      std::vector<char*> newArgs;
      newArgs.push_back(strdup("-rose:read_executable_file_format_only"));
      newArgs.push_back(strdup(filename.c_str()));


      //for(int i =0; i < argc ; ++i)
      //  newArgs.push_back(strdup(argv[i]));

      SgProject* project = frontend( newArgs.size(), &newArgs[0] );

      ROSE_ASSERT (project != NULL);
      std::cout << "A" << std::endl;

      //Files will only exist for executables
      for(int i =0; i < project->numberOfFiles(); ++i)
      {
        SgFile* file = (*project)[i];
        std::cout << "B" << std::endl;

        if( isSgBinaryComposite(file) ){
          std::string filename = file->getFileName(); 

          std::cout << "B " << filename << std::endl;
          boost::filesystem::path path(filename);
          path = boost::filesystem::system_complete(path);
          std::string directory      = path.directory_string();
          std::string filenameString = path.root_name();


          std::string s = filename;
          string s_filename = StringUtility::stripPathFromFileName(s);
          string s_path     = StringUtility::getPathFromFileName(s);

          string s_nosuffix = s_filename.substr(0,s_filename.find_first_of(".") );
          printf ("s = %s s_filename = %s \n",s.c_str(),s_filename.c_str());
          printf ("s = %s s_path     = %s \n",s.c_str(),s_path.c_str());
          printf ("s = %s s_nosuffix = %s \n",s.c_str(),s_nosuffix.c_str());

          {
            //Need to create .idb first since that is the only way
            //to enable IDA analysis of the binaries

            ostringstream ostr;
            ostr << idaPath+"/idal";
            ostr << " -B ";
            ostr << filename.c_str();
            ostr << " -o"+s_path+"/"+s_nosuffix+".idb";

            std::cout << ostr.str() << std::endl;
            system(ostr.str().c_str());
          }

          {
            //Create .sql


            ostringstream ostr;
            ostr << idaPath+"/idal ";
            ostr << " -OIDAPython:" + idaPath + "/ida2sql.py ";
            ostr << s_path+"/"+s_nosuffix+".idb";
            //ostr << " -o"+filename+".sql";
            std::cout << ostr.str() << std::endl;
            system(ostr.str().c_str());

          }

          {
            //create -tsv directory which is the supported format in ROSE
            ostringstream ostr;
            ostr << " mv " + s_path+"/"+s_nosuffix+ ".sql " + filename + ".sql; "; 
            ostr << " cd " + s_path+";";
            ostr << " tclsh ";
            ostr << ROSE_COMPILE_TREE_PATH;
            ostr << "/projects/AstEquivalence/split-into-tables.tcl " + s_filename + ".sql;";
            std::cout << ostr.str() << std::endl;
            system(ostr.str().c_str());


          }


        }


      }

      DeleteSgTree(project);
    }

     return 1;
   }
