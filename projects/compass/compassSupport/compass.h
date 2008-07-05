#ifndef ROSE_COMPASS_H
#define ROSE_COMPASS_H

#include <rose.h>

namespace Compass {
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

  /// The base class required for checkers.
  /// No particular constructors are required; the factory is always used to
  /// construct objects derived from this base class.
     class TraversalBase
        {
          protected:
            // DQ (7/6/2007): Added so that information from the factory could be available in the checkers.
               Compass::OutputObject* output;

          private:

               std::string m_checkerName;
               std::string m_shortDescription;
               std::string m_longDescription;

          public:
               void setName( std::string s ) { m_checkerName = s; }
               std::string getName() { return m_checkerName; }
               void setShortDescription( std::string s ) { m_shortDescription = s; }
               std::string getShortDescription() { return m_shortDescription; }
               void setLongDescription( std::string s ) { m_longDescription = s; }
               std::string getLongDescription() { return m_longDescription; }

               TraversalBase(Compass::OutputObject* out, const std::string & name, const std::string & shortDescriptionInput, const std::string & longDescriptionInput) 
                  : output(out), m_checkerName(name), m_shortDescription(shortDescriptionInput), m_longDescription(longDescriptionInput) {}

               virtual ~TraversalBase() {}

           /// Run the given checker starting at a top node
               virtual void run(SgNode* n) = 0;
               virtual void visit(SgNode* n) = 0;
               Compass::OutputObject* getOutput() { return output; };
        };

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
     void outputTgui( std::string & tguiXML,std::vector<Compass::TraversalBase*> & checkers, PrintingOutputObject & output );

}

#endif // ROSE_COMPASS_H
