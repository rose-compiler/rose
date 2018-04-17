// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/LineVector.h>
#include <Sawyer/MappedBuffer.h>
#include <Sawyer/StaticBuffer.h>

namespace Sawyer {
namespace Container {

LineVector::LineVector(const boost::filesystem::path &path)
    : charBuf_(NULL), bufSize_(0), nextCharToScan_(0) {
    load(path);
}

LineVector::LineVector(const Buffer<size_t, char>::Ptr &buffer)
    : charBuf_(NULL), bufSize_(0), nextCharToScan_(0) {
    load(buffer);
}

LineVector::LineVector(size_t nBytes, const char *buf)
    : charBuf_(NULL), bufSize_(0), nextCharToScan_(0) {
    load(nBytes, buf);
}

void
LineVector::load(const boost::filesystem::path &path) {
    clear();
    buffer_ = MappedBuffer<size_t, char>::instance(path);
    charBuf_ = buffer_->data();
    bufSize_ = buffer_->size();
    ASSERT_require(charBuf_ != NULL || bufSize_ == 0);
}

void
LineVector::load(const Buffer<size_t, char>::Ptr &buffer) {
    clear();
    ASSERT_not_null(buffer);
    buffer_ = buffer;
    charBuf_ = buffer->data();
    bufSize_ = buffer_->size();
    ASSERT_require(charBuf_ != NULL || bufSize_ == 0);
}

void
LineVector::load(size_t nBytes, const char *buf) {
    clear();
    buffer_ = StaticBuffer<size_t, char>::instance(buf, nBytes);
    charBuf_ = buffer_->data();
    bufSize_ = buffer_->size();
    ASSERT_require(charBuf_ != NULL || bufSize_ == 0);
}

bool
LineVector::isEmpty() const {
    return 0 == bufSize_;
}

bool
LineVector::isLastLineTerminated() const {
    size_t n = bufSize_;
    return n>0 && '\n' == charBuf_[n-1];
}

size_t
LineVector::nLines() const {
    cacheLines((size_t)(-1));                           // cache to end of file
    return lineFeeds_.size() + (!isEmpty() && !isLastLineTerminated() ? 1 : 0);
}

size_t
LineVector::characterIndex(size_t lineIdx) const {
    if (0 == lineIdx)
        return 0;
    cacheLines(lineIdx);
    if (lineIdx-1 >= lineFeeds_.size())
        return nCharacters();
    return lineFeeds_[lineIdx-1] + 1;
}

size_t
LineVector::nCharacters(size_t lineIdx) const {
    return characterIndex(lineIdx+1) - characterIndex(lineIdx);
}

void
LineVector::cacheLines(size_t nLineFeeds) const {
    size_t n = nCharacters();
    size_t i = lineFeeds_.empty() ? (size_t)0 : lineFeeds_.back() + 1;
    i = std::max(i, nextCharToScan_);
    while (lineFeeds_.size() < nLineFeeds && i < n) {
        if ('\n' == charBuf_[i])
            lineFeeds_.push_back(i);
        ++i;
    }
    nextCharToScan_ = i;
}

void
LineVector::cacheCharacters(size_t nChars) const {
    size_t n = std::min(nCharacters(), nChars);
    size_t i = lineFeeds_.empty() ? (size_t)0 : lineFeeds_.back() + 1;
    i = std::max(i, nextCharToScan_);
    while (i < n) {
        if ('\n' == charBuf_[i])
            lineFeeds_.push_back(i);
        ++i;
    }
    nextCharToScan_ = i;
}

int
LineVector::character(size_t lineIdx, size_t colIdx) const {
    size_t i = characterIndex(lineIdx);
    if (i >= nCharacters())
        return EOF;
    size_t n = nCharacters(lineIdx);
    if (colIdx >= n)
        return 0;
    return charBuf_[i+colIdx];
}

const char*
LineVector::characters(size_t charIndex) const {
    if (charIndex >= nCharacters())
        return NULL;
    return charBuf_ + charIndex;
}

const char*
LineVector::lineChars(size_t lineIdx) const {
    size_t i = characterIndex(lineIdx);
    if (i >= nCharacters())
        return NULL;
    return charBuf_ + i;
}

std::string
LineVector::lineString(size_t lineIdx) const {
    if (const char *begin = lineChars(lineIdx)) {
        const char *end = begin + nCharacters(lineIdx);
        return std::string(begin, end);
    } else {
        return std::string();
    }
}

size_t
LineVector::lineIndex(size_t charIdx) const {
    if (charIdx >= nCharacters())
        return nLines();
    cacheCharacters(charIdx+1);
    std::vector<size_t>::const_iterator found = std::lower_bound(lineFeeds_.begin(), lineFeeds_.end(), charIdx);
    if (found == lineFeeds_.end()) {
        return lineFeeds_.size();
    } else {
        return found - lineFeeds_.begin();
    }
}

std::pair<size_t, size_t>
LineVector::location(size_t charIdx) const {
    size_t lineIdx = lineIndex(charIdx);
    size_t colIdx = charIdx - characterIndex(lineIdx);
    return std::make_pair(lineIdx, colIdx);
}

} // namespace
} // namespace
