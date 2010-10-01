#ifndef ROSE_BINARY_REGISTERS_H
#define ROSE_BINARY_REGISTERS_H

#include <map>
#include <set>
#include <string>
#include <vector>

/* Defines register names available for a particular architecture. */
class RegisterDictionary {
    std::string name; /*name of the dictionary, usually an architecture name like 'i386'*/
    std::map<std::string/*name*/, RegisterReference> registers;
public:
    const RegisterReference &lookup(const std::string &name) const;
    RegisterReference &lookup(const std::string &name);

    const std::string &lookup(const RegisterReference&) const;

    const std::map<std::string, RegisterReference>& get_registers() const;
    std::map<std::string, RegisterReference>& get_registers();
};

#endif /*!ROSE_BINARY_REGISTERRS_H*/
