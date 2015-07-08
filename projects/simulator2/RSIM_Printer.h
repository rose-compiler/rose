#ifndef ROSE_RSIM_Print_H
#define ROSE_RSIM_Print_H

class RSIM_Thread;

/** Holds information about how to translate a constant or flag to a string.  Some of the printing functions take an array
 *  of these objects, and the array is terminated with value having a null pointer for the 'str' member. The T() and E_END
 *  macros can be used to build the list succinctly.  For example:
 *
 *  \code
 *    Translate t[] = { TE(PROT_READ), TE(PROT_WRITE), TE(PROT_EXEC), TE(PROT_NONE), T_END};
 *  \endcode
 *
 *  This data structure supports a number of cases:
 *
 *  <ul>
 *    <li>It can interpret an integer value as either an enumerated constant (specified with the "TE" family of macros) or
 *        a vector of single- or multi-bit flags (specified with the "TF" family of macros).</li>
 *    <li>The symbol can be defined before the macro that adds it to the Translate array, or it can be defined as it's
 *        added to the array by using the macros that have a "V" (value) argument.</li>
 *    <li>Flag symbols look only for the bits that are set in the symbol's value.  However, a mask can be supplied by
 *        using macros that have an "M" (mask) argument.  When checking for a symbol, we look at all the bits specified
 *        in the mask.  Under the covers, the only difference between a vector of flags and an enumeration value is that the
 *        mask for the enumeration value has all bits set.</li>
 *    <li>For a bit vector, bits that don't correspond to any known symbols can be formatted as an unsigned integer using
 *        a user-supplied printf format string.  This is useful, for instance, for printing permission bits as an octal
 *        number.</li>
 *  </ul>
 *
 *  Note: doxygen doesn't seem to document C preprocessor symbols, so you'll have to look at the source code. See
 *  RSIM_Printer.h for the macro definitions; see RSIM_Common.h for example definitions; see system call handlers for example
 *  usage.
 */
struct Translate {
    unsigned    mask;
    unsigned    val;
    const char  *str;
};

// See class Translate
#define TF(X)           {(0==X?unsigned(-1):X), X, #X}  // Define a bit flag.
#define TF2(M,X)        {M, X, #X}                      // Define a bit vector with possible zero bits.
#define TF3(M,V,X)      {M, V, #X}                      // Define a masked flag when X is not defined.
#define TF_FMT(M,FMT)   {0, M, FMT}                     // Format remaining bits; FMT is a printf format string.
#define TE(X)           {(uint32_t)-1, X, #X}           // Define an enumerated constant.
#define TE2(V,X)        {(uint32_t)-1, V, #X}           // Define an enumerated constant with no predefined name symbol.
#define T_END           {0, 0, NULL}                    // Terminates a Translate table.

typedef void (*StructPrinter)(Sawyer::Message::Stream&, const uint8_t*, size_t);

/** Used for printing syscall arguments. */
class Printer {
    RSIM_Thread *thread_;
    Sawyer::Message::Stream &out_;
    const uint32_t *args32_;                            // optional args instead of querying from thread
    const uint64_t *args64_;                            // ditto; use either args32_ or args64_ but not both.
    int argNum_;                                        // -1 means return value
    size_t nPrinted_;                                   // number of values printed (not counting eret() and str())
    bool hadRetError_;                                  // true if eret() displayed an error
    std::string onDestruction_;                         // what to print when destroyed
public:
    Printer(Sawyer::Message::Stream &out, RSIM_Thread *thread, const std::string &onDestruction)
        : thread_(thread), out_(out), args32_(NULL), args64_(NULL), argNum_(0), nPrinted_(0), hadRetError_(false),
          onDestruction_(onDestruction) {};
    Printer(Sawyer::Message::Stream &out, RSIM_Thread *thread, const uint32_t *args, const std::string &onDestruction)
        : thread_(thread), out_(out), args32_(args), args64_(NULL), argNum_(0), nPrinted_(0), hadRetError_(false),
          onDestruction_(onDestruction) {};
    Printer(Sawyer::Message::Stream &out, RSIM_Thread *thread, const uint64_t *args, const std::string &onDestruction)
        : thread_(thread), out_(out), args32_(NULL), args64_(args), argNum_(0), nPrinted_(0), hadRetError_(false),
          onDestruction_(onDestruction) {};

    ~Printer();

    static std::string flags_to_str(const Translate *tlist, uint32_t value);

    // set to a specific argument number (-1 means return value)
    Printer& arg(int n);

    // get an argument and advance the pointer.
    uint64_t nextArg();

    // print a string
    Printer& str(const std::string&);

    // skip argument, print "<unused>"
    Printer& unused();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOTE: The following single-letter method names come directly from the original C version that
    // passed this information as a "const char*" of format characters.

    // print buffer unless hadRetError
    Printer& b(int64_t nbytes);
    Printer& b(rose_addr_t va, int64_t nbytes);
    Printer& b(rose_addr_t va, const uint8_t *buf, size_t actualSize, size_t printSize);
    static void print_buffer(Sawyer::Message::Stream&, rose_addr_t va, const uint8_t *buf, size_t actualSize, size_t printSize);

    // print signed decimal unless hadRetError
    Printer& d();
    Printer& d(int64_t value);
    static void print_decimal(Sawyer::Message::Stream&, int64_t value);

    // Print enum unless hadRetError
    Printer& e(const Translate *tlist);
    Printer& e(uint64_t value, const Translate *tlist);
    static void print_enum(Sawyer::Message::Stream&, const Translate *tlist, uint64_t value);
    
    // Print flags unless hadRetError
    Printer& f(const Translate *tlist);
    Printer& f(uint64_t value, const Translate *tlist);
    static void print_flags(Sawyer::Message::Stream&, const Translate *tlist, uint64_t value);

    // Print pointer unless hadRetError
    Printer& p();
    Printer& p(rose_addr_t va);
    static void print_pointer(Sawyer::Message::Stream&, rose_addr_t va);

    // Print pointer to struct unless hadRetError
    Printer& P(size_t nBytes, StructPrinter);
    Printer& P(rose_addr_t va, size_t structSize, StructPrinter);
    Printer& P(rose_addr_t va, const uint8_t *buffer, size_t structSize, size_t bufferSize, StructPrinter);
    static void print_struct(Sawyer::Message::Stream&, rose_addr_t va, const uint8_t *buf, size_t structSize, size_t bufferSize,
                             StructPrinter);

    // Print string unless hadRetError
    Printer& s();
    Printer& s(rose_addr_t va);
    static void print_string(Sawyer::Message::Stream&, rose_addr_t va, const std::string&, bool trunc=false, bool error=false);
    static void print_string(Sawyer::Message::Stream&,                 const std::string&, bool trunc=false, bool error=false);

    // Print time unless hadRetError
    Printer& t();
    Printer& t(uint64_t value);
    static void print_time(Sawyer::Message::Stream&, rose_addr_t value);

    // Print hex value unless hadRetError
    Printer& x();
    Printer& x(uint64_t value);
    static void print_hex(Sawyer::Message::Stream&, uint64_t value);

    // Print integer or error (setting hadRetError)
    Printer& ret();
    Printer& ret(int64_t value);
    static bool print_ret(Sawyer::Message::Stream&, int64_t value);

    // Print error number and set hadRetError, or do nothing; does not advance argument or output counters
    Printer& eret();
    Printer& eret(int64_t value);
    static bool print_eret(Sawyer::Message::Stream&, int64_t value);
};

#endif
