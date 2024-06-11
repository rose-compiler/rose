#include <Rose/StringUtility/Diagnostics.h>

#include <Rose/StringUtility/Convert.h>

#include <regex>

namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions related to diagnostic messages
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
indentMultilineString(const std::string& inputString, int statementColumnNumber) {
#if 1 // [Robb Matzke 2021-09-29]
    assert(statementColumnNumber > 0);
    return prefixLines(inputString, std::string(statementColumnNumber - 1, ' '));
#else
  // Indent the transformation to match the statement that it is transforming

     std::string returnString;

  // Put in linefeeds to avoid endless recursion in the copyEdit (I think)
     ROSE_ASSERT (statementColumnNumber > 0);
     std::string cr_and_added_space = std::string(statementColumnNumber, ' ');
     cr_and_added_space[0] = '\t';

     returnString = copyEdit (inputString,"\n",cr_and_added_space);

  // Now exchange the line feeds for carriage returns
     returnString = copyEdit (returnString,"\t","\n");

  // Now indent the first line (since there was no CR) there
     returnString = cr_and_added_space.substr(1) + returnString;

  // printf ("In StringUtility::indentMultilineString(): returnString = %s \n",returnString);
     return returnString;
#endif
}

void
add_to_reason_string(std::string &result, bool isset, bool do_pad, const std::string &abbr, const std::string &full) {
    if (isset) {
        if (do_pad) {
            result += abbr;
        } else {
            if (result.size()>0) result += ", ";
            result += full;
        }
    } else if (do_pad) {
        for (size_t i=0; i<abbr.size(); ++i)
            result += ".";
    }
}

std::string
appendAsmComment(const std::string &s, const std::string &comment) {
    if (comment.empty())
        return s;
    if (s.empty())
        return "<" + comment + ">";
    if (s[s.size()-1] == '>')
        return s.substr(0, s.size()-1) + "," + comment + ">";
    return s + "<" + comment + ">";
}

std::string
insertCommas(const std::string &s) {
    std::regex numberRe("([0-9]+)(\\.[0-9+])?");
    std::string retval;
    size_t prevEnd = 0;                                 // end (one past last) of previous match
    for (auto i = std::sregex_iterator(s.begin(), s.end(), numberRe); i != std::sregex_iterator(); ++i) {
        // Stuff prior to number
        std::smatch found = *i;
        retval += s.substr(prevEnd, found.position() - prevEnd);

        // Number, possibly expanded with commas
        const std::string number = found.str(1);
        if (number.size() <= 3) {
            retval += number;
        } else {
            const size_t offset = number.size() % 3;
            for (size_t i = 0; i < number.size(); ++i) {
                if (i > 0 && i % 3 == offset)
                    retval += ',';
                retval += number[i];
            }
        }

        // Fractional part
        retval += found.str(2);
        prevEnd = found.position() + found.length();
    }

    // stuff after last match, or the whole string
    retval += s.substr(prevEnd);
    return retval;
}

} // namespace
} // namespace
