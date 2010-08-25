
#if ROSE_MPI
#include <mpi.h>
#endif

#ifdef PALETTE_USE_ROSE
#include "compass.h"
#else
#include "rose.h"
#include "compass.h"
#endif


//for exists
#include "boost/filesystem/operations.hpp"
//#include <rose.h>
#include <sstream>
#include <fstream>
#include <iostream> 


// Default setting for verbosity level (-1 is silent, and values greater then zero indicate different levels of verbosity)
int  Compass::verboseSetting   = 0;
bool Compass::compilerWarnings = false;
bool Compass::compilerRemarks  = false;

// DQ (1/17/2008): New Flymake mode
bool Compass::UseFlymake       = false;

//! Support for ToolGear XML viewer for output data when run as batch
bool Compass::UseToolGear      = false; 
std::string Compass::tguiXML;

//! Support for outputting to SQLite database when run as batch
bool Compass::UseDbOutput        = false; 
std::string Compass::outputDbName;
#ifdef HAVE_SQLITE3
sqlite3x::sqlite3_connection Compass::con;
#endif


// TPS, needed for DEFUSE
unsigned int Compass::global_arrsize=-1;
unsigned int Compass::global_arrsizeUse=-1;
unsigned int* Compass::def_values_global=NULL;
unsigned int* Compass::use_values_global=NULL;
bool Compass::quickSave=false;


/******************************************************************
 * MPI CODE TO RUN DEFUSE IN PARALLEL WITH MPI
 ******************************************************************/

#if ROSE_MPI
int Compass::my_rank=0;
int Compass::processes=0;


void Compass::loadDFA(std::string name, SgProject* project) {
  MPI_Status Stat;
  int res[2];
  res[0]=0;
  res[1]=1;
  bool done=false;
  global_arrsize=-2; global_arrsizeUse=-2;
  std::cerr << " Starting the load with " << processes << " processes. " << std::endl;
  while (!done) {

    Compass::sourceDefUsePrerequisite.load(project);
    ROSE_ASSERT(Compass::sourceDefUsePrerequisite.done==true);
    // The dataflow analysis must return at least one definition;
    // This test was added because the dataflow analysis was disabled.

    //    if (defuse==NULL) {
    //  defuse = new DefUseAnalysis(project);
    //  std::cerr << " creating defuse ... " <<std::endl;
    //}
    int nrOfSimultaniousProcs = 3;
    if (my_rank==0 && processes>1) {
      for (int turn=0; turn <nrOfSimultaniousProcs; ++turn) {
	for (int i=0; i<processes; i+=nrOfSimultaniousProcs) {
	  // processor 0 does communication
	  int proc=i+turn;
	  if (proc==0) continue;
	  if (proc<processes) {
	    std::cerr << my_rank << " ... server, waiting to receive signal -- proc = " << (proc) << "/" << turn << std::endl;
	    MPI_Recv(res, 2, MPI_INT, proc, 1, MPI_COMM_WORLD, &Stat);
	    res[0]=i;
	    res[1]=i+1;
	    std::cerr << my_rank << " ... server, sending signal " << (proc) <<  "/" << turn << std::endl;
	    MPI_Send(res, 2, MPI_INT, proc, 1, MPI_COMM_WORLD);      
	  }
	}
	for (int i=0; i<processes; i+=nrOfSimultaniousProcs) {
	  int proc=i+turn;
	  if (proc==0) continue;
	  if (proc<processes) {
	    std::cerr << my_rank << " ... server waiting for " << (proc) <<  "/" << turn << std::endl;
	    MPI_Recv(res, 2, MPI_INT, proc, 1, MPI_COMM_WORLD, &Stat);
	  }
	}
      } // turn
      std::cerr << my_rank << " ... server done " <<  std::endl;
      done=true;
    }
  
    if ((my_rank!=0 && processes>1) || (my_rank==0 && processes==1)) {
      if (processes==1) {
	done=true;
      } else {
	std::cerr << my_rank << " ... sending signal " << std::endl;
	MPI_Send(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
	std::cerr << my_rank << " ... awaiting signal " << std::endl;
	MPI_Recv(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &Stat);
	std::cerr << my_rank << " ... running " << std::endl;
      }

      std::ifstream loadFile(name.c_str(), std::ios::in | std::ios::binary);
      int sizeOfArrayItem = -1;

      global_arrsize=-1;
      global_arrsizeUse=-1;
      ROSE_ASSERT(global_arrsize!=0);
      std::cerr <<"\n " << my_rank << ":  Loading DFA from File " << name << std::endl;
      loadFile.read((char*)&global_arrsize, sizeof(unsigned int));
      loadFile.read((char*)&global_arrsizeUse, sizeof(unsigned int));
      loadFile.read((char*)&sizeOfArrayItem, sizeof(int));
      ROSE_ASSERT(sizeOfArrayItem==sizeof(int));
      std::cerr << " sizeof (def_values) : " << sizeOfArrayItem << 
	"   total bytes: " << (sizeOfArrayItem*global_arrsize) <<  "\n";
      def_values_global = new unsigned int[global_arrsize];
      use_values_global = new unsigned int[global_arrsizeUse];
      std::cerr << my_rank <<": loading data ...  globalArrSize: " << global_arrsize <<std::endl;
      loadFile.read((char*)&def_values_global[0], sizeOfArrayItem*global_arrsize);
      std::cerr << my_rank <<": loading data ...   globalArrSizeUse: " << global_arrsizeUse <<std::endl;
      loadFile.read((char*)&use_values_global[0], sizeOfArrayItem*global_arrsizeUse);
      loadFile.close();
      std::cerr <<" Done Loading DFA to File " << std::endl;
      std::cerr << " DefSize : " << global_arrsize << "  UseSize : " << global_arrsizeUse << std::endl;
      std::cerr << " Def[0] : " << def_values_global[0] << "  Use[0] : " << use_values_global[0] << std::endl;

      // create map with all nodes and indices
      MemoryTraversal* memTrav = new MemoryTraversal();
      memTrav->traverseMemoryPool();
      std::cerr << my_rank << " >> MemoryTraversal - Elements : " << memTrav->counter << std::endl;
      ROSE_ASSERT(memTrav->counter>0);
      ROSE_ASSERT(memTrav->counter==memTrav->nodeMap.size());
  

      /* deserialize all results */
      // write the global def_use_array back to the defmap (for each processor)
      deserializeDefUseResults(global_arrsize, (DefUseAnalysis*)Compass::sourceDefUsePrerequisite.getSourceDefUse(), def_values_global, memTrav->nodeMap, true);
      std::cerr << my_rank << " : deserialization of Defs done." << std::endl;
      delete[] def_values_global;
      def_values_global=NULL;
      deserializeDefUseResults(global_arrsizeUse, (DefUseAnalysis*)Compass::sourceDefUsePrerequisite.getSourceDefUse(), use_values_global, memTrav->nodeMap, false);
      std::cerr << my_rank << " : deserialization of Uses done." << std::endl;
      delete[] use_values_global;
      use_values_global=NULL;
      delete memTrav;
      /* deserialize all results */
      


      if (processes>1)
	MPI_Send(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
      done=true;
    }
  }

  
  if ((my_rank==0 && processes==1) || (my_rank==1 && processes>1)) {
    my_map defmap = Compass::sourceDefUsePrerequisite.getSourceDefUse()->getDefMap();
    my_map usemap = Compass::sourceDefUsePrerequisite.getSourceDefUse()->getUseMap();
    std::cerr <<  my_rank << ": Total number of def nodes: " << defmap.size() << std::endl;
    std::cerr <<  my_rank << ": Total number of use nodes: " << usemap.size() << std::endl << std::endl;
  }

}

void Compass::saveDFA(std::string name, SgProject* project) {
  //unsigned int arrsize, unsigned int *values) {
  std::ofstream writeFile(name.c_str(), std::ios::out | std::ios::binary);
  quickSave=true;
  global_arrsize=0; global_arrsizeUse=0;
  //runDefUseAnalysis(project);
  Compass::sourceDefUsePrerequisite.run(project);
  ROSE_ASSERT(Compass::sourceDefUsePrerequisite.done==true);
  quickSave=false;
  // The dataflow analysis must return at least one definition;
  // This test was added because the dataflow analysis was disabled.
  ROSE_ASSERT(global_arrsize!=0);
  MPI_Barrier(MPI_COMM_WORLD);

  if (my_rank==0) {
    std::cerr <<" Saving DFA to File : " << name << std::endl;
    ROSE_ASSERT(global_arrsize!=0);
    std::cerr << " DefSize : " << global_arrsize << "  UseSize : " << global_arrsizeUse << std::endl;
    writeFile.write(reinterpret_cast<const char *>(&global_arrsize), sizeof(unsigned int));
    writeFile.write(reinterpret_cast<const char *>(&global_arrsizeUse), sizeof(unsigned int));
    int sizeOfArrayItem = sizeof(*def_values_global);
    writeFile.write(reinterpret_cast<const char *>(&sizeOfArrayItem), sizeof(int));
    //    for (unsigned int j=0;j<global_arrsize;j++) {
      writeFile.write((char*)&def_values_global[0], sizeof(def_values_global[0])*global_arrsize);
      // }
      //    for (unsigned int j=0;j<global_arrsizeUse;j++) {
      writeFile.write((char*)&use_values_global[0], sizeof(use_values_global[0])*global_arrsizeUse);
      //}
    std::cerr << " sizeof (def_values) : " << (sizeof(def_values_global[0])*global_arrsize) << " : " 
	      << (sizeOfArrayItem*global_arrsize) << " bytes \n";
    writeFile.close();
    std::cerr <<" Done Saving DFA to File " << std::endl;

    if (global_arrsize>0 && use_values_global>0)
      std::cerr << " Def[0] : " << def_values_global[0] << "  Use[0] : " << use_values_global[0] << std::endl;
    else
      std::cerr << "Something might have gone wrong ... no defs or uses found!" << std::endl;
    // test the load after saving
    /*
      for (unsigned int j=0;j<global_arrsize;j++) 
      def_values_global[j]=0;
      global_arrsize=-1;
      for (unsigned int j=0;j<global_arrsizeUse;j++) 
      use_values_global[j]=0;
      global_arrsizeUse=-1;
      loadDFA(name,project);
    */
  }


}



void Compass::serializeDefUseResults(unsigned int *values,
                                     std::map< SgNode* , std::vector <std::pair < SgInitializedName* , SgNode* > > > &defmap,
				     std::map<SgNode*,unsigned int > &nodeMapInv) {
  int counter=0;
  std::map< SgNode* , std::vector <std::pair < SgInitializedName* , SgNode* > > >::const_iterator it;
  for (it=defmap.begin();it!=defmap.end();++it) {
    SgNode* first = it->first;
    ROSE_ASSERT(first);
    std::vector < std::pair < SgInitializedName* , SgNode*>  > mm = it->second;
    std::vector < std::pair < SgInitializedName* , SgNode*>  >::const_iterator it2;
    for (it2=mm.begin();it2!=mm.end();++it2) {
      SgInitializedName* second = isSgInitializedName(it2->first);
      SgNode* third = it2->second;
      ROSE_ASSERT(second);
      ROSE_ASSERT(third);
      values[counter]=nodeMapInv.find(first)->second;
      values[counter+1]=nodeMapInv.find(second)->second;
      values[counter+2]=nodeMapInv.find(third)->second;
      counter+=3;
    }
  }
}

void Compass::deserializeDefUseResults(unsigned int arrsize, DefUseAnalysis* defuse, unsigned int *values,
                                       std::vector<SgNode* > &nodeMap, 
				       bool definition) {
  struct timespec b_time_node, e_time_node;
  //  defuse->flushHelp();
  if (my_rank==0) {
    Compass::gettime(b_time_node);
  }


  for (unsigned int i=0; i <arrsize;i+=3) {
    unsigned int first = values[i];
    unsigned int second = values[i+1];
    unsigned int third = values[i+2];
    //        cerr << i << "/"<<arrsize<<"::  first : " << first << " second : " << second << " third : " << third << endl;
    SgNode* node1 = nodeMap[first];
    SgInitializedName* node2 = isSgInitializedName(nodeMap[second]);
    SgNode* node3 = nodeMap[third];
    ROSE_ASSERT(node1);
    ROSE_ASSERT(node2);
    ROSE_ASSERT(node3);

    if (definition)
      defuse->addDefElement(node1, node2, node3);
    else
      defuse->addUseElement(node1, node2, node3);
    if ((i%10000000)==0 ) {
      double memusage_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
      std::cerr << my_rank << ":  >> deserializing " << i << " of " << arrsize << 
	"   MemUsage: " << (memusage_e/1024) << " GB " << std::endl;
    }
  }

  if (my_rank==0) {
    Compass::gettime(e_time_node);
    double restime = Compass::timeDifference(e_time_node, b_time_node);
    double memusage_e = ROSE_MemoryUsage().getMemoryUsageMegabytes();
    std::cerr << " >>> >>> deserialization. TIME : " << restime << "   Memory " <<
      (memusage_e/1024) << " GB " << std::endl;
  }

}

// ************************************************************
// Time Measurement
// ************************************************************
double Compass::timeDifference(struct timespec end, struct timespec begin) {
  return (end.tv_sec + end.tv_nsec / 1.0e9)
    - (begin.tv_sec + begin.tv_nsec / 1.0e9);
}

void Compass::MemoryTraversal::visit ( SgNode* node )
{
  ROSE_ASSERT(node != NULL);
  //    nodeMap[counter]=node;
  nodeMap.push_back(node);
  nodeMapInv[node]=counter;
  counter++;
}

/******************************************************************/
#endif



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

 
  if( boost::filesystem::exists(filename) == false )  
  {
//    std::cerr << "Error: Please specify which checkers you want to run in " << filename << std::endl;
    std::cerr << "Error: Filename " << filename << " does not exists. " << std::endl;
    abort();

  }
  std::ifstream* streamPtr = new std::ifstream(filename.c_str());

  ROSE_ASSERT(streamPtr != NULL);
  ROSE_ASSERT(streamPtr->good());

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
  else 
  {
    std::cout<<"Warning: environment variable COMPASS_PARAMETERS is not set, trying to use compass_parameters of the current path"<<std::endl;
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
      if (getNode()) {
  //      std::cerr << "Node : " << getNode()->class_name() << std::endl;
        Sg_File_Info* start = getNode()->get_file_info();
      // tps : 22Jan 2009 - commented the following out because it does not work with binaries
      //ROSE_ASSERT(start != NULL);
        if (start)
	  sourceCodeLocation = Compass::formatStandardSourcePosition(start);
      }
    }

  // tps Jan 23 2009: getNode() can be NULL because it could be a binary node
  // added a test for this
  std::string nodeName = "unknown";
	if (getNode())
            getNode()->class_name();

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
      Compass::UseToolGear = true; 
    }

  if ( CommandlineProcessing::isOptionWithParameter( commandLineArray, std::string("--outputDb"), std::string("*"), outputDbName, remove ) )
    {
#ifdef HAVE_SQLITE3
      Compass::UseDbOutput = true;
      con.open(outputDbName.c_str());
#else
      std::cerr << "Compile ROSE with --with-sqlite3 to enable the --outputDb option " << std::endl;
      abort();
#endif
    }

  // Adding a new command line parameter (for mechanisms in ROSE that take command lines)

  // printf ("commandLineArray.size() = %zu \n",commandLineArray.size());
  // printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(commandLineArray).c_str());
}



void
Compass::outputTgui( std::string & tguiXML,
                     std::vector<const Compass::Checker*> & checkers,
		     Compass::OutputObject *output )
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

      for( std::vector<const Compass::Checker*>::const_iterator itr = 
             checkers.begin(); itr != checkers.end(); itr++ )
        {
	  std::string checkerName( (*itr)->checkerName );

	  xml << "  <message_folder>\n"
	      << "    <tag>" << checkerName << "</tag>\n"
	      << "    <title>" << checkerName << " Checker</title>\n"
	      << "    <if_empty>hide</if_empty>\n"
	      << "  </message_folder>\n";
	} //for, itr
    } //if( pos == 0 )

  const std::vector<Compass::OutputViolationBase*>& outputList = 
    output->getOutputList();

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

#ifdef HAVE_SQLITE3

#include "sqlite3x.h"
#include <boost/lexical_cast.hpp>

void
Compass::outputDb( std::string  dbName,
                   std::vector<const Compass::Checker*> & checkers,
                   Compass::OutputObject *output )
{
  using namespace std;
  sqlite3x::sqlite3_connection con(dbName.c_str());

  //con.executenonquery("create table IF NOT EXISTS clusters(row_number INTEGER PRIMARY KEY, cluster INTEGER, function_id INTEGER, index_within_function INTEGER, vectors_row INTEGER, dist INTEGER)")
  try {
    con.executenonquery("create table IF NOT EXISTS violations( row_number INTEGER PRIMARY KEY, checker_name TEXT,  error_body TEXT, filename TEXT, line INTEGER, short_description TEXT )");

  }
  catch(std::exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }


  const std::vector<Compass::OutputViolationBase*>& outputList = 
    output->getOutputList();


  string db_select_n = "INSERT INTO violations( checker_name,  error_body, filename, line, short_description ) VALUES(?,?,?,?,?)";

  for( std::vector<Compass::OutputViolationBase*>::const_iterator itr =
      outputList.begin(); itr != outputList.end(); itr++ )
  {
    const Sg_File_Info *info = (*itr)->getNode()->get_file_info();

 // DQ (1/24/2009): Added debugging code to understand where errors are happening...fails when (*itr)->getNode() is a SgProject (because a SgProject does not have a source file position)
    if (info == NULL)
       {
         ROSE_ASSERT( (*itr)->getNode() != NULL );
         printf ("This IR node's use of get_file_info() results in NULL pointer! (*itr)->getNode() = %p = %s \n",(*itr)->getNode(),(*itr)->getNode()->class_name().c_str());
       }

 // DQ (1/24/2009): Assertion added for debugging...
    ROSE_ASSERT(info != NULL);

    sqlite3x::sqlite3_command cmd(con, db_select_n.c_str());
    cmd.bind(1, (*itr)->getCheckerName() );
    cmd.bind(2, (*itr)->getString() );
    cmd.bind(3, info->get_filenameString() );
    cmd.bind(4, boost::lexical_cast<string>(info->get_line()));
    cmd.bind(5, (*itr)->getShortDescription());

    cmd.executenonquery();
  } //for, itr


  con.close();

  return;
} //outputTgui()

#endif


using namespace Compass;

#include "instantiate_prerequisites.h"


static void runPrereqList(const PrerequisiteList& ls, SgProject* proj) {
  for (size_t i = 0; i < ls.size(); ++i) {
    runPrereqs(ls[i], proj);
    std::string preStr = ls[i]->name;
    std::set<std::string>::const_iterator it=pre.find(preStr);
    if (it==pre.end()) {
      pre.insert(preStr);
      std::cerr << " Running Prerequisite " << preStr <<  std::endl;
    }
    ls[i]->run(proj);
  }

}


void Compass::runPrereqs(const Checker* checker, SgProject* proj) {
  runPrereqList(checker->prerequisites, proj);
}

void Compass::runPrereqs(Prerequisite* prereq, SgProject* proj) {
  runPrereqList(prereq->getPrerequisites(), proj);
}

void Compass::runCheckerAndPrereqs(const Checker* checker, SgProject* proj, Parameters params, OutputObject* output) {
  runPrereqs(checker, proj);
  checker->run(params, output);
}
