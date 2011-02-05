// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \defgroup ROSEHPCT_XML2PROFIR ROSE-HPCT XML-to-ProfIR conversion
 *  \ingroup ROSEHPCT
 *
 *  This submodule implements conversion from the raw, in-memory
 *  XML tree representation to the profile IR representation.
 *
 *  $Id: xml2profir.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*!
 *  \file rosehpct/xml2profir/xml2profir.hh
 *  \brief Module to convert a parsed XML HPCToolkit profile tree into
 *  an RoseHPCTIR tree.
 *  \ingroup ROSEHPCT_XML2PROFIR
 *
 *  $Id: xml2profir.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#if !defined(INC_XML2PROFIR_XML2PROFIR_HH)
//! rosehpct/xml2profir/xml2profir.hh included.
#define INC_XML2PROFIR_XML2PROFIR_HH

#include <string>
#include <map>
#include <vector>

#include <rosehpct/xml/xml.hh>
#include <rosehpct/profir/profir.hh>

namespace RoseHPCT
{
  //! \addtogroup ROSEHPCT_XML2PROFIR
  //@{

  //! Parsed HPCToolkit XML profile tree node.
  typedef XMLUtil::XMLHandler::node_t XMLNode_t;

  /*!
   *  \brief Defines basic query operations on the XML representation
   *  of HPCToolkit profile data.
   */
  namespace XMLQuery
  {
    //! \name Basic XML tree query routines.
    /*@{*/
    //! Returns the version of the XML file.
    std::string getVersion (const XMLNode_t* doc);
    //! Returns a lookup table of short-to-long metric names.
    std::map<std::string, std::string> getMetricTable (const XMLNode_t* doc);
    //! Return all statement elements in the XML tree.
    std::vector<XMLUtil::XMLElem> getStatements (const XMLNode_t* doc);
    /*@}*/
  }

  //! Creates an RoseHPCTIR tree from an XML HPCToolkit profile tree.
  IRTree_t* convertXMLToProfIR (const XMLNode_t* prog_root);

  //! List of filenames.
  typedef std::vector<std::string> FilenameList_t;

  //! List of profile IR trees.
  typedef std::vector<RoseHPCT::IRTree_t *> ProgramTreeList_t; 

  /*!
   *  Parses a list of XML HPCToolkit profile files, and returns a
   *  list of the corresponding program tree nodes.
   */
  ProgramTreeList_t loadXMLTrees (const FilenameList_t& filenames);

  //! Equivalent paths mapping
  typedef std::map<std::string, std::string> EquivPathMap_t;

  //! Fixup file pointer, calculate percentage, replace path names using the translation map.
  bool postProcessingProfIR(IRTree_t* root, const EquivPathMap_t& eqpaths);

  //! Replace short-hand metric names with full names.
  bool translateMetricNames (IRTree_t* root,
                             const std::map<std::string, std::string>& mets);

  /*! \brief Scan a program tree and return a list of all unique
    metric names. */
  void collectMetricNames (const IRTree_t* root,
                           std::map<std::string, size_t>& metrics);

  //! Function that creates an IRNode from an XMLElem
  typedef IRNode * (*IRNodeCreator_t) (const XMLUtil::XMLElem&);

  /*!
   *  \brief XMLElem-to-RoseHPCTIR node factory.
   */
  class ProfIRFactory
    : public Types::Factory<IRNode, std::string, IRNodeCreator_t>
  {
  public:
    ProfIRFactory (void);
    void registerDefaults (void);
    void setElement (const XMLUtil::XMLElem& e);

  protected:
    virtual IRNode* callCreate (const std::string& id,
                                IRNodeCreator_t creator) const;

  private:
    XMLUtil::XMLElem element_; // argument to pass to create routine
  };

  //! Singleton factory type
  typedef Types::SingletonHolder<ProfIRFactory> ProfIRFactory_t;

  //@}
}

#endif

/* eof */
