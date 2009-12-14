/*!
 *  \file CPreproc/IfDirectiveExtractor.hh
 *
 *  \brief Implements a module to extract the '#if' directive
 *  structure of a program.
 */
#include "rose.h"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"
#include "IfDirectiveExtractor.hh"

// ========================================================================

using namespace std;

// ========================================================================

CPreproc::IfDirectiveExtractor::IfDirectiveExtractor (CPreproc::Ifs_t& ifs)
  : ifs_ (ifs)
{
}
    
void
CPreproc::IfDirectiveExtractor::visitTopDown (SgNode* n)
{
  visit (n, true);
}

void
CPreproc::IfDirectiveExtractor::visitBottomUp (SgNode* n)
{
  visit (n, false);
}

const CPreproc::If::Case*
CPreproc::IfDirectiveExtractor::getCurrentContext (void) const
{
  return pending_.empty () ? 0 : pending_.top ()->lastCase ();
}
    
CPreproc::If::Case*
CPreproc::IfDirectiveExtractor::getCurrentContext (void)
{
  return pending_.empty () ? 0 : pending_.top ()->lastCase ();
}
    
void
CPreproc::IfDirectiveExtractor::validateNonEmptyStack (const PreprocessingInfo* info,
                                                       const SgLocatedNode* node) const
{
  // Must have at least 1 '#if' at this point.
  if (pending_.empty ())
    {
      cerr << "*** ERROR at " << ASTtools::toStringFileLoc (node) << " ***" << endl
           << '\'' << info->getString () << '\'' << endl;
      ROSE_ASSERT (!"Program has bad '#if-#endif' structure!");
    }
}

void
CPreproc::IfDirectiveExtractor::visit (SgNode* n, bool isTopDown)
{
  SgLocatedNode* s = isSgLocatedNode (n);
  if (s)
    {
      AttachedPreprocessingInfoType* info_list =
        s->getAttachedPreprocessingInfo ();
      if (info_list)
        {
          AttachedPreprocessingInfoType::const_iterator i;
          for (i = info_list->begin (); i != info_list->end (); ++i)
          {  
            if (ASTtools::isPositionBefore (*i) == isTopDown)
              handleCase (*i, s);
#if 0
      // only handle before position by default
      // warning if wrongfully attached to a relative position of after or middle.
            else  
            {
      cout<<"Warning: Unhandled preprocessingInfo in after or inside position in CPreproc::IfDirectiveExtractor::visit()"<<endl;
      cout<<"Location: " <<(*i)->get_file_info ()->get_filenameString ().c_str () << endl;
      cout<<"Type:"<< PreprocessingInfo::directiveTypeName((*i)->getTypeOfDirective ()) <<endl;
            }
#endif            
          }    
        }
    }
}

void
CPreproc::IfDirectiveExtractor::handleCase (PreprocessingInfo* info,
                                            SgLocatedNode* node)
{
  if (ASTtools::isIfDirectiveBegin (info))  // Start new '#if/ifdef/ifndef'
    newIf (info, node);
  else if (ASTtools::isIfDirectiveMiddle (info))  // An '#else' or '#elif'
    addCase (info, node);
  else if (ASTtools::isIfDirectiveEnd (info))  // An '#endif'
    {
      addCase (info, node);
      pending_.pop ();
    }
}

void
CPreproc::IfDirectiveExtractor::newIf (PreprocessingInfo* info,
                                       SgLocatedNode* node)
{
  // Create the new '#if' representation.
  If* new_if = new If;
  ROSE_ASSERT (new_if);
  // Liao, 3/11/2008. eliminate the side effect of assertion, fix a bug caused by NDEBUG
  //ROSE_ASSERT (new_if->appendCase (info, node));
  If::Case* result = new_if->appendCase (info, node);
  ROSE_ASSERT(result);
  
  // If there is an '#if' at the top of the stack, make the new
  // '#if' a child of the top's last case.
  if (!pending_.empty ())
    {
      If* cur_if = pending_.top ();
      ROSE_ASSERT (cur_if);
      checkLastCaseNotEndif (cur_if);
      
      If::Case* last_case = cur_if->lastCase ();
      ROSE_ASSERT (last_case);
      last_case->appendChild (new_if);
    }
  else // Nothing on the stack, so this is a top-level directive.
    ifs_.push_back (new_if);
  
  // Make this new '#if' the top of the stack.
  pending_.push (new_if);
}

void
CPreproc::IfDirectiveExtractor::addCase (PreprocessingInfo* info,
                                         SgLocatedNode* node)
{
  validateNonEmptyStack (info, node);

  // Append this directive case.
  If* cur_if = pending_.top ();
  ROSE_ASSERT (cur_if);
  checkLastCaseNotEndif (cur_if);
  
  If::Case* new_last_case = cur_if->appendCase (info, node);
  ROSE_ASSERT (new_last_case);
}

void
CPreproc::IfDirectiveExtractor::checkLastCaseNotEndif (const If* i)
{
  if (i)
    {
      const If::Case* last_case = i->lastCase ();
      if (last_case && last_case->isEndif ())
        ROSE_ASSERT (!"An '#endif cannot have a child '#if'!");
    }
}

// eof
