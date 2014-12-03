/**
 * \file compass.cpp
 * \author Justin Too <too1@llnl.gov>
 */

#ifndef PALETTE_USE_ROSE
#include "rose.h"
#endif

/*-----------------------------------------------------------------------------
 * C/C++ system includes
 **--------------------------------------------------------------------------*/
#include <sstream>
#include <fstream>
#include <iostream>

/*-----------------------------------------------------------------------------
 * Library includes
 **--------------------------------------------------------------------------*/
// Boost C++ libraries
#include "boost/filesystem/operations.hpp"

/*-----------------------------------------------------------------------------
 * Project includes
 **--------------------------------------------------------------------------*/
#include "compass2/compass.h"

std::ifstream*
Compass::openFile (const std::string &a_filename)
{
  if (boost::filesystem::exists (a_filename) == false)
  {
    std::cerr
        << "Error: file does not exist: " << a_filename << std::endl;
    abort(); // TODO: throw error
  }
  else if (boost::filesystem::is_regular_file (a_filename) == false)
  {
    std::cerr
        << "Error: not a regular file: " << a_filename << std::endl;
    abort(); // TODO: throw error
  }

  std::cout << "Opening file: " << a_filename << std::endl;
  std::ifstream* streamPtr = new std::ifstream (a_filename.c_str ());

  ROSE_ASSERT (streamPtr != NULL);
  ROSE_ASSERT (streamPtr->good ());

  return streamPtr;
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

	  // Strip off the path, since flymake will prepend stuff: e.g. "../../../../../../../../home/dquinlan/ROSE/NEW_ROSE/projects/compass2/compassMain"
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


  // Adding a new command line parameter (for mechanisms in ROSE that take command lines)

  // printf ("commandLineArray.size() = %zu \n",commandLineArray.size());
  // printf ("Preprocessor (after): argv = \n%s \n",StringUtility::listToString(commandLineArray).c_str());
}

using namespace Compass;

//#include "instantiate_prerequisites.h"

static void runPrereqList(const PrerequisiteList& ls, SgProject* proj) {
  for (size_t i = 0; i < ls.size(); ++i) {
    runPrereqs(ls[i], proj);
    //std::string preStr = ls[i]->name;
    //std::set<std::string>::const_iterator it=pre.find(preStr);
    //if (it==pre.end()) {
    //  pre.insert(preStr);
    //  std::cerr << " Running Prerequisite " << preStr <<  std::endl;
    //}
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

namespace Compass
{

  void
  print_parameters(const ParametersMap& parameters)
  {
    BOOST_FOREACH(const ParametersMap::value_type& parameter, parameters)
    {
        std::string values = boost::algorithm::join( parameter.second, ", " );
        std::cout << parameter.first << "=" << values << std::endl;
    }
  }

  bool
  IsNodeInUserLocation(const SgLocatedNode *const node, const std::string& path)
    {
      std::string filename = node->getFilenameString();

      StringUtility::FileNameClassification file_classification =
          StringUtility::classifyFileName(filename, path);

      StringUtility::FileNameLocation file_location =
          file_classification.getLocation();

      return StringUtility::FILENAME_LOCATION_USER == file_location;
    }

/*------------------------------------------------------------------------------
 * Parameters
 *----------------------------------------------------------------------------*/

ParametersMap
Parameters::operator[](const boost::regex &a_regex) const
    throw(ParameterNotFoundException)
  {
    Compass::ParametersMap parameters;
    BOOST_FOREACH(const ParametersMap::value_type& pair, _parameters)
    {
        std::string parameter_name  = pair.first;
        std::vector<std::string> parameter_values = pair.second;

        boost::sregex_iterator it(
            parameter_name.begin(), parameter_name.end(), a_regex);
        boost::sregex_iterator end;
        for ( ; it != end; ++it )
        {
            // std::cout << "Found: ";
            // std::string::const_iterator s = (*it)[0].first;
            // std::string::const_iterator e = (*it)[0].second;
            // for ( ; s != e; ++s )
            // {
            //     std::cout << *s;
            // }
            // std::cout << std::endl;

            parameters[parameter_name] = parameter_values;
        }
    }

    if (parameters.size() <= 0)
    {
        std::cerr
          << "[Compass] [Parameters] "
          << "ERROR: parameter not found: "
          << a_regex.str()
          << std::endl;
        throw ParameterNotFoundException(a_regex.str());
    }
    else
    {
        return parameters;
    }
  }

// TODO: throw error
Compass::ParameterValues
Parameters::operator[](const std::string &a_parameter_name) const
    throw(ParameterNotFoundException)
  {
      Compass::ParametersMap matches =
          this->operator[](boost::regex("^" + a_parameter_name + "$"));
      ROSE_ASSERT(matches.size() > 0);
      return matches[a_parameter_name];
  }

Compass::ParameterValues
Parameters::operator[](const char *a_parameter_name) const
    throw(ParameterNotFoundException)
  {
    ROSE_ASSERT(a_parameter_name != NULL);
    return this->operator[](std::string(a_parameter_name));
  }

std::string Parameters::get_unique(const std::string &a_parameter_name) const
    throw(ParameterNotFoundException)
  {
      Compass::ParameterValues values =
          this->operator[](a_parameter_name);
      ROSE_ASSERT(values.size() == 1);
      return values.front();
  }

namespace Xml
{
using namespace xercesc;

/*------------------------------------------------------------------------------
 * Utilities
 *----------------------------------------------------------------------------*/

  void
  Throw (const std::string a_title, const XMLCh* a_message) throw (Exception)
  {
    char* message = XMLString::transcode( a_message );
    Exception e(a_title + ": " + message);
    XMLString::release( &message );
    throw e;
  }

Compass::StringMap
getAttributes(const DOMElement* a_root)
  {
    ROSE_ASSERT( a_root != NULL );
    Compass::StringMap attribute_map;

    DOMNamedNodeMap* attributes = a_root->getAttributes();
    for (size_t i = 0; i < attributes->getLength(); i++)
    {
        DOMNode* attribute = attributes->item(i);
        std::string name = transcodeToStr( attribute->getNodeName() );
        std::string value = transcodeToStr( attribute->getNodeValue() );
        attribute_map[name] = boost::trim_copy( value );
    }

    return attribute_map;
  }

std::vector<DOMElement*>
getElementsByTagName (const DOMElement* a_root,
                      const char *const a_tagname)
  {
    ROSE_ASSERT( a_root != NULL && a_tagname != NULL );

    XMLCh* tagname = XMLString::transcode( a_tagname );
    DOMNodeList* elements = a_root->getElementsByTagName( tagname );
    XMLString::release( &tagname );

    std::vector<DOMElement*> children;

    for (size_t i = 0; i < elements->getLength(); i++)
    {
        DOMNode* node = elements->item(i);
        //if ( node->getNodeType() == DOMNode::ELEMENT_NODE )
        //{
            children.push_back( static_cast<DOMElement*>(node) );
        //}
    }

    return children;
  }

Xml::DOMElements
getChildElementsByTagName(const DOMElement* a_root,
                          const char* a_tagname)
  {
    ROSE_ASSERT( a_root != NULL && a_tagname != NULL );
    Xml::DOMElements elements;

    Xml::DOMElements children = getChildElements(a_root);
    BOOST_FOREACH(DOMElement* element, children)
    {
        std::string tagname = transcodeToStr( element->getTagName() );
        if ( tagname == a_tagname )
        {
            elements.push_back(element);
        }
    }

    // TODO: enable only if warn logging is requested.
    if (elements.size() <= 0)
    {
        std::cout << "Warning: No elements detected for ";
        std::cout << "<" << a_tagname << ">.";
        std::cout << std::endl;
    }

    return elements;
  }

std::vector<DOMElement*>
getChildElements (const DOMElement* a_root)
  {
    ROSE_ASSERT( a_root != NULL );

    std::vector<DOMElement*> children;

    DOMNodeList* groups = a_root->getChildNodes();
    for (size_t i = 0; i < groups->getLength(); i++)
    {
        DOMNode* node = groups->item(i);
        if ( node->getNodeType() == DOMNode::ELEMENT_NODE )
        {
            children.push_back( static_cast<DOMElement*>(node) );
        }
    }

    return children;
  }

std::map<DOMElement*, std::vector<DOMElement*> >
getChildElements (std::vector<DOMElement*> a_roots)
  {
    std::map<DOMElement*, std::vector<DOMElement*> > child_map;

    std::vector<DOMElement*>::iterator it;
    for ( it = a_roots.begin(); it != a_roots.end(); it++ )
    {
        std::vector<DOMElement*> children = getChildElements( *it );
        child_map[ *it ] = children;
    }

    return child_map;
  }

std::string
transcodeToStr (const XMLCh* a_xch)
  {
    ROSE_ASSERT( a_xch != NULL );
    char* name = XMLString::transcode(a_xch);
    std::string str(name);
    XMLString::release(&name);
    return str;
  }

void
stat_file (const char *const a_filename) throw(std::runtime_error)
  {
    struct stat file_status;
    int status = stat("a", &file_status);

    if( status == ENOENT )
        throw ( std::runtime_error("Path file_name does not exist, or path is an empty string.\n") );
    else if( status == ENOTDIR )
        throw ( std::runtime_error("A component of the path is not a directory.\n"));
    else if( status == ELOOP )
        throw ( std::runtime_error("Too many symbolic links encountered while traversing the path.\n"));
    else if( status == EACCES )
        throw ( std::runtime_error("Permission denied.\n"));
    else if( status == ENAMETOOLONG )
      throw ( std::runtime_error("File can not be read.\n"));
  }

DOMLSParser*
create_parser (XMLGrammarPool* pool)
{
  const XMLCh ls_id [] = {chLatin_L, chLatin_S, chNull};

  DOMImplementation* impl (
    DOMImplementationRegistry::getDOMImplementation (ls_id));

  DOMLSParser* parser (
    impl->createLSParser (
      DOMImplementationLS::MODE_SYNCHRONOUS,
      0,
      XMLPlatformUtils::fgMemoryManager,
      pool));

  DOMConfiguration* conf (parser->getDomConfig ());

  // Commonly useful configuration.
  //
  conf->setParameter (XMLUni::fgDOMComments, false);
  conf->setParameter (XMLUni::fgDOMDatatypeNormalization, true);
  conf->setParameter (XMLUni::fgDOMEntities, false);
  conf->setParameter (XMLUni::fgDOMNamespaces, true);
  conf->setParameter (XMLUni::fgDOMElementContentWhitespace, false);

  // Enable validation.
  //
  conf->setParameter (XMLUni::fgDOMValidate, true);
  conf->setParameter (XMLUni::fgXercesSchema, true);
  conf->setParameter (XMLUni::fgXercesSchemaFullChecking, false);

  // Use the loaded grammar during parsing.
  //
  conf->setParameter (XMLUni::fgXercesUseCachedGrammarInParse, true);

  // Don't load schemas from any other source (e.g., from XML document's
  // xsi:schemaLocation attributes).
  //
  conf->setParameter (XMLUni::fgXercesLoadSchema, false);

  // Xerces-C++ 3.1.0 is the first version with working multi
  // import support.
  //
#if _XERCES_VERSION >= 30100
  conf->setParameter (XMLUni::fgXercesHandleMultipleImports, true);
#endif

  // We will release the DOM document ourselves.
  //
  conf->setParameter (XMLUni::fgXercesUserAdoptsDOMDocument, true);

  return parser;
}

/*------------------------------------------------------------------------------
 * Parser
 *----------------------------------------------------------------------------*/

bool ErrorHandler::
handleError (const xercesc::DOMError& e)
  {
    bool warn (e.getSeverity () == DOMError::DOM_SEVERITY_WARNING);

    if (!warn)
      _failed = true;

    DOMLocator* loc (e.getLocation ());

    std::string uri = transcodeToStr (loc->getURI ());
    std::string msg = transcodeToStr (e.getMessage ());
    std::stringstream ss;
    ss << uri                     << ":"
       << loc->getLineNumber ()   << ":"
       << loc->getColumnNumber () << " "
       << (warn ? "warning: " : "error: ")
       << msg
       << std::endl;
    set_message (ss.str ());

    return true;
  }

Parser::~Parser ()
  {
    if (_dom_document)
    {
        _dom_document->release ();
    }
    XMLPlatformUtils::Terminate ();
  }

DOMElement*
Parser::getDocumentElement() const
  {
    return get_document ()->getDocumentElement ();
  }

DOMDocument*
Parser::get_document () const
  {
    return _dom_document;
  }

Parser::Parser (const std::string& a_filename)
  {
    init (a_filename, "");
  }

Parser::Parser (const std::string& a_filename,
                const std::string& a_grammar_filename)
  {
    init (a_filename, a_grammar_filename);
  }


void Parser::init (const std::string& a_filename,
                   const std::string& a_grammar_filename)
  {
    ROSE_ASSERT (a_filename.empty () == false);

    _dom_document = 0;
    set_filename (a_filename);
    set_grammar_filename (a_grammar_filename);
    XMLPlatformUtils::Initialize ();

    // ---------------------------------
    // Parse and load the Grammar
    // ---------------------------------
    MemoryManager* memory_manager (XMLPlatformUtils::fgMemoryManager);
    XMLGrammarPool* grammar_pool (new XMLGrammarPoolImpl (memory_manager));

    DOMLSParser* parser (create_parser (grammar_pool));

    ErrorHandler error_handler;
    parser->getDomConfig ()->setParameter (XMLUni::fgDOMErrorHandler, &error_handler);

    if (! a_grammar_filename.empty ())
    {
        if (!parser->loadGrammar (
              get_grammar_filename ().c_str (), Grammar::SchemaGrammarType, true))
        {
            throw Compass::ParameterException (
                "XSD: error: unable to load grammar " + get_grammar_filename ());
        }

        if (error_handler.failed ())
        {
            throw Compass::ParameterException (error_handler.get_message ());
        }

        // Lock the grammar pool. This is necessary if we plan to use the
        // same grammar pool in multiple threads (this way we can reuse the
        // same grammar in multiple parsers). Locking the pool disallows any
        // modifications to the pool, such as an attempt by one of the threads
        // to cache additional schemas.
        grammar_pool->lockPool ();
    }
    else
    {
        std::cout << "No grammar file!" << std::endl;
    }

    // ---------------------------------
    // Parse the XML file
    // ---------------------------------
    std::cout << "Loading parameters from " << get_filename () << std::endl;
    _dom_document = parser->parseURI (get_filename ().c_str ());
    if (error_handler.failed ())
    {
        throw Compass::ParameterException (error_handler.get_message ());
    }

    parser->release ();
  }

/**
  * Order of evaluation:
  *
  *   1. ENV (environment)
  *   2. CWD (current working directory)
  *   3. Install prefix
  *   4. Local source directory
  */
std::string
Parser::locate_xml_file (const std::string& a_filename, const std::string& a_env_name) const
  {
    boost::filesystem::path filepath (a_filename);

    // Environment variable - path or filename:
    //  /path/to/xml/dir/containing/compass_parameters.xsd/
    //  /path/to/compass_parameters.xsd
    //  compass_parameters.xsd
    if (getenv (a_env_name.c_str ()) != NULL)
    {
        filepath = getenv (a_env_name.c_str ());
    }

    // Check if the current path exists
    {
        if (boost::filesystem::is_regular_file (filepath))
        {
            return filepath.string ();
        }
        else if (boost::filesystem::is_directory (filepath))
        {
            boost::filesystem::path file (filepath / a_filename);
            if (boost::filesystem::is_regular_file (file))
            {
                return file.string ();
            }
        }
    }

    // Check in the current working directory (cwd)
    {
        boost::filesystem::path prefix (boost::filesystem::current_path ());
        if (boost::filesystem::is_regular_file (prefix / filepath))
            return (prefix / filepath).string ();
    }

    #ifdef PREFIX_COMPASS2_XML_DIR
    // Check for installed version
    {
        boost::filesystem::path prefix (PREFIX_COMPASS2_XML_DIR);
        if (boost::filesystem::is_regular_file (prefix / filepath))
            return (prefix / filepath).string ();
    }
    #endif

    #ifdef SOURCE_COMPASS2_XML_DIR
    // Check for local source version
    {
        boost::filesystem::path prefix (SOURCE_COMPASS2_XML_DIR);
        if (boost::filesystem::is_regular_file (prefix / filepath))
            return (prefix / filepath).string ();
    }
    #endif

    ROSE_ASSERT (!"Error: compass XSD validation file could not be located!");
  }

/*------------------------------------------------------------------------------
 * ParametersParser
 *----------------------------------------------------------------------------*/

// TODO: check for duplicates.
Compass::Parameters
ParametersParser::parse_parameters ()
  {
    ParametersMap parameters;

    // General parameters
    //
    // <general>
    //   <parameter>...</parameter>
    //   ...
    //   ...
    // </general>

    DOMElements general_elements =
        getChildElementsByTagName(getDocumentElement(), Xml::Tag::GENERAL);
    ROSE_ASSERT(general_elements.size() <= 1); // grammar validated
    BOOST_FOREACH(DOMElement* general, general_elements)
    {
        ParametersMap general_parameters =
            parse_parameters(general, Xml::Tag::GENERAL);
        //print_parameters(general_parameters);

        parameters.insert(general_parameters.begin(),
                          general_parameters.end());
    }

    // Checker-specific parameters
    //
    // <checkers>
    //   <checker>
    //     <parameter>...</parameter>
    //     ...
    //   </checker>
    //   ...
    //   ...
    // </checkers>

    DOMElements checkers_elements =
        getChildElementsByTagName(getDocumentElement(), Xml::Tag::CHECKERS);
    ROSE_ASSERT(checkers_elements.size() <= 1); // grammar validated
    BOOST_FOREACH(DOMElement* checkers, checkers_elements)
    {
        DOMElements checker_elements =
            getChildElementsByTagName(checkers, Xml::Tag::CHECKER);
        BOOST_FOREACH(DOMElement* checker, checker_elements)
        {
            std::map<std::string, std::string> attributes =
              getAttributes(checker);

            std::string checker_name = attributes["name"];
            ParametersMap checkers_parameters =
                parse_parameters(checker, checker_name.c_str());
            //print_parameters(checkers_parameters);

            parameters.insert(checkers_parameters.begin(),
                              checkers_parameters.end());
        }
    }

    return Parameters(parameters);
  }

ParametersMap
ParametersParser::parse_parameters (DOMElement* root,
    const std::string& group_name)
  {
    ROSE_ASSERT (root != NULL);
    ParametersMap r_parameters;

    // <root>
    //   <parameter>
    //   ...
    // </root>

    DOMElements dom_parameters =
        getChildElementsByTagName(root, Xml::Tag::PARAMETER);

    if (dom_parameters.size() <= 0)
    {
        std::cout << "Warning: no parameters under "
                  << "<"
                  << transcodeToStr (root->getTagName ())
                  << ">"
                  << std::endl;
    }

    BOOST_FOREACH(DOMElement* parameter, dom_parameters)
    {
        // Value

        DOMNode* text = parameter->getFirstChild();
        std::string value =
            boost::trim_copy(transcodeToStr(text->getNodeValue()));

        // Attributes

        StringMap attributes = Xml::getAttributes(parameter);
        if (attributes.size() > 1)
        {
            std::cout << "Error: expected only 1 attribute for parameter ";
            std::cout << value << std::endl;
            throw;
        }
        else
        {
            StringMap::iterator name_it = attributes.find("name");
            if (name_it == attributes.end())
            {
                std::cout << "Error: name attribute not found in parameter.";
                std::cout << std::endl;
                throw;
            }
            else
            {
                std::string name = group_name + "::" + name_it->second;
                r_parameters[name].push_back(value);
            }
        }
    }

    return r_parameters;
  }
} // namespace Xml
} // namespace Compass

