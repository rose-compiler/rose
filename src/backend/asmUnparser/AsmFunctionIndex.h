#ifndef ROSE_AsmFunctionIndex_H
#define ROSE_AsmFunctionIndex_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <algorithm>
#include <ostream>
#include <vector>

#include "callbacks.h"
#include "Diagnostics.h"

namespace Rose {
namespace BinaryAnalysis {

/** Functions indexed by entry address.
 *
 *  This class is designed to be a highly-configurable way to print a table describing known functions.  The way it works is
 *  the user adds functions to the AsmFunctionIndex object (explicitly, or by having this object traverse an AST), and then
 *  prints the object to an std::ostream.  The index contains a list of functions to be included in the index, but it also
 *  contains a list of callbacks responsible for printing the table column headers and data.
 *
 *  For instance, to print an index of all functions sorted by function name and using the default table style, simply invoke
 *  this code:
 *  @code
 *  #include <AsmFunctionIndex.h>
 *
 *  SgProject *project = ...;
 *  std::cout <<AsmFunctionIndex(project).sort_by_name();
 *  @endcode
 *
 *  If you want a list of functions sorted by decreasing size in bytes, you could do this:
 *  @code
 *  std::cout <<AsmFunctionIndex(project).sort_by_byte_size().reverse();
 *  @endcode
 *
 *  Here's an example of augmenting the function index so that it always sorts the functions by entry address and it prints an
 *  extra column named "Hash" that consists of the first 16 characters of some computed function hash.
 *  @code
 *  class MyFunctionIndex: public AsmFunctionIndex {
 *  public:
 *      struct HashCallback: public OutputCallback {
 *          HashCallback(): OutputCallback("Hash", 16) {}
 *          virtual bool operator()(bool enabled, const DataArgs &args) {
 *              if (enabled)
 *                  args.output <<data_prefix <<std::setw(width) <<function_hash(args.func).substr(0,16);
 *              return enabled;
 *          }
 *      } hashCallback;
 *
 *      MyFunctionIndex(SgNode *ast): AsmFunctionIndex(ast) {
 *          sort_by_entry_addr();
 *          output_callbacks.before(&nameCallback, &hashCallback);
 *      }
 *  };
 *
 *  std::cout <<MyFuncitonIndex(project);
 *  @endcode
 *
 *  The output might look something like this:
 *  @verbatim
  Num Entry-Addr Begin-Addr  End-Addr  Insns/ Bytes      Reason        Kind         Hash                     Name              
 ---- ---------- ---------- ---------- ------------ ---------------- -------- ---------------- --------------------------------
    0 0x08048278 0x0804828e 0x0804828f     8/23     .C..S...........  unknown                  _init                           
    1 0x080482a0 0x080482a0 0x080482a6     1/6      .C.I............  unknown                  malloc@@plt                      
    2 0x080482b0 0x080482b0 0x080482b6     1/6      .C.I............  unknown                  __libc_start_main@@plt           
    3 0x080482c0 0x080482e1 0x080482e2    14/34     E...S...........  unknown                  _start                          
    4 0x080482e4 0x08048304 0x08048305    15/33     .C..S...........  unknown                  call_gmon_start                 
    5 0x08048310 0x0804833e 0x0804833f    16/47     .C..S...........  unknown                  __do_global_dtors_aux           
    6 0x08048340 0x08048362 0x08048363    13/35     .C..S...........  unknown                  frame_dummy                     
    7 0x08048364 0x080483bf 0x080483c0    29/92     ....S...........  unknown 91badca44c59fae6 main                            
    8 0x080483c0 0x08048411 0x08048412    34/82     ....S...........  unknown                  __libc_csu_init                 
    9 0x08048414 0x08048455 0x08048456    27/66     ....S...........  unknown                  __libc_csu_fini                 
   10 0x08048460 0x08048490 0x08048491    20/49     .C..S...........  unknown                  __do_global_ctors_aux           
   11 0x08048494 0x080484ad 0x080484ae    12/26     .C..S...........  unknown                  _fini                           
 ---- ---------- ---------- ---------- ------------ ---------------- -------- ---------------- --------------------------------
@endverbatim
 */
class AsmFunctionIndex {
    /**************************************************************************************************************************
     *                                  The main public members
     **************************************************************************************************************************/
public:

    /** Constructs an empty index. */
    AsmFunctionIndex() {
        init();
    }

    /** Constructs an index from an AST. */
    AsmFunctionIndex(SgNode *ast) {
        init();
        add_functions(ast);
    }
    
    virtual ~AsmFunctionIndex() {}

    /** Adds a function to the end of this index. The function is added regardless of whether it already exists. */
    virtual void add_function(SgAsmFunction*);

    /** Adds functions to this index.  The specified AST is traversed and all SgAsmFunction nodes are added to this index. All
     *  encountered functions are added to the index in the order they are encountered regardless of whether they already
     *  exist. */
    virtual void add_functions(SgNode *ast);

    /** Clears the index.  Removes all functions from the index, but does not change any callback lists. */
    virtual void clear() {
        functions.clear();
    }

    /** Determines if an index is empty. Returns true if the index contains no functions, false otherwise. */
    virtual bool empty() const {
        return functions.empty();
    }

    /** Returns the number of functions in the index. */
    virtual size_t size() const {
        return functions.size();
    }

    /**************************************************************************************************************************
     *                                  Functors for sorting
     * These are expected to be commonly used, so we define them here for convenience.  The generic sorting method is defined
     * below.
     **************************************************************************************************************************/
public:

    /** Functor for sorting by function entry virtual address. */
    struct SortByEntryAddr {
        bool operator()(SgAsmFunction *a, SgAsmFunction *b) {
            return a->get_entry_va() < b->get_entry_va();
        }
        bool unique(SgAsmFunction *a, SgAsmFunction *b) {
            return a->get_entry_va() != b->get_entry_va();
        }
    };

    /** Functor for sorting by function beginning address. */
    struct SortByBeginAddr {
        bool operator()(SgAsmFunction *a, SgAsmFunction *b) {
            return val(a)<val(b);
        }
        bool unique(SgAsmFunction *a, SgAsmFunction *b) {
            return val(a)!=val(b);
        }
        rose_addr_t val(SgAsmFunction *x) {
            rose_addr_t lo;
            x->get_extent(NULL, &lo);
            return lo;
        }
    };

    /** Functor for sorting by number of instructions in function. */
    struct SortByInsnsSize {
        bool operator()(SgAsmFunction *a, SgAsmFunction *b) {
            return val(a) < val(b);
        }
        bool unique(SgAsmFunction *a, SgAsmFunction *b) {
            return val(a) != val(b);
        }
        size_t val(SgAsmFunction *x) {
            return SageInterface::querySubTree<SgAsmInstruction>(x).size();
        }
    };

    /** Functor for sorting by number of bytes in function. Bytes are counted only once no matter in how many overlapping
     *  instructions and/or data blocks they appear. */
    struct SortByBytesSize {
        bool operator()(SgAsmFunction *a, SgAsmFunction *b) {
            return val(a) < val(b);
        }
        bool unique(SgAsmFunction *a, SgAsmFunction *b) {
            return val(a) != val(b);
        }
        size_t val(SgAsmFunction *x) {
            AddressIntervalSet extent;
            x->get_extent(&extent);
            return extent.size();
        }
    };

    /** Functor to sort functions by name. */
    struct SortByName {
        bool operator()(SgAsmFunction *a, SgAsmFunction *b) {
            return a->get_name().compare(b->get_name()) < 0;
        }
        bool unique(SgAsmFunction *a, SgAsmFunction *b) {
            return 0 != a->get_name().compare(b->get_name());
        }
    };



    /**************************************************************************************************************************
     *                                  Sorting methods
     **************************************************************************************************************************/
public:

    /** Sort the functions in the index.  The supplied functor takes two SgAsmFunction pointers as arguments and returns true
     *  if the first argument goes before the second argument in the specific strict weak ordering it defines, and false
     *  otherwise.  If @p unique is defined, then a final pass is made over the functions and any adjacent functions for which
     *  the Comparator::unique() method, which takes two SgAsmFunction pointer arguments, returns false will cause the second
     *  argument to be removed from the index. */
    template<class Comparator> AsmFunctionIndex& sort(Comparator comp, bool unique=false) {
        std::stable_sort(functions.begin(), functions.end(), comp);
        if (unique) {
            Functions newlist;
            for (Functions::iterator fi=functions.begin(); fi!=functions.end(); fi++) {
                if (newlist.empty() || comp.unique(newlist.back(), *fi))
                    newlist.push_back(*fi);
            }
            if (newlist.size()!=functions.size())
                functions = newlist;
        }
        return *this;
    }

    /** Specific sorting method.  This method is expected to be commonly used, so we define it here for convenience.  If some
     *  other sorting method is needed, see the generic sort() method.
     * @{ */
    AsmFunctionIndex& sort_by_entry_addr(bool unique=false) { return sort(SortByEntryAddr(), unique); }
    AsmFunctionIndex& sort_by_begin_addr(bool unique=false) { return sort(SortByBeginAddr(), unique); }
    AsmFunctionIndex& sort_by_ninsns(bool unique=false)     { return sort(SortByInsnsSize(), unique); }
    AsmFunctionIndex& sort_by_nbytes(bool unique=false)     { return sort(SortByBytesSize(), unique); }
    AsmFunctionIndex& sort_by_name(bool unique=false)       { return sort(SortByName(),      unique); }
    /** @} */

    /** Reverse the order of the functions.  This is typically called after sorting. */
    AsmFunctionIndex& reverse() {
        std::reverse(functions.begin(), functions.end());
        return *this;
    }

    /**************************************************************************************************************************
     *                                  Footnotes
     **************************************************************************************************************************/
public:
    class Footnotes {
    public:
        Footnotes() {
            set_footnote_title("== Footnotes ==");
        }

        /** Adds a footnote to the table.  Footnotes are printed at the end of the table by the ShowFootnotes callback
         *  class. Footnotes are numbered starting at one rather than zero, and this function returns the footnote number.
         *  Calling this function with an empty @p text argument will reserve a footnote number but will not print the footnote
         *  in the final output.
         *
         *  The first line of the footnote is prefixed by the string "Footnote *N: " and subsequent lines are indented by the
         *  length of the prefix. */
        size_t add_footnote(const std::string &text);

        /** Change the text associated with a footnote.  The footnote must already exist, and the footnote @p idx is the value
         *  returned by a previous call to add_footnote().  Setting @p text to the empty string causes the footnote to not
         *  appear in the final output, but other footnotes are not renumbered to close the gap (because their references might
         *  have already been printed.  See also, add_footnote(). */
        void change_footnote(size_t idx, const std::string &text);

        /** Get the string for a footnote.  The footnote must already exist and @p idx is a footnote number returned by a previous
         *  call to add_footnote(). */
        const std::string& get_footnote(size_t idx) const;

        /** Returns the number of footnotes.  Footnotes are numbered starting at one.  The return value includes empty footnotes
         *  that won't be printed by the final output. */
        size_t size() const { return footnotes.size(); }

        /** Change the footnote title string.  The title string is printed before the first footnote if there are any footnotes
         *  with a non-empty string. */
        void set_footnote_title(const std::string &title);

        /** Get the footnote title.  The default title is "== Footnotes ==". */
        const std::string& get_footnote_title() const;

        /** Set the footnote prefix string.  This string is printed before every line of the footnote area. The default is the
         *  empty string. */
        void set_footnote_prefix(const std::string &prefix) { footnote_prefix = prefix; }

        /** Get the footnote prefix string.  This string is printed before every line of the footnote area. */
        const std::string& get_footnote_prefix() const { return footnote_prefix; }

        /** Generates a footnote name from a footnote index.  The default is to return the index prefixed by an asterisk. */
        std::string get_footnote_name(size_t idx) const;

        /** Print non-empty footnotes. */
        void print(std::ostream&) const;

        /** Print non-empty footnotes. */
        friend std::ostream& operator<<(std::ostream &o, const Footnotes *footnotes) {
            footnotes->print(o);
            return o;
        }

    protected:
        std::vector<std::string> footnotes;     /**< List of footnotes. Elmt zero is an optional  footnote title string.*/
        std::string footnote_prefix;            /**< String to emit before every footnote line. */
    };
    
    /**************************************************************************************************************************
     *                                  Output methods
     **************************************************************************************************************************/
public:
    /** Prints a function index to an output stream. */
    virtual void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const AsmFunctionIndex&);



    /**************************************************************************************************************************
     *                                  Output callback base classes
     **************************************************************************************************************************/
public:

    /** Base class for printing table cells.
     *
     *  Three kinds of callback are defined:
     *  <ol>
     *    <li>A callback to print the table heading and separator</li>
     *    <li>A callback to print the data content</li>
     *    <li>A callback invoked before and after the entire table</li>
     *  </ol>
     *
     *  Subclasses almost always override the data content callback, but seldom override the others.  The default heading and
     *  separator callback knows how to print a properly formatted column heading, and the default before and after callback
     *  does nothing, but could be used to print keys, footnotes, etc.
     *
     *  If a callback has no column name and zero width, then its header and content callback is not invoked; its content
     *  callback is still invoked but should not produce any output. */
    class OutputCallback {
    public:
        /** Base class for callback arguments. */
        struct GeneralArgs {
            GeneralArgs(const AsmFunctionIndex *index, std::ostream &output, Footnotes *footnotes)
                : index(index), output(output), footnotes(footnotes) {}
            const AsmFunctionIndex *index;              /**< Index object being printed. */
            std::ostream &output;                       /**< Stream to which index is being printed. */
            Footnotes *footnotes;                       /**< Footnotes (newly created for each index output). */
        };

        /** Arguments for before-and after. */
        struct BeforeAfterArgs: public GeneralArgs {
            BeforeAfterArgs(const AsmFunctionIndex *index, std::ostream &output, Footnotes *footnotes, int when)
                : GeneralArgs(index, output, footnotes), when(when) {}
            int when;                                  /**< Zero implies before table, one implies after table. */
        };

        /** Arguments for column heading callbacks.  If @p set is non-NUL then instead of printing the column name it should
         *  print a separator line using the @p sep character. */
        struct HeadingArgs: public GeneralArgs {
            HeadingArgs(const AsmFunctionIndex *index, std::ostream &output, Footnotes *footnotes, char sep='\0')
                : GeneralArgs(index, output, footnotes), sep(sep) {}
            char sep;                                   /**< If non-NUL, then print a line of these characters. */
        };

        /** Arguments for column cells. */
        struct DataArgs: public GeneralArgs {
            DataArgs(const AsmFunctionIndex *index, std::ostream &output, Footnotes *footnotes, SgAsmFunction *func, size_t rowid)
                : GeneralArgs(index, output, footnotes), func(func), rowid(rowid) {}
            SgAsmFunction *func;
            size_t rowid;
        };

        /** Constructor.  Every column must have a name and non-zero width.  If a column is given a description then the column
         *  name will reference a footnote containing the description.  The footnote is added by the default HeadingArgs
         *  callback, so subclasses should take care to add their own footnote if necessary. */
        OutputCallback(const std::string &name, size_t width, const std::string description="")
            : name(name), desc(description), width(width), header_prefix(" "), separator_prefix(" "), data_prefix(" ") {
            ASSERT_require(width>0 || name.empty());
        }

        virtual ~OutputCallback() {}

        /** Set prefix characters. */
        void set_prefix(const std::string &header, const std::string &separator=" ", const std::string &data=" ");

        /** Callback for before and after the table.  The default does nothing, but subclasses can override this to do things
         *  like print descriptions, footnotes, etc. */
        virtual bool operator()(bool enabled, const BeforeAfterArgs&);

        /** Callback to print a column heading. The base class implementation prints the column name using the specified column
         * width.  Subclasses probably don't need to override this method. */
        virtual bool operator()(bool enabled, const HeadingArgs&);

        /** Callback to print data for a table cell.  The base class implementation prints white space only, so subclasses
         * almost certainly want to override this method. */
        virtual bool operator()(bool enabled, const DataArgs&);

    protected:
        std::string center(const std::string&, size_t width); /**< Center @p s in a string of length @p width. */

        std::string name;                               /**< Column name used when printing table headers. */
        std::string desc;                               /**< Optional description to appear in footnote. */
        size_t width;                                   /**< Minimum width of column header or data. */
        std::string header_prefix;                      /**< Character(s) to print before headings. */
        std::string separator_prefix;                   /**< Character(s) to print before line separators. */
        std::string data_prefix;                        /**< Character(s) to print before data cells. */
    };



    /**************************************************************************************************************************
     *                                  Predefined output callbacks
     **************************************************************************************************************************/
public:

    /** Print index row numbers. */
    class RowIdCallback: public OutputCallback {
    public:
        RowIdCallback(): OutputCallback("Num", 4) {}
        virtual bool operator()(bool enabled, const DataArgs&);
    } rowIdCallback;

    /** Print function entry address. */
    class EntryAddrCallback: public OutputCallback {
    public:
        EntryAddrCallback(): OutputCallback("Entry-Addr", 10) {}
        virtual bool operator()(bool enabled, const DataArgs&);
    } entryAddrCallback;

    /** Print function minimum address. */
    class BeginAddrCallback: public OutputCallback {
    public:
        BeginAddrCallback(): OutputCallback("Begin-Addr", 10) {}
        virtual bool operator()(bool enabled, const DataArgs&);
    } beginAddrCallback;

    /** Print function ending address. */
    class EndAddrCallback: public OutputCallback {
    public:
        EndAddrCallback(): OutputCallback("End-Addr", 10) {}
        virtual bool operator()(bool enabled, const DataArgs&);
    } endAddrCallback;

    /** Print number of instructions in function. */
    class SizeInsnsCallback: public OutputCallback {
    public:
        SizeInsnsCallback(): OutputCallback("Insns", 5) {}
        virtual bool operator()(bool enabled, const DataArgs&);
    } sizeInsnsCallback;

    /** Print function size in bytes. */
    class SizeBytesCallback: public OutputCallback {
    public:
        SizeBytesCallback(): OutputCallback("Bytes", 6) {
            set_prefix("/", "-", "/");
        }
        virtual bool operator()(bool enabled, const DataArgs&);
    } sizeBytesCallback;

    /** Print function reason bits. */
    class ReasonCallback: public OutputCallback {
    public:
        ReasonCallback(): OutputCallback("Reason", 1) {} // width will be overridden in the callback
        virtual bool operator()(bool enabled, const HeadingArgs&);
        virtual bool operator()(bool enabled, const DataArgs&);
    } reasonCallback;

    /** Print calling convention. */
    class CallingConventionCallback: public OutputCallback {
    public:
        CallingConventionCallback(): OutputCallback("CallConv", 8) {}
        virtual bool operator()(bool enabled, const DataArgs&);
    } callingConventionCallback;

    /** Print whether function can return. */
    class MayReturnCallback: public OutputCallback {
    public:
        MayReturnCallback(): OutputCallback("Returns", 9) {}
        virtual bool operator()(bool enabled, const DataArgs&);
    } mayReturnCallback;

    /** Print stack delta. */
    class StackDeltaCallback: public OutputCallback {
    public:
        StackDeltaCallback(): OutputCallback("Stack", 9) {}
        virtual bool operator()(bool enabled, const HeadingArgs&);
        virtual bool operator()(bool enabled, const DataArgs&);
    } stackDeltaCallback;

    /** Function name. Prints function name if known, nothing otherwise. */
    class NameCallback: public OutputCallback {
    public:
        NameCallback(): OutputCallback("Name", 32) {}
        virtual bool operator()(bool enabled, const DataArgs&);
    } nameCallback;

    /** Footnotes at the end of the table. */
    class FootnotesCallback: public OutputCallback {
    public:
        FootnotesCallback(): OutputCallback("", 0) {}   // not a table column
        virtual bool operator()(bool enabled, const BeforeAfterArgs&);
    } footnotesCallback;



    /**************************************************************************************************************************
     *                                  Miscellaneous
     **************************************************************************************************************************/
protected:
    typedef std::vector<SgAsmFunction*> Functions;
    Functions functions;                                /**< Functions in index order. */

    /** Initializes the callback lists.  This is invoked by the default constructor. */
    virtual void init();

    /** List of callbacks to be invoked when printing columns. */
    Callbacks::List<OutputCallback> output_callbacks;
};

} // namespace
} // namespace

#endif
#endif
