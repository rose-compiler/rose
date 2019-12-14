// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \defgroup ROSEHPCT_LIBXML2  ROSE-HPCT XML processing based on libxml2
 *  \ingroup ROSEHPCT
 *
 *  This submodule creates the raw representation of the HPCToolkit
 *  profile data as stored in the XML file. This implementation
 *  specifically uses the Gnome libxml2 library to parse the XML.
 *
 *  See also, \ref ROSEHPCT_XERCESC .
 *
 *  $Id: xml.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */
/*@{*/

/*!
 *  \file rosehpct/xml/xml.hh
 *  \brief XML parser for HPCToolkit profile data.
 *  \ingroup ROSEHPCT_LIBXML2
 *
 *  $Id: xml.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#if !defined(INC_XML_LIBXML2_XML_HH)
//! xml/xml.hh included
#define INC_XML_LIBXML2_XML_HH

#include <ostream>
#include <vector>
#include <stack>
#include <functional>

#include <rosehpct/util/tree2.hh>
#include <rosehpct/util/general.hh>

#include <libxml/parser.h>

/*!
 *  \brief XML processing support routines, which are mostly wrappers
 *  around the Gnome libxml2 library.
 */
namespace XMLUtil
{
  //! \addtogroup ROSEHPCT_LIBXML2
  //@{

  //! Rename libxml2 character type
  typedef xmlChar XMLCh;
  //! Rename libxml2 raw XML tag attributes list
  typedef xmlChar** AttributeList;
  //! Rename libxml2 raw XML tag attributes list (read-only)
  typedef const xmlChar** ConstAttributeList;

  //! Convert an XMLCh string to a C++ string
  std::string toString (const XMLCh* const s);

  //! Output an XMLCh string to an ostream.
  std::ostream& operator<< (std::ostream& o, const XMLCh* const s);

  //! Value class representing an XML element attribute.
  struct XMLAttr
  {
    std::string name; //!< Name of the attribute
    std::string type; //!< Attribute type
    std::string value; //!< Value of the attribute

    //! Construct XML attribute from a name, attribute type, and value.
    XMLAttr (const std::string& n, const std::string& t,
             const std::string& v);
    //! Copy constructor.
    XMLAttr (const XMLAttr& a);

    //! Function to match an attribute by name
    struct MatchName
      : public std::unary_function <XMLAttr, bool>
    {
      std::string name; //!< Name to match 
      MatchName (const std::string& n); //!< Construct with name to match 
      bool operator() (const XMLAttr& a); //!< Ask if a matches 
    };
  };

  //! Value class representing an XML element tag
  struct XMLElem
  {
    std::string name; //!< Tag name 
    std::vector<XMLAttr> attributes; //!< List of attributes 

    //! Construct an empty XML element
    XMLElem (void);
    //! Construct an XML element with the specified name and attribute list.
    XMLElem (const std::string& n, ConstAttributeList attr);
    //! Construct a named XML element with no attributes.
    XMLElem (const std::string& n);
    //! Copy constructor
    XMLElem (const XMLElem& e);

    //! Retrieve the value of an attribute by name
    std::string getAttr (const std::string & n) const;

    //! Function to match an element by name
    struct MatchName
      : public std::unary_function <XMLElem, bool>
    {
      std::string name; //!< Name to match 
      MatchName (const std::string& n); //!< Construct with name to match 
      bool operator() (const XMLElem& e); //!< Ask if e matches 
    };
  };

  /*!
   *  \brief Implements an XML parser callback.
   *
   *  XMLHandler is a helper class for XMLParser that defines various
   *  call-back routines to construct a generic tree representation of
   *  the XML document.
   *
   *  This tree is an instantiation of the template type, Tree<T> in
   *  util/tree2.hh. The node values are of type XMLHandler::Elem.
   */
  class XMLHandler
  {
  public:
    //! Initialize an XMLHandler.
    XMLHandler (void);
    //! Clean-up.
    ~XMLHandler (void);

    //! Call-back when a new element is encountered.
    void startElement (const XMLCh* const name, ConstAttributeList attr);
    //! Call-back before next element processed.
    void endElement (const XMLCh* const name);

    //! \name XML element tree.
    /*@{*/
    typedef Trees::Tree<XMLElem> node_t; //!< Tree node type. 
    //! Returns root of the parsed tree, or NULL if an error occurred.
    const node_t* getRoot (void) const;
    /*@}*/

  private:
    node_t* root_; //!< Root of the parsed XML tree. 
    std::stack<node_t *> elements_; //!< List of elements being processed. 
    size_t level_; //!< Current depth in the tree. 
  };

  //! Output an element value.
  std::ostream& operator<< (std::ostream& o,
                            const XMLElem& e);

  //! Output an element tree node.
  std::ostream& operator<< (std::ostream& o,
                            const XMLHandler::node_t* n);

  /*!
   *  \brief XML parsing class.
   *
   *  An object of type XMLParser uses Xerces-C to parse an XML
   *  document. The parser uses XMLHandler to create an XML element
   *  tree whose nodes are of type TreeNode<XMLHandler::Elem>. See
   *  XMLHandler for more details on the structure of this tree.
   */
  class XMLParser
  {
  public:
    //! Initialize
    XMLParser (void);

    //! Initialize and parse the specified XML input file.
    XMLParser (const char* infile);

    //! Initialize and parse the specified XML input file.
    XMLParser (const std::string& infile);

    //! Clean-up
    virtual ~XMLParser (void);

    /*!
     *  \name Parses the specified XML input file and returns true on
     *  success.
     */
    /*@{*/
    virtual bool parse (const char* infile);
    virtual bool parse (const std::string& infile);
    /*@}*/

    //! Returns true <==> last document parsed successfully.
    bool didParse (void) const;

    /*!
     *  Returns the root node of the last XML document successfully
     *  parsed, or NULL on error.
     */
    const XMLHandler::node_t* getDoc (void) const;

  protected:
    //! Initialize
    virtual void init (void);

  private:
    static bool did_init_; //!< true <==> Xerces-C is initialized 
    bool did_parse_; //!< true <==> last document parsed successfully 

    //! \name Parser.
    /*@{*/
    xmlDocPtr doc_;
    XMLHandler* handler_;
    /*@}*/
  };

  /*!
   *  \brief Support class for an XML tree traversal to locate elements.
   *
   *  This class implements an 'iterator' over nodes in an XML tree
   *  for a particular type of element.
   *
   *  \todo Current implementation seeks an exact name match
   *  (including case); expand so that clients can search for elements
   *  using a regular expression.
   */
  class XMLElemFinder : public Trees::PreTraversal<XMLHandler::node_t>
  {
  public:
    //! XML element container.
    typedef std::vector<XMLElem> ElemList_t;

    //! Initialize
    XMLElemFinder (void);
    /*!
     *  Initialize to find elements of a particular name, in a
     *  specified document.
     */
    XMLElemFinder (const std::string& elem_name,
                   const XMLHandler::node_t* doc = NULL);

    virtual ~XMLElemFinder (void) {}

    //! Request elements matching the specified name.
    void setElemNamePattern (const std::string& elem_name);

    //! Call-back to visit the next node in the tree.
    virtual void visit (const XMLHandler::node_t* cur_node);

    //! \name Iterator-like semantics for matched-element traversal.
    /*@{*/
    ElemList_t::const_iterator begin (void) const;
    ElemList_t::const_iterator end (void) const;
    size_t getNumMatches (void) const;
    /*@}*/

  private:
    std::string pattern_elem_name_; //!< Element name to seek. 
    ElemList_t matches_; //!< List of current matches. 
  };

  //@}
}

#endif

/*@}*/
/* eof */
