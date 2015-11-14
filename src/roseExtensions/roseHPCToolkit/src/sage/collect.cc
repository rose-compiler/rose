// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/sage/collect.cc
 *  \brief Implements Sage III node collection utilities.
 *  \ingroup ROSEHPCT_SAGE
 *
 *  $Id: collect.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#include "sage3basic.h"

#include "rosehpct/sage/sage.hh"
#include "rosehpct/util/general.hh"
#include <boost/foreach.hpp>

using namespace std;
using namespace RoseHPCT;

/* ---------------------------------------------------------------- */
FileCollector::FileCollector (void)
  : filename_ ("")
{
}

FileCollector::FileCollector (const string& filename)
  : filename_ (filename)
{
}

void
FileCollector::setFilename (const string& filename)
{
  filename_ = filename;
}

bool
FileCollector::matches (SgGlobal* node) const
{
  if (node == NULL) return false;

  string sg_fullpath = RoseHPCT::getFilename (node);
  string sg_dir = GenUtil::getDirname (sg_fullpath);
  string sg_file = GenUtil::getBaseFilename (sg_fullpath);

  string target_dir = GenUtil::getDirname (filename_);
  string target_file = GenUtil::getBaseFilename (filename_);

  return sg_dir == target_dir && sg_file == target_file;
}

/* ---------------------------------------------------------------- */

FuncCollectorByLoc::FuncCollectorByLoc (void)
  : funcname_ ("")
{
}

FuncCollectorByLoc::FuncCollectorByLoc (const string& filename,
					const string& funcname,
					size_t b, size_t e)
  : NodeCollectorByLoc<SgFunctionDefinition> (filename, b, e),
    funcname_ (funcname)
{
}

void
FuncCollectorByLoc::setFuncname (const std::string& funcname)
{
  funcname_ = funcname;
}

bool
FuncCollectorByLoc::matches (SgFunctionDefinition* node) const
{
  if (node == NULL) return false;

  SgFunctionDeclaration* decl = node->get_declaration ();
  if (decl == NULL) return false;

  const char* funcname = decl->get_name ().str ();
  return funcname == funcname_ && matchesLoc (node);
}

/*!
 *  \brief
 *
 *  Overrides default behavior, returning true <==> the
 *  caller-specified lines _overlap_ with the current line numbers.
 */
bool
FuncCollectorByLoc::linesMatch (size_t sg_start, size_t sg_end) const
{
  size_t cur_start = getLineStart ();
  size_t cur_end = getLineEnd ();
  return (sg_start <= cur_start && cur_end <= sg_end)
    || (cur_start <= sg_start && sg_end <= cur_end)
    || (cur_start <= sg_start && sg_end <= cur_end)
    || (sg_start <= cur_start && cur_end <= sg_end);
}

/* ---------------------------------------------------------------- */

StmtCollectorByLoc::StmtCollectorByLoc (void)
{
}

StmtCollectorByLoc::StmtCollectorByLoc (const string& filename,
					size_t b, size_t e)
  : NodeCollectorByLoc<SgStatement> (filename, b, e)
{
}

bool
StmtCollectorByLoc::matches (SgStatement* node) const
{
  /* Ignore some classes of SgStatements */
  if (isSgScopeStatement (node) != NULL)
    return false;

#if DO_SPECIAL_CASES_STMT
  /* Some special cases */
  if (matches (isSgForInitStatement (node)))
    return true;
#endif

  /* Ok to process */
  return matchesLoc (node);
}

bool
StmtCollectorByLoc::matches (const SgForInitStatement* node) const
{
  if (node == NULL) return false;

  const SgForStatement* loop = isSgForStatement (node->get_parent ());
  if (loop == NULL) return false;

  /* "Derive" line numbers for this node by taking the starting line
   * of the surrounding loop as the starting line, and the starting
   * line of the body minus 1 as the ending line.
   */
  string filename = RoseHPCT::getFilename (node);
  int line_start = RoseHPCT::getLineStart (loop);
  int line_end = RoseHPCT::getLineEnd (loop->get_loop_body ()) - 1;

  if (line_end < line_start) line_end = line_start;

  return filenamesMatch (filename) && linesMatch (line_start, line_end);
}

/* ---------------------------------------------------------------- */

ScopeStmtCollectorByLoc::ScopeStmtCollectorByLoc (void)
{
}

ScopeStmtCollectorByLoc::ScopeStmtCollectorByLoc (const string& filename,
						  size_t b, size_t e)
  : NodeCollectorByLoc<SgScopeStatement> (filename, b, e)
{
}

bool
ScopeStmtCollectorByLoc::matches (SgScopeStatement* node) const
{
  return matchesLoc (node);
}

/* ---------------------------------------------------------------- */

LoopCollectorByLoc::LoopCollectorByLoc (void)
{
}

LoopCollectorByLoc::LoopCollectorByLoc (const string& filename,
					size_t b, size_t e)
  : ScopeStmtCollectorByLoc (filename, b, e)
{
}

bool
LoopCollectorByLoc::matches (SgScopeStatement* node) const
{
  return (isSgWhileStmt (node) != NULL
          || isSgDoWhileStmt (node) != NULL
          || isSgForStatement (node) != NULL)
    ? matchesLoc (node)
    : false;
}

void Vis_PreOrder::visitDefault(SgNode* pNode)
{
   int childNum = pNode->get_numberOfTraversalSuccessors ();
   pNode->accept(*_vis);  // process the parent node
   for (int i = 0; i < childNum; i++)
   {
      SgNode* pChild = pNode->get_traversalSuccessorByIndex (i);
      if (pChild)
      {
         pChild->accept(*this);  //recursively visit and process the child node
       }
   }
}

void Vis_PrintMetricInfo::printFileLoc(SgLocatedNode* node, bool printFlag)
{
    if (printFlag)
    {
        string class_name = string("(") + node->sage_class_name() + string(")");
        Sg_File_Info* info_start = node->get_startOfConstruct ();
        Sg_File_Info* info_end = node->get_endOfConstruct ();
        string full_filename = info_start->get_filenameString();
        string base_filename = GenUtil::getBaseFilename(full_filename);
        int line_start = info_start->get_line();
        int line_end = info_end->get_line();
        os_ << "@ [" << GenUtil::toFileLoc(base_filename, line_start, line_end) << " "  << class_name << "]" << endl;
   }

}

void Vis_PrintMetricInfo::printAttributes(SgLocatedNode* node)
{
      AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
      bool printFlag = true;
      if (astAttributeContainer != NULL)
      {
        BOOST_FOREACH (const std::string &name, astAttributeContainer->getAttributeIdentifiers())
        {
           RoseHPCT::MetricAttr* attribute = dynamic_cast<RoseHPCT::MetricAttr *>(astAttributeContainer->operator[](name));
           // DXN: If an IR node is written to file, its AstAttribute objects somehow lose their type info.
           // For example, MetricAttr objects are no longer MetricAttr.
           // RPM: This is perhaps because many AstAttribute subclasses don't properly implement the virtual copy method.
           /// AstAttribute* attribute = i->second;  // may have names like "tree_depth" and "AstUnparseAttribute"
           if (attribute)
           {
               printFileLoc(node, printFlag);
               printFlag = false;
               os_   << "     " << name  << " = "<< attribute->toString() << endl;
           }
        }
      }
}

/* eof */
