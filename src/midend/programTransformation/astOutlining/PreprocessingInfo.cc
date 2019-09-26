/**
 *  \file PreprocessingInfo.cc
 *
 *  \brief Higher-level wrappers related to query and manipulation of
 *  PreprocessingInfo objects.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"

// =====================================================================

using namespace std;

// =====================================================================

//! Trims leading and trailing whitespace.
static
void
trim (string& str)
{
  string::size_type pos = str.find_last_not_of (" \t\n\r");
  if (pos != string::npos)
    {
      str.erase (pos + 1);
      pos = str.find_first_not_of (" \t\n\r");
      if(pos != string::npos) str.erase (0, pos);
    }
  else str.erase (str.begin (), str.end ());
}

// =====================================================================

/*!
 *  \brief Returns 'true' iff the given info object is valid and has
 *  the specified position.
 */
static
bool
isNotRelPos (const PreprocessingInfo* info,
             PreprocessingInfo::RelativePositionType pos)
{
  return info && (info->getRelativePosition () != pos);
}

/*!
 *  \brief Returns 'true' iff the given info object is valid and does
 *  not have the specified position.
 */
static
bool
isRelPos (const PreprocessingInfo* info,
             PreprocessingInfo::RelativePositionType pos)
{
  return info && !isNotRelPos (info, pos);
}

// =====================================================================

bool
ASTtools::isPositionBefore (const PreprocessingInfo* info)
{
  return info && info->getRelativePosition () == PreprocessingInfo::before;
}

bool
ASTtools::isPositionAfter (const PreprocessingInfo* info)
{
  return info && info->getRelativePosition () == PreprocessingInfo::after;
}

bool
ASTtools::isPositionInside (const PreprocessingInfo* info)
{
  return info && info->getRelativePosition () == PreprocessingInfo::inside;
}

bool
ASTtools::isIfDirectiveBegin (const PreprocessingInfo* info)
{
  if (info)
    switch (info->getTypeOfDirective ())
      {
      case PreprocessingInfo::CpreprocessorIfDeclaration:
      case PreprocessingInfo::CpreprocessorIfdefDeclaration:
      case PreprocessingInfo::CpreprocessorIfndefDeclaration:
        return true;
      default:
        break;
      }
  // Answer: I guess not.
  return false;
}

bool
ASTtools::isIfDirectiveMiddle (const PreprocessingInfo* info)
{
  if (info)
    switch (info->getTypeOfDirective ())
      {
      case PreprocessingInfo::CpreprocessorElifDeclaration:
      case PreprocessingInfo::CpreprocessorElseDeclaration:
        return true;
      default:
        break;
      }
  // Answer: I guess not.
  return false;
}

bool
ASTtools::isIfDirectiveEnd (const PreprocessingInfo* info)
{
  if (info)
    switch (info->getTypeOfDirective ())
      {
      case PreprocessingInfo::CpreprocessorEndifDeclaration:
        return true;
      default:
        break;
      }
  // Answer: I guess not.
  return false;
}

// =====================================================================

void
ASTtools::attachComment (const char* comment, SgStatement* s)
{
  attachComment (string (comment), s);
}

void
ASTtools::attachComment (const string& comment, SgStatement* s)
{
  if (s)
    {
   // DQ (7/19/2008): Modified interface to PreprocessingInfo.
   // Note that this function could directly call SageInterface::attachComment(SgLocatedNode*,std::string);
      PreprocessingInfo* comment_info =
        new PreprocessingInfo (PreprocessingInfo::CplusplusStyleComment,
                               "// " + comment, "user-generated", 0, 0, 0,
                               PreprocessingInfo::before);

   // DQ (3/12/2019): We need to mark the added comments and CPP directives as a transformation so that then can be output.
   // This is a result of a fix to support the correct handling of comments and CPP directives for shared IR nodes as happen 
   // when multiple files are used on the command line.
      comment_info->get_file_info()->setTransformation();

      s->addToAttachedPreprocessingInfo (comment_info);
    }
}

// =====================================================================

bool
ASTtools::insertHeader (const string& filename, SgProject* project)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t globalScopeList = NodeQuery::querySubTree (project, V_SgGlobal);

  for (NodeList_t::iterator i = globalScopeList.begin ();
       i != globalScopeList.end (); i++)
    {
      SgGlobal* globalScope = isSgGlobal (*i);
      ROSE_ASSERT (globalScope);

      SgDeclarationStatementPtrList & stmtList =
	globalScope->get_declarations ();
      for (SgDeclarationStatementPtrList::iterator j = stmtList.begin ();
	   j != stmtList.end (); j++)
	{
	  //must have this judgement, otherwise wrong file will be modified!
	  if (((*j)->get_file_info ())->
	      isSameFile (globalScope->get_file_info ()))
	    {
      // DQ (7/19/2008): Modified interface to PreprocessingInfo.
      // Note that this function could directly call SageInterface::attachComment(SgLocatedNode*,std::string);
	      (*j)->addToAttachedPreprocessingInfo (new
						PreprocessingInfo
						(PreprocessingInfo::
						 CpreprocessorIncludeDeclaration,
						 string
						 ("#include \"" + filename + "\" \n"),
						 "Compiler-generated by Outliner",
						 0, 0, 0,
						 PreprocessingInfo::before));
	      break;
	    } //end if
	} //end for
    } //end for
  // Assume always successful currently
  return true;
}

// =====================================================================

AttachedPreprocessingInfoType *
ASTtools::createInfoList (SgStatement* s)
{
  ROSE_ASSERT (s);

  AttachedPreprocessingInfoType* info_list = s->get_attachedPreprocessingInfoPtr ();
  if (!info_list)
    {
      info_list = new AttachedPreprocessingInfoType;
      ROSE_ASSERT (info_list);
      s->set_attachedPreprocessingInfoPtr (info_list);
    }

  // Guarantee a non-NULL pointer.
  ROSE_ASSERT (info_list);
  return info_list;
}

void
ASTtools::dumpPreprocInfo (const SgStatement* s, ostream& o)
{
  if (!s) return;

  o << "=== PreprocessingInfo (" << toStringFileLoc (s) << ") ===" << endl;

  const AttachedPreprocessingInfoType* pp =
    const_cast<SgStatement *> (s)->getAttachedPreprocessingInfo ();
  if (!pp)
    o << "   (none)" << endl;
  else // pp
    {
      size_t count = 0;
      AttachedPreprocessingInfoType::const_iterator i;
      for (i = pp->begin (); i != pp->end (); ++i)
        {
          const PreprocessingInfo* info = *i;
          string text = info->getString ();
          trim (text);
          o << "  (" << ++count << ") '" << text << "'"
            << ":<" << PreprocessingInfo::directiveTypeName (info->getTypeOfDirective ()) << '>'
            << "; " << PreprocessingInfo::relativePositionName (info->getRelativePosition ())
            << endl;
        }
      o << endl;
    }
}

void
ASTtools::cutPreprocInfo (SgBasicBlock* b,
                          PreprocessingInfo::RelativePositionType pos,
                          AttachedPreprocessingInfoType& save_buf)
{
  AttachedPreprocessingInfoType* info = createInfoList (b);
  ROSE_ASSERT (info);
  remove_copy_if (info->begin (), info->end (),
                  back_inserter (save_buf),
                  bind2nd (ptr_fun (isNotRelPos), pos));

// DQ (9/26/2007): Commented out as part of move from std::list to std::vector
// info->remove_if (bind2nd (ptr_fun (isRelPos), pos));
// Liao (10/3/2007), implement list::remove_if for vector, which lacks sth. like erase_if
  AttachedPreprocessingInfoType::iterator new_end =
	   remove_if(info->begin(),info->end(),bind2nd(ptr_fun (isRelPos), pos));
  info->erase(new_end, info->end());
  
}

void
ASTtools::pastePreprocInfoFront (AttachedPreprocessingInfoType& save_buf, SgStatement* s)
{
  AttachedPreprocessingInfoType* info = createInfoList (s);
  ROSE_ASSERT (info);

// DQ (9/26/2007): Commented out as part of move from std::list to std::vector
// printf ("Commented out front_inserter() as part of move from std::list to std::vector \n");
// copy (save_buf.rbegin (), save_buf.rend (), front_inserter (*info));

// Liao (10/3/2007), vectors can only be appended at the rear
   for(AttachedPreprocessingInfoType::reverse_iterator i=save_buf.rbegin();i!=save_buf.rend();i++)
       info->insert(info->begin(),*i);

}

void
ASTtools::pastePreprocInfoBack (AttachedPreprocessingInfoType& save_buf,
                                SgStatement* s)
{
  AttachedPreprocessingInfoType* info = createInfoList (s);
  ROSE_ASSERT (info);
  copy (save_buf.begin (), save_buf.end (), back_inserter (*info));
}

void
ASTtools::moveBeforePreprocInfo (SgStatement* src, SgStatement* dest)
{
  ROSE_ASSERT (src && dest);

  AttachedPreprocessingInfoType* s_info =
    src->get_attachedPreprocessingInfoPtr ();
  if (!s_info) return; // No work to do

  AttachedPreprocessingInfoType* d_info = createInfoList (dest);
  ROSE_ASSERT (s_info && d_info);

// DQ (9/26/2007): Commented out as part of move from std::list to std::vector
//   printf ("Commented out remove_copy_if() as part of move from std::list to std::vector \n");
// remove_copy_if (s_info->rbegin (), s_info->rend (), front_inserter (*d_info), bind2nd (ptr_fun (isNotRelPos), PreprocessingInfo::before));
// Liao (10/3/2007), workaround for inserting at the beginning of a vector 
  for(AttachedPreprocessingInfoType::reverse_iterator i = s_info->rbegin(); i != s_info->rend(); i++)
    if (!isNotRelPos(*i,PreprocessingInfo::before))
        d_info->insert(d_info->begin(),*i);


// DQ (9/26/2007): Commented out as part of move from std::list to std::vector
//   printf ("Commented out s_info->remove_if() as part of move from std::list to std::vector \n");
// s_info->remove_if (bind2nd (ptr_fun (isRelPos), PreprocessingInfo::before));
// Liao (10/3/2007), vectors do not support remove_if
  AttachedPreprocessingInfoType::iterator new_end =
       remove_if(s_info->begin(),s_info->end(),bind2nd(ptr_fun (isRelPos), PreprocessingInfo::before));
  s_info->erase(new_end, s_info->end());

}

void
ASTtools::moveInsidePreprocInfo (SgBasicBlock* src, SgBasicBlock* dest)
{
  AttachedPreprocessingInfoType* s_info =
    src->get_attachedPreprocessingInfoPtr ();
  if (!s_info) return; // No work to do

  AttachedPreprocessingInfoType* d_info = createInfoList (dest);
  ROSE_ASSERT (d_info);

  // Determine an insertion point.
  AttachedPreprocessingInfoType::iterator i =
    find_if (d_info->begin (), d_info->end (),
             bind2nd (ptr_fun (isRelPos), PreprocessingInfo::inside));

  if (i == d_info->end ()) // Destination has no 'inside' preprocessing info.
    i = find_if (d_info->begin (), d_info->end (),
                 bind2nd (ptr_fun (isRelPos), PreprocessingInfo::after));

  if (i == d_info->end ()) // Destination has no 'after' preprocessing info.
    remove_copy_if (s_info->begin (), s_info->end (), back_inserter (*d_info),
                    bind2nd (ptr_fun (isNotRelPos),
                             PreprocessingInfo::inside));
  else // Insert before 'i'
    remove_copy_if (s_info->begin (), s_info->end (), inserter (*d_info, i),
                    bind2nd (ptr_fun (isNotRelPos),
                             PreprocessingInfo::inside));

  // Erase from source.

// DQ (9/26/2007): Commented out as part of move from std::list to std::vector
//   printf ("Commented out s_info->remove_if() as part of move from std::list to std::vector \n");
// s_info->remove_if (bind2nd (ptr_fun (isRelPos), PreprocessingInfo::inside));
// Liao (10/3/2007), vectors do not support remove_if
  AttachedPreprocessingInfoType::iterator new_end =
       remove_if(s_info->begin(),s_info->end(),bind2nd(ptr_fun (isRelPos), PreprocessingInfo::inside));
  s_info->erase(new_end, s_info->end());

}

void
ASTtools::moveAfterPreprocInfo (SgStatement* src, SgStatement* dest)
{
  ROSE_ASSERT (src && dest);

  AttachedPreprocessingInfoType* s_info =
    src->get_attachedPreprocessingInfoPtr ();
  if (!s_info) return; // No work to do

  AttachedPreprocessingInfoType* d_info = createInfoList (dest);
  ROSE_ASSERT (s_info && d_info);

  remove_copy_if (s_info->begin (), s_info->end (),
                  back_inserter (*d_info),
                  bind2nd (ptr_fun (isNotRelPos), PreprocessingInfo::after));

// DQ (9/26/2007): Commented out as part of move from std::list to std::vector
//   printf ("Commented out s_info->remove_if() as part of move from std::list to std::vector \n");
// s_info->remove_if (bind2nd (ptr_fun (isRelPos), PreprocessingInfo::after));
// Liao (10/3/2007), vectors do not support remove_if
  AttachedPreprocessingInfoType::iterator new_end =
       remove_if(s_info->begin(),s_info->end(),bind2nd(ptr_fun (isRelPos), PreprocessingInfo::after));
  s_info->erase(new_end, s_info->end());

}
// Move preprocessing info. of stmt2 to stmt1. 
void
ASTtools::moveUpPreprocInfo (SgStatement * stmt1, SgStatement * stmt2)
{
  if (stmt1 == stmt2) return; // No work to do.

  ROSE_ASSERT (stmt1 != NULL);
  ROSE_ASSERT (stmt2 != NULL);
  AttachedPreprocessingInfoType *infoList = stmt2->getAttachedPreprocessingInfo ();
  AttachedPreprocessingInfoType *infoToRemoveList = new AttachedPreprocessingInfoType ();

  if (infoList == NULL)
    return;

  for (Rose_STL_Container< PreprocessingInfo * >::iterator i = (*infoList).begin ();
       i != (*infoList).end (); i++)
    {
      PreprocessingInfo *info = dynamic_cast < PreprocessingInfo * >(*i);
      ROSE_ASSERT (info != NULL);

      if ((info->getTypeOfDirective () ==
	   PreprocessingInfo::CpreprocessorIncludeDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorDefineDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorUndefDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorIfdefDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorIfndefDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorIfDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorElseDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorElifDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::C_StyleComment)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorEndifDeclaration))
	{
	  stmt1->addToAttachedPreprocessingInfo (info,
						 PreprocessingInfo::after);
	  (*infoToRemoveList).push_back (*i);
	}			// end if
    }				// end for

  // Remove the element from the list of comments at the current astNode
  AttachedPreprocessingInfoType::iterator j;
  for (j = (*infoToRemoveList).begin (); j != (*infoToRemoveList).end (); j++)
  {
 // (*infoList).remove (*j);
    //infoList->erase(find(infoToRemoveList->begin(),infoToRemoveList->end(),*j));
    //Liao, 10/2/07, segmentation fault for the stmt above
    infoList->erase(find(infoList->begin(),infoList->end(),*j));
  }

  return;
}

// eof
