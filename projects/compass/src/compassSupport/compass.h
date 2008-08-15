#ifndef ROSE_COMPASS_H
#define ROSE_COMPASS_H
#if ROSE_MPI
#include <mpi.h>
#endif

#include <rose.h>
#include "DefUseAnalysis.h"
#include "DefUseAnalysis_perFunction.h"
#include <boost/function.hpp>
#include <boost/any.hpp>

#if ROSE_MPI
#include "functionLevelTraversal.h"
#include "functionNames.h"
#endif


namespace Compass {
  extern unsigned int global_arrsize;
  extern unsigned int global_arrsizeUse;
  extern unsigned int *def_values_global;
  extern unsigned int *use_values_global;
  typedef std::map< SgNode* , std::vector < std::pair <SgInitializedName* , SgNode*>  > > my_map; 
  extern bool quickSave;
  // Support for DefUse Analysis
  void runDefUseAnalysis(SgProject* root);

#if ROSE_MPI
  //  struct timespec begin_time_node, end_time_node;
  extern int my_rank, processes;

  void loadDFA(std::string name, SgProject* project);
  void saveDFA(std::string name, SgProject* project);

  /****************************************************************************/
  // (tps - 11Jul08) : needed for MPI run of Compass
  void serializeDefUseResults(unsigned int *values,
			      std::map< SgNode* , std::vector <std::pair  < SgInitializedName* , SgNode* > > > &defmap,
			      std::map<SgNode*,unsigned int > &nodeMap);
  void deserializeDefUseResults(unsigned int arrsize, DefUseAnalysis* defuse, unsigned int *values,
				//std::map<unsigned int, SgNode* > &nodeMap, 
				std::vector<SgNode* > &nodeMap, 
				bool definition);

  double timeDifference(struct timespec end, struct timespec begin);
  inline void gettime(struct timespec &t) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
  }
  class MemoryTraversal : public ROSE_VisitTraversal {
  public:
  MemoryTraversal():counter(0){}
    unsigned int counter;
    void visit ( SgNode* node );
    //std::map <unsigned int, SgNode* > nodeMap;
    std::vector <SgNode* > nodeMap;
    std::map <SgNode*, unsigned int > nodeMapInv;
  };
  
#endif
  /****************************************************************************/


  //! Silent setting is -1, default is 0, and values greater than zero indicate different levels of detail to be output.
  extern int verboseSetting;

  //! Compiler warnings are included in Compass messages by default
  extern bool compilerWarnings;

  //! Compiler remarks (more detailed warnings about style) are included in Compass messages by default
  extern bool compilerRemarks;

  //! Flymake mode for use of Compass with Emacs
  extern bool UseFlymake;

  //! Support for ToolGear XML viewer for output data when run as batch
  extern bool UseToolGear;
  extern std::string tguiXML;

     
  void commandLineProcessing(Rose_STL_Container<std::string> & commandLineArray);

  ///Andreas' function
  std::string parseString(const std::string& str);

  std::ifstream* openFile(std::string filename);

  /// The ParameterNotFoundException class contains information about an input
  /// parameter required by a checker which is not found in the parameter file
  class ParameterNotFoundException: public std::exception {
    const char* errorMessage;

  public:
  ParameterNotFoundException(const std::string& parameterName)
    : errorMessage(strdup(("Parameter not found: " + parameterName).c_str()))
      {}

    virtual const char* what() const throw () {
      return errorMessage;
    }
  };

  /// The ParseError class contains information about the failure to parse
  /// information from an input parameter file or parameter value
  class ParseError: public std::exception {
    const char* errorMessage;

  public:
  ParseError(const std::string& badString, const std::string& wanted)
    : errorMessage(strdup(("Error parsing " + wanted + " from string '" +
			   badString + "'").c_str()))
      {}

    virtual const char* what() const throw () {
      return errorMessage;
    }
  };

  /// The Parameters class contains the result of reading a parameter file.
  /// It is a list of key-value pairs; both the key and value are strings.
  /// The parameter file is a sequence of lines, with the following formats:
  /// - Blank line: ignored
  /// - Line beginning with #: ignored
  /// - key=value (note no spaces): text before = is key, rest of line is value
  class Parameters {
    std::map<std::string, std::string> data;

  public:
    /// Read parameters from the given file
    Parameters(const std::string& filename)
      throw (ParseError);
    /// Get the value of a parameter
    std::string operator[](const std::string& name) const
      throw (ParameterNotFoundException);
  };

  std::string findParameterFile();

  /// Parse an integer from the given string
  int parseInteger(const std::string& str);
  /// Parse a double from the given string
  double parseDouble(const std::string& str);
  /// Parse a bool from the given string
  bool parseBool(const std::string& str);
  /// Parse a white-space separated list of integers from the given string
  std::vector<int> parseIntegerList(const std::string& str);
  /// Parse a white-space separated list of doubles from the given string
  std::vector<double> parseDoubleList(const std::string& str);

  class OutputViolationBase;

  ///
  /// Base class for objects which can be used to output error messages from
  /// checkers
  ///
  class OutputObject
  {
  protected:
    std::vector<OutputViolationBase*> outputList;

  public:
    //! Emit one error message
    virtual void addOutput(OutputViolationBase* theOutput) = 0;

    // virtual SgNode* getNode() const = 0;
    std::vector<OutputViolationBase*> getOutputList() { return outputList; }
    void clear() { outputList.erase(outputList.begin(),outputList.end()); }

    virtual ~OutputObject() {}
  };

  /// Format the information from fi using the GNU standard format given at http://www.gnu.org/prep/standards/html_node/Errors.html
  std::string formatStandardSourcePosition(const Sg_File_Info* fi);
  /// Format the information from (start, end) using the GNU standard format given at http://www.gnu.org/prep/standards/html_node/Errors.html
  std::string formatStandardSourcePosition(const Sg_File_Info* start,
                                           const Sg_File_Info* end);
  /// Format the information from the parameters using the GNU standard format given at http://www.gnu.org/prep/standards/html_node/Errors.html
  std::string formatStandardSourcePosition(const std::string& filename,
                                           int line, int col);
  /// Format the information from the parameters using the GNU standard format given at http://www.gnu.org/prep/standards/html_node/Errors.html
  std::string formatStandardSourcePosition(const std::string& startFilename,
                                           int startLine, int startCol,
                                           const std::string& endFilename,
                                           int endLine, int endCol);

  /// The possible languages that a Compass checker can support.  The enum
  /// values are powers of 2 and can be ORed together to represent sets.
  enum {
    C = (1 << 0),
    Cpp = (1 << 1),
    Fortran = (1 << 2),
    PHP = (1 << 3),
    X86Assembly = (1 << 4),
    ArmAssembly = (1 << 5)
  };
  typedef int LanguageSet; // To allow operator|() on values

  class Prerequisite;

  /// A list of prerequisite pointers.
  typedef std::vector<Prerequisite*> PrerequisiteList;

  /// A prerequisite description for an analysis used by checkers, such as
  /// def-use.  Actual prerequisites are global variables that are instances of
  /// classes that inherit from this one.  Each such class has its own run()
  /// method and way to get its result.  The assumption is that the
  /// prerequisites are global variables and are only run once in a session
  /// (i.e., not on different projects or with different parameter files).
  class Prerequisite {
  public:
    std::string name;
    mutable bool done;
  Prerequisite(const std::string& name): name(name), done(false) {}
    virtual PrerequisiteList getPrerequisites() const = 0;
    virtual void run(SgProject* proj) = 0;
    virtual ~Prerequisite() {}
  };

  /// The basic class with metadata about a checker.  Most checkers will be
  /// instances (not subclasses) of this class.  Some subclasses are for
  /// checkers that have extra information, such as the ability to be
  /// combined with other instances of AstSimpleProcessing.
  class Checker {
  public:
    typedef boost::function<void /*run*/(Parameters, OutputObject*)> RunFunction;
    std::string checkerName;
    std::string shortDescription;
    std::string longDescription;
    LanguageSet supportedLanguages;
    PrerequisiteList prerequisites;
    RunFunction run;
    virtual ~Checker() {} // Allow RTTI

  Checker(std::string checkerName, std::string shortDescription, std::string longDescription, LanguageSet supportedLanguages, const PrerequisiteList& prerequisites, RunFunction run):
    checkerName(checkerName), shortDescription(shortDescription), longDescription(longDescription), supportedLanguages(supportedLanguages), prerequisites(prerequisites), run(run) {}
  };

  // ---- AST ---------------------------------------------------------------------------------------------------

  class AstSimpleProcessingWithRunFunction : public AstSimpleProcessing {
  public:
    const Checker* checker;
    AstSimpleProcessingWithRunFunction():checker(NULL) {}
    AstSimpleProcessingWithRunFunction(Checker* mychecker):checker(mychecker) {}
    virtual ~AstSimpleProcessingWithRunFunction() {}
    virtual void run(SgNode*)=0;
    virtual void visit(SgNode* n)=0;
  };


  /// A checker that supports combining with other instances of
  /// AstSimpleProcessing
  class CheckerUsingAstSimpleProcessing: public Checker {
  public:
    typedef boost::function<AstSimpleProcessingWithRunFunction* /*createSimpleTraversal*/(Parameters, OutputObject*)> SimpleTraversalCreationFunction;
    SimpleTraversalCreationFunction createSimpleTraversal;

  CheckerUsingAstSimpleProcessing(std::string checkerName, std::string shortDescription, std::string longDescription, LanguageSet supportedLanguages, const PrerequisiteList& prerequisites, RunFunction run, SimpleTraversalCreationFunction createSimpleTraversal):
    Checker(checkerName, shortDescription, longDescription, supportedLanguages, prerequisites, run), createSimpleTraversal(createSimpleTraversal) {}
  };

  // ---- AST ---------------------------------------------------------------------------------------------------



  /// The base class for outputs from a checker
  class OutputViolationBase
  {
  public:
    /// Get the name of the checker which produced this error

    // Constructor for use with a single IR node.
  OutputViolationBase(SgNode* node, const std::string & checker = "default value", const std::string & description = "default value")
    : m_checkerName(checker), m_shortDescription(description) { nodeArray.push_back(node); }

    // Constructor for use with multiple IR nodes.
  OutputViolationBase(std::vector<SgNode*> & inputNodeArray, const std::string & checker = "default value", const std::string & description = "default value")
    : m_checkerName(checker), m_shortDescription(description) { nodeArray = inputNodeArray; }

    virtual ~OutputViolationBase() {}

    virtual std::string getString() const;
 
    // Access function
    SgNode* getNode() const { return nodeArray[0]; }
    SgNode* operator[]( int i ) const { return nodeArray[i]; }

    const std::vector<SgNode*> & getNodeArray() const { return nodeArray; }

    std::string getCheckerName() { return m_checkerName; }
    std::string getShortDescription() { return m_shortDescription; }

  protected:
    // Store the locations in the AST where the violation occured
    std::vector<SgNode*> nodeArray;

    // Store the name of the checker (or maybe a reference to the checker), 
    // not sure we need this unless it is because parallel evaluation could 
    // make the simple text output confusing.
    std::string m_checkerName;

    // Additional information about the violation (optional)
    std::string m_shortDescription;
  };

  /// A simple output object which just prints each error message
  class PrintingOutputObject: public OutputObject
  {
  private:
    std::ostream& stream;

  public:
  PrintingOutputObject(std::ostream& stream): stream(stream) {}

#if 0
    // DQ (1/16/2008): Moved this implementation from the header file to the source file
    virtual void addOutput(OutputViolationBase* theOutput)
    {
      outputList.push_back(theOutput);
      stream << theOutput->getString() << std::endl;
    }
#else
    virtual void addOutput(OutputViolationBase* theOutput);
#endif
  };

  // ToolGear Support
  void outputTgui( std::string & tguiXML, std::vector<const Compass::Checker*> & checkers, PrintingOutputObject & output );

  // The prerequisite for getting the SgProject
  class ProjectPrerequisite: public Prerequisite {
    SgProject* proj;
  public:
  ProjectPrerequisite(): Prerequisite("SgProject"), proj(NULL) {}
    void run(SgProject* p) {
      if (done) return;
      proj = p;
      done = true;
    }

    PrerequisiteList getPrerequisites() const {
      return PrerequisiteList();
    }

    SgProject* getProject() const {
      ROSE_ASSERT (done);
      return proj;
    }
  };

  extern ProjectPrerequisite projectPrerequisite;

  // The prerequisite for getting the SgProject
  class SourceDefUsePrerequisite: public Prerequisite {
    DefUseAnalysis* defuse;
  public:
  SourceDefUsePrerequisite(): Prerequisite("SourceDefUse"), defuse(NULL) {}

    void load(SgProject* root) {
      done=true;
      defuse = new DefUseAnalysis(root);
    }


    void run(SgProject* root) {
      if (done) return;


  if (defuse==NULL) {
    //#define DEFUSE
    std::cerr << " DEFUSE ==NULL ... running defuse analysis " << std::endl;
#if ROSE_MPI
    /* ---------------------------------------------------------- 
     * MPI code for DEFUSE
     * ----------------------------------------------------------*/
    // --------------------------------------------------------
    // (tps, 07/24/08): added support for dataflow analysis
    // this should run right before any other checker is executed.
    // Other checkers rely on this information.
    struct timespec begin_time_node, end_time_node;
    struct timespec b_time_node, e_time_node;

    // create map with all nodes and indices
    MemoryTraversal* memTrav = new MemoryTraversal();
    memTrav->traverseMemoryPool();
    std::cerr << my_rank << " >> MemoryTraversal - Elements : " << memTrav->counter << std::endl;
    ROSE_ASSERT(memTrav->counter>0);
    ROSE_ASSERT(memTrav->counter==memTrav->nodeMap.size());

    MPI_Barrier(MPI_COMM_WORLD);
    gettime(begin_time_node);

    if (defuse==NULL)
      defuse = new DefUseAnalysis(root);

    //defuse->disableVisualization();
    Rose_STL_Container<SgNode *> funcs = 
      NodeQuery::querySubTree(root, V_SgFunctionDefinition);
    if (my_rank==0)
      std::cerr << "\n>>>>> running defuse analysis (with MPI)...  functions: " << funcs.size() << 
	"  processes : " << processes << std::endl;
    int resultDefUseNodes=0;


    // ---------------- LOAD BALANCING of DEFUSE -------------------
    // todo: try to make the load balancing for DEFUSE better
    FunctionNamesPreTraversal preTraversal;
    MyAnalysis myanalysis;
    int initialDepth=0;
    std::vector<int> bounds;

    myanalysis.computeFunctionIndicesPerNode(root, bounds, initialDepth, &preTraversal);

    if (processes==1) {
      for (int i = 0; i<(int)bounds.size();i++) {
	//std::cerr << "bounds [" << i << "] = " << bounds[i] << "   my_rank: " << my_rank << std::endl;
	if (bounds[i]== my_rank) {

	  SgFunctionDeclaration* funcDecl = myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i];
	  SgFunctionDefinition* funcDef = NULL;
	  if (funcDecl)
	    funcDef = funcDecl->get_definition();
	  if (funcDef) {
	    int nrNodes = ((DefUseAnalysis*)defuse)->start_traversal_of_one_function(funcDef);
	    resultDefUseNodes+=nrNodes;
	  }
	}
      }
    } else {
      int currentJob = -1;
      MPI_Status Stat;
      int *res = new int[2];
      res[0]=5;
      res[1]=5;
      int *res2 = new int[2];
      res2[0]=-1;
      res2[1]=-1;
      bool done = false;
      int jobsDone = 0;
      // **********************************************************
      int scale = 1;
      // **********************************************************


      MPI_Request request[2]; 
      MPI_Status status[2];
      int min = -1;
      int max = -1;
      if (my_rank != 0) {
	//std::cout << " process : " << my_rank << " sending. " << std::endl;
	MPI_Send(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD);
	MPI_Recv(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &Stat);
	min = res[0];
	max = res[1];
	if (res[0]==-1) 
	  done =true;
      }

      while (!done) {
	// we are ready, make sure to notify 0
	double total_node=0;
	if (my_rank != 0) {
	  MPI_Isend(res, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &request[0]);
	  MPI_Irecv(res2, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &request[1]);
	
	  if (((max-min) % 20)==0 || min <10)
	    std::cout << " process : " << my_rank << " receiving nr: [" << min << ":" << max << "[ of " <<
	      bounds.size() << "     range : " << (max-min) << std::endl;

	  for (int i=min; i<max;i++) { 
	    SgNode* mynode = isSgNode(myanalysis.DistributedMemoryAnalysisBase<int>::funcDecls[i]);
	    ROSE_ASSERT(mynode);
	    SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(mynode);
	    SgFunctionDefinition* funcDef = NULL;
	    if (funcDecl)
	      funcDef = funcDecl->get_definition();
	    if (funcDef) {
	      int nrNodes = ((DefUseAnalysis*)defuse)->start_traversal_of_one_function(funcDef);
	      resultDefUseNodes+=nrNodes;
	    }
	  }

	  MPI_Waitall(2,request,status);
	  min = res2[0];
	  max = res2[1];
	  if (res2[0]==-1) 
	    break;
	} // if
	if (my_rank == 0) {
	  //std::cout << " process : " << my_rank << " receiving. " << std::endl;
	  MPI_Recv(res, 2, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &Stat);
	  gettime(begin_time_node);
	  currentJob+=scale;

	  if ((currentJob % 10)==9) scale+=1;
	  if (currentJob>=(int)bounds.size()) {
	    res[0] = -1;
	    jobsDone++;
	  }      else {
	    res[0] = currentJob;
	    res[1] = currentJob+scale;
	    if (res[1]>=(int)bounds.size())
	      res[1] = bounds.size();
	    //	    dynamicFunctionsPerProcessor[Stat.MPI_SOURCE] += scale;
	  }
	  //      std::cout << " processes done : " << jobsDone << "/" << (processes-1) << std::endl;
	  //std::cout << " process : " << my_rank << " sending rank : " << res[0] << std::endl;
	  gettime(end_time_node);
	  double my_time_node = timeDifference(end_time_node, begin_time_node);
	  total_node += my_time_node;
	  //calc_time_processor+=total_node;
	  MPI_Send(res, 2, MPI_INT, Stat.MPI_SOURCE, 1, MPI_COMM_WORLD);      
	  if (jobsDone==(processes-1))
	    break;
	}
	
      } //while
      if (my_rank==0)
	std::cerr << ">>> Final scale = " << scale << std::endl; 
    
    } //else
    // ---------------- LOAD BALANCING of DEFUSE -------------------


    gettime(end_time_node);
    double my_time_node = timeDifference(end_time_node, begin_time_node);
    std::cerr << my_rank << ": DefUse Analysis complete. Nr of nodes: " << resultDefUseNodes << 
      "  time: " << my_time_node << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank==0)
      std::cerr << "\n>> Collecting defuse results ... " << std::endl;


    ROSE_ASSERT(defuse);
    my_map defmap = defuse->getDefMap();
    my_map usemap = defuse->getUseMap();
    std::cerr << my_rank << ": Def entries: " << defmap.size() << "  Use entries : " << usemap.size() << std::endl;
    gettime(end_time_node);
    my_time_node = timeDifference(end_time_node, begin_time_node);


    /* communicate times */
    double *times_defuse = new double[processes];
    MPI_Gather(&my_time_node, 1, MPI_DOUBLE, times_defuse, 1, MPI_DOUBLE, 0,
	       MPI_COMM_WORLD);
    double totaltime=0;
    for (int i=0;i<processes;++i)
      if (times_defuse[i]>totaltime)
	totaltime=times_defuse[i];
    delete[] times_defuse;

    if (my_rank==0) {
      std::cerr << ">> ---- Time (max) needed for DefUse : " << totaltime << std::endl <<std::endl;
      Compass::gettime(begin_time_node);
      Compass::gettime(b_time_node);
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
    std::cerr << my_rank << ": defmapsize : " << defmap.size() << "  usemapsize: " << usemap.size() 
	      << ": defs : " << arrsize << "  uses: " << arrsizeUse << std::endl;
    // communicate total size to allocate global arrsize
    global_arrsize = -1;
    global_arrsizeUse = -1;
    MPI_Allreduce(&arrsize, &global_arrsize, 1, MPI_UNSIGNED, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&arrsizeUse, &global_arrsizeUse, 1, MPI_UNSIGNED, MPI_SUM, MPI_COMM_WORLD);
    /* communicate arraysizes */

    /* serialize all results */
    def_values_global = new unsigned int[global_arrsize];
    unsigned int *def_values =new unsigned int[arrsize];
    use_values_global = new unsigned int[global_arrsizeUse];
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
    std::cerr << my_rank << " : serialization done."  << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank==0) {
      Compass::gettime(e_time_node);
      double restime = Compass::timeDifference(e_time_node, b_time_node);
      std::cerr << "\n >>> serialization done. TIME : " << restime << std::endl;
      Compass::gettime(b_time_node);
    }

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
    std::cerr << my_rank << " : serialization done."  
	      <<  "  waiting to gather...   arrsize: " << arrsize << "  offset : " << offset[my_rank] << " globalarrsize: " << global_arrsize<< std::endl;

    delete[] global_length;
    delete[] global_lengthUse;

    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank==0) {
      Compass::gettime(e_time_node);
      double restime = Compass::timeDifference(e_time_node, b_time_node);
      std::cerr << "\n >>> communication (ARRSIZE) done. TIME : " << restime << "  BROADCASTING ... " << std::endl;
      Compass::gettime(b_time_node);
    }


    // tps: This version seems slightly faster than the one following
    MPI_Allgatherv(def_values, arrsize, MPI_UNSIGNED, def_values_global, length, 
		   offset, MPI_UNSIGNED,  MPI_COMM_WORLD);
    MPI_Allgatherv(use_values, arrsizeUse, MPI_UNSIGNED, use_values_global, lengthUse, 
		   offsetUse, MPI_UNSIGNED,  MPI_COMM_WORLD);

    delete[] def_values;
    delete[] use_values;
    delete[] offset;
    delete[] length;
    delete[] offsetUse;
    delete[] lengthUse;
    def_values=NULL;
    use_values=NULL;
    offset=NULL;
    length=NULL;
    offsetUse=NULL;
    lengthUse=NULL;

    if (quickSave==false) {
      /* communicate all results */
      std::cerr << my_rank << " : communication done. Deserializing ..." << std::endl;

      MPI_Barrier(MPI_COMM_WORLD);
      if (my_rank==0) {
	Compass::gettime(e_time_node);
	double restime = Compass::timeDifference(e_time_node, b_time_node);
	std::cerr << "\n >>> communication (ARRAY) done. TIME : " << restime << 
	  "   arrsize Def : " << global_arrsize << "  arrsize Use : " << global_arrsizeUse << std::endl;
      }



      /* deserialize all results */
      // write the global def_use_array back to the defmap (for each processor)
      ((DefUseAnalysis*)defuse)->flushDefuse();
      deserializeDefUseResults(global_arrsize, (DefUseAnalysis*)defuse, def_values_global, memTrav->nodeMap, true);
      deserializeDefUseResults(global_arrsizeUse, (DefUseAnalysis*)defuse, use_values_global, memTrav->nodeMap, false);
      std::cerr << my_rank << " : deserialization done." << std::endl;
      /* deserialize all results */


      MPI_Barrier(MPI_COMM_WORLD);
      if (my_rank==0) {
	Compass::gettime(end_time_node);
	double restime = Compass::timeDifference(end_time_node, begin_time_node);
	std::cerr << ">> ---- DefUse Analysis - time for ALL communication :  " << restime << " sec " << std::endl;
      }

      defmap = defuse->getDefMap();
      usemap = defuse->getUseMap();

      if (my_rank==0) {
	std::cerr <<  my_rank << ": Total number of def nodes: " << defmap.size() << std::endl;
	std::cerr <<  my_rank << ": Total number of use nodes: " << usemap.size() << std::endl << std::endl;
	//((DefUseAnalysis*)defuse)->printDefMap();
      }
    }
    //#endif
#else
    std::cerr << ">>>>>> running defuse analysis in SEQUENCE (NO MPI). "  << std::endl;
    defuse = new DefUseAnalysis(root);

    // tps: fixme (9 Jul 2008)
    // skipping defuse tests until they pass all compass tests
    ((DefUseAnalysis*)defuse)->run(false);
    std::cerr <<  "Total number of def nodes: " << defuse->getDefMap().size() << std::endl;
    std::cerr <<  "Total number of use nodes: " << defuse->getUseMap().size() << std::endl << std::endl;
  
#endif

  }

  ROSE_ASSERT(defuse);





      done = true;
    }

    PrerequisiteList getPrerequisites() const {
      return PrerequisiteList();
    }

    DefUseAnalysis* getSourceDefUse() const {
      ROSE_ASSERT (done);
      return defuse;
    }

  };


 extern SourceDefUsePrerequisite sourceDefUsePrerequisite;

  // The prerequisite for getting the SgProject
  class AuxiliaryInfoPrerequisite: public Prerequisite {
    VirtualBinCFG::AuxiliaryInformation* info;
  public:
    AuxiliaryInfoPrerequisite(): Prerequisite("Auxiliary"), info(NULL) {}
    void run(SgProject* project) {
      if (done) return;

//      SgAsmFile* file = project->get_file(0).get_binaryFile();
      info = new VirtualBinCFG::AuxiliaryInformation(project);
      done = true;
    }

    PrerequisiteList getPrerequisites() const {
      return PrerequisiteList();
    }

    VirtualBinCFG::AuxiliaryInformation* getAuxiliaryInfo() const {
      ROSE_ASSERT (done);
      return info;
    }
  };
  extern AuxiliaryInfoPrerequisite auxiliaryInfoPrerequisite;

  class BinDataFlowPrerequisite: public Prerequisite {
    RoseBin_DataFlowAnalysis* dfanalysis;
    RoseBin_Graph* graph;
  public:
  BinDataFlowPrerequisite(): Prerequisite("BinDataFlow"), dfanalysis(NULL) {}
    void run(SgProject* project) {
      SgAsmFile* file = project->get_file(0).get_binaryFile();
      if (file==NULL)
	done =true;
      if (done) return;

      bool forward = true;
      bool interprocedural = false;
      bool edges = true;
      bool mergedEdges = true;
      
      VirtualBinCFG::AuxiliaryInformation* info = auxiliaryInfoPrerequisite.getAuxiliaryInfo();
  
      std::string dfgFileName = "dfg.dot";
      graph = new RoseBin_DotGraph(info);
      ROSE_ASSERT(graph);

      dfanalysis = new RoseBin_DataFlowAnalysis(file->get_global_block(), forward, new RoseObj(), info);
      dfanalysis->init(interprocedural, edges);
      dfanalysis->run(graph, dfgFileName, mergedEdges);

      done = true;
    }

    PrerequisiteList getPrerequisites() const {
      PrerequisiteList prerequisiteList;
      prerequisiteList.push_back(&auxiliaryInfoPrerequisite);
      return prerequisiteList;
    }

    RoseBin_DataFlowAnalysis* getBinDataFlowInfo() const {
      ROSE_ASSERT (done);
      return dfanalysis;
    }
    RoseBin_Graph* getGraph() const {
      ROSE_ASSERT (done);
      return graph;
    }
  };


 

  extern BinDataFlowPrerequisite binDataFlowPrerequisite;

  /// Run the prerequisites for a checker
  void runPrereqs(const Checker* checker, SgProject* proj);

  /// Run the prerequisites of a prerequisite, but not prereq itself
  void runPrereqs(Prerequisite* prereq, SgProject* proj);

  /// Run a checker and its prerequisites
  void runCheckerAndPrereqs(const Checker* checker, SgProject* proj, Parameters params, OutputObject* output);
}

#endif // ROSE_COMPASS_H
