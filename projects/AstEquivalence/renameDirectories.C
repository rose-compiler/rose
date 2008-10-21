#include <rose.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include "string_functions.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

using namespace std;
using namespace boost::filesystem;

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

    path dir_path(argv[1]);
//      newArgs.push_back(strdup("-rose:read_executable_file_format_only"));

    rename_windows_directories(dir_path);
    return 1;
   }
