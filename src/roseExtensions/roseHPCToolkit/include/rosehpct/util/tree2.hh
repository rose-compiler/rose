// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file rosehpct/util/tree2.hh
 *  \brief Generic 'safe' tree abstraction (version 2).
 *  \ingroup ROSEHPCT_UTIL
 *
 *  $Id: tree2.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#if !defined(INC_UTIL_TREE2_HH)
//! util/tree2.hh included.
#define INC_UTIL_TREE2_HH

#include <vector>
#include <map>
#include <rosehpct/util/types.hh>

/*!
 *  \brief Implements a generic tree data structure and traversal
 *  mechanisms.
 */
namespace Trees
{
  //! \addtogroup ROSEHPCT_UTIL
  //@{

  /*!
   *  \brief Templated tree data structure (a tree node), parameterized by the type
   *  of the value (VT) stored at each node and type for indices
   *  (IT) used to 'name' the child nodes.
   *
   *  Both IT and VT should have value semantics.
   *  This tree abstraction provides interfaces to 
   *    create, destroy trees,
   *    retrieve and set child nodes
   *    iterate children nodes
   */
  template <class VT, class IT = size_t>
  class Tree
  {
    //! Child node container type. A map from indices to pointers of child nodes
    typedef typename std::map<IT, Tree<VT, IT>*> ChildList_t;

  public:
    typedef VT Val_t; 
    typedef IT Ind_t;

    //! Value stored at this node
    VT value;

    //! Value type when passing as a parameter.
    typedef typename Types::TypeTraits<Val_t>::ParameterType ValParam_t;
    //! Index type when passing as a parameter.
    typedef typename Types::TypeTraits<Ind_t>::ParameterType IndParam_t;

    //! Creates a tree with a single node.
    Tree (const ValParam_t v);

    //! Destroys the subtree rooted at this node.
    virtual ~Tree (void);

    //! Creates a copy of the subtree rooted at this node
    Tree<VT, IT>* clone (void) const;

    //! \name Iterator-like semantics for enumerating child nodes
    /*@{*/
    typedef typename ChildList_t::iterator iterator;
    typedef typename ChildList_t::const_iterator const_iterator;

    iterator beginChild (void);
    const_iterator beginChild (void) const;
    iterator endChild (void);
    const_iterator endChild (void) const;
    /*@}*/

    //! Returns the number of direct child nodes
    size_t getNumChildren (void) const;

    //! Sets the value of the root of the subtree at i to v.
    Tree<VT, IT>* setChildValue (const IndParam_t i, const ValParam_t v);

    //! Get the value of the root of the subtree at i.
    VT getChildValue (const IndParam_t i) const;

    //! Returns the subtree rooted at child node i.
    Tree<VT, IT>* getSubtree (const IndParam_t i) const;

    //! Sets the subtree rooted at child i to subtree.
    void setSubtree (const IndParam_t i, Tree<VT, IT>* subtree);

    //! Free the memory associated with the subtree rooted at child i.
    void deleteSubtree (const IndParam_t i);

  protected:
    /*!
     *  Returns true if the given subtree would be a valid subtree of
     *  the current tree, called before setting a child subtree in
     *  'setSubtree ().'
     */
    virtual bool isValidSubtree (const Tree<VT, IT>* subtree)
      const;

  private:
    Tree (void);
    Tree (const Tree<VT, IT>& tree);

    ChildList_t children_; //!< Child node container. 
  };

  /* ---------- BEGIN Template member function bodies ---------- */
  template <class VT, class IT>
  Tree<VT, IT>::Tree (void)
  {
  }

  template <class VT, class IT>
  Tree<VT, IT>::Tree (const Tree<VT, IT>& tree)
  {
  }

  template <class VT, class IT>
  Tree<VT, IT>::Tree (ValParam_t v)
    : value (v)
  {
  }

  template <class VT, class IT>
  Tree<VT, IT>::~Tree (void)
  {
    for (iterator i = beginChild (); i != endChild (); ++i)
      {
        delete i->second;
        i->second = NULL;
      }
  }

  template <class VT, class IT>
  Tree<VT, IT> *
  Tree<VT, IT>::clone (void) const
  {
    Tree<VT, IT>* root = new Tree (value);
    if (root == NULL) // out of memory
      return NULL;

    for (const_iterator i = beginChild (); i != endChild (); ++i)
      {
        const Tree<VT, IT>* orig_subtree = i->second;
        if (orig_subtree != NULL)
          {
            Tree<VT, IT>* subtree = orig_subtree->clone ();
            root->setSubtree (i->first, subtree);
          }
      }
    return root;
  }

  template <class VT, class IT>
  typename Tree<VT, IT>::iterator
  Tree<VT, IT>::beginChild (void)
  {
    return children_.begin ();
  }

  template <class VT, class IT>
  typename Tree<VT, IT>::const_iterator
  Tree<VT, IT>::beginChild (void) const
  {
    return children_.begin ();
  }

  template <class VT, class IT>
  typename Tree<VT, IT>::iterator
  Tree<VT, IT>::endChild (void)
  {
    return children_.end ();
  }

  template <class VT, class IT>
  typename Tree<VT, IT>::const_iterator
  Tree<VT, IT>::endChild (void) const
  {
    return children_.end ();
  }

  template <class VT, class IT>
  size_t
  Tree<VT, IT>::getNumChildren (void) const
  {
    return children_.size ();
  }

  template <class VT, class IT>
  Tree<VT, IT> *
  Tree<VT, IT>::setChildValue (const IndParam_t i, const ValParam_t v)
  {
    Tree<VT, IT>* child_node = getSubtree (i);
    if (child_node == NULL)
      {
        child_node = new Tree<VT, IT> (v);
        setSubtree (i, child_node);
      }
    else
      child_node->value = v;
    return child_node;
  }

  template <class VT, class IT>
  VT
  Tree<VT, IT>::getChildValue (const IndParam_t i) const
  {
    Tree<VT, IT>* subtree = getSubtree (i);
    if (subtree == NULL)
      return VT (0);
    else
      return subtree->value;
  }

  template <class VT, class IT>
  Tree<VT, IT> *
  Tree<VT, IT>::getSubtree (const IndParam_t i) const
  {
    const_iterator iter = children_.find (i);
    return (iter != endChild ()) ? iter->second : NULL;
  }

  template <class VT, class IT>
  void
  Tree<VT, IT>::setSubtree (const IndParam_t i,
                            Tree<VT, IT>* subtree)
  {
    deleteSubtree (i);
    if (subtree != NULL && isValidSubtree (subtree))
      children_[i] = subtree;
  }

  template <class VT, class IT>
  void
  Tree<VT, IT>::deleteSubtree (const IndParam_t i)
  {
    iterator iter = children_.find (i);
    if (iter != endChild ())
      {
        Tree<VT, IT>* node = iter->second;
        children_.erase (i);
        delete node;
      }
  }

  /*!
   *  \brief
   *
   *  The default implementation always returns 'true'.
   */
  template <class VT, class IT>
  bool
  Tree<VT, IT>::isValidSubtree (const Tree<VT, IT> *)
    const
  {
    return true;
  }
  /* ---------- END Template member function bodies ---------- */

  /*!
   *  \brief Defines a generic tree traversal which visits each node exactly
   *  once.
   *
   *  Use the policy parameter isConst to define whether or not the
   *  traversal is 'read-only' (i.e., whether the traversal modifies
   *  the tree values or structure).
   */
  template <class TreeType, bool isConst>
  class Traversal
  {
  public:
    /*!
     *  Define how pointers to a tree are passed among the various
     *  traversal member functions (i.e., as read-only 'const' or
     *  not).
     */
    typedef typename Types::Select<isConst,
                                   const TreeType*,
				   TreeType*>::Result
    TreeParamPtr_t;

    /*!
     *  \brief Visit all nodes of the specified tree, calling
     *  'visit()' on each subtree.
     */
    virtual void traverse (TreeParamPtr_t tree) = 0;
    virtual ~Traversal(){};

  protected:
    /*!
     *  Define child node iterator consistent with TreeParamPtr_t.
     */
    typedef typename Types::Select<isConst,
                                   typename TreeType::const_iterator,
				   typename TreeType::iterator>::Result
    ChildIter_t;

    /*!
     *  \brief Called by 'traverse ()' to visit a particular subtree.
     */
    virtual void visit (TreeParamPtr_t tree) = 0;

    /*!
     *  \brief Called after a call to visit but just before descending
     *  one level deeper into the tree.
     */
    virtual void descending (void) {}

    /*!
     *  \brief Called after a call to 'visit' but just before
     *  ascending one level higher in the tree.
     */
    virtual void ascending (void) {}
  };

  /*!
   *  \brief Defines a generic pre-order tree traversal.
   */
  template <class TreeType, bool isConst = true>
  class PreTraversal : public Traversal<TreeType, isConst>
  {
  public:
    typedef typename Traversal<TreeType, isConst>::TreeParamPtr_t
    TreeParamPtr_t;
    virtual void traverse (TreeParamPtr_t tree);
    virtual ~PreTraversal(){}

  protected:
    typedef typename Traversal<TreeType, isConst>::ChildIter_t ChildIter_t;
  };

  /*!
   *  \brief Defines a generic post-order tree traversal.
   */
  template <class TreeType, bool isConst = true>
  class PostTraversal : public Traversal<TreeType, isConst>
  {
  public:
    typedef typename Traversal<TreeType, isConst>::TreeParamPtr_t
    TreeParamPtr_t;
    virtual void traverse (TreeParamPtr_t tree);
    virtual ~PostTraversal(){}

  protected:
    typedef typename Traversal<TreeType, isConst>::ChildIter_t ChildIter_t;
  };

  /* ---------- BEGIN Template member function bodies ---------- */
  template <class TreeType, bool isConst>
  void
  PreTraversal<TreeType, isConst>::traverse (TreeParamPtr_t tree)
  {
    if (tree == NULL)
      return;

    visit (tree);
    this->descending ();
    for (ChildIter_t i = tree->beginChild (); i != tree->endChild (); ++i)
      traverse (i->second);
    this->ascending ();
  }

  template <class TreeType, bool isConst>
  void
  PostTraversal<TreeType, isConst>::traverse (TreeParamPtr_t tree)
  {
    if (tree == NULL)
      return;

    this->descending ();
    for (ChildIter_t i = tree->beginChild (); i != tree->endChild (); ++i)
      traverse (i->second);
    visit (tree);
    this->ascending ();
  }
  /* ---------- END Template member function bodies ---------- */

  //@}
}

#endif

/* eof */
