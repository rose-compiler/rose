// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file rosehpct/sage/collectors.hh
 *  \brief Implements utility routines that collect nodes of a Sage
 *  III tree.
 *  \ingroup ROSEHPCT_SAGE
 *
 *  $Id: collectors.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#if !defined(INC_SAGE_COLLECTORS_HH)
//! rosehpct/sage/collectors.hh included.
#define INC_SAGE_COLLECTORS_HH

#include <iostream>
#include <string>
#include <vector>

//#include "rose.h"
#include "rosehpct/util/general.hh"

namespace RoseHPCT
{
  //! \addtogroup ROSEHPCT_SAGE
  //@{

  /*!
   *  \brief Simple class to collect nodes in the Sage III IR.
   *
   *  The template type T should be a child class of SgLocatedNode.
   *
   *  Create a node collector, initializing it to identify nodes that
   *  of type T whose physical line numers lie between a specified
   *  interval (b, e) inclusive. Call 'traverse' on a particular root
   *  node to initiate the search. On return, use beginMatch/endMatch
   *  to iterate over matching nodes.
   */
  template <class T>
  class NodeCollector : public AstSimpleProcessing
  {
  public:
    typedef typename std::vector<T *>::iterator iterator;
    typedef typename std::vector<T *>::const_iterator const_iterator;

    virtual ~NodeCollector (void);

    //! Initiate a traversal, and return the number of matching nodes.
    virtual size_t doTraversal (SgProject* root);
    virtual size_t doTraversal (SgNode* subtree);

    /*!
     *  Visits a node in the tree.
     */
    virtual void visit (SgNode* node);

    size_t getNumMatches (void) const;

    typename std::vector<T *>::iterator beginMatch (void);
    typename std::vector<T *>::const_iterator beginMatch (void) const;
    typename std::vector<T *>::iterator endMatch (void);
    typename std::vector<T *>::const_iterator endMatch (void) const;

  protected:
    virtual bool matches (T* node) const = 0;

  private:
    std::vector<T *> matches_; // matching nodes
  };

  /* ---------- BEGIN Template member function bodies ---------- */
  template <class T>
  NodeCollector<T>::~NodeCollector (void)
  {
  }

  template <class T>
  size_t
  NodeCollector<T>::doTraversal (SgProject* root)
  {
    traverseInputFiles (root, preorder);
    return getNumMatches ();
  }

  template <class T>
  size_t
  NodeCollector<T>::doTraversal (SgNode* node)
  {
    traverse (node, preorder);
    return getNumMatches ();
  }

  template <class T>
  void
  NodeCollector<T>::visit (SgNode* node)
  {
    T* spec_node = dynamic_cast<T *> (node);
    if (spec_node != NULL && matches (spec_node))
      matches_.push_back (spec_node);
  }

  template <class T>
  size_t
  NodeCollector<T>::getNumMatches (void) const
  {
    return matches_.size ();
  }

  template <class T>
  typename std::vector<T *>::iterator
  NodeCollector<T>::beginMatch (void)
  {
    return matches_.begin ();
  }

  template <class T>
  typename std::vector<T *>::iterator
  NodeCollector<T>::endMatch (void)
  {
    return matches_.end ();
  }

  template <class T>
  typename std::vector<T *>::const_iterator
  NodeCollector<T>::beginMatch (void) const
  {
    return matches_.begin ();
  }

  template <class T>
  typename std::vector<T *>::const_iterator
  NodeCollector<T>::endMatch (void) const
  {
    return matches_.end ();
  }
  /* ---------- END Template member function bodies ---------- */

  /*!
   *  \brief Collects nodes by physical source location.
   *
   *  This class provides iterator-like behavior for a container that
   *  stores SgLocatedNode objects whose physical location lies in a
   *  particular file and within a specified line number interval.
   */
  template <class T>
  class NodeCollectorByLoc : public NodeCollector<T>
  {
  public:
    NodeCollectorByLoc (void);
    NodeCollectorByLoc (const std::string& filename,
                        size_t line_start, size_t line_end = 0);
    virtual ~NodeCollectorByLoc (void);

    void setFilename (const std::string& filename);
    void setLineRange (size_t line_start, size_t line_end = 0);

  protected:
    /*!
     *  Returns true <==> the give node matches the current location.
     *
     *  This routine checks if 'node' is an SgLocatedNode, and if
     *  so, returns the value of 'matchesLoc', or false otherwise.
     */
    virtual bool matches (T* node) const;

    /*!
     *  Returns true <==> the given SgLocatedNode matches the current
     *  location.
     *
     *  To compare filenames, this routine calls 'filenamesMatch'.
     *  To compare the line numbers, this routine calls 'linesMatch'.
     */
    virtual bool matchesLoc (SgLocatedNode* node) const;

    /*!
     *  Returns true <==> the Sage IR node line numbers provided by
     *  the caller 'match' the current line numbers.
     */
    virtual bool linesMatch (size_t sg_start, size_t sg_end) const;

    /*!
     *  Returns true <==> the Sage IR node filename provided by the
     *  caller 'matches' the current file.
     */
    virtual bool filenamesMatch (const std::string& sg_filename) const;

    std::string getFilename (void) const;
    size_t getLineStart (void) const;
    size_t getLineEnd (void) const;

  private:
    std::string filename_;
    /*!
     *  \note This class should maintain the invariant: line_end_ >=
     *  line_start_.
     */
    size_t line_start_;
    size_t line_end_;
  };

  /* ---------- BEGIN Template member function bodies ---------- */
  template <class T>
  NodeCollectorByLoc<T>::NodeCollectorByLoc (const std::string& filename,
                                             size_t line_start,
                                             size_t line_end)
    : filename_ (filename), line_start_ (line_start), line_end_(line_end)
  {
    if (line_end_ < line_start_)
      line_end_ = line_start_;
  }

  template <class T>
  NodeCollectorByLoc<T>::NodeCollectorByLoc (void)
    : filename_ (""), line_start_ (0), line_end_ (0)
  {
  }

  template <class T>
  NodeCollectorByLoc<T>::~NodeCollectorByLoc (void)
  {
  }

  template <class T>
  void
  NodeCollectorByLoc<T>::setFilename (const std::string& filename)
  {
    filename_ = filename;
  }

  template <class T>
  void
  NodeCollectorByLoc<T>::setLineRange (size_t line_start, size_t line_end)
  {
    line_start_ = line_start;
    line_end_ = (line_end < line_start) ? line_start : line_end;
  }

  template <class T>
  bool
  NodeCollectorByLoc<T>::matches (T* node) const
  {
    /* To match, node must be a valid SgLocatedNode */
    if (node == NULL)
      return false;
    else
      return matchesLoc (node);
  }

  template <class T>
  bool
  NodeCollectorByLoc<T>::matchesLoc (SgLocatedNode* node) const
  {
    if (node == NULL)
      return false;

    Sg_File_Info* info_start = node->get_startOfConstruct ();
    ROSE_ASSERT (info_start != NULL);

    if (GenUtil::isSamePathname (filename_, info_start->get_raw_filename ()))
      return false;
    // else, at least source file name matches

    int line_start = info_start->get_line ();
    Sg_File_Info* info_end = node->get_endOfConstruct ();
    int line_end = (info_end == NULL) ? line_start : info_end->get_line ();

    if (line_end < line_start)
      line_end = line_start;

    return linesMatch (line_start, line_end);
  }

  /*!
   *  \brief
   *
   *  The default behavior returns 'true' iff the caller-specified lines
   *  are wholly contained within the current line numbers.
   */
  template <class T>
  bool
  NodeCollectorByLoc<T>::linesMatch (size_t sg_start, size_t sg_end) const
  {
    return line_start_ <= sg_start && sg_end <= line_end_;
  }

  /*!
   *  \brief
   *
   *  The default behavior returns 'true' iff the caller-specified
   *  filename exactly matches the current filename.
   */
  template <class T>
  bool
  NodeCollectorByLoc<T>::filenamesMatch (const std::string& sg_filename) const
  {
    return GenUtil::isSamePathname (sg_filename, filename_);
  }

  template <class T>
  std::string
  NodeCollectorByLoc<T>::getFilename (void) const
  {
    return filename_;
  }

  template <class T>
  size_t
  NodeCollectorByLoc<T>::getLineStart (void) const
  {
    return line_start_;
  }

  template <class T>
  size_t
  NodeCollectorByLoc<T>::getLineEnd (void) const
  {
    return line_end_;
  }
  /* ---------- END Template member function bodies ---------- */

  //! Specializes NodeCollector for SgGlobal nodes
  class FileCollector : public NodeCollector<SgGlobal>
  {
  public:
    FileCollector (void);
    FileCollector (const std::string& filename);

    void setFilename (const std::string& filename);

  protected:
    virtual bool matches (SgGlobal* node) const;

  private:
    std::string filename_;
  };

  //! Specializes NodeCollectorByLoc for SgFunctionDefinition nodes
  class FuncCollectorByLoc : public NodeCollectorByLoc<SgFunctionDefinition>
  {
  public:
    FuncCollectorByLoc (void);
    FuncCollectorByLoc (const std::string& filename,
                        const std::string& funcname,
                        size_t b, size_t e);

    void setFuncname (const std::string& funcname);

  protected:
    virtual bool matches (SgFunctionDefinition* node) const;
    virtual bool linesMatch (size_t sg_start, size_t sg_end) const;

  private:
    std::string funcname_;
  };

  //! Specializes NodeCollectorByLoc for SgStatement nodes
  class StmtCollectorByLoc : public NodeCollectorByLoc<SgStatement>
  {
  public:
    StmtCollectorByLoc (void);
    StmtCollectorByLoc (const std::string& filename,
                        size_t b, size_t e);

  protected:
    virtual bool matches (SgStatement* node) const;

    //! \name Statement special cases
    /*@{*/
    bool matches (const SgForInitStatement* node) const;
    /*@}*/
  };

  //! Specializes NodeCollectorByLoc for scope statements
  class ScopeStmtCollectorByLoc : public NodeCollectorByLoc<SgScopeStatement>
  {
  public:
    ScopeStmtCollectorByLoc (void);
    ScopeStmtCollectorByLoc (const std::string& filename,
                             size_t b, size_t e);

  protected:
    virtual bool matches (SgScopeStatement* node) const;
  };

  //! Specializes ScopeStmtCollectorByLoc for explicit loops
  class LoopCollectorByLoc : public ScopeStmtCollectorByLoc
  {
  public:
    LoopCollectorByLoc (void);
    LoopCollectorByLoc (const std::string& filename,
                        size_t b, size_t e);

  protected:
    virtual bool matches (SgScopeStatement* node) const;
  };

  //@}
}

#endif

/* eof */
