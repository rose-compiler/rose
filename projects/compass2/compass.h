/**
 * \file compass.h
 * \author Justin Too <too1@llnl.gov>
 */

#ifndef ROSE_COMPASS_H
#define ROSE_COMPASS_H

/*-----------------------------------------------------------------------------
 * C/C++ system includes
 **--------------------------------------------------------------------------*/
#include <sys/stat.h>
#include <errno.h>
#include <memory>   // std::auto_ptr

/*-----------------------------------------------------------------------------
 * Library includes
 **--------------------------------------------------------------------------*/
// Boost C++ libraries
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>

// Xerces-C XML libraries
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>

/*-----------------------------------------------------------------------------
 * Project includes
 **--------------------------------------------------------------------------*/
//#include <rose.h>
#include "string_functions.h"

/*-----------------------------------------------------------------------------
 *
 * CPP: Preprocessor Macros
 *
 **--------------------------------------------------------------------------*/
/**
 * @defgroup CPP
 * @{
 *//*------------------------------------------------------------------------*/

/**
 * Disallows the copy constructor and <b>operator=</b> functions.
 *
 * This should be used in the <b>private</b> declarations section
 * for a class.
 */
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

/**
 * Disallows the no-argument constructor function.
 *
 * This should be used in the <b>private</b> declarations section
 * for a class.
 */
#define DISALLOW_NO_ARG_CTOR(TypeName) \
  TypeName() {}

/**
 * Safe deallocation of dynamically allocated memory from <b>new</b>.
 */
#define SAFE_DELETE(ptr)  \
  if (ptr != NULL )       \
  {                       \
      delete ptr;         \
      ptr = NULL;         \
  }

/**----------------------------------------------------------------------------
 *
 * End of CPP: Preprocessor Macros group
 *
 * @}
 **--------------------------------------------------------------------------*/

namespace Compass
{
/**
 * @defgroup Compass
 * @{
 */

  /*---------------------------------------------------------------------------
   *
   * Globals
   *
   **------------------------------------------------------------------------*/
  /**
   * @defgroup Globals
   * @ingroup Compass
   * @{
   *//*----------------------------------------------------------------------*/

  using namespace xercesc;

  typedef std::map< SgNode* , std::vector < std::pair <SgInitializedName* , SgNode*>  > > my_map;
  typedef std::map<std::string, std::string> StringMap;
  typedef std::vector<std::string> StringVector;

  /**--------------------------------------------------------------------------
   *
   * End of Globals group
   *
   * @}
   **------------------------------------------------------------------------*/

  /**
   * @return True if <tt>SgLocatedNode</tt> exists in a file beneath
   * <tt>path</tt>.
   */
  bool
  IsNodeInUserLocation(const SgLocatedNode *const node, const std::string& path);

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

  /*---------------------------------------------------------------------------
   *
   * Parameters
   *
   **------------------------------------------------------------------------*/
  /**
   * @defgroup Parameters
   * @ingroup Compass
   * @{
   *//*----------------------------------------------------------------------*/

  /** { parameter-name => [parameter-value-1, parameter-value-2, ...] } */
  typedef std::map<std::string, std::vector<std::string> > ParametersMap;
  typedef StringVector ParameterValues;

  class ParameterException: public std::exception
  {
    public:
      explicit ParameterException(const std::string &a_message)
        : m_error_message(a_message)
        {}

      virtual const char* what() const throw ()
        {
          return m_error_message.c_str();
        }

    private:
      const std::string &m_error_message;
  };

  /**
   * Contains information about an input parameter required by a checker which
   * is not found in the parameter file.
   */
  class ParameterNotFoundException: public Compass::ParameterException
  {
    public:
      explicit ParameterNotFoundException(const std::string &a_parameter_name)
        : ParameterException("Parameter not found: ")
        {}
  };

  /**
   * Contains information about an input parameter required by a checker which
   * should have exactly one value.
   */
  class ParameterNotUniqueException: public Compass::ParameterException
  {
    public:
      explicit ParameterNotUniqueException(const std::string &a_parameter_name)
        : ParameterException("Parameter not unique, multiple definitions found for ")
        {}
  };

  /**
   * Print parameters to standard output (for debugging).
   */
  void
  print_parameters(const ParametersMap& parameters);

  /**
   * TODO: provide mechanism to search ParametersMap
   */
  //std::vector<ParametersMap::const_iterator> find(
  //    const ParametersMap& map, const boost::regex& a_regex)
  //{
  //}

  /**
   * \brief Checker parameters.
   */
  class Parameters
  {
    public:
      Parameters () {} // TODO: disallow

      explicit Parameters (const ParametersMap &a_parameters)
      : _parameters(a_parameters) {}

      /**
       * \brief Get a parameter by name.
       * Parameters are named in the form: namespace::name.
       *
       * \throws ParameterNotFoundException
       */
      ParametersMap operator[](const boost::regex &a_regex) const
          throw(ParameterNotFoundException);

      /**
       * @copydoc Compass::Parameters::operator[](const boost::regex&)
       *
       * Note: will match using this regular expression: /^a_parameter_name$/.
       *
       * Note: only exactly one match can be found since parameter names are
       *       stored in a map.
       *
       * \returns a list of values for a_parameter_name.
       */
      ParameterValues operator[](const std::string &a_parameter_name) const
          throw(ParameterNotFoundException);

      ParameterValues operator[](const char *a_parameter_name) const
          throw(ParameterNotFoundException);

      std::string get_unique(const std::string &a_parameter_name) const
          throw(ParameterNotFoundException);

      void debug()
        {
          Compass::print_parameters(_parameters);
        }

    private:
      ParametersMap _parameters;
  };

  /**--------------------------------------------------------------------------
   *
   * End of Parameters group
   *
   * @}
   **------------------------------------------------------------------------*/

// TODO: doxygen documentation for static const char *const
namespace Xml
{
  /** 
   * @defgroup Xml
   * @ingroup Compass
   * @{
   */

  static const char *const
      FILE_COMPASS_PARAMETERS_XML = "compass_parameters.xml";

  static const char *const
      FILE_COMPASS_PARAMETERS_XSD = "compass_parameters.xsd";

  static const std::string
      ENV_COMPASS_PARAMETERS = "COMPASS_PARAMETERS";

  static const std::string
      ENV_COMPASS_PARAMETERS_XSD = "COMPASS_PARAMETERS_XSD";

  typedef std::vector<DOMElement*> DOMElements;

  // TODO: doxygen documentation for static const char *const
  namespace Tag
  {
  /** 
   * @defgroup Tag
   * @ingroup Xml
   * @{
   */
    static const char *const GENERAL = "general";
    static const char *const CHECKER = "checker";
    static const char *const CHECKERS = "checkers";
    static const char *const PARAMETER = "parameter";
  /** @} End of Tag namespace */
  }

  /*---------------------------------------------------------------------
   *
   * Utilities
   *
   **------------------------------------------------------------------*/
  /**
   * @defgroup Utilities
   * @ingroup Xml
   * @{
   *//*----------------------------------------------------------------*/


  // Forward function declarations
  std::vector<DOMElement*> getChildElements (const DOMElement* a_root);
  std::string transcodeToStr (const XMLCh* a_xch);

  /**
   */
  Compass::StringMap getAttributes (const DOMElement* a_root);

  /**
   * Get child elements by tag name.
   *
   * @param a_root is the parent to start searching from.
   * @param a_tagname is the target identifier to match in children.
   *
   * @return the list of matching child elements.
   */
  std::vector<DOMElement*>
  getElementsByTagName (const DOMElement* a_root,
                        const char *const a_tagname);

  /**
   * Get child elements by tag name.
   *
   * @param a_root is the parent to start searching from.
   * @param a_tagname is the target identifier to match in children.
   *
   * @return the list of matching child elements.
   *
   * TODO?: boost::regex match
   */
  Xml::DOMElements
  getChildElementsByTagName (const DOMElement* a_root,
                            const char* a_tagname);

  /**
   * Get all child Elements.
   *
   * @param a_root is the parent to start searching from.
   *
   * @return the list of child DOM Elements.
   */
  std::vector<DOMElement*>
  getChildElements (const DOMElement* a_root);

  /**
   * Get all child Elements for a list of parent Elements.
   *
   * @param a_roots are the parents to start searching from.
   *
   * @return the map of [parents]-->[children DOM Elements].
   */
  std::map<DOMElement*, std::vector<DOMElement*> >
  getChildElements (std::vector<DOMElement*> a_roots);

  /**
   * Transcode Xerces-character-encoded string to std::string.
   *
   * @param a_xch is the Xerces-encoded-character string.
   *
   * @return std::string.
   */
  std::string transcodeToStr (const XMLCh* a_xch);

  /**
   * Check if a file is accessible.
   */
  void
  stat_file (const char *const a_filename) throw( std::runtime_error );

  DOMLSParser*
  create_parser (XMLGrammarPool* pool);

  /**--------------------------------------------------------------------
   *
   * End of Utilities group
   *
   * @}
   **------------------------------------------------------------------*/

  /*---------------------------------------------------------------------
   *
   * Exceptions
   *
   **------------------------------------------------------------------*/
  /**
   * @defgroup Exceptions
   * @ingroup Xml
   * @{
   *//*----------------------------------------------------------------*/

  /**
   * Base class for XML Exceptions.
   */
  class Exception : public std::runtime_error
  {
    public:
      Exception (std::string message)
      : std::runtime_error( message ) {}
  };

  /**
   * @throws Xml::Exception
   */
  void
  Throw (const std::string a_title, const XMLCh* a_message) throw (Exception);

  /**--------------------------------------------------------------------
   *
   * End of Exceptions group
   *
   * @}
   **------------------------------------------------------------------*/

  /*--------------------------------------------------------------------
   *
   * Parsers
   *
   **------------------------------------------------------------------*/
  /**
   * @addtogroup Parsers
   * @ingroup Xml
   * @{
   *//*----------------------------------------------------------------*/

  class ErrorHandler : public xercesc::DOMErrorHandler
  {
    public:
      ErrorHandler ()
        : _failed (false),
          _message ("Xml::ErrorHandler")
        {}

      bool failed () const { return _failed; }

      virtual bool handleError (const xercesc::DOMError& e);

      std::string get_message () const
        {
          return _message;
        }

      void set_message (const std::string a_message)
        {
          _message.assign (a_message);
        }

    private:
      bool        _failed;
      std::string _message;
  };

  /**
   * Manage parsing of XML file with Xerces' DOM parser.
   */
  class Parser
  {
    public:
      /*-----------------------------------------------------------------------
       *  Constructors and Destructor
       *---------------------------------------------------------------------*/

      /** @name Constructor and Destructor methods */
      //@{

      /**
       * Parse file with DOM parser.
       *
       * @param a_filename is a path to an XML file.
       * @param a_grammar_filename  is a path to an XSD grammar file.
       * @throws Compass::Xml::Exception
       */
      Parser (const std::string& a_filename);

      Parser (const std::string& a_filename,
              const std::string& a_grammar_filename = "grammar.xsd");

      void init (const std::string& a_filename,
                 const std::string& a_grammar_filename);

      /** Cleanup dynamically allocated resources. */
      virtual ~Parser();

      //@}

      /*-----------------------------------------------------------------------
       *  Accessors
       *---------------------------------------------------------------------*/

      /** @name Accessor methods */
      //@{

      /**
       * Set name of file to be parsed.
       *
       * Note: #Parse is not called.
       */
      void set_filename (const std::string &a_filename)
        {
          _filename.assign (
              locate_xml_file (a_filename, Xml::ENV_COMPASS_PARAMETERS));
        }

      /**
       * \returns a copy of this Parser's filename.
       */
      const std::string get_filename() const
        {
          return _filename;
        }

      /**
       * Set name of XML grammar.
       *
       * Note: #Parse is not called.
       */
      void set_grammar_filename(const std::string &a_filename)
        {
          _grammar_filename.assign (
              locate_xml_file (a_filename, Xml::ENV_COMPASS_PARAMETERS_XSD));
        }

      /**
       * \returns the grammar filename.
       */
      const std::string get_grammar_filename () const
        {
          return _grammar_filename;
        }

      std::string locate_xml_file (
          const std::string& a_filename, const std::string& a_env_name) const;

      DOMElement* getDocumentElement () const;
      DOMDocument* get_document () const;

      //@}

      /*-----------------------------------------------------------------------
       * Utilities
       *---------------------------------------------------------------------*/

    private:
      /*-----------------------------------------------------------------------
       * Attributes
       *---------------------------------------------------------------------*/
      std::string       _filename;          ///< Input XML filename.
      std::string       _grammar_filename;  ///< XML validation filename.
      DOMDocument*      _dom_document;

      /*-----------------------------------------------------------------------
       * Utilities
       *---------------------------------------------------------------------*/
      DISALLOW_NO_ARG_CTOR(Parser);
      DISALLOW_COPY_AND_ASSIGN(Parser);
  };//end class Parser

  /**
   * Manage parsing of compass parameters XML file.
   */
  class ParametersParser : public Parser
  {
    public:
      /**
       * @copydoc Xml::Parser::Parser(const char*)
       */
      ParametersParser (const std::string& a_filename =
                            FILE_COMPASS_PARAMETERS_XML,
                        const std::string& a_grammar_filename =
                            FILE_COMPASS_PARAMETERS_XSD
          )
        : Parser(a_filename, a_grammar_filename)
        {}

      virtual ~ParametersParser() {};

      /*-----------------------------------------------------------------------
       * Utilities
       *---------------------------------------------------------------------*/

      /**
       * Parse general and checker-specific parameters.
       */
      virtual Compass::Parameters parse_parameters ();

    private:
      /*-----------------------------------------------------------------------
       * Utilities
       *---------------------------------------------------------------------*/

      /**
       * Parse parameters.
       *
       * Parameters must have at least one attribute defined, namely the
       * +name+ attribute.
       *
       * @param root is the parameters' parent/group element.
       */
      ParametersMap parse_parameters (DOMElement* root,
          const std::string& group);
  };

  /**--------------------------------------------------------------------
   *
   * End of Parsers group
   *
   * @}
   **------------------------------------------------------------------*/
/** @} End of Xml namespace */
}

  using namespace Xml;

  /*--------------------------------------------------------------------
   *
   * Utilities
   *
   **------------------------------------------------------------------*/
  /**
   * @addtogroup Utilities
   * @ingroup Compass
   * @{
   *//*----------------------------------------------------------------*/
  void commandLineProcessing (Rose_STL_Container<std::string> &commandLineArray);

  std::ifstream* openFile (const std::string &filename);
  /**--------------------------------------------------------------------
   *
   * End of Utilities group
   *
   * @}
   **------------------------------------------------------------------*/

  /*--------------------------------------------------------------------
   *
   * Output (Results)
   *
   **------------------------------------------------------------------*/
  /**
   * @addtogroup Output
   * @ingroup Compass
   * @{
   *//*----------------------------------------------------------------*/
  /// The base class for outputs from a checker
  class OutputViolationBase
    {
      public:
        /// Get the name of the checker which produced this error

        // Constructor for use with a single IR node.
        OutputViolationBase (SgNode* node,
                             const std::string& checker = "default value",
                             const std::string & description = "default value")
          : m_checkerName (checker),
            m_shortDescription (description)
          {
            nodeArray.push_back(node);
          }

        // Constructor for use with two IR nodes.
        OutputViolationBase (SgNode* first_node,
                             SgNode* second_node,
                             const std::string& checker = "default value",
                             const std::string & description = "default value")
          : m_checkerName (checker),
            m_shortDescription (description)
          {
            nodeArray.push_back(first_node);
            nodeArray.push_back(second_node);
          }

        // Constructor for use with multiple IR nodes.
        OutputViolationBase (std::vector<SgNode*>& inputNodeArray,
                             const std::string& checker = "default value",
                             const std::string & description = "default value")
          : m_checkerName (checker),
            m_shortDescription (description)
          {
            nodeArray = inputNodeArray;
          }

        virtual ~OutputViolationBase() {}

        virtual std::string getString() const;
        SgNode* getNode() const { return nodeArray[0]; }
        SgNode* operator[]( int i ) const { return nodeArray[i]; }
        const std::vector<SgNode*>& getNodeArray() const { return nodeArray; }
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
    };// end class OutputViolationBase class

  /** Base class for objects which can be used to output error messages from
    * checkers.
    */
  class OutputObject
    {
      public:
        //! Emit one error message
        virtual void addOutput (OutputViolationBase* theOutput) = 0;

        // virtual SgNode* getNode() const = 0;
        std::vector<OutputViolationBase*> getOutputList ()
          {
            return outputList;
          }

        void clear ()
          {
            outputList.erase(outputList.begin(),outputList.end());
          }

        virtual ~OutputObject() {}

      protected:
        std::vector<OutputViolationBase*> outputList;
    };// end OutputObject class

  /** A simple output object which just prints each error message
    */
  class PrintingOutputObject: public OutputObject
    {
      public:
        PrintingOutputObject (std::ostream& stream)
          : stream (stream)
        {}

        virtual void addOutput (OutputViolationBase* theOutput);

      private:
        std::ostream& stream;
    };// end PrintingOutputObject class

  /** \brief Format file info according to the GNU standard.
    *
    * See http://www.gnu.org/prep/standards/html_node/Errors.html
    */
  std::string formatStandardSourcePosition(const Sg_File_Info* fi);

  /** \brief Format start-to-end file info according to the GNU standard.
    *
    * See http://www.gnu.org/prep/standards/html_node/Errors.html
    */
  std::string formatStandardSourcePosition(const Sg_File_Info* start,
                                           const Sg_File_Info* end);

  /** \brief Format line-column file info according to the GNU standard.
    *
    * See http://www.gnu.org/prep/standards/html_node/Errors.html
    */
  std::string formatStandardSourcePosition(const std::string& filename,
                                           int line, int col);

  /** \brief Format start-to-end file line-column file info according to the GNU standard.
    *
    * See http://www.gnu.org/prep/standards/html_node/Errors.html
    */
  std::string formatStandardSourcePosition(const std::string& startFilename,
                                           int startLine, int startCol,
                                           const std::string& endFilename,
                                           int endLine, int endCol);
  /**--------------------------------------------------------------------
   *
   * End of Output (Results) group
   *
   * @}
   **------------------------------------------------------------------*/


  /*--------------------------------------------------------------------
   *
   * Checkers
   *
   **------------------------------------------------------------------*/
  /**
   * @defgroup Checkers
   * @ingroup Compass
   * @{
   *//*----------------------------------------------------------------*/

  /** The possible languages that a Compass checker can support.  The enum
    * values are powers of 2 and can be ORed together to represent sets.
    */
  enum
    {
      C = (1 << 0),
      Cpp = (1 << 1),
      Fortran = (1 << 2),
      PHP = (1 << 3),
      X86Assembly = (1 << 4),
      ArmAssembly = (1 << 5)
    };
  typedef int LanguageSet; // To allow operator|() on values

  class Prerequisite;
  //! A list of prerequisite pointers.
  typedef std::vector<Prerequisite*> PrerequisiteList;

  /** A prerequisite description for an analysis used by checkers, such as
    * def-use.  Actual prerequisites are global variables that are instances of
    * classes that inherit from this one.  Each such class has its own run()
    * method and way to get its result.  The assumption is that the
    * prerequisites are global variables and are only run once in a session
    * (i.e., not on different projects or with different parameter files).
    */
  class Prerequisite
    {
      public:
        mutable bool  done;
        std::string   name;

        //! \todo document
        Prerequisite (const std::string& input_name)
          : name (input_name),
            done (false)
          {}

        //! \todo document
        virtual PrerequisiteList getPrerequisites () const = 0;

        //! \todo document
        virtual void run (SgProject* proj) = 0;

         // DQ (9/28/2009): Having memory problems that are GNU 4.3 specific (reported by Valgrind).
         // This appears to fix a memory problem, though it is not clear why.
        virtual ~Prerequisite ()
          {
            name = "";
          }
    };// end Prerequisite class

  // The prerequisite for getting the SgProject
  class ProjectPrerequisite: public Prerequisite
    {
      public:
        ProjectPrerequisite ()
          : Prerequisite ("SgProject"),
            proj (NULL)
          {}

        void run (SgProject* p)
          {
            if (done) return;
            proj = p;
            done = true;
          }

        PrerequisiteList getPrerequisites() const
          {
            return PrerequisiteList ();
          }

        SgProject* getProject () const
          {
            ROSE_ASSERT (done);
            return proj;
          }

      private:
        SgProject* proj;
    };// end ProjectPrerequisite class

  extern ProjectPrerequisite projectPrerequisite;

  /** The basic class with metadata about a checker.  Most checkers will be
    * instances (not subclasses) of this class.  Some subclasses are for
    * checkers that have extra information, such as the ability to be
    * combined with other instances of AstSimpleProcessing.
    */
  class Checker
    {
      public:
        typedef boost::function<void /*run*/(Parameters, OutputObject*)> RunFunction;
        std::string checkerName;
        std::string shortDescription;
        std::string longDescription;
        LanguageSet supportedLanguages;
        PrerequisiteList prerequisites;
        RunFunction run;
        virtual ~Checker() {} // Allow RTTI

      Checker(std::string checkerName,
              std::string shortDescription,
              std::string longDescription,
              LanguageSet supportedLanguages,
              const PrerequisiteList& prerequisites,
              RunFunction run)
        : checkerName (checkerName),
          shortDescription (shortDescription),
          longDescription (longDescription),
          supportedLanguages (supportedLanguages),
          prerequisites (prerequisites),
          run (run)
        {}
    };// end Checker class

  /*--------------------------------------------------------------------
   *
   * AST traversal
   *
   **------------------------------------------------------------------*/
  /**
   * @defgroup AST
   * @ingroup Checkers
   * @{
   *//*----------------------------------------------------------------*/

  class AstSimpleProcessingWithRunFunction : public AstSimpleProcessing {
  public:
    const Checker* checker;
    AstSimpleProcessingWithRunFunction():checker(NULL) {}
    AstSimpleProcessingWithRunFunction(Checker* mychecker):checker(mychecker) {}
    virtual ~AstSimpleProcessingWithRunFunction() {}
    virtual void run(SgNode*)=0;
    virtual void visit(SgNode* n)=0;
  };


  /** A checker that supports combining with other instances of
    * AstSimpleProcessing
    */
  class CheckerUsingAstSimpleProcessing: public Checker
    {
      public:
        typedef boost::function<AstSimpleProcessingWithRunFunction*
            /*createSimpleTraversal*/ (Parameters, OutputObject*)>
            SimpleTraversalCreationFunction;

          SimpleTraversalCreationFunction createSimpleTraversal;

        CheckerUsingAstSimpleProcessing (
            std::string checkerName,
            std::string shortDescription,
            std::string longDescription,
            LanguageSet supportedLanguages,
            const PrerequisiteList& prerequisites,
            RunFunction run,
            SimpleTraversalCreationFunction createSimpleTraversal)
          : Checker (checkerName,
                     shortDescription,
                     longDescription,
                     supportedLanguages,
                     prerequisites,
                     run),
            createSimpleTraversal(createSimpleTraversal)
          {}
    };// end CheckerUsingAstSimpleProcessing class

  /**--------------------------------------------------------------------
   *
   * End of AST group
   *
   * @}
   **------------------------------------------------------------------*/

  /*--------------------------------------------------------------------
   *
   * Graph traversal
   *
   **------------------------------------------------------------------*/
  /**
   * @defgroup Graph
   * @ingroup Checkers
   * @{
   *//*----------------------------------------------------------------*/

  class GraphProcessingWithRunFunction : public RoseBin_DataFlowAbstract
    {
      public:
        const Checker* checker;

        GraphProcessingWithRunFunction ()
          : RoseBin_DataFlowAbstract (NULL),
            checker (NULL)
          {}

        GraphProcessingWithRunFunction (Checker* mychecker)
          : RoseBin_DataFlowAbstract (NULL),
            checker (mychecker)
          {}

        virtual ~GraphProcessingWithRunFunction() {}
        virtual void run(SgNode*) = 0;
        //virtual void run(std::string, SgDirectedGraphNode*, SgDirectedGraphNode*)=0;
        //virtual void visit(SgNode* n)=0;
    };

  class CheckerUsingGraphProcessing: public Checker
    {
      public:
        typedef boost::function<GraphProcessingWithRunFunction*
          /*createGraphTraversal*/(Parameters, OutputObject*)>
          GraphTraversalCreationFunction;

        GraphTraversalCreationFunction createGraphTraversal;

        CheckerUsingGraphProcessing (
            std::string checkerName,
            std::string shortDescription,
            std::string longDescription,
            LanguageSet supportedLanguages,
            const PrerequisiteList& prerequisites,
            RunFunction run,
            GraphTraversalCreationFunction createGraphTraversal)
          : Checker (checkerName,
                     shortDescription,
                     longDescription,
                     supportedLanguages,
                     prerequisites,
                     run),
            createGraphTraversal (createGraphTraversal)
          {}
    };// end CheckerUsingGraphProcessing class

  /**--------------------------------------------------------------------
   *
   * End of Graph Traversal group
   *
   * @}
   **------------------------------------------------------------------*/

//  #include "prerequisites.h"

  //! Run the prerequisites for a checker
  void runPrereqs (const Checker* checker, SgProject* proj);

  //! Run the prerequisites of a prerequisite, but not prereq itself
  void runPrereqs (Prerequisite* prereq, SgProject* proj);

  //! Run a checker and its prerequisites
  void runCheckerAndPrereqs (const Checker* checker, SgProject* proj, Parameters params, OutputObject* output);
  /**--------------------------------------------------------------------
   *
   * End of Checkers group
   *
   * @}
   **------------------------------------------------------------------*/

/** @} End of Compass namespace */
}
#endif // ROSE_COMPASS_H

