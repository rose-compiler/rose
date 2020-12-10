/**
 *  \file PragmaInterface.cc
 *  \brief A source-level, pragma-based interface to the outliner.
 *
 *  This module implements a high-level wrapper that permits a user to
 *  insert "#pragma rose_outline" statements into the body of the
 *  source, thereby directing what to outline at the source-level.
 *
 *  For Fortran codes, using a special comment, !$rose_outline, 
 *  for the same purpose 
 *
 *  \todo Extend this interface.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <string>


#include "Outliner.hh"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"
#include <boost/algorithm/string/trim.hpp>
#include "abstract_handle.h"
#include "roseAdapter.h"

//! Simplest outlining directives, applied to a single statement.
static const std::string PRAGMA_OUTLINE ("rose_outline");

//! Stores a list of valid outlining pragma directives.
typedef Rose_STL_Container<SgPragmaDeclaration *> PragmaList_t;
//! Stores a list of valid outlining targets, used only for Fortran for now
typedef Rose_STL_Container<SgStatement*> TargetList_t;

// =====================================================================

using namespace std;
using namespace AbstractHandle;

// =====================================================================

/*!
 *  \brief Check whether the specified pragma is an outlining
 *  directive.
 *
 *  This routine checks whether the specified pragma is an outlining
 *  directive, and if so, returns the statement that should be
 *  outlined. Returns NULL if the pragma is not an outlining directive
 *  or no such statement exists.
 */
static
SgStatement *
processPragma (SgPragmaDeclaration* decl)
{
  if (!decl || !decl->get_pragma ())
    return 0;

  string pragmaString = decl->get_pragma ()->get_pragma ();
  if (pragmaString != PRAGMA_OUTLINE) // Not an outlining pragma.
    return 0;
    
  // Get statement to outline
  return const_cast<SgStatement *> (SageInterface::getNextStatement(decl));
  //return const_cast<SgStatement *> (ASTtools::findNextStatement (decl));
}

/*!
 *  \brief Check whether the specified comment is an outlining
 *  directive for Fortran: one of the following three cases: 
 *    !$rose_outline
 *    c$rose_outline
 *    *$rose_outline
 *  This routine checks whether the specified source comment an outlining
 *  directive, and if so, returns the statement that should be
 *  outlined. Returns NULL if the comment is not an outlining directive
 *  or no such statement exists.
 *  Liao, 12/19/2008
 */
static
SgStatement *
processFortranComment(SgLocatedNode* node)
{
  SgStatement* target = NULL;
  ROSE_ASSERT(node);
  AttachedPreprocessingInfoType *comments =
    node->getAttachedPreprocessingInfo ();
  if (comments==NULL)
    return 0;
  AttachedPreprocessingInfoType::iterator i;
  std::vector< PreprocessingInfo* > removeList;
  for (i = comments->begin (); i != comments->end (); i++)
  { 
    if ((*i)->getTypeOfDirective() == PreprocessingInfo::FortranStyleComment)
    {
      string commentString = (*i)->getString();
      boost::algorithm::trim(commentString);
      if (   (commentString == "!$"+PRAGMA_OUTLINE) 
          || (commentString == "c$"+PRAGMA_OUTLINE) 
          || (commentString == "*$"+PRAGMA_OUTLINE)) 
      {
        target = isSgStatement(node);
        if (target==NULL)
        {
          cerr<<"Unhandled case when a Fortran statement is attached to a non-statement node!!"<<endl;
          ROSE_ASSERT(false);
        }
        removeList.push_back(*i);
      }
    } // end if Fortran comment
  } // end for-loop

  // remove those special comments
  for (std::vector<PreprocessingInfo* >::iterator j = removeList.begin();
       j!=removeList.end();j++)
  {
    comments->erase(find(comments->begin(), comments->end(),*j));

 // DQ (2/27/2009): Don't mix free with C++ code (malloc/free for C and new/delete for C++)
 // free memory also
 // free(*j);
    delete(*j);
  }

  return target; // const_cast<SgStatement *> (target);
}


/* =====================================================================
 *  Main routine to outline a single statement immediately following
 *  an outline directive (pragma).
 */

Outliner::Result
Outliner::outline (SgPragmaDeclaration* decl)
   {
     SgStatement* s = processPragma (decl);
     if (!s)
          return Result ();
//cout<<"Debug: Outliner::outline(), outlining target is:"<<s<<endl;
  // Generate outlined function, removing 's' from the tree.
     string name = generateFuncName (s);
     Result result = outline (s, name);
     if (!preproc_only_)
     {
       ROSE_ASSERT (result.isValid());

       // DQ (2/26/2009): If this was sucessful, then delete the input block? (this may be a problem for moreTest1.c).
       // SageInterface::deleteAST(s);

       // Remove pragma
       ASTtools::moveBeforePreprocInfo (decl, result.call_);
       ASTtools::moveAfterPreprocInfo (decl, result.call_);

#if 1
       // This will search the parent for the location of decl, but this is not found
#ifndef _MSC_VER
	   LowLevelRewrite::remove (decl);
#else
	   ROSE_ASSERT(false);
#endif
#else
       // DQ (2/24/2009): Added more direct concept of remove.
       // We just want a more direct and simpler concept of remove (remove the AST, 
       // because potential dangling pointers have been taken care of).
       SageInterface::deleteAST(decl);
#endif
     }
     return result;
   }

SgBasicBlock *
Outliner::preprocess (SgPragmaDeclaration* decl)
{
  SgStatement* s = processPragma (decl);
  if (s)
    return preprocess (s);
  else
    return 0;
}


// =====================================================================

/*!
 *  \brief Collects all outlining pragmas.
 *
 * This routine scans the given project for all outlining pragmas, and
 * returns them in the order in which they should be processed.
 *
 * The ordering is important because neither preorder nor postorder
 * tree traversals yield the desired bottomup processing for outlining
 * pragmas. To see why, consider the following code example:
 *
 * \code
 * #pragma rose_outline
 * {
 *   #pragma rose_outline
 *   statement1;
 * }
 * \endcode
 *
 * The corresponding AST is:
 *
 *   SgBasicBlock1
 *     /      \
 *    /        \
 * SgPragma1  SgBasicBlock2
 *              /      \
 *          SgPragma2  SgStatement1
 *
 * The standard traversal orders are:
 *
 * - Preorder: bb1, pragma1, bb2, pragma2, stmt1
 * - Postorder: pragma1, pragma2, stmt1,bb2, bb1
 *
 * In both cases, pragma1 is always visited before pragma2.
 *
 * The routine obtains a "correct" ordering by using the default
 * preorder AST query and then reversing the results.  In this we, we
 * obtain the ordering:
 *
 * - stmt1, pragma2, bb2,pragma1, bb1
 */
static size_t
collectPragmas (SgProject* proj, PragmaList_t& pragmas)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t raw_list = NodeQuery::querySubTree (proj, V_SgPragmaDeclaration);
  size_t count = 0;
  for (NodeList_t::reverse_iterator i = raw_list.rbegin ();
       i != raw_list.rend (); ++i)
    {
      SgPragmaDeclaration* decl = isSgPragmaDeclaration (*i);
      if (processPragma (decl))
        {
          pragmas.push_back (decl);
          ++count;
        }
    }
  return count;
}
//! Collect target Fortran statement with matching comments for rose_outline
// Save them into targetList
static size_t
collectFortranTarget (SgProject* proj, TargetList_t& targets)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t raw_list = NodeQuery::querySubTree (proj, V_SgStatement);
  size_t count = 0;
  for (NodeList_t::reverse_iterator i = raw_list.rbegin ();
      i != raw_list.rend (); ++i)
  {
    SgStatement* decl = isSgStatement(*i);
    if (processFortranComment(decl))
    {
      targets.push_back (decl);
      ++count;
    }
  }
  return count;
}

// Collect OpenMP for loops in the AST
// The assumption is that OpenMP parsing and AST creation is already turned on. 
// The AST has normalized #pragma omp parallel for also (split into separated paralle and for AST nodes).
static size_t
collectOmpLoops(SgProject* proj, Rose_STL_Container<SgOmpForStatement*> & ompfor_stmts)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t raw_list = NodeQuery::querySubTree (proj, V_SgOmpForStatement);
  size_t count = 0;
  for (NodeList_t::reverse_iterator i = raw_list.rbegin ();
      i != raw_list.rend (); ++i)
  {
    SgOmpForStatement* decl = isSgOmpForStatement(*i);
    if (decl)
    {
      ompfor_stmts.push_back (decl);
      ++count;
    }
  }
  return count;
}

//! Collect outlining targets specified using abstract handles
// save them into targetList
static size_t
collectAbstractHandles(SgProject* proj,  TargetList_t& targets)
{
  ROSE_ASSERT(proj != NULL);
  SgFilePtrList & filelist = proj->get_fileList();
  SgFilePtrList::iterator iter= filelist.begin();
  for (;iter!=filelist.end();iter++)
  {
    SgSourceFile* sfile = isSgSourceFile(*iter);
    if (sfile != NULL)
    {
      // prepare a file handle first
      abstract_node * file_node = buildroseNode(sfile);
      ROSE_ASSERT (file_node);
      abstract_handle* fhandle = new abstract_handle(file_node);
      ROSE_ASSERT (fhandle);
      // try to match the string and get the statement handle
      std::vector <std::string>::iterator iter2 = Outliner::handles.begin();
      for (;iter2!=Outliner::handles.end(); iter2++)
      {
        std::string cur_handle = *iter2;
        abstract_handle * shandle = new abstract_handle (fhandle,cur_handle);
        // it is possible that a handle is created but no matching IR node is found
        if (shandle != NULL)
        {
          if (shandle->getNode() != NULL)
          { // get SgNode from the handle
#ifdef _MSC_VER
 // tps (12/09/09) :  error C2039: 'getNode' : is not a member of 'AbstractHandle::abstract_node'
	SgNode* target_node = NULL;
	assert(false);
#else
            SgNode* target_node = (SgNode*) (shandle->getNode()->getNode());
#endif
			ROSE_ASSERT(isSgStatement(target_node));
            targets.push_back(isSgStatement(target_node));
            if (Outliner::enable_debug)
              cout<<"Found a matching target from a handle:"<<target_node->unparseToString()<<endl;
          }
          else
          {
            if (Outliner::enable_debug)
              cout<<"Cannot find a matching target from a handle:"<<cur_handle<<endl;
          }
        }

      }
      // TODO do we care about the memory leak here?
    } //end if sfile
  } // end for 
#ifdef _MSC_VER
//tps (12/09/09) : error C4716: '`anonymous namespace'::collectAbstractHandles' : must return a value
#pragma warning "PragmaInterface: Returning arbitrary value";
  return 0;
#endif
return 0;
}

//-------------------top level drivers----------------------------------------
size_t
Outliner::outlineAll (SgProject* project)
{
  size_t num_outlined = 0;
  TargetList_t targets;
  //generic abstract handle based target selection
  if (Outliner::handles.size()>0)
  {
    collectAbstractHandles(project,targets);
    for (TargetList_t::iterator i = targets.begin ();
        i != targets.end (); ++i)
      if (outline(*i).isValid())
        ++num_outlined;

  } //TODO do we want to have non-exclusive abstract handle options?
  else  if (SageInterface::is_Fortran_language ()) 
  { // Search for the special source comments for Fortran input
    if (collectFortranTarget(project, targets))
    {
      for (TargetList_t::iterator i = targets.begin ();
          i != targets.end (); ++i)
        if (outline(*i).isValid())
          ++num_outlined;
    }
  } 
  else // Search for pragmas for C/C++
  {
    PragmaList_t pragmas;
    if (collectPragmas (project, pragmas))
    {
      for (PragmaList_t::iterator i = pragmas.begin (); i != pragmas.end (); ++i)
      {
        if (outline (*i).isValid ())
        {
          ++num_outlined;
        }
      }

      // DQ (2/24/2009): Now remove the pragma from the original source code.
      // Any dangling pointers have already been taken care of.
      // Liao (4/14/2009):delete only if it is not preprocessing only
      if (!preproc_only_) 
      {
        for (PragmaList_t::iterator i = pragmas.begin (); i != pragmas.end (); ++i)
        {
          SageInterface::deleteAST(*i);
        }
      }
    }

    // New interface : OpenMP for loops, for testing purpose, only for C/C++ code
    if (select_omp_loop)
    {
      Rose_STL_Container<SgOmpForStatement*> ompfor_stmts;
      if (collectOmpLoops(project, ompfor_stmts))
      {  
        for ( Rose_STL_Container<SgOmpForStatement*> ::iterator i = ompfor_stmts.begin (); i != ompfor_stmts.end (); ++i)
        {  
          SgOmpForStatement* omp_stmt = *i; 
          SgStatement* body = omp_stmt->get_body();
          string func_name =  generateFuncName(body);

          if (outline(body, func_name).isValid ())
          { 
            ++num_outlined;
          }
        }
#if 0
        // Any dangling pointers have already been taken care of.
        // delete only if it is not preprocessing only
        if (!preproc_only_)
        {
          for (PragmaList_t::iterator i = pragmas.begin (); i != pragmas.end (); ++i)
          {
            SageInterface::deleteAST(*i);
          }
        }
#endif
      }
    } // end select_omp_loop

  }
  return num_outlined;
}
//! The driver for preprocessing
size_t
Outliner::preprocessAll (SgProject* proj)
{
  size_t count = 0;
  if (SageInterface::is_Fortran_language ()) 
  { // Search for the special source comments for Fortran input
    TargetList_t targets;
    if (collectFortranTarget(proj, targets))
    {
      for (TargetList_t::iterator i = targets.begin ();
          i != targets.end (); ++i)
        if (preprocess (*i))
          ++count;
    }

  }
  else{  // Pragmas exist Only for C/C++ code
    PragmaList_t pragmas;
    if (collectPragmas (proj, pragmas))
    {
      for (PragmaList_t::iterator i = pragmas.begin ();
          i != pragmas.end (); ++i)
        if (preprocess (*i))
          ++count;
    }
  }
  return count;
}

// eof
