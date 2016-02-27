
#include "GrammarFile.h"

GrammarFile::GrammarFile ( std::string inputFilename = "", const StringUtility::FileWithLineNumbers inputBuffer = StringUtility::FileWithLineNumbers() ) {
  setFilename (inputFilename);
  setBuffer(inputBuffer);
}

GrammarFile::GrammarFile ( const GrammarFile & X ) {
  *this = X;
}

GrammarFile::~GrammarFile () {
}

GrammarFile & GrammarFile::operator= ( const GrammarFile & X ) {
  setFilename (X.getFilename());
  setBuffer(X.getBuffer());
  return *this;
}

void GrammarFile::setFilename ( const std::string& inputFilename ) {
  filename = inputFilename;
}

std::string GrammarFile::getFilename() const {
  return filename;
}

void GrammarFile::setBuffer ( const StringUtility::FileWithLineNumbers& inputBuffer ) {
               buffer = inputBuffer;
}

const StringUtility::FileWithLineNumbers& GrammarFile::getBuffer() const {
  return buffer;
}
