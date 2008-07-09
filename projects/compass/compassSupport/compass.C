#include "compass.h"
#include <rose.h>
#include <sstream>
#include <fstream>

// Default setting for verbosity level (-1 is silent, and values greater then zero indicate different levels of verbosity)
int  Compass::verboseSetting   = 0;
bool Compass::compilerWarnings = false;
bool Compass::compilerRemarks  = false;

// DQ (1/17/2008): New Flymake mode
bool Compass::UseFlymake       = false;

 //! Support for ToolGear XML viewer for output data when run as batch
bool Compass::UseToolGear      = false; 
std::string Compass::tguiXML;

DefUseAnalysis* Compass::defuse = NULL;
void Compass::runDefUseAnalysis(SgProject* root) {

  if (defuse==NULL) {
    //#define DEFUSE
#if ROSE_MPI
  /* ---------------------------------------------------------- 
   * MPI code for DEFUSE
   * ----------------------------------------------------------*/
  // --------------------------------------------------------
  // (tps, 07/24/08): added support for dataflow analysis
  // this should run right before any other checker is executed.
  // Other checkers rely on this information.
	//#if 0
  MPI_Barrier(MPI_COMM_WORLD);
  gettime(begin_time_node);

  defuse = new DefUseAnalysis(root);
  Rose_STL_Container<SgNode *> funcs = 
    NodeQuery::querySubTree(root, V_SgFunctionDefinition);
  if (my_rank==0)
    std::cerr << ">>>>> running defuse analysis (parallel)...  functions: " << funcs.size() << std::endl;
  int resultDefUseNodes=0;
  // run the following in parallel
  for (int p=0; p<processes;++p) {
    int start = funcs.size()/processes*p;
    int end = funcs.size()/processes*(p+1);
    if (my_rank==p) {
      //    cerr << my_rank <<": start: "<<start<<"  end: " << end<<endl;
      for (int i=start; i< end; ++i) {
	//      for (Rose_STL_Container<SgNode *>::iterator i = 
	//     funcs.begin(); i != funcs.end(); i++) {
	SgFunctionDefinition* funcDef = isSgFunctionDefinition(funcs[i]);
	int nrNodes = ((DefUseAnalysis*)defuse)->start_traversal_of_one_function(funcDef);
	resultDefUseNodes+=nrNodes;
      }
    }
  }
  std::cerr << my_rank << ": DefUse Analysis complete. Nr of Nodes: " << resultDefUseNodes << std::endl;
  MPI_Barrier(MPI_COMM_WORLD);
  if (my_rank==0)
    std::cerr << "\n>> Collecting defuse results ... " << endl;

  typedef std::map< SgNode* , std::multimap < SgInitializedName* , SgNode* > > my_map; 

  ROSE_ASSERT(defuse);
  my_map defmap = defuse->getDefMap();
  my_map usemap = defuse->getUseMap();
  std::cerr << my_rank << ": Def entries: " << defmap.size() << "  Use entries : " << usemap.size() << std::endl;
  gettime(end_time_node);
  double my_time_node = timeDifference(end_time_node, begin_time_node);


  /* communicate times */
  double *times_defuse = new double[processes];
  MPI_Gather(&my_time_node, 1, MPI_DOUBLE, times_defuse, 1, MPI_DOUBLE, 0,
	     MPI_COMM_WORLD);
  double totaltime=0;
  for (int i=0;i<processes;++i)
    if (times_defuse[i]>totaltime)
      totaltime=times_defuse[i];

  if (my_rank==0) {
    cerr << "Time (max) needed for DefUse : " << totaltime << endl;
  }
  //((DefUseAnalysis*)defuse)->printDefMap();
  /* communicate times */


  /* communicate arraysizes */
  unsigned int arrsize = 0;
  unsigned int arrsizeUse = 0;
  my_map::const_iterator dit = defmap.begin();
  for (;dit!=defmap.end();++dit) {
    arrsize +=(dit->second).size()*3;
  }
  my_map::const_iterator dit2 = usemap.begin();
  for (;dit2!=usemap.end();++dit2) {
    arrsizeUse +=(dit2->second).size()*3;
  }
  cerr << my_rank << ": defmapsize : " << defmap.size() << "  usemapsize: " << usemap.size() 
       << ": defs : " << arrsize << "  uses: " << arrsizeUse << endl;
  // communicate total size to allocate global arrsize
  unsigned int global_arrsize = -1;
  unsigned int global_arrsizeUse = -1;
  MPI_Allreduce(&arrsize, &global_arrsize, 1, MPI_UNSIGNED, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&arrsizeUse, &global_arrsizeUse, 1, MPI_UNSIGNED, MPI_SUM, MPI_COMM_WORLD);
  /* communicate arraysizes */


  /* serialize all results */
  unsigned int *def_values_global = new unsigned int[global_arrsize];
  unsigned int *def_values =new unsigned int[arrsize];
  unsigned int *use_values_global = new unsigned int[global_arrsizeUse];
  unsigned int *use_values =new unsigned int[arrsizeUse];
  for (unsigned int i=0; i<arrsize;++i) 
    def_values[i]=0;
  for (unsigned int i=0; i<global_arrsize;++i) 
    def_values_global[i]=0;
  for (unsigned int i=0; i<arrsizeUse;++i) 
    use_values[i]=0;
  for (unsigned int i=0; i<global_arrsizeUse;++i) 
    use_values_global[i]=0;

  serializeDefUseResults(def_values, defmap, memTrav->nodeMapInv);
  serializeDefUseResults(use_values, usemap, memTrav->nodeMapInv);
  /* serialize all results */


  /* communicate all results */
  int* offset=new int[processes];
  int* length=new int[processes];
  int* global_length=new int [processes];
  int* offsetUse=new int[processes];
  int* lengthUse=new int[processes];
  int* global_lengthUse=new int [processes];
  MPI_Allgather(&arrsize, 1, MPI_INT, global_length, 1, MPI_INT, MPI_COMM_WORLD);
  MPI_Allgather(&arrsizeUse, 1, MPI_INT, global_lengthUse, 1, MPI_INT, MPI_COMM_WORLD);

  for (int j=0;j<processes;++j) {
    if (j==0) {
	offset[j]=0;
	offsetUse[j]=0;
    } else {
	offset[j]=offset[j - 1] + global_length[j-1];
	offsetUse[j]=offsetUse[j - 1] + global_lengthUse[j-1];
    }
    length[j]=global_length[j]; 
    lengthUse[j]=global_lengthUse[j]; 
  }
  cerr << my_rank << " : serialization done."  
       <<  "  waiting to gather...   arrsize: " << arrsize << "  offset : " << offset[my_rank] << " globalarrsize: " << global_arrsize<< endl;

  MPI_Allgatherv(def_values, arrsize, MPI_UNSIGNED, def_values_global, length, 
		 offset, MPI_UNSIGNED,  MPI_COMM_WORLD);
  MPI_Allgatherv(use_values, arrsizeUse, MPI_UNSIGNED, use_values_global, lengthUse, 
		 offsetUse, MPI_UNSIGNED,  MPI_COMM_WORLD);
  /* communicate all results */


  /* deserialize all results */
  // write the global def_use_array back to the defmap (for each processor)
  ((DefUseAnalysis*)defuse)->flushDefuse();
  deserializeDefUseResults(global_arrsize, (DefUseAnalysis*)defuse, def_values_global, memTrav->nodeMap, true);
  deserializeDefUseResults(global_arrsizeUse, (DefUseAnalysis*)defuse, use_values_global, memTrav->nodeMap, false);
  cerr << my_rank << " : deserialization done." << endl;
  /* deserialize all results */



  MPI_Barrier(MPI_COMM_WORLD);
  defmap = defuse->getDefMap();
  usemap = defuse->getUseMap();

  if (my_rank==0) {
    cerr <<  my_rank << ": Total number of def nodes: " << defmap.size() << endl;
    cerr <<  my_rank << ": Total number of use nodes: " << usemap.size() << endl << endl;
    //((DefUseAnalysis*)defuse)->printDefMap();
  }
  //#endif
#else
  std::cerr << ">>>>>> running defuse analysis in sequence. "  << std::endl;
        defuse = new DefUseAnalysis(root);
    ((DefUseAnalysis*)defuse)->run(false);
  
#endif

  }
}


//Andreas' function
std::string Compass::parseString(const std::string& str)
   {
     std::istringstream is(str);
     std::string i;
     is >> i;
     if (!is || !is.eof())
          throw Compass::ParseError(str, "string");

     return i;
   } // std::string Compass::parseString(const std::string& str) 

std::ifstream* Compass::openFile(std::string filename)
{
    std::ifstream* streamPtr = new std::ifstream(filename.c_str());
     if (streamPtr->good() == false)
        {
#if 0
          std::cerr << "Warning: Parameter file in current directory: " << filename
                    << " not found or cannot be opened (looking in user's home directory and then Compass source tree)"
                    << std::endl;
#endif
       // std::cerr << "--- The current directory is expected to have a compass parameter file.\n\n";

       // If not in the current directory, then search in the user's home directory
          std::string alternativeFile = "~/" + filename;
       // printf ("Compass parameters file not found in current directory: looking for it in user's home directory: alternativeFile = %s \n",alternativeFile.c_str());

          streamPtr->close();
          delete streamPtr;
          streamPtr = new std::ifstream(alternativeFile.c_str());

          if (streamPtr->good() == false)
             {
            // Look into the build tree's compass directory
               alternativeFile = ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR "/projects/compass/" + filename;
            // printf ("Compass parameters file not found in user's home directory: looking for it in the Compass source tree: alternativeFile = %s \n",alternativeFile.c_str());

               streamPtr->close();
               delete streamPtr;
               streamPtr = new std::ifstream(alternativeFile.c_str());

               if (streamPtr->good() == false)
                  {
                    std::cerr << "Error: Parameter file in Compass source directory: " << alternativeFile << " not found or cannot be opened" << std::endl;
                    exit(1);
                  }
             }
        }

     ROSE_ASSERT(streamPtr != NULL);

	 return streamPtr;

};

Compass::Parameters::Parameters(const std::string& filename) throw (Compass::ParseError)
   {
  // This function needs a set of default paths where the Compass parameter file can be found.
  // This list should be:
  //    1) The current directory
  //    2) The user's home directory
  //    3) Check the Compass source tree

	 std::ifstream* streamPtr = openFile(filename);

       std::string line;
     while (streamPtr != NULL && streamPtr->eof() == false)
        {
          std::getline(*streamPtr, line);

       // Ignore comments in the parameter file
          if (line.empty() || line[0] == '#')
               continue;

          std::string name, value;
          std::string::size_type pos = line.find('=');
          if (pos == std::string::npos || pos == 0)
          throw Compass::ParseError(line, "parameter assignment");
          name = line.substr(0, pos);

       // strip spaces off the end of the name string
          std::string::size_type spaces = name.find_last_not_of(' ');
          if (spaces != std::string::npos)
               name.resize(spaces + 1);
          value = line.substr(pos + 1);

          if (this->data.find(name) != this->data.end())
             {
               throw Compass::ParseError(name, "non-duplicate parameter name");
             }
          data[name] = value;
        }
   }

std::string Compass::Parameters::operator[](const std::string& name) const
				throw (Compass::ParameterNotFoundException) {
  std::map<std::string, std::string>::const_iterator i = this->data.find(name);
  if (i == this->data.end()) {
    throw Compass::ParameterNotFoundException(name);
  } else {
    return i->second;
  }
}

std::string Compass::findParameterFile() {
  std::string filename = "compass_parameters";
  if (getenv("COMPASS_PARAMETERS")) {
    filename = getenv("COMPASS_PARAMETERS");
  }
  return filename;
}

int Compass::parseInteger(const std::string& str) {
  std::istringstream is(str);
  int i;
  is >> i;
  if (!is || !is.eof()) throw Compass::ParseError(str, "integer");
  return i;
}

double Compass::parseDouble(const std::string& str) {
  std::istringstream is(str);
  double d;
  is >> d;
  if (!is || !is.eof()) throw Compass::ParseError(str, "double");
  return d;
}

bool Compass::parseBool(const std::string& str) {
  std::istringstream is(str);
  bool b;
  is >> b;
  if (!is || !is.eof()) throw Compass::ParseError(str, "bool");
  return b;
}

std::vector<int> Compass::parseIntegerList(const std::string& str) {
  std::istringstream is(str);
  std::vector<int> v;
  if (str.empty())
    return v;
  int i;
  is >> i;
  while (is && !is.eof()) {
    v.push_back(i);
    is >> i;
  }
  if (!is || !is.eof()) throw Compass::ParseError(str, "integer list");
  else
    v.push_back(i);
  return v;
}

std::vector<double> Compass::parseDoubleList(const std::string& str) {
  std::istringstream is(str);
  std::vector<double> v;
  if (str.empty())
    return v;
  double d;
  is >> d;
  while (is && !is.eof()) {
    v.push_back(d);
    is >> d;
  }
  if (!is || !is.eof()) throw Compass::ParseError(str, "double list");
  else
    v.push_back(d);
  return v;
}

std::string Compass::formatStandardSourcePosition(const Sg_File_Info* fi) {
  return Compass::formatStandardSourcePosition(fi->get_filenameString(),
                                               fi->get_line(),
                                               fi->get_col());
}

std::string Compass::formatStandardSourcePosition(const Sg_File_Info* start,
                                                  const Sg_File_Info* end) {
  return Compass::formatStandardSourcePosition(start->get_filenameString(),
                                               start->get_line(),
                                               start->get_col(),
                                               end->get_filenameString(),
                                               end->get_line(),
                                               end->get_col());
}

std::string Compass::formatStandardSourcePosition(const std::string& filename,
                                                  int line, int col) {
  std::ostringstream os;
  os << filename << ":" << line << "." << col;
  return os.str();
}

std::string Compass::formatStandardSourcePosition(const std::string& sfilename,
                                                  int sline, int scol,
                                                  const std::string& efilename,
                                                  int eline, int ecol) {
  std::ostringstream os;
  if (sfilename != efilename) {
    os << Compass::formatStandardSourcePosition(sfilename, sline, scol) <<
          "-" <<
          Compass::formatStandardSourcePosition(efilename, eline, ecol);
  } else if (sline != eline) {
    os << sfilename << ":" << sline << "." << scol << "-" <<
          eline << "." << ecol;
  } else if (scol != ecol) {
    os << sfilename << ":" << sline << "." << scol << "-" << ecol;
  } else {
    os << Compass::formatStandardSourcePosition(sfilename, sline, scol);
  }
  return os.str();
}


std::string
Compass::OutputViolationBase::getString() const
   {
     ROSE_ASSERT(getNodeArray().size() <= 1);

  // Default implementation for getString
     SgLocatedNode* locatedNode = isSgLocatedNode(getNode());
     std::string sourceCodeLocation;
     if (locatedNode != NULL)
        {
          Sg_File_Info* start = locatedNode->get_startOfConstruct();
          Sg_File_Info* end   = locatedNode->get_endOfConstruct();
          sourceCodeLocation = (end ? Compass::formatStandardSourcePosition(start, end) 
                                    : Compass::formatStandardSourcePosition(start));
       }
      else
       {
      // Else this could be a SgInitializedName or SgTemplateArgument (not yet moved to be a SgLocatedNode)
         Sg_File_Info* start = getNode()->get_file_info();
         ROSE_ASSERT(start != NULL);
         sourceCodeLocation = Compass::formatStandardSourcePosition(start);
       }

     std::string nodeName = getNode()->class_name();

  // The short description used here needs to be put into a separate function (can this be part of what is filled in by the script?)
  // return loc + ": " + nodeName + ": variable requiring static constructor initialization";

  // return m_checkerName + ": " + sourceCodeLocation + ": " + nodeName + ": " + m_shortDescription;
     return m_checkerName + ": " + sourceCodeLocation + ": " + m_shortDescription;
   }



// DQ (1/16/2008): Moved this implementation from the header file to the source file
void
Compass::PrintingOutputObject::addOutput(Compass::OutputViolationBase* theOutput)
   {
     ROSE_ASSERT(theOutput != NULL);
     SgNode* errorNode = theOutput->getNode();

     bool skipOutput = false;

  // printf ("In Compass::PrintingOutputObject::addOutput() errorNode = %s \n",errorNode->class_name().c_str());

     if (errorNode->get_startOfConstruct() != NULL)
        {
          const std::string & errorNodeFile          = errorNode->get_startOfConstruct()->get_filenameString();
          const std::string & errorNodeFile_path     = StringUtility::getPathFromFileName(errorNodeFile);
          const std::string & errorNodeFile_filename = StringUtility::stripPathFromFileName(errorNodeFile);

       // Make this static so that it need not be computed all the time!
          static SgProject* project = TransformationSupport::getProject(errorNode);
          ROSE_ASSERT(project != NULL);

          bool excludeErrorOutput = false;
          bool forceErrorOutput   = false;

          const SgStringList & includePathList = project->get_includePathList();
          const SgStringList & excludePathList = project->get_excludePathList();
          const SgStringList & includeFileList = project->get_includeFileList();
          const SgStringList & excludeFileList = project->get_excludeFileList();

       // printf ("project->get_includePathList = %zu project->get_excludePathList = %zu \n",project->get_includePathList().size(),project->get_excludePathList().size());
       // printf ("includePathList = %zu excludePathList = %zu \n",includePathList.size(),excludePathList.size());
       // printf ("includeFileList = %zu excludeFileList = %zu \n",includeFileList.size(),excludeFileList.size());

       // If this is a compiler generated IR node then skip the output of its position
       // We might want to have a special mode for this since it could be that the 
       // postion is still available in the raw data.
          if (errorNode->get_startOfConstruct()->isCompilerGenerated() == true)
             {
               excludeErrorOutput = true;
               forceErrorOutput   = false;
             }

       // Only permit output of error messages from IR nodes that live along this path
          SgStringList::const_iterator i = includePathList.begin();
          while (forceErrorOutput == false && i != includePathList.end())
             {
            // Don't let a false value for excludeErrorOutput and a false value for forceErrorOutput cause skipOutput to be false!
               excludeErrorOutput = true;

               forceErrorOutput = forceErrorOutput || (errorNodeFile_path.find(*i) != std::string::npos);
               i++;
             }

       // Exclude error messages from IR nodes located along paths where these are a substring
          SgStringList::const_iterator j = excludePathList.begin();
          while (excludeErrorOutput == false && j != excludePathList.end())
             {
               excludeErrorOutput = excludeErrorOutput || (errorNodeFile_path.find(*j) != std::string::npos);
               j++;
             }

       // Only permit output of error messages from IR nodes that live along this path
          SgStringList::const_iterator k = includeFileList.begin();
          while (forceErrorOutput == false && k != includeFileList.end())
             {
            // Don't let a false value for excludeErrorOutput and a false value for forceErrorOutput cause skipOutput to be false!
               excludeErrorOutput = true;

            // Strip off the path, since flymake will prepend stuff: e.g. "../../../../../../../../home/dquinlan/ROSE/NEW_ROSE/projects/compass/compassMain"
            // std::string k_filename = StringUtility::stripPathFromFileName(*k);

            // forceErrorOutput = forceErrorOutput || (errorNodeFile.find(*k) != std::string::npos);
            // forceErrorOutput = forceErrorOutput || (errorNodeFile_filename.find(k_filename) != std::string::npos);
               forceErrorOutput = forceErrorOutput || (errorNodeFile_filename.find(*k) != std::string::npos);
            // printf ("In Compass::PrintingOutputObject::addOutput(): errorNodeFile = %s k = %s forceErrorOutput = %s \n",errorNodeFile.c_str(),k->c_str(),forceErrorOutput ? "true" : "false");
            // printf ("In Compass::PrintingOutputObject::addOutput(): errorNodeFile_filename = %s k_filename = %s forceErrorOutput = %s \n",errorNodeFile_filename.c_str(),k_filename.c_str(),forceErrorOutput ? "true" : "false");
               k++;
             }

       // Exclude error messages from IR nodes from files with these names
          SgStringList::const_iterator l = excludeFileList.begin();
          while (excludeErrorOutput == false && l != excludeFileList.end())
             {
            // excludeErrorOutput = excludeErrorOutput || (errorNodeFile.find(*l) != std::string::npos);
               excludeErrorOutput = excludeErrorOutput || (errorNodeFile_filename.find(*l) != std::string::npos);
               l++;
             }

          skipOutput = (forceErrorOutput ? false : excludeErrorOutput);
       // printf ("skipOutput = %s forceErrorOutput = %s excludeErrorOutput = %s \n",skipOutput ? "true" : "false",forceErrorOutput ? "true" : "false",excludeErrorOutput ? "true" : "false");
        }

  // printf ("skipOutput = %s \n",skipOutput ? "true" : "false");
     if (skipOutput == false)
        {
          outputList.push_back(theOutput);
          stream << theOutput->getString() << std::endl;
        }
   }



void
Compass::commandLineProcessing(Rose_STL_Container<std::string> & commandLineArray)
   {
  // printf ("Preprocessor (before): argv = \n%s \n",StringUtility::listToString(commandLineArray).c_str());

  // Add option to force EDG warnings and errors to be put onto a single line. This helps
  // Flymake present the message in Emacs when using the connection of Compass to Emacs.
  // commandLineArray.push_back("--edg:remarks");
  // commandLineArray.push_back("--edg:brief_diagnostics");
     commandLineArray.push_back("--edg:no_wrap_diagnostics");
     commandLineArray.push_back("--edg:display_error_number");

#if 1
  // We need these to exclude the C++ header files that don't have ".h" suffix extensions.

  // Exclude reporting Compass errors from specific paths or header files
  // These have to be entered as two separate options
  // For more details on Flymake: /nfs/apps/emacs/22.1/share/emacs/22.1/lisp/progmodes/
     commandLineArray.push_back("-rose:excludePath");
     commandLineArray.push_back("/include/g++_HEADERS/");
     commandLineArray.push_back("-rose:excludePath");
     commandLineArray.push_back("/usr/include/");
     commandLineArray.push_back("-rose:excludePath");
     commandLineArray.push_back("/tests/CompileTest/");
#endif

  // commandLineArray.push_back("-rose:excludePath");
  // commandLineArray.push_back("/home/dquinlan/ROSE/NEW_ROSE/");

  // Skip header files (but only works on non-C++ standard header files with ".h"
     commandLineArray.push_back("-rose:excludeFile");
     commandLineArray.push_back(".h");

  // Add a test for a custom command line option
     if ( CommandlineProcessing::isOption(commandLineArray,"--compass:","(s|silent)",true) )
        {
       // printf ("Setting Compass silent mode to ON \n");
          Compass::verboseSetting = -1;
        }

     if ( CommandlineProcessing::isOption(commandLineArray,"--compass:","(warnings)",true) )
        {
       // Turn EDG warnings on
          Compass::compilerWarnings = true;
        }
       else
        {
       // Turn EDG warnings off
          Compass::compilerWarnings = false;
          commandLineArray.push_back("--edg:no_warnings");
        }

     if ( CommandlineProcessing::isOption(commandLineArray,"--compass:","(remarks)",true) )
        {
       // Turn EDG remarks on
          Compass::compilerRemarks = true;
          commandLineArray.push_back("--edg:remarks");
        }
       else
        {
       // Turn EDG remarks off
          Compass::compilerRemarks = false;
        }

     int integerOptionForVerboseMode = 0;
     if ( CommandlineProcessing::isOptionWithParameter(commandLineArray,"--compass:","(v|verbose)",integerOptionForVerboseMode,true) )
        {
          printf ("Setting Compass verbose mode to ON (set to %d) \n",integerOptionForVerboseMode);
          Compass::verboseSetting = integerOptionForVerboseMode;
        }

  // Flymake option
     if ( CommandlineProcessing::isOption(commandLineArray,"--compass:","(flymake)",true) )
        {
       // printf ("Setting Compass flymake mode to ON \n");
          Compass::UseFlymake = true;
        }

  // This is the ToolGear Option
     const bool remove = true;

  // std::vector<std::string> argvList = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);
  // if ( CommandlineProcessing::isOptionWithParameter( argvList, std::string("--tgui"), std::string("*"), tguiXML, remove ) )
     if ( CommandlineProcessing::isOptionWithParameter( commandLineArray, std::string("--tgui"), std::string("*"), tguiXML, remove ) )
        {
           UseToolGear = true; 
        }


  // Adding a new command line parameter (for mechanisms in ROSE that take command lines)

  // printf ("commandLineArray.size() = %zu \n",commandLineArray.size());
  // printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(commandLineArray).c_str());
   }



void
Compass::outputTgui( std::string & tguiXML,
                 std::vector<Compass::TraversalBase*> & checkers,
                 Compass::PrintingOutputObject & output )
{
// DQ (1/3/2008): This has to be read/write since we generate an output file for use with ToolGear.
  std::fstream xml( tguiXML.c_str(), std::ios::out|std::ios::app );

  if( xml.good() == false )
  {
    std::cerr << "Error: outputTgui()\n";
    exit(1);
  }

  long pos = xml.tellp();

  if( pos == 0 )
  {
    xml << "<tool_gear>\n"
        << "<format>1</format>\n"
        << "  <version>2.00</version>\n"
        << "  <tool_title>Compass Analysis Static View</tool_title>\n";

    for( std::vector<Compass::TraversalBase*>::const_iterator itr = 
         checkers.begin(); itr != checkers.end(); itr++ )
    {
      std::string checkerName( (*itr)->getName() );

      xml << "  <message_folder>\n"
          << "    <tag>" << checkerName << "</tag>\n"
          << "    <title>" << checkerName << " Checker</title>\n"
          << "    <if_empty>hide</if_empty>\n"
          << "  </message_folder>\n";
    } //for, itr
  } //if( pos == 0 )

  const std::vector<Compass::OutputViolationBase*>& outputList = 
    output.getOutputList();

  for( std::vector<Compass::OutputViolationBase*>::const_iterator itr =
         outputList.begin(); itr != outputList.end(); itr++ )
  {
    const Sg_File_Info *info = (*itr)->getNode()->get_file_info();

    xml << "  <message>\n"
        << "    <folder>" << (*itr)->getCheckerName() << "</folder>\n"
        << "    <heading>" << (*itr)->getCheckerName() << ": " << info->get_filenameString() << " : " << info->get_line() << "</heading>\n"
        << "    <body><![CDATA[" << (*itr)->getString() << "]]></body>\n"
        << "    <annot>\n"
        << "      <site>\n"
        << "        <file>" << info->get_filenameString() << "</file>\n"
        << "        <line>" << info->get_line() << "</line>\n"
        << "        <desc><![CDATA[" << (*itr)->getShortDescription() << "]]></desc>\n" 
        << "      </site>\n"
        << "    </annot>\n"
        << "  </message>\n";
  } //for, itr

  xml.close();

  return;
} //outputTgui()


