// #include <rose.h>
#include "sage3basic.h"

#include "IncludeDirective.h"

IncludeDirective::IncludeDirective(const string& directiveText) {
    this -> directiveText = directiveText;
    isQuotedIncludeDirective = false;
    size_t endPos;
    startPos = directiveText.find_first_of("\"");
    if (startPos != string::npos) {                     // #include "file"
        startPos++; //skip the delimiter
        endPos = directiveText.find_last_of("\"");
        isQuotedIncludeDirective = true;
    } else {                                            // #include <file>
        startPos = directiveText.find_first_of("<");
        if (startPos != string::npos) {
            startPos++; //skip the delimiter
            endPos = directiveText.find_last_of(">");
        }
        else                                            // include macro
        {
            // Syntax of the accepted macros: BOOST_PP_ITERATE()
            startPos = directiveText.find("#include") + 9;
            while ((startPos < directiveText.size()) && (directiveText[startPos] == ' '))
                startPos++;
            endPos = directiveText.find(")", startPos)+1;
            ROSE_ASSERT(endPos != string::npos);
        }
    }
    
    includedPath = directiveText.substr(startPos, endPos - startPos);
}

string IncludeDirective::getIncludedPath() {
    return includedPath;
}

bool IncludeDirective::isQuotedInclude() {
    return isQuotedIncludeDirective;
}

size_t IncludeDirective::getStartPos() {
    return startPos;
}

