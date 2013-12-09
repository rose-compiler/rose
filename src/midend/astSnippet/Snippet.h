// Main documentation is in the "Snippet" class.

#ifndef ROSE_AST_SNIPPET
#define ROSE_AST_SNIPPET

#include "sage3basic.h"
#include "Map.h"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace rose {

/** Smart pointer to a SnippetFile object.  SnippetFile objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class SnippetFile> SnippetFilePtr;

/** Smart pointer to a Snippet object. Snippet objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class Snippet> SnippetPtr;

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
private:
    std::string fileName;                       // non-canonical source file name
    SgFile *ast;                                // AST corresponding to the file; null after parse errors

    typedef Map<std::string/*functionName*/, SgFunctionDefinition*> FunctionDefinitionMap;
    FunctionDefinitionMap functions;            // cache the functions so we don't have to do a traversal every lookup

    std::set<SgGlobal*> globals;                // global scopes where this snippet has been inserted

    typedef Map<std::string/*fileName*/, SnippetFilePtr> Registry;
    static Registry registry;

protected:
    /** Use instance() instead. */
    explicit SnippetFile(const std::string &fileName): fileName(fileName), ast(NULL) {}

public:
    ~SnippetFile() { registry.erase(fileName); }

    /** Constructor. Returns an existing SnippetFile if one has previosly been created for this file name, or creates a new
     * one.  No attempt is made to determine whether unequal names resolve to the same file. */
    static SnippetFilePtr instance(const std::string &fileName);

    /** Look up the SnippetFile for this file name. Returns the SnippetFile for this file if it exists, otherwise returns
     *  null. No attempt is made to determine whether unequal names resolve to teh same file. */
    static SnippetFilePtr lookup(const std::string &fileName);

    /** Returns the name of the file. This is the same name given to the instance() constructor. */
    const std::string& getName() const { return fileName; }

    /** Returns the list of snippet names. */
    std::vector<std::string> getSnippetNames() const;

    /** Return a Snippet having the specified name.  The snippet name is either a global function or a fully qualified function
     *  name. Returns null if the snippet cannot be found in this SnippetFile. */
    SnippetPtr findSnippet(const std::string &snippetName);

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

protected:
    /** Parse the snippet file. Snippet files are normally parsed when the SnippetFile object is constructed via instance()
     *  class method. Throws an std::runtime_error on failure. */
    void parse();
};



/** Simple mechanism for inserting statements into a specimen.
 *
 *  This class implements a simple API for inserting crafted statements into locations in a (typically) larger specimen. It
 *  isn't intended to handle all possible cases, but rather to be an alternative to the more complicated SageBuilder
 *  interface.
 *
 *  A snippet is a function in a snippet source file (SnippetFile) along with prerequisites in its global scope. The statements
 *  of the snippet are injected into a specimen function at a chosen insertion point akin to inlining, and the snippets global
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
 *  A snippet is injected along with its global prerequisites via its insert() method.  The first argument is a SgStatement
 *  cursor to indicate where the insertion is to take place (the snippet is inserted after before the cursor). The remaining
 *  arguments are either variable declarations (SgInitializedName) or expressions (SgExpression) that are bound to the formal
 *  arguments of the snippet and thereby expanded into the specimen during the injection. They should be variables or
 *  expressions that are valid at the point where the snippet is injected.
 *
 * @code
 *  SgStatement *cursor = ...;
 *  SgInitializedName *var_a = ..., *var_b = ...;
 *  SgExpression *expr_1 = ...;
 *
 *  foo->insert(cursor);
 *  bar->insert(cursor, var_a);
 *  bannana->insert(cursor, var_a, expr_1, var_b);
 * @endcode
 *
 *  Sometimes a snippet needs to know the type of an actual argument, and this is accomplished with a function-local typedef
 *  that has a special name.  If the snippet has a formal argument named "a" then a typedef for "typeof_a" will be modified so
 *  its base type is the same type as the actual value bound to "a". This only works when the type of the actaul value is
 *  consistent with the default value provided in the typedef.  For instance, here's the implementation of a snippet that swaps
 *  the value of two variables regardless of the type of the variables (this works as long as 'char' can be replaced with the
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
 *  Snippet insertion is recursive.  If one snippet's expansion results in calls to other snippets, then the other snippets are
 *  injected at the point of their call. Only direct calls (not function pointers) at the statement level (not in
 *  subexpressions) are recognized. Here's an example: [FIXME: Currently only snippets in the same SnippetFile are expanded.]
 *
 * @code
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
 *      notNull(dst);
 *      notNull(src);
 *      memcpy(dst, src, nbytes);
 *  }
 *          
 *  void storeStringInHeap(const char *s) {
 *      unsigned s_size = strlen(s) + 1;
 *      char *storage = malloc(s_size);
 *      checkAllocation(storage);
 *      copyTo(storage, s, s_size);
 *  }
 * @endcode
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
 */
class Snippet {
    friend class SnippetFile;                           // for protected constructor

private:
    typedef Map<SgSymbol*, SgNode*> ArgumentBindings;   // bindings from snippet formals to actual vars and/or expressions
    std::string name;                                   // name of snippet
    SnippetFilePtr file;                                // file containing the snippet definition
    SgFunctionDefinition *ast;                          // snippet definition

protected:
    // Use one of the "instance" methods instead.
    Snippet(const std::string &name, const SnippetFilePtr &file, SgFunctionDefinition *ast)
        : name(name), file(file), ast(ast) {
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

    /** Returns the number of formal arguments for the snippet. */
    size_t numberOfArguments() const;

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

protected:
    /** Mark nodes so they're unparsed when the insertion point is unparsed. */
    static void causeUnparsing(SgNode *ast);

    /** Replace formal argument occurrances with actual values. */
    static void replaceArguments(SgNode *ast, const ArgumentBindings&);

    /** Replace a variable reference with some other expression. */
    static void replaceVariable(SgVarRefExp*, SgExpression*);
    
    /** Insert stuff from the snippet's global scope into the insertion point's global scope. Only do this for things that
     *  aren't already inserted. */
    void insertGlobalStuff(SgStatement *insertionPoint);

    /** Rename snippet local variables so they don't interfere with names visible at the insertion point. Only local variables
     * whose names begin with "tmp" are renamed. */
    void renameTemporaries(SgNode *ast);
};

} // namespace
#endif
