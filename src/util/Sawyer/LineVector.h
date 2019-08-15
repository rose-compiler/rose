// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Container_LineVector_H
#define Sawyer_Container_LineVector_H

#include <Sawyer/Buffer.h>
#include <Sawyer/Sawyer.h>

#include <algorithm>
#include <boost/filesystem.hpp>
#include <vector>

namespace Sawyer {
namespace Container {

/** A buffer of characters indexed by line number.
 *
 *  This is a character array indexed lines. The line indexes are computed only when necessary. */
class LineVector {
    Buffer<size_t, char>::Ptr buffer_;
    const char *charBuf_;                               // cached ptr to first byte of buffer
    size_t bufSize_;                                    // cached size of buffer
    mutable std::vector<size_t> lineFeeds_;
    mutable size_t nextCharToScan_;

public:
    /** Constructor that creates an empty line vector. */
    LineVector(): charBuf_(NULL), bufSize_(0) {}

    /** Constructor that opens a file.
     *
     *  This constructor doesn't actually read the entire file, it only maps it into memory. */
    explicit LineVector(const boost::filesystem::path&);

    /** Construct a line vector from a buffer. */
    explicit LineVector(const Buffer<size_t, char>::Ptr&);

    /** Construct a line vector from a string. */
    LineVector(size_t nBytes, const char *buf);

    /** (Re)load a line vector from a file. **/
    void load(const boost::filesystem::path&);

    /** (Re)load a line vector from a buffer.) **/
    void load(const Buffer<size_t, char>::Ptr&);

    /** (Re)load a line vector from a string.) **/
    void load(size_t nBytes, const char *buf);

    /** Clears data.
     *
     *  Resets this object to the state created by the default constructor. */
    void clear() {
        buffer_ = Buffer<size_t, char>::Ptr();
        charBuf_ = NULL;
        lineFeeds_.clear();
        nextCharToScan_ = 0;
    }

    /** Number of lines.
     *
     *  Total number of lines including any final line that lacks line termination. Calling this function will cause the entire
     *  file to be read (if it hasn't been already) in order to find all line termination characters. */
    size_t nLines() const;

    /** Number of characters.
     *
     *  Total number of characters including all line termination characters. */
    size_t nCharacters() const {
        return bufSize_;
    }

    /** Number of characters in a line.
     *
     *  Returns the number of characters in the specified line including line termination characters, if any. If @p lineIdx
     *  refers to a line that doesn't exist then the return value is zero. */
    size_t nCharacters(size_t lineIdx) const;

    /** Character at file offset.
     *
     *  Returns the character (as an int) at the specified file offset. If the @p charIdx is beyond the end of the file then
     *  EOF is returned. */
    int character(size_t charIdx) const {
        return charIdx >= nCharacters() ? EOF : (int)charBuf_[charIdx];
    }

    /** Characters at file offset.
     *
     *  Returns a pointer to the character at the specified file offset. The arrary of characters is valid through at least the
     *  following line terminator or the end of the file, whichever comes first. The end of the file is not necessarily
     *  NUL-terminated.  Returns a null pointer if the character index is beyond the end of the file. */
    const char *characters(size_t charIdx) const;

    /** Character at line offset.
     *
     *  Returns the character (as an int) at the specified offset within a line. If @p lineIdx is beyond the end of the file
     *  then EOF is returned. Otherwise, if @p colIdx is beyond the end of a line then NUL characters are returned (which are
     *  also valid characters within a line). */
    int character(size_t lineIdx, size_t colIdx) const;

    /** Characters for a line.
     *
     *  Returns a pointer to a line's characters.  The array is guaranteed to contain at least those characters that appear in
     *  the specified line. The array is not necessarily NUL-terminated, and if a line contains NUL characters then the array
     *  will have NUL characters.  If @p lineIdx is beyond the end of the file then a null pointer is returned. See also, @ref
     *  lineString. */
    const char* lineChars(size_t lineIdx) const;

    /** Line as a string.
     *
     *  Returns a string containing the characters on the line.  The line termination is included in the returned string. If
     *  the line index is out of range then an empty string is returned. This method is slower than using the direct character
     *  pointers because it requires that the characters be copied from the file buffer into the return value.  See also, @ref
     *  lineChars. */
    std::string lineString(size_t lineIdx) const;

    /** Character index for start of line.
     *
     *  Returns the character index for the first character in the specified line. If @p lineIdx is beyond the end of the file
     *  then the total number of characters is returned. */
    size_t characterIndex(size_t lineIdx) const;

    /** Convert a character index to a line index.
     *
     *  Returns the zero-origin line index that contains the specified character. LF characters (line feeds) are considered to
     *  be at the end of a line.  If @p charIndex is beyond the end of the file then the number of lines is returned. */
    size_t lineIndex(size_t charIndex) const;

    /** Convert a character index to a line and column index.
     *
     *  In error messages, line and column numbers are usually 1-origin, but this function returns zero-origin indexes. */
    std::pair<size_t, size_t> location(size_t charIndex) const;

    /** Determines whether the file ends with line termination.
     *
     *  Determining whether the file ends with line termination can be done without reading the entire file. An empty file is
     *  considered to be not terminated. */
    bool isLastLineTerminated() const;

    /** Determines whether the file is empty.
     *
     *  Returns true if the file is empty without trying to read any data. */
    bool isEmpty() const;

private:
    void cacheLines(size_t nLines) const;
    void cacheCharacters(size_t nCharacters) const;
};

} // namespace
} // namespace

#endif

