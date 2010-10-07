#ifndef ROSE_BINARY_REGISTERS_H
#define ROSE_BINARY_REGISTERS_H

#include <map>
#include <string>

/** Defines registers available for a particular architecture.
 *
 *  The dictionary maps each register name to a RegisterDescriptor.  The RegisterDescriptor describes how a register name maps
 *  to (part of) a physical CPU register by providing a major number, minor number, bit offset, and number of bits.  The major
 *  number is typically a register class number and the minor number is typically an offset within the class.  For instance,
 *  for x86 the major numbers might indicate whether a register is a general purpose register, an 80-bit floating point
 *  register, a 128-bit MMX register, etc.
 *
 *  Users should not assume that the values of a RegisterDescriptor correspond to actual values found in the machine
 *  instructions that were disassembled.  What they can assume is that two unrelated registers (such as "eax" and "ebx") do not
 *  overlap in the RegisterDescriptor address space (major, minor, offset, size). They can also assume that two related
 *  registers (such as "eax" and "rax", the 32- and 64-bit versions of a single CPU register) do, in fact, overlap in the
 *  RegisterDescriptor address space and that the overlap indicates how the registers are related.
 *
 *  Users should not assume that RegisterDescriptor entries from two separate dictionaries are compatible. Looking up the "eax"
 *  register in one dictionary may return a different descriptor than "eax" looked up in a different dictionary.  Components of
 *  the ROSE binary support that generate RegisterDescriptors will provide a mechanism for obtaining (and possibly setting) the
 *  register dictionary.  For instance, the Disassembler class has get_registers() and set_registers() methods. */
class RegisterDictionary {
public:
    typedef std::map<std::string/*name*/, RegisterDescriptor> Entries;

    /* Functions that return a dictionary for a particular machine architecute. (See implementation for documentation.) */
    static const RegisterDictionary *i8086();
    static const RegisterDictionary *i8088();
    static const RegisterDictionary *i286();
    static const RegisterDictionary *i386();
    static const RegisterDictionary *i486();
    static const RegisterDictionary *pentium();
    static const RegisterDictionary *pentium4();
    static const RegisterDictionary *amd64();
    static const RegisterDictionary *arm7();
    static const RegisterDictionary *powerpc();

    RegisterDictionary(const std::string &name)
        :name(name) {}
    RegisterDictionary(const RegisterDictionary& other) {
        *this = other;
    }

    /** Obtain the name of the dictionary. */
    const std::string &get_architecture_name() const {
        return name;
    }

    /** Set the name of the dictionary. Dictionary names are generally architecture names.  Dictionaries created by one of the
     *  built-in static methods of this class have the same name as the method that created it. */
    void set_architecture_name(const std::string &name) {
        this->name = name;
    }

    /** Insert a definition into the dictionary.  If the name already exists in the dictionary then the new RegisterDescriptor
     *  will replace the one that already exists. */
    void insert(const std::string &name, const RegisterDescriptor&);

    /** Insert a definition into the dictionary.  If the name already exists in the dictionary then the new RegisterDescriptor
     *  will replace the one that already exists. */
    void insert(const std::string &name, unsigned majr, unsigned minr, unsigned offset, unsigned nbits);

    /** Inserts definitions from another dictionary into this dictionary. Names in the other dictionary that are the same as
     *  names in this dictionary will replace the definitions in this dictionary. */
    void insert(const RegisterDictionary*);

    /** Inserts definitions from another dictionary into this dictionary. Names in the other dictionary that are the same as
     *  names in this dictionary will replace the definitions in this dictionary. */
    void insert(const RegisterDictionary&);

    /** Returns a descriptor for a given register name. Returns the null pointer if the name is not found. */
    const RegisterDescriptor *lookup(const std::string &name) const;

    /** Returns a descriptor for a given register name. Returns the null pointer if the name is not found. */
    RegisterDescriptor *lookup(const std::string &name);

    /** Returns a register name for a given descriptor. Returns the empty string if the descriptor cannot be found. If more
     *  than one register has the same descriptor then one of the matching names is arbitrarily chosen for the return value. */
    const std::string &lookup(const RegisterDescriptor&) const;

    /** Returns the list of all register definitions in the dictionary. */
    const Entries& get_registers() const;
    /** Returns the list of all register definitions in the dictionary. */
    Entries& get_registers();

    /** Prints the contents of this register dictionary.  The first line of output contains the dictionary name. One additional
     *  line of output will be generated for each entry in the dictionary. */
    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const RegisterDictionary&);

private:
    std::string name; /*name of the dictionary, usually an architecture name like 'i386'*/
    std::map<std::string/*name*/, RegisterDescriptor> regs;
};

#endif /*!ROSE_BINARY_REGISTERRS_H*/
