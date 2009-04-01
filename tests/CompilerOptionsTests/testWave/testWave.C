#include "rose.h"
#include <fstream>

using namespace std;
void
print_out_all_macros(std::ostream& outStream, SgProject* project){
     typedef std::map<std::string, std::vector<PreprocessingInfo*>* > attached_attr_type;
     typedef  std::vector<PreprocessingInfo*> rose_attr_list_type;

     for( std::map<std::string,ROSEAttributesList* > ::iterator  it_files = mapFilenameToAttributes.begin();
		     it_files != mapFilenameToAttributes.end();
		     ++it_files){

	  std::string filename = it_files->first;

	  if(SgProject::get_verbose() >= 1){
	       std::cout << "Filename to check " << filename << std::endl;
	  }

          //If the filename is in the exclude path list skip it
          SgStringList & excludePathList = project->get_excludePathList();

          bool excludePath = false;
          for( SgStringList::iterator iItr = excludePathList.begin(); 
              iItr != excludePathList.end(); iItr++)
            if( filename.find(*iItr) != std::string::npos )
            {
              excludePath = true;
              break;
            }

          if(excludePath == true) continue;
         
          filename = StringUtility::stripPathFromFileName(filename);
          if(filename.size()<1 || (filename.size()==1 && ( filename.substr(0,1) == ".") ))
              continue;

	  for (rose_attr_list_type::iterator it_1 =
			  it_files->second->getList().begin(); it_1 != it_files->second->getList().end();
			  ++it_1)
	     {

	       PreprocessingInfo *currentInfo = *it_1;
               ROSE_ASSERT(currentInfo != NULL);
               PreprocessingInfo::DirectiveType typeOfDirective =
		       currentInfo->getTypeOfDirective ();

               std::string macroDefFilename = currentInfo->getString();
               macroDefFilename = StringUtility::stripPathFromFileName(macroDefFilename);

          
	    // Here attach include directives to a separate graph

               switch(typeOfDirective)
               {
                  case PreprocessingInfo::CpreprocessorUnknownDeclaration:
                  case PreprocessingInfo::C_StyleComment:
                  case PreprocessingInfo::CplusplusStyleComment:
                  case PreprocessingInfo::FortranStyleComment:
                  case PreprocessingInfo::CpreprocessorBlankLine:
                  case PreprocessingInfo::CpreprocessorIncludeDeclaration:
                  case PreprocessingInfo::CpreprocessorIncludeNextDeclaration:
                  case PreprocessingInfo::CpreprocessorDefineDeclaration:
                  case PreprocessingInfo::CpreprocessorUndefDeclaration:
                  case PreprocessingInfo::CpreprocessorIfdefDeclaration:
                  case PreprocessingInfo::CpreprocessorIfndefDeclaration:
                  case PreprocessingInfo::CpreprocessorIfDeclaration:
                  case PreprocessingInfo::CpreprocessorDeadIfDeclaration:
                  case PreprocessingInfo::CpreprocessorElseDeclaration:
                  case PreprocessingInfo::CpreprocessorElifDeclaration:
                  case PreprocessingInfo::CpreprocessorEndifDeclaration:
                  case PreprocessingInfo::CpreprocessorLineDeclaration:
                  case PreprocessingInfo::CpreprocessorErrorDeclaration:
                  case PreprocessingInfo::CpreprocessorWarningDeclaration:
                  case PreprocessingInfo::CpreprocessorEmptyDeclaration:
                  case PreprocessingInfo::CSkippedToken:
                  case PreprocessingInfo::CMacroCall:
		  {
                    outStream << "PreprocessingInfo " << filename << " line " << currentInfo->getLineNumber()
                              << " col " << currentInfo->getColumnNumber() << " : " << currentInfo->getString();
                    if(typeOfDirective == PreprocessingInfo::CMacroCall)
                              outStream << " expanded: " << currentInfo->get_macro_call()->get_expanded_string();
                    outStream << std::endl;
                    break;
                  }
                  default:
                  break;
               }

             }
     }




};

int main( int argc, char * argv[] ) 
   {

     std::vector<std::string> newArgv(argv,argv+argc);
     newArgv.insert(newArgv.begin()++,"-rose:wave");

     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/include/g++_HEADERS/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/include/gcc_HEADERS/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/include-staging/g++_HEADERS/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/include-staging/gcc_HEADERS/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/usr/include/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("/tests/CompileTest/");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("<builtin>");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("<built-in>");
     newArgv.push_back("-rose:excludePath");
     newArgv.push_back("<builltin>");


  // Build the AST used by ROSE
     SgProject* project = frontend(newArgv);
     ROSE_ASSERT(project != NULL);


  // Run internal consistency tests on AST
     AstTests::runAllTests(project);


     // Assume that there is only one file
     std::string filename;
     for(int i = 0 ; i < project->numberOfFiles(); i++)
     {
       SgSourceFile* file = isSgSourceFile(&project->get_file(i));
       if( file != NULL)
         filename = file->getFileName();

     };

     ROSE_ASSERT(filename != "");

     filename+=".out";
     filename = StringUtility::stripPathFromFileName(filename);


     //Output the macro diretives to the file
     ofstream myfile;
     myfile.open (filename.c_str());

     ROSE_ASSERT(myfile.is_open());
     std::cout << "Outputing to the file " << filename << std::endl;



     backend(project);
     print_out_all_macros(myfile, project);
  // Insert your own manipulation of the AST here...

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }

