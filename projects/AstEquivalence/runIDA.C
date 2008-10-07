#include <rose.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include "string_functions.h"

using namespace std;

std::string
dotsToSlash(std::string )
{


};

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

     std::vector<char*> newArgs;
//     newArgs.push_back(strdup("-rose:read_executable_file_format_only"));

     for(int i =0; i < argc ; ++i)
       newArgs.push_back(strdup(argv[i]));
 
     SgProject* project = frontend( newArgs.size(), &newArgs[0] );

     ROSE_ASSERT (project != NULL);
	 std::cout << "A" << std::endl;

     //Files will only exist for executables
         for(int i =0; i < project->numberOfFiles(); ++i)
         {
           SgFile* file = (*project)[i];
           std::cout << "B" << std::endl;

           if( isSgBinaryFile(file) ){
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

     return 1;
   }
