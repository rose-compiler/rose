/*!
 *  \file PreprocIfs.cc
 *
 *  \brief Preprocessor phase to break-up any preprocessing control
 *  structure that may straddle the boundaries of a basic block.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include <iostream>
#include <list>
#include <string>
#include <algorithm>



#include "Preprocess.hh"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"
#include "StmtRewrite.hh"
#include "If.hh"

//! Relative positions at which '#if' cases may be inserted.
enum ContextPosType
  {
    e_beforeBlock,
    e_firstInBlock,
    e_lastInBlock,
    e_beforeLastInBlock
  };

//! Insert '#if' directives to close a context.
static void closeContext (const CPreproc::If::Case *,
                          ContextPosType,
                          SgBasicBlock *);

//! Insert '#if' directives to open a context.
static void openContext (const CPreproc::If::Case *,
                         ContextPosType,
                         SgBasicBlock *);

//! Stores a list of preprocessing directives.
typedef Rose_STL_Container<PreprocessingInfo *> PreprocInfoList_t;

// =====================================================================

using namespace std;

// =====================================================================

SgBasicBlock *
Outliner::Preprocess::transformPreprocIfs (SgBasicBlock* b)
{
  ROSE_ASSERT (b && b->get_parent ());

  // Determine the '#if' directive context at 'b'.
  CPreproc::Ifs_t ifs;
  CPreproc::If::Case* top = 0;
  CPreproc::If::Case* bottom = 0;
  CPreproc::findIfDirectiveContext (b, ifs, top, bottom);

  //debugging
  //  CPreproc::dump(ifs);
  // if(top) top->dump();
  // if(bottom) bottom->dump();

  if (!top && !bottom) // Not guarded by any '#if'.
    return b;
  // move the source block b to be a nested block inside a shell block b_new
  SgBasicBlock* b_new = ASTtools::transformToBlockShell (b);
  ROSE_ASSERT (b_new);

  closeContext (top, e_beforeBlock, b_new);
  openContext (top, e_firstInBlock, b_new);

  closeContext (bottom, e_lastInBlock, b_new);
  openContext (bottom, e_beforeLastInBlock, b);

  return b_new;
}

// =====================================================================

//! Sanitizes a string for insertion into a C-style comment.
static
string
makeCommentSafe (const string& s)
{
  string s_new (s);
  string::size_type pos;

  while ((pos = s_new.find ("/*")) != string::npos)
    s_new.erase (pos, 2);
  while ((pos = s_new.find ("*/")) != string::npos)
    s_new.erase (pos, 2);
  while ((pos = s_new.find ("*/")) != string::npos)
    s_new.erase (pos, 2);

  return s_new;
}

/*!
 *  \brief Returns the '#if' case of the parent directive of the given
 *  case, if any.
 *
 *  For example, let 'c' be as marked below:
 *
 *    #if A
 *      // ...
 *    #elif B  // Associated with some case, 'b'
 *      // ...
 *      #if C   // Associated with input case 'c'
 *
 *  Then, this routine returns the case 'b'.
 */
static
const CPreproc::If::Case *
getParentCase (const CPreproc::If::Case* c)
{
  const CPreproc::If* c_if = c->getIf ();
  ROSE_ASSERT (c_if);
  const CPreproc::If::Case* c_if_parent = c_if->getParent ();
  return c_if_parent ? c_if_parent : 0;
}

/*!
 *  \brief Returns the '#if ...' that opens the directive to which
 *  this case belongs.
 *
 *  For example, let 'c' be as marked below:
 *
 *    #if A  // Associated with some case, 'a'
 *      // ...
 *    #elif B
 *      // ...
 *    #elif C   // Associated with input case, 'c'
 *      // ...
 *
 *  Then, this routine returns 'a'.
 *
 *  \returns A non-NULL result, given a non-NULL input case.
 */
static
const CPreproc::If::Case *
getOpenCase (const CPreproc::If::Case* c)
{
  if (c)
    {
      const CPreproc::If* c_if = c->getIf ();
      ROSE_ASSERT (c_if);
      const CPreproc::If::Case* c_open = c_if->firstCase ();
      ROSE_ASSERT (c_open);
      return c_open;
    }
  return 0;
}

/*!
 *  \brief Generates PreprocessingInfo objects for use when 'closing'
 *  an '#if' directive context.
 */
static
void
genCloseDirectives (const CPreproc::If::Case* c,
                    PreprocessingInfo::RelativePositionType pos,
                    PreprocInfoList_t& D)
{
  if (c)
    {
      // Close 'c'.

      // Build a debugging comment so we can see to whom this new
      // '#endif' belongs.
      stringstream s_closer;
      const CPreproc::If::Case* c_open = getOpenCase (c);
      ROSE_ASSERT (c_open);
      if (c_open != c)
        s_closer << c_open->getDirective ()
                 << ' '
                 << c_open->getCondition ()
                 << " ... ";

      s_closer << c->getDirective ()
               << ' '
               << c->getCondition ();

      // Build preprocessing info object for this string.
      string closer = "#endif /* " + makeCommentSafe (s_closer.str ()) + " */\n";
      PreprocessingInfo* d =
        new PreprocessingInfo (PreprocessingInfo::CpreprocessorEndifDeclaration,
                               closer,
                               string ("transformation"), 0, 0,
                               1 /* no. of lines */,
                               pos);
      ROSE_ASSERT (d);

   // DQ (3/12/2019): We need to mark the added comments and CPP directives as a transformation so that then can be output.
   // This is a result of a fix to support the correct handling of comments and CPP directives for shared IR nodes as happen 
   // when multiple files are used on the command line.
      d->get_file_info()->setTransformation();

      D.push_back (d);

      // Close all parents.
      genCloseDirectives (getParentCase (c), pos, D);
    }
}

/*!
 *  \brief Generates PreprocessingInfo objects for use when 'opening'
 *  an '#if' directive context.
 */
static
void
genOpenDirectives (const CPreproc::If::Case* c,
                   PreprocessingInfo::RelativePositionType pos,
                   PreprocInfoList_t& D)
{
  if (c)
    {
      // Open all parents.
      genOpenDirectives (getParentCase (c), pos, D);

      stringstream s_opener;
      const CPreproc::If::Case* c_open = getOpenCase (c);
      ROSE_ASSERT (c_open);
      if (c_open != c)
        s_opener << c_open->getDirective ()
                 << ' '
                 << c_open->getCondition ()
                 << " ... ";
      s_opener << c->getDirective ()
               << ' '
               << c->getCondition ();

      string opener = "#if 1 /* " + makeCommentSafe (s_opener.str ()) + "*/\n";
      PreprocessingInfo* d =
        new PreprocessingInfo (PreprocessingInfo::CpreprocessorIfDeclaration,
                               opener,
                               string ("transformation"), 0, 0,
                               1 /* no. of lines */,
                               pos);

      ROSE_ASSERT (d);

   // DQ (3/12/2019): We need to mark the added comments and CPP directives as a transformation so that then can be output.
   // This is a result of a fix to support the correct handling of comments and CPP directives for shared IR nodes as happen 
   // when multiple files are used on the command line.
      d->get_file_info()->setTransformation();

      d->setRelativePosition (pos);
      D.push_back (d);
    }
}

/*!
 *  \brief Interprets (converts) an open/close context's relative
 *  position into a PreprocessingInfo::RelativePositionType position.
 */
static
PreprocessingInfo::RelativePositionType
getRelPos (ContextPosType pos)
{
  switch (pos)
    {
    case e_beforeBlock:
    case e_firstInBlock:
      return PreprocessingInfo::before;
    case e_lastInBlock:
    case e_beforeLastInBlock:
      return PreprocessingInfo::inside;
    default:
      break;
    }
  return PreprocessingInfo::defaultValue;
}

/*!
 *  \brief Inserts preprocessing info at the
 *  'PreprocessingInfo::inside' position of the given target
 *  statement.
 */
static
void
insertMiddle (PreprocInfoList_t& D, SgStatement* s)
{
  AttachedPreprocessingInfoType* info = ASTtools::createInfoList (s);
  ROSE_ASSERT (info);
  AttachedPreprocessingInfoType::iterator ins_pos = info->begin ();
  while (ins_pos != info->end ())
    {
      if ((*ins_pos)->getRelativePosition () != PreprocessingInfo::before)
        break; // Found insertion point.
      ++ins_pos;
    }

  if (ins_pos == info->end ())
    copy (D.begin (), D.end (), back_inserter (*info));
  else
    copy (D.begin (), D.end (), inserter (*info, ins_pos));
}

//! Returns 'true' if the given statement will not be unparsed.
static
bool
isHiddenStmt (const SgStatement* s)
{
  if (s)
    {
      const Sg_File_Info* info = s->get_file_info ();
      ROSE_ASSERT (info);
      if (info->get_filenameString () == "NULL_FILE"
          || !info->isOutputInCodeGeneration ())
        return true;
    }

  // Default: Assume not.
  return false;
}

/*!
 *  \brief Inserts the given preprocessing info at the front of the
 *  first statement in b.
 *
 *  \pre b is non-NULL and has at least 1 statement in it.
 */
static
void
prependAtFirstStatement (PreprocInfoList_t& D, SgBasicBlock* b)
{
  ROSE_ASSERT (b && !b->get_statements ().empty ());

  SgStatementPtrList& stmts = b->get_statements ();
  SgStatement* s = *(stmts.begin ());
  ROSE_ASSERT (s);

  if (isHiddenStmt (s))
    {
      /*! \note This code handles a somewhat esoteric special case in
       * which the statement 's' will not be unparsed, and so any
       * preprocessing information attached to it would also go
       * unparsed. This occurred in
       * CompileTests/Cxx_tests/test2001_09.C, where an implicit
       * return statement (represented explicitly in the tree but
       * unparsed) was being outlined.
       */
      SgStatement* s_blank = SageBuilder::buildNullStatement ();
      ROSE_ASSERT (s_blank);

   // DQ (9/26/2007): Moved from std::list to std::vector uniformly in ROSE.
   // stmts.push_front (s_blank);
      stmts.insert(stmts.begin(),s_blank);

      s_blank->set_parent (b);

   // DQ (9/26/2007): Moved from std::list to std::vector uniformly in ROSE.
   //   printf ("Commentout out front_inserter since it is unavailable in std::vector \n");
   // copy (D.rbegin (), D.rend (), front_inserter (*ASTtools::createInfoList (s_blank)));

      // Liao (10/3/2007), append elements for a vector
     AttachedPreprocessingInfoType *info = ASTtools::createInfoList (s_blank); 
     for(PreprocInfoList_t::reverse_iterator i=D.rbegin();i!=D.rend();i++)
     	info->insert(info->begin(),*i);
     	
    }
   else // Attach at the front of preprocessing info at 's'.
    {
   // DQ (9/26/2007): Moved from std::list to std::vector uniformly in ROSE.
   //   printf ("Commentout out front_inserter since it is unavailable in std::vector \n");
   // copy (D.rbegin (), D.rend (), front_inserter (*ASTtools::createInfoList (s)));

      // Liao (10/3/2007), append elements for a vector
     AttachedPreprocessingInfoType *info = ASTtools::createInfoList (s); 
     for(PreprocInfoList_t::reverse_iterator i=D.rbegin();i!=D.rend();i++)
     	info->insert(info->begin(),*i);

    }
}

//! Inserts preprocessing info objects into a basic block.
static
void
insertContext (PreprocInfoList_t& D, ContextPosType pos, SgBasicBlock* b)
{
  if (D.empty () || !b) return; // no work

  switch (pos)
    {
      case e_beforeBlock:
        {
       // DQ (9/26/2007): Moved from std::list to std::vector uniformly in ROSE.
       //   printf ("Commentout out front_inserter since it is unavailable in std::vector \n");
       // copy (D.rbegin (), D.rend (), front_inserter (*ASTtools::createInfoList (b)));
       
          // Liao (10/3/2007), append elements for a vector
	   AttachedPreprocessingInfoType *info = ASTtools::createInfoList (b); 
	   for(PreprocInfoList_t::reverse_iterator i=D.rbegin();i!=D.rend();i++)
           info->insert(info->begin(),*i);

          break;
        }

    case e_lastInBlock:
      copy (D.begin (), D.end (), back_inserter (*ASTtools::createInfoList (b)));
      break;
    case e_firstInBlock:
      {
        if (b->get_statements ().empty ())
          insertContext (D, e_beforeLastInBlock, b);
        else
          prependAtFirstStatement (D, b);
      }
      break;
    case e_beforeLastInBlock:
      insertMiddle (D, b);
      break;
    default:
      break;
    }
}

// =====================================================================

static
void
closeContext (const CPreproc::If::Case* context, ContextPosType pos,
              SgBasicBlock* b)
{
  // New directives.
  PreprocInfoList_t new_dirs;
  genCloseDirectives (context, getRelPos (pos), new_dirs);
  insertContext (new_dirs, pos, b);
}

static
void
openContext (const CPreproc::If::Case* context, ContextPosType pos,
             SgBasicBlock* b)
{
  // New directives.
  PreprocInfoList_t new_dirs;
  genOpenDirectives (context, getRelPos (pos), new_dirs);
  insertContext (new_dirs, pos, b);
}

// eof
