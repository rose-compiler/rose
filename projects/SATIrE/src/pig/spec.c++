#include <string>
#include <sstream>

#include "spec.h"

std::string splice(std::string s)
{
    std::string::size_type pos;

    for (pos = 0; pos != std::string::npos; pos = s.find('\n', pos))
    {
        if (pos > 0 && s[pos-1] != '\\')
        {
            s.insert(pos, "\\");
        }
        ++pos;
    }

    return s;
}

std::string macroname(std::string rulename, unsigned long ruleno)
{
    std::stringstream s;
    
    s << "PIG_RULE_" << rulename << '_' << ruleno;
    
    return s.str();
}
