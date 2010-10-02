#ifndef ROSE_BINARY_REGISTERS_H
#define ROSE_BINARY_REGISTERS_H

#include <map>
#include <string>

/* Defines register names available for a particular architecture. */
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

    RegisterDictionary(const std::string &name)
        :name(name) {}
    RegisterDictionary(const RegisterDictionary& other) {
        *this = other;
    }

    /** Insert a definition into the dictionary. */
    void insert(const std::string &name, const RegisterDescriptor&);
    void insert(const std::string &name, unsigned majr, unsigned minr, unsigned offset, unsigned nbits);

    /** Inserts definitions from another dictionary into this dictionary. */
    void insert(const RegisterDictionary*);
    /** Inserts definitions from another dictionary into this dictionary. */
    void insert(const RegisterDictionary&);

    /** Returns a descriptor for a given register name. Returns the null pointer if the name is not found. */
    const RegisterDescriptor *lookup(const std::string &name) const;
    /** Returns a descriptor for a given register name. Returns the null pointer if the name is not found. */
    RegisterDescriptor *lookup(const std::string &name);

    /** Returns a register name for a given descriptor. Returns the empty string if the descriptor cannot be found. If more
     * than one register has the same descriptor then one of the matching names is arbitrarily chosen for the return value. */
    const std::string &lookup(const RegisterDescriptor&) const;

    /** Returns the list of all register definitions in the dictionary. */
    const Entries& get_registers() const;
    /** Returns the list of all register definitions in the dictionary. */
    Entries& get_registers();

    /** Prints the contents of this register dictionary. */
    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream&, const RegisterDictionary&);
    

private:
    std::string name; /*name of the dictionary, usually an architecture name like 'i386'*/
    std::map<std::string/*name*/, RegisterDescriptor> regs;
};

#endif /*!ROSE_BINARY_REGISTERRS_H*/
