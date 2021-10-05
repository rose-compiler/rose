#include <Rose/StringUtility/Diagnostics.h>

#include <Rose/StringUtility/Convert.h>

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

} // namespace
} // namespace
