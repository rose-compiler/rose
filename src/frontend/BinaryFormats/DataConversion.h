#ifndef ROSE_DATA_CONVERSION_H
#define ROSE_DATA_CONVERSION_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <string>

/** Encodes and decodes a buffer using an algorithm defined in subclasses.  These objects are used by such things as
 *  SgAsmGenericFile::parse(), where an encoded file is read into a buffer from disk and then decoded in memory.  This allows
 *  ROSE to operate on malicious software while hiding the software from the operating system while it sits on disk. */
class DataConverter {
public:
    virtual ~DataConverter() {}

    /** Name of the converter, mostly for debugging purposes. */
    virtual std::string name() const = 0;

    /** Encodes a buffer.  Encodes the supplied buffer either in place or into a newly allocated buffer. The return value is
     *  the location of the encoded data and the @p nbytes argument should be updated to reflect the size of the encoded data.
     *  The original buffer should not be deleted by this method. */
    virtual uint8_t* encode(uint8_t* buffer, size_t* nbytes) = 0;

    /** Decodes a buffer.  Decodes the supplied buffer either in place or into a newly allocated buffer. The return value is
     *  the location of the decoded data and the @p nbytes argument should be updated to reflect the size of the decoded data.
     *  The original buffer should not be deleted by this method. */
    virtual uint8_t* decode(uint8_t* buffer, size_t* nbytes) = 0;
};

/** The ROT-13 algorithm, extended to operate over the entire 8-bit domain/range. */
class Rot13: public DataConverter {
    virtual ~Rot13() {}
    virtual std::string name() const {return "ROT-13";}
    virtual uint8_t* encode(uint8_t* buffer, size_t* nbytes);
    virtual uint8_t* decode(uint8_t* buffer, size_t* nbytes);
};

#endif
#endif
