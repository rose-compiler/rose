/*!
 *  \file CPreproc/If.hh
 *
 *  \brief Implements a tree structure suitable for storing a
 *  collection of nested '#if' directives.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */

#if !defined(INC_CPREPROC_IF_HH)
#define INC_CPREPROC_IF_HH //!< CPreproc/If.hh included.

#include <list>
#include <string>

class SgBasicBlock;
class SgProject;

namespace CPreproc
{
  // ======================================================================

  class If;

  //! Stores lists of directives.
  typedef std::list<If *> Ifs_t;
      
  //! Extracts the '#if' directive structure from a project.
  void extractIfDirectives (SgNode* root, Ifs_t& ifs);

  //! Output '#if' directive structure (for debugging).
  void dump (Ifs_t::const_iterator b, Ifs_t::const_iterator e,
             size_t level = 0);
  //! Output a list of directives
  void dump (const Ifs_t ifs);

  // ======================================================================

  /*!
   *  \brief Stores a preprocessor '#if' directive and nested
   *  directives.
   *
   *  If represents one instance of a preprocessor '#if'
   *  directive, including all its cases. That is, a If object
   *  corresponding to the following '#if' directive,
   *
   *    #if C_1 ... #elif C_2 ... #elif C_k ... #endif // k+1
   *
   *  stores a sequence of k cases plus the final '#endif'. One may view
   *  a If object as a container of cases (If::Cases).
   *
   *  In addition, If represents the complete tree of nested
   *  '#if' directives. See the case object, If::Case, for more
   *  information.
   */
  class If
  {
  public:
    // ========= If::Case =========
    /*!
     *  \brief Stores one case of a preprocessor '#if' directive.
     *
     *  A If::Case object stores a particular case, identified
     *  through a (PreprocessingInfo, SgLocatedNode) pair. It also
     *  stores pointers to child If objects, representing nested
     *  '#if' directives.
     */
    class Case
    {
    protected:
      /*!
       * The default constructor, hidden to prevent users from
       * instantiating a case without specifying the actual directive
       * (PreprocessingInfo object and AST node).
       */
      Case (void);
      
    public:
      Case (PreprocessingInfo *, SgLocatedNode *, If * = 0);
      Case (const Case &);
      
      void dump(size_t level = 0);
      //! Returns 'true' if this case is a '#if'.
      bool isIf (void) const;
      
      //! Returns 'true' if this case is a '#ifdef'.
      bool isIfdef (void) const;
      
      //! Returns 'true' if this case is a '#ifndef'.
      bool isIfndef (void) const;
      
      //! Returns 'true' if this case is an '#elif'.
      bool isElif (void) const;
      
      //! Returns 'true' if this case is an '#else'.
      bool isElse (void) const;
      
      //! Returns 'true' if this case is an '#endif'.
      bool isEndif (void) const;
      
      //! Returns the complete, raw directive, as a string.
      std::string getRaw (void) const;
      
      //! Returns the directive as a string (i.e., '#if').
      std::string getDirective (void) const;
      
      //! Get the condition represented by this case.
      std::string getCondition (void) const;

      //! \name Get the node represented by this case.
      //@{
      SgLocatedNode* getNode (void);
      const SgLocatedNode* getNode (void) const;
      //@}

      //! \name Get the preprocessing info represented by this case.
      //@{
      PreprocessingInfo* getInfo (void);
      const PreprocessingInfo* getInfo (void) const;
      //@}

      //! \name Iterators over the child directives of this case.
      //@{
      typedef Ifs_t::iterator iterator;
      typedef Ifs_t::const_iterator const_iterator;
      typedef Ifs_t::reverse_iterator reverse_iterator;
      typedef Ifs_t::const_reverse_iterator const_reverse_iterator;
      iterator begin (void);
      const_iterator begin (void) const;
      reverse_iterator rbegin (void);
      const_reverse_iterator rbegin (void) const;
      iterator end (void);
      const_iterator end (void) const;
      reverse_iterator rend (void);
      const_reverse_iterator rend (void) const;
      //@}

      //! Makes the given '#if' a child of this case.
      void appendChild (If *);

      //! \name Gets the '#if' of which this case is a part.
      //@{
      If* getIf (void);
      const If* getIf (void) const;
      //@}

      //! Associates this case with a particular directive.
      void setIf (If *);

    private:
      //! Raw preprocessing info object that this case represents.
      PreprocessingInfo* info_;
      
      //! AST node to which this case is physically attached.
      SgLocatedNode* node_;
      
      //! The directive of which this case is a part.
      If* parent_;

      //! Child directives.
      Ifs_t kids_;
    };
    // ========= If::Case ==========

    //! Default constructor.
    If (void);
    
    //! Destructor---frees memory associated with cases.
    ~If (void);

    //! Stores the cases composing an '#if' directive.
    typedef std::list<Case *> Cases_t;
    
    //! \name Iterators over the cases of this directive.
    //@{
    typedef Cases_t::iterator iterator;
    typedef Cases_t::const_iterator const_iterator;
    typedef Cases_t::reverse_iterator reverse_iterator;
    typedef Cases_t::const_reverse_iterator const_reverse_iterator;
    iterator begin (void);
    const_iterator begin (void) const;
    reverse_iterator rbegin (void);
    const_reverse_iterator rbegin (void) const;
    iterator end (void);
    const_iterator end (void) const;
    reverse_iterator rend (void);
    const_reverse_iterator rend (void) const;
    //@}
    
    //! Appends a new case.
    Case* appendCase (PreprocessingInfo *, SgLocatedNode *);

    //! \name Returns the first or last case.
    //@{
    Case* firstCase (void);
    const Case* firstCase (void) const;
    Case* lastCase (void);
    const Case* lastCase (void) const;
    //@}

    //! \name Returns the parent directive, if any.
    //@{
    Case* getParent (void);
    const Case* getParent (void) const;
   //@}

    //! Sets the parent directive.
    void setParent (If::Case *);
        
    /*!
     * \brief Returns 'true' if this appears to be a complete '#if'
     * directive.
     *
     * A 'valid' directive is one which begins with an '#if', ends with
     * an '#endif', and contains zero or more '#elif' in between.
     */
    bool isValid (void) const;
    
  private:
    //! Parent directive (may be NULL).
    Case* parent_;

    //! Stores the cases composing this directive.
    Cases_t cases_;
  }; // class If

  // ======================================================================

  //! Locate the '#if' directive context at a node.
  void findIfDirectiveContext (const SgLocatedNode* target,
                               Ifs_t& ifs, If::Case*& top, If::Case*& bottom);

} // namespace CPreproc

#endif // !defined(INC_IF_HH)

// eof
