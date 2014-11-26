// Main documentation is in the "Snippet" class.

#ifndef ROSE_AST_SNIPPET
#define ROSE_AST_SNIPPET

// rose.h and sage3basic.h should not be included in librose header files. [Robb P. Matzke 2014-10-15]
// #include "sage3basic.h"
#include "Map.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace rose {

/** Smart pointer to a SnippetFile object.  SnippetFile objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class SnippetFile> SnippetFilePtr;

/** Smart pointer to a Snippet object. Snippet objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class Snippet> SnippetPtr;

/** Information about something that was inserted.
 *
 *  A record of what was inserted, where it came from, and where it was inserted. */
struct SnippetInsertion {
    SgNode *inserted;                                   /**< What was inserted */
    SgNode *original;                                   /**< Copied from this node of the snippet file */
    SgNode *insertedBefore;                             /**< Inserted before this node in the target file */
    SnippetInsertion()
        : inserted(NULL), original(NULL), insertedBefore(NULL) {}
    SnippetInsertion(SgNode *inserted, SgNode *original, SgNode *insertedBefore)
        : inserted(inserted), original(original), insertedBefore(insertedBefore) {}
};

std::ostream& operator<<(std::ostream&, const SnippetInsertion&);

/** Represents a source file containing related snippets.
 *
 *  See Snippet class for top-level documentation.
 *
 *  A SnippetFile represents a source file that contains one or more related snippet functions along with other necessary
 *  information at the global scope.  Each Snippet is associated with exactly one SnippetFile and whenever any snippet from the
 *  file is inserted into a specimen, certain statements from the snippet file's global scope are copied into the specimen's
 *  global scope. This copying happens only once per SnippetFile-specimen pair. By placing related snippets in the same source
 *  file we can be sure that only one copy of the snippet's global prerequisites are injected into the specimen. By keeping
 *  unrelated snippets in separate files we can prevent global prerequisites for one snippet to be inserted when an unrelated
 *  snippet is injected.
 *
 *  The SnippetFile class keeps a list of all its objects and returns a pointer to an existing SnippetFile in preference to
 *  creating a new one.  The lookup is performed according to the source file name supplied to the instance() constructor (no
 *  attempt is made to determine when unequal names resolve to the same source file). */
class SnippetFile: public boost::enable_shared_from_this<SnippetFile> {
    friend class Snippet;
private:
    std::string fileName;                               // non-canonical source file name
    SgSourceFile *ast;                                  // AST corresponding to the file; null after parse errors

    typedef Map<std::string/*functionName*/, std::vector<SgFunctionDefinition*> > FunctionDefinitionMap;
    FunctionDefinitionMap functions;                    // cache the functions so we don't have to do a traversal every lookup

    std::set<SgGlobal*> globals;                        // global scopes where this snippet has been inserted
    std::map<std::string/*filename*/, std::set<SgGlobal*>/*insertion points*/> headersIncluded;

    typedef Map<std::string/*fileName*/, SnippetFilePtr> Registry;
    static Registry registry;

    static std::vector<std::string> varNameList;        // list of variable names to use when renaming things
    bool copyAllSnippetDefinitions;                     // should all snippet definitions be copied to global scope?

    Map<std::string, SgTypePtrList> blackListedDeclarations; // things we don't want to copy to the target AST

    std::vector<SnippetInsertion> insertions;           // list of everything we've inserted related to this snippet file

protected:
    /** Use instance() instead. */
    explicit SnippetFile(const std::string &fileName, SgSourceFile *ast=NULL)
        : fileName(fileName), ast(ast), copyAllSnippetDefinitions(false) {}

public:
    /** Constructor. Returns an existing SnippetFile if one has previosly been created for this file name, or creates a new
     *  one.  No attempt is made to determine whether unequal names resolve to the same file.  If a new SnippetFile needs to be
     *  created then we either use the provided AST or we parse the file. */
    static SnippetFilePtr instance(const std::string &fileName, SgSourceFile *snippetAst=NULL);

    /** Look up the SnippetFile for this file name. Returns the SnippetFile for this file if it exists, otherwise returns
     *  null. No attempt is made to determine whether unequal names resolve to the same file. */
    static SnippetFilePtr lookup(const std::string &fileName);

    /** Returns the name of the file. This is the same name given to the instance() constructor. */
    const std::string& getName() const { return fileName; }

    /** Returns the list of snippet names. */
    std::vector<std::string> getSnippetNames() const;

    /** Return a Snippet having the specified name.  The name must be fully a fully qualified function name. Returns null if
     *  the snippet cannot be found in this SnippetFile. If there is more than one snippet with this name then only one is
     *  returned (the first one). */
    SnippetPtr findSnippet(const std::string &snippetName);

    /** Returns all snippets having the specified name.  All snippets in this snippet file that have the specified name are
     *  returned in the order they are defined in the file. */
    std::vector<SnippetPtr> findSnippets(const std::string &snippetName);

    /** Insert snippets in marked code. The specified AST is traversed and function calls to snippets which are defined in this
     *  SnippetFile are recursively expanded. */
    void expandSnippets(SgNode *ast);

    /** Indicates that global entities have been injected.  For every snippet injected into a larger specimen some things
     *  from the snippet's global scope need to be injected into the speicmen's global scope.  This global injection should only
     *  happen once per SnippetFile/specimen pair regardless of how many times a snippet is injected or how many snippets from
     *  the same SnippetFile are injected.
     *
     *  This method records the fact that global entities from this SnippetFile have been injected into the specified global
     *  scope, and returns a boolean to indicate whether they had already been injected (true if injected, false if not). */
    bool globallyInjected(SgGlobal *destination_scope);

    /** Indicates that the specified file has been included into the specified scope. Returns the previous value. */
    bool fileIsIncluded(const std::string &filename, SgGlobal *destination_scope);

    /** Load variable names from a file. The file should have one name per line. Returns the number of variable names added to
     *  the list. */
    static size_t loadVariableNames(const std::string &fileName);

    /** Return a random variable name. */
    static std::string randomVariableName();

    /** Get the AST for the entire snippet file. */
    SgFile *getAst() const { return ast; }

    /** Accessor for the property that controls whether snippet definitions are copied into the global scope. If true, then all
     *  function definitions in the snippet file are copied into the global scope of the file into which the snippet is being
     *  inserted.
     *  @{ */
    bool getCopyAllSnippetDefinitions() const { return copyAllSnippetDefinitions; }
    void setCopyAllSnippetDefinitions(bool b=true) { copyAllSnippetDefinitions = b; }
    void clearCopyAllSnippetDefinitions() { copyAllSnippetDefinitions = false; }
    /** @} */

    /** Black list.  This is a list of declarations that should not be copied from the snippet file to the target
     *  file. Declarations are specified with a qualified name and an optional type. If the type is omitted then only names are
     *  compared. */
    void doNotInsert(const std::string &name, SgType *type=NULL);

    /** Return true if the declaration is black listed. */
    bool isBlackListed(SgDeclarationStatement*);

    /** Information about things that have been inserted. */
    const std::vector<SnippetInsertion>& getInsertedItems() const { return insertions; }

protected:
    /** Parse the snippet file. Snippet files are normally parsed when the SnippetFile object is constructed via instance()
     *  class method. Throws an std::runtime_error on failure. */
    static SgSourceFile* parse(const std::string &fileName);

    /** Find all snippet functions (they are the top-level function definitions) and add them to this SnippetFile. */
    void findSnippetFunctions();

    /** Add an insertion record. */
    void addInsertionRecord(const SnippetInsertion &inserted) { insertions.push_back(inserted); }
};



/** Simple mechanism for inserting statements into a specimen.
 *
 *  This class implements a simple API for inserting crafted statements into locations in a (typically) larger specimen. It
 *  isn't intended to handle all possible cases, but rather to be an alternative to the more complicated SageBuilder
 *  interface. See "Limitations" below.
 *
 * @section S1 What is a snippet?
 *
 *  A snippet is a function in a snippet source file (SnippetFile) along with prerequisites in its global scope. The statements
 *  of the snippet are injected into a specimen function at a chosen insertion point akin to inlining, and the snippet's global
 *  prerequisites are injected ino the specimen's global scope. For C source code, each function definition in the snippet file
 *  is a snippet.
 *
 *  A Snippet object is usually created with the instanceFromFile() class method, which takes a snippet name and a file name
 *  and returns a reference-counted pointer to the snippet. This method efficiently handles the case when multiple related
 *  snippets are defined in a single file (a practice that allows related snippets to share their global prerequisites).
 *
 * @code
 *  SnippetPtr foo = Snippet::instanceFromFile("foo", "snippetfile.c");
 *  SnippetPtr bar = Snippet::instanceFromFile("bar", "snippetfile.c");
 *  SnippetPtr banana = Snippet::instanceFromFile("banana", "fruit.c");
 * @endcode
 *
 * @section S2 The injection process
 *
 *  A snippet is injected along with its global prerequisites via its insert() method.  The first argument is a SgStatement
 *  cursor to indicate where the insertion is to take place (the snippet is inserted before the cursor). The remaining
 *  arguments are either variable declarations (SgInitializedName) or expressions (SgExpression) that are bound to the formal
 *  arguments of the snippet and thereby expanded into the specimen during the injection. They should be variables or
 *  expressions that are valid at the point where the snippet is injected.
 *
 * @code
 *  SgStatement *cursor = ...;                    // statement before which snippet is inserted
 *  SgInitializedName *var_a = ..., *var_b = ...; // insertion point variables to be bound to snippet
 *  SgExpression *expr_1 = ...;                   // insertion point expression to be bound to snippet
 *
 *  foo->insert(cursor);                          // insert snippet foo() with no arguments
 *  bar->insert(cursor, var_a);                   // 1st arg of snippet bar() is bound to var_a
 *  bannana->insert(cursor, var_a, expr_1, var_b);// 3 args of bannana() are bound to things
 * @endcode
 *
 *  Two modes of insertion are supported: The INJECT_BODY mode copies the snippet's body scope into the insertion point so that
 *  all snippet local variables remain in the same (new) scope as the rest of the inserted snippet statements.  This is quick
 *  and easy, but doesn't allow two snippets that are inserted at the same level in a function to share any local variables
 *  (but they can still both refer to variables at the injection site via argument binding in the insert() method).  The
 *  alternative is INJECT_STMTS, which doesn't create a new scope for the injected statements, but rather copies each statement
 *  individually to the injection site.  During INJECT_STMTS mode, snippet local declarations are copied to either the
 *  beginning of the injection point's function, or to the end of that function's list of declarations (see
 *  setLocalDeclarationPosition()).
 *
 * @section S3 Parameterized data types
 *
 *  Sometimes a snippet needs to know the type of an actual argument, and this is accomplished with a function-local typedef
 *  that has a special name.  If the snippet has a formal argument named "a" then a typedef for "typeof_a" will be modified so
 *  its base type is the same type as the actual value bound to "a". This only works when the type of the actaul value is
 *  consistent with the default value provided in the typedef.  For instance, here's the implementation of a snippet that swaps
 *  the value of two variables regardless of the type of the variables (this works as long as 'int' can be replaced with the
 *  actual type and still be syntactically correct):
 *
 * @code
 *  // This snippet operates only on integers
 *  void swap_ints(int a, int b) {
 *      int tmp = a;
 *      a = b;
 *      b = tmp;
 *  }
 *
 *  // This snippet operates on any type. The type "int" here is
 *  // only a place holder so the snippet can be parsed.
 *  void swap(int a, int b) {
 *      typedef int typeof_a;
 *      typeof_a tmp = a;
 *      a = b;
 *      b = tmp;
 *  }
 * @endcode
 *
 * @section S4 Global declarations
 *
 *  Snippets often require additional support in the form of global variables, data types, and function declarations. Whenever
 *  a snippet is inserted into a target, other declarations and definitions at the same level as the snippet (global scope for
 *  C, or class scope for Java) are also copied into the target AST.  This also includes all "import" statements for Java.
 *  However, function definitions (and their enclosing declaration) are not copied unless the copyAllSnippetDefinitions
 *  property of the associated SnippetFile is set.  Copying these things occurs at most once per (snippet-file, target-file)
 *  pair so that multiple definitions are not inserted when more than one snippet is inserted into a single target.
 *
 *  A declaration in the snippet file can be black-listed for copying by calling SnippetFile::doNotInsert.
 *
 * @section S5 Recursive insertion
 *
 *  Snippet insertion is optionally recursive (see setInsertRecursively()).  If one snippet's expansion results in calls to
 *  other snippets defined in the same snippet file, then the other snippets are injected at the point of their call. Only
 *  direct calls (not function pointers) at the statement level (not in subexpressions) are recognized. Here's an example:
 *
 * @code
 *  // Snippet file
 *  void assert(int);
 *  void *malloc(unsigned);
 *  void *memcpy(void*, const void*, unsigned);
 *  unsigned strlen(const char *);
 *
 *  void notNull(const void *x) {
 *      assert(x != (const void*)0);
 *  }
 *
 *  void copyTo(void *dst, const void *src, unsigned nbytes) {
 *      notNull(dst);                           // may be expanded recursively
 *      notNull(src);                           // may be expanded recursively
 *      memcpy(dst, src, nbytes);
 *  }
 *          
 *  void storeStringInHeap(const char *s) {
 *      unsigned s_size = strlen(s) + 1;
 *      char *storage = malloc(s_size);
 *      checkAllocation(storage);               // may be expanded recursively
 *      copyTo(storage, s, s_size);             // may be expanded recursively
 *  }
 * @endcode
 *
 * @section S5 Variable renaming
 *
 *  In order to avoid conflicts between the names of local variables in the snippet code and variables that are visible at the
 *  point of insertion, any snippet local variable whose name begins with "tmp" will be renamed to "T_xxxxxx" where "xxxxxx" is
 *  a randomly generated string of letters.  For example, when inserting allocate_string which also inserts copy_string10 from
 *  the snippet below, the two tmp_size variables are given two different names, but the heap_storage variable is not
 *  renamed--presumably because the user is interested in that specific variable.
 *
 * @code
 *  void copy_string10(char *dst, const char *src) {
 *      unsigned tmp_size = strlen(src);
 *      tmp_size = tmp_size > 10 ? 10 : tmp_size;
 *      memcpy(dst, src, tmp_size);
 *      dst[tmp_size] = '\0';
 *  }
 * 
 *  void allocate_string(const char *s) {
 *      unsigned tmp_size = strlen(s) + 1;
 *      char *heap_storage = malloc(tmp_size);
 *      copy_string10(heap_storage, s);
 *  }
 * @endcode
 *
 * @section S6 Limitations
 *
 *  Any header-file declarations referenced by any code copied from a snippet file's AST to the target AST (snippets themselves
 *  or those copied things described in the "Global declarations" section above) must also already be present in the target
 *  AST.  Practically speaking, this means any #include directives in snippet files must also exist in the target file.
 */
class Snippet {
    friend class SnippetFile;                           // for protected constructor

public:
    /** Determines how a snippet is injected at the insertion point.  Either the entire body scope of the snippet can be
     *  inserted in its entirety as a new scope, or each statement of the body can be inserted individually without using a new
     *  scope.  The latter allows sharing of snippet local variables since they're essentially hoisted into the function scope
     *  of the injection point. */
    enum InsertMechanism {
        INSERT_BODY,                                    /**< Insert entire snippet body as a single scope. */
        INSERT_STMTS,                                   /**< Insert snippet statements one at a time. */
    };

    /** Determines where local declarations are injected when using INSERT_STMTS.  New declarations can be injected starting at
     * the beginning of the injection site's function scope, after the last leading declaration statement in that scope, or
     * in the same place that the non-declaration statements are being inserted.  The last case is only useful in languages
     * like C++ and Java that don't require declarations to be at the beginning of a scope. In any case, the snippet's
     * declarations will appear in the injected code in the same order as in the snippet. */
    enum LocalDeclarationPosition {
        LOCDECLS_AT_BEGINNING,                          /**< Local declarations inserted at beginning of function. */
        LOCDECLS_AT_END,                                /**< Local declarations inserted at end of leading declarations. */
        LOCDECLS_AT_CURSOR                              /**< Local declarations are not moved to a declarations area. */
    };

private:
    typedef Map<SgSymbol*, SgNode*> ArgumentBindings;   // bindings from snippet formals to actual vars and/or expressions
    std::string name;                                   // name of snippet
    SnippetFilePtr file;                                // file containing the snippet definition
    SgFunctionDefinition *ast;                          // snippet definition
    InsertMechanism insertMechanism;                    // how snippet is inserted
    LocalDeclarationPosition locDeclsPosition;          // position for local declarations for INSERT_STMTS mode
    bool insertRecursively;                             // is the insert() operation recursive?
    bool fixupAst;                                      // whether to fix up the target AST after inserting things

protected:
    // Use one of the "instance" methods instead.
    Snippet(const std::string &name, const SnippetFilePtr &file, SgFunctionDefinition *ast)
        : name(name), file(file), ast(ast), insertMechanism(INSERT_STMTS), locDeclsPosition(LOCDECLS_AT_END),
          insertRecursively(true), fixupAst(true) {
        assert(!name.empty());
        assert(file!=NULL);
        assert(ast!=NULL);
    }

public:
    /** Construct a new Snippet.  The name of a snippet corresponds to a name of a global function in a snippet source
     *  file. The snippet source file can be specified as either a file name or a SnippetFile object.
     * @{ */
    static SnippetPtr instance(const std::string &snippetName, const SnippetFilePtr &snippetFile);
    static SnippetPtr instanceFromFile(const std::string &snippetName, const std::string &fileName);
    /** @} */
    
    /** Return the snippet name.  Snippet names are set when the snippet is constructed, and are read-only. */
    const std::string& getName() const /*final*/ { return name; }

    /** Return the file where this snippet is defined. */
    SnippetFilePtr getFile() const { return file; }

    /** Return the function definition for this snippet. */
    SgFunctionDefinition* getDefinition() const { return ast; }

    /** Returns the number of formal arguments for the snippet. */
    size_t numberOfArguments() const;

    /** Accessor for the snippet insertion mechanism. See enum for documentation.
     *  @{ */
    InsertMechanism getInsertMechanism() const { return insertMechanism; }
    void setInsertMechanism(InsertMechanism im) { insertMechanism = im; }
    /** @} */

    /** Accessor for local declaration insertion position. See enum for documentation.
     *  @{ */
    LocalDeclarationPosition getLocalDeclarationPosition() const { return locDeclsPosition; }
    void setLocalDeclarationPosition(LocalDeclarationPosition pos) { locDeclsPosition = pos; }
    /** @} */

    /** Accessor for the property that indicates whether an insert() should be recursive.  If insertion is recursive, then any
     *  calls in the inserted code to another snippet in the same snippet file as the inserted snippet will be inserted
     *  recursively.
     *  @{ */
    bool getInsertRecursively() const { return insertRecursively; }
    void setInsertRecursively(bool b=true) { insertRecursively = b; }
    void clearInsertRecursively() { insertRecursively = false; }
    /** @} */

    /** Insert a snippet into the project.  Inserts the snippet before the @p insertionPoint statement.  The remaining arguments
     *  of this method are bound to formal arguments in the snippet code; they can be either variable declarations
     *  (SgInitializedName) or expressions (SgExpression).
     * @{ */
    void insert(SgStatement *insertionPoint);
    void insert(SgStatement *insertionPoint, SgNode *arg1);
    void insert(SgStatement *insertionPoint, SgNode *arg1, SgNode *arg2);
    void insert(SgStatement *insertionPoint, SgNode *arg1, SgNode *arg2, SgNode *arg3);
    void insert(SgStatement *insertionPoint, SgNode *arg1, SgNode *arg2, SgNode *arg3, SgNode *arg4);
    void insert(SgStatement *insertionPoint, const std::vector<SgNode*> &args);
    /** @} */

    /** Determines whether the target AST should be fixed up after insertion.  Fixing up the target AST makes it so that the
     *  things inserted from the snippet file point only to things in the target part of the AST--the snippet file part of the
     *  AST can be deleted without consequence, and analysis will work propertly on the target AST after the insertion and
     *  fixup.  Fixing up the AST is the default, and turning this feature off is probably only useful for debugging the
     *  insertion mechanism itself while bypassing the AST fixups.
     * @{ */
    bool getFixupAst() const { return fixupAst; }
    void setFixupAst(bool b=true) { fixupAst = b; }
    void clearFixupAst() { fixupAst = false; }
    /** @} */

protected:
    /** Mark nodes so they're unparsed when the insertion point is unparsed. */
    void causeUnparsing(SgNode *ast, Sg_File_Info *targetLocation);

    /** Replace formal argument occurrances with actual values. */
    static void replaceArguments(SgNode *ast, const ArgumentBindings&);

    /** Replace a variable reference with some other expression. */
    static void replaceVariable(SgVarRefExp*, SgExpression*);

    /** Remove C preprocessor #include directives from the specified node. */
    static void removeIncludeDirectives(SgNode*);

    /** Insert other things from the snippet file into the target file. These are things like variables and functions that are
     *  above the snippet function in the snippet files's AST and must be inserted above the snippet insertion point in the
     *  target file. */
    void insertRelatedThings(SgStatement *snippetInsertionPoint);

    /** Java-specific things that need to be copied from the snippet file to the target file. */
    void insertRelatedThingsForJava(SgStatement *snippetInsertionPoint);

    /** C-specific things that need to be copied from the snippet file to the target file. */
    void insertRelatedThingsForC(SgStatement *snippetInsertionPoint);

    /** Rename snippet local variables so they don't interfere with names visible at the insertion point. Only local variables
     * whose names begin with "tmp" are renamed. */
    void renameTemporaries(SgNode *ast);

    // DQ (2/26/2014): Added functionality to address requirement to make snippet AST conform to expectations for any new transforamtion.
    /* Fixup the AST fragement being inserted into the target AST to reset all possible references to the original snippet file. */
    // void fixupSnippetInNewTargetAST(SgStatement *insertionPoint, SgStatement *toInsert);

};

/** Java-aware AST traversal. This is a pre/post depth-first traversal that is aware of certain Java attributes and follows
 *  them even when AstSimpleProcessing would not follow them.  The functor should take two arguments: SgNode*, and
 *  AstSimpleProcessing::Order (the constant preorder or postorder depending on whether the call is before or after the
 *  children are traversed).  Attributes are considered to be the first children of a node. */
class SnippetAstTraversal {
public:
    virtual ~SnippetAstTraversal() {}
    void traverse(SgNode *ast);
    virtual void operator()(SgNode*, AstSimpleProcessing::Order) = 0;
};

} // namespace
#endif
