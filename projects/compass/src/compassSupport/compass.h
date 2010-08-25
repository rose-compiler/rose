#ifndef ROSE_COMPASS_H
#define ROSE_COMPASS_H
#if ROSE_MPI
#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#include <mpi.h>
#endif

//#include <rose.h>
#include "DefUseAnalysis_perFunction.h"
#include "DefUseAnalysis.h"
#include <boost/function.hpp>
#include <boost/any.hpp>

#if ROSE_MPI
#include "functionLevelTraversal.h"
#include "functionNames.h"
#endif


#ifdef HAVE_SQLITE3

#include "sqlite3x.h"
#include <boost/lexical_cast.hpp>
#endif

namespace Compass {
  extern unsigned int global_arrsize;
  extern unsigned int global_arrsizeUse;
  extern unsigned int *def_values_global;
  extern unsigned int *use_values_global;
  typedef std::map< SgNode* , std::vector < std::pair <SgInitializedName* , SgNode*>  > > my_map; 
  extern bool quickSave;

  static std::set<std::string> pre;


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

  //! Support for using SQLite as output data when run as batch
  extern bool UseDbOutput;
  extern std::string outputDbName;
#ifdef HAVE_SQLITE3
  extern sqlite3x::sqlite3_connection con;
#endif

     
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
    Prerequisite(const std::string& input_name): name(input_name), done(false) {}
    virtual PrerequisiteList getPrerequisites() const = 0;
    virtual void run(SgProject* proj) = 0;

 // DQ (9/28/2009): Having memory problems that are GNU 4.3 specific (reported by Valgrind).
 // This appears to fix a memory problem, though it is not clear why.
    virtual ~Prerequisite() { name = ""; }
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
#if 0
    bool isBinary() {
      bool isbinary=false;
      int language = supportedLanguages;
      language >>= 4;
      int lan = language &0x1u ; // x86
      if (lan==1) isbinary=true;
      language >>= 1;
      lan = language &0x1u ;  // ARM
      if (lan==1) isbinary=true;
      return isbinary;
    }
#endif
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

  // ---- GRAPH ---------------------------------------------------------------------------------------------------
  class GraphProcessingWithRunFunction : public RoseBin_DataFlowAbstract {
  public:
    const Checker* checker;
  GraphProcessingWithRunFunction():RoseBin_DataFlowAbstract(NULL),checker(NULL) {}
  GraphProcessingWithRunFunction(Checker* mychecker):RoseBin_DataFlowAbstract(NULL),checker(mychecker) {}
    virtual ~GraphProcessingWithRunFunction() {}
    virtual void run(SgNode*)=0;
    //virtual void run(std::string, SgDirectedGraphNode*, SgDirectedGraphNode*)=0;
    //virtual void visit(SgNode* n)=0;
  };

  class CheckerUsingGraphProcessing: public Checker {
  public:
    typedef boost::function<GraphProcessingWithRunFunction* /*createGraphTraversal*/(Parameters, OutputObject*)> GraphTraversalCreationFunction;
    GraphTraversalCreationFunction createGraphTraversal;

  CheckerUsingGraphProcessing(std::string checkerName, std::string shortDescription, std::string longDescription, LanguageSet supportedLanguages, const PrerequisiteList& prerequisites, RunFunction run, GraphTraversalCreationFunction createGraphTraversal):
    Checker(checkerName, shortDescription, longDescription, supportedLanguages, prerequisites, run), createGraphTraversal(createGraphTraversal) {}
  };

  // ---- GRAPH ---------------------------------------------------------------------------------------------------


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
  void outputTgui( std::string & tguiXML, std::vector<const Compass::Checker*> & checkers, Compass::OutputObject *output );
  // tps (18Dec2008) : Added a guard because javaport testcase brakes on this
#ifdef HAVE_SQLITE3
  // Output to SQLITE database
  void outputDb( std::string  dbName, std::vector<const Compass::Checker*> & checkers, Compass::OutputObject *output );
#endif

  #include "prerequisites.h"

  /// Run the prerequisites for a checker
  void runPrereqs(const Checker* checker, SgProject* proj);

  /// Run the prerequisites of a prerequisite, but not prereq itself
  void runPrereqs(Prerequisite* prereq, SgProject* proj);

  /// Run a checker and its prerequisites
  void runCheckerAndPrereqs(const Checker* checker, SgProject* proj, Parameters params, OutputObject* output);
}

#endif // ROSE_COMPASS_H
