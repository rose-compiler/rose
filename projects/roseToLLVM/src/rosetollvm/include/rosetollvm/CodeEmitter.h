#ifndef CODE_EMITTER
#define CODE_EMITTER

#include <assert.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>
#include <rosetollvm/FunctionAstAttribute.h>

class CodeEmitter {
public:
    CodeEmitter(Option &option, Control &control, std::string input_file);

    ~CodeEmitter() {
        assert(bufferPool.size() == 0);
    }

    static const char *indent()        { return "        "; }
    static const char *commentIndent() { return "      ; "; }

    CodeEmitter& operator<< (bool val)                             { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (short val)                            { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (unsigned short val)                   { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (int val)                              { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (unsigned int val)                     { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (long val)                             { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (unsigned long val)                    { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (long long val)                        { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (unsigned long long val)               { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (float val)                            { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (double val)                           { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (long double val)                      { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (const void *val)                      { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (const std::string &val)               { (*Cout) << val; if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (std::streambuf* sb)                   { (*Cout) << sb;  if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (std::ostream& (*pf)(std::ostream&))   { pf(*Cout);      if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (std::ios& (*pf)(std::ios&))           { pf(*Cout);      if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (std::ios_base& (*pf)(std::ios_base&)) { pf(*Cout);      if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }

    CodeEmitter& operator<< (char c)                               { (*Cout) << c;   if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (signed char c)                        { (*Cout) << c;   if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (unsigned char c)                      { (*Cout) << c;   if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
 
    CodeEmitter& operator<< (const char* s)                        { (*Cout) << s;   if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (const signed char* s)                 { (*Cout) << s;   if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }
    CodeEmitter& operator<< (const unsigned char* s)               { (*Cout) << s;   if (Cout == default_output) last_instruction_was_an_unconditional_branch = false; return *this; }

    void startOutputToBuffer();
    void flushTopBuffer();
    void endOutputToBuffer();
    void emitUnconditionalBranch(const char *label);
    void emitUnconditionalBranch(std::string label);
    void emitLabel(FunctionAstAttribute *attribute, std::string label);

protected:

    std::vector<std::stringstream *> bufferPool;

    std::ostream *Cout,
                 *default_output;

    bool last_instruction_was_an_unconditional_branch;
};

#endif
