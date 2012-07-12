#include <rose.h>

#include "IncludeDirective.h"

IncludeDirective::IncludeDirective(const string& directiveText) {
    this -> directiveText = directiveText;
    isQuotedIncludeDirective = false;
    size_t endPos;
    startPos = directiveText.find_first_of("\"");
    if (startPos != string::npos) {
        endPos = directiveText.find_last_of("\"");
        isQuotedIncludeDirective = true;
    } else {
        startPos = directiveText.find_first_of("<");
        ROSE_ASSERT(startPos != string::npos); //should have at least "\"" or "<"
        endPos = directiveText.find_last_of(">");
    }
    ROSE_ASSERT(endPos != string::npos); //should find the matching delimeter
    startPos++; //skip the delimiter
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