#include <Rose/StringUtility/SplitJoin.h>

#include <Rose/StringUtility/Convert.h>

namespace Rose {
namespace StringUtility {

std::vector<std::string>
split(char separator, const std::string &str, size_t maxparts, bool trim_white_space) {
    return split(std::string(1, separator), str, maxparts, trim_white_space);
}

std::vector<std::string>
split(const std::string &separator, const std::string &str, size_t maxparts, bool trim_white_space) {
    std::vector<std::string> retval;
    if (0==maxparts || str.empty())
        return retval;
    if (separator.empty()) {
        for (size_t i=0; i<str.size() && i<maxparts-1; ++i)
            retval.push_back(str.substr(i, 1));
        retval.push_back(str.substr(retval.size()));
    } else {
        size_t at = 0;
        while (at<=str.size() && retval.size()+1<maxparts) {
            if (at==str.size()) {
                retval.push_back("");                   // string ends with separator
                break;
            }
            size_t sep_at = str.find(separator, at);
            if (sep_at==std::string::npos) {
                retval.push_back(str.substr(at));
                at = str.size() + 1;                    // "+1" means string doesn't end with separator
                break;
            } else {
                retval.push_back(str.substr(at, sep_at-at));
                at = sep_at + separator.size();
            }
        }
        if (at<str.size() && retval.size()<maxparts)
            retval.push_back(str.substr(at));
    }

    if (trim_white_space) {
        for (size_t i=0; i<retval.size(); ++i)
            retval[i] = trim(retval[i]);
    }
    return retval;
}

std::list<std::string>
tokenize(const std::string &s, char delim) {
    std::list<std::string> l;
    std::string token;
    std::istringstream iss(s);
    while (getline(iss, token, delim))
        l.push_back(token);
    return l;
}

std::string
join(const std::string &separator, char *strings[], size_t nstrings) {
    return join_range(separator, strings, strings+nstrings);
}

std::string
join(const std::string &separator, const char *strings[], size_t nstrings) {
    return join_range(separator, strings, strings+nstrings);
}

std::string
join(char separator, char *strings[], size_t nstrings) {
    return join_range(std::string(1, separator), strings, strings+nstrings);
}

std::string
join(char separator, const char *strings[], size_t nstrings) {
    return join_range(std::string(1, separator), strings, strings+nstrings);
}

std::string
joinEnglish(const std::vector<std::string> &phrases, const std::string &separator, const std::string &finalIntro) {
    if (phrases.empty())
        return "";
    if (phrases.size() == 1)
        return phrases[0];
    if (phrases.size() == 2)
        return phrases[0] + " " + finalIntro + " " + phrases[1];

    std::string s;
    for (size_t i=0; i<phrases.size()-1; ++i)
        s += phrases[i] + separator + " ";
    return s + finalIntro + " " + phrases.back();
}

std::string
listToString(const std::list<std::string> &container, bool separateStrings) {
    std::string separator = separateStrings ? " \n" : " ";
    std::string retval = join_range(separator, container.begin(), container.end());
    if (!container.empty())
        retval += separator;
    return retval;
}

std::string
listToString(const std::vector<std::string> &container, bool separateStrings) {
    std::string separator = separateStrings ? " \n" : " ";
    std::string retval = join_range(separator, container.begin(), container.end());
    if (!container.empty())
        retval += separator;
    return retval;
}

std::string
listToString(const std::list<int> &container, bool separateStrings) {
    std::string separator = separateStrings ? " \n" : " ";
    std::string retval = join_range(separator, container.begin(), container.end());
    if (!container.empty())
        retval += separator;
    return retval;
}

std::list<std::string>
stringToList(const std::string &input) {
    std::vector<std::string> substrings = split('\n', input);
    std::list<std::string> retval(substrings.begin(), substrings.end());
    retval.remove("");
    return retval;
}

// This function was written by Bobby Philip in support of the newer approach toward handling a broader number of back-end C++
// compilers.
void
splitStringIntoStrings(const std::string& inputString, char separator, std::vector<std::string>& stringList) {
    stringList.clear();

    std::string::size_type pos = 0, lastPos = 0;
    while (true) {
        pos = inputString.find(separator, pos);
        if (pos == std::string::npos) {
            stringList.push_back(inputString.substr(lastPos));
            return;
        } else {
            if (pos != lastPos) {
                stringList.push_back(inputString.substr(lastPos, pos - lastPos));
            }
            lastPos = pos = pos + 1;
        }
    }
}

} // namespace
} // namespace
