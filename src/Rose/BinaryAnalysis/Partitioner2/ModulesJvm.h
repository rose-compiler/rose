#ifndef ROSE_BinaryAnalysis_Partitioner2_ModulesJvm_H
#define ROSE_BinaryAnalysis_Partitioner2_ModulesJvm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <sageContainer.h>

#include <iostream>
#include <map>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Utility functions, primarily for handling Java jar files. */
namespace ModulesJvm {

using Rose::BinaryAnalysis::ByteOrder::leToHost;

void warn(const std::string &msg);

//-------------------------------------------------------------------------------------
// The following borrowed largely verbatim (starting with license)
//

// Copyright (c) 2014-2017 Thomas Fussell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, WRISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE
//
// @license: http://www.opensource.org/licenses/mit-license.php


// Various ZIP archive enums. To completely avoid cross platform compiler alignment and platform endian issues, miniz.c doesn't use structs for any of this stuff.
enum {
  // ZIP archive identifiers and record sizes
  MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIG = 0x06054b50,
  MZ_ZIP_CENTRAL_DIR_HEADER_SIG = 0x02014b50,
  MZ_ZIP_LOCAL_DIR_HEADER_SIG = 0x04034b50,
  MZ_ZIP_LOCAL_DIR_HEADER_SIZE = 30,
  MZ_ZIP_CENTRAL_DIR_HEADER_SIZE = 46,
  MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE = 22,
  // Central directory header record offsets
  MZ_ZIP_CDH_SIG_OFS = 0,
  MZ_ZIP_CDH_VERSION_MADE_BY_OFS = 4,
  MZ_ZIP_CDH_VERSION_NEEDED_OFS = 6,
  MZ_ZIP_CDH_BIT_FLAG_OFS = 8,
  MZ_ZIP_CDH_METHOD_OFS = 10,
  MZ_ZIP_CDH_FILE_TIME_OFS = 12,
  MZ_ZIP_CDH_FILE_DATE_OFS = 14,
  MZ_ZIP_CDH_CRC32_OFS = 16,
  MZ_ZIP_CDH_COMPRESSED_SIZE_OFS = 20,
  MZ_ZIP_CDH_DECOMPRESSED_SIZE_OFS = 24,
  MZ_ZIP_CDH_FILENAME_LEN_OFS = 28,
  MZ_ZIP_CDH_EXTRA_LEN_OFS = 30,
  MZ_ZIP_CDH_COMMENT_LEN_OFS = 32,
  MZ_ZIP_CDH_DISK_START_OFS = 34,
  MZ_ZIP_CDH_INTERNAL_ATTR_OFS = 36,
  MZ_ZIP_CDH_EXTERNAL_ATTR_OFS = 38,
  MZ_ZIP_CDH_LOCAL_HEADER_OFS = 42,
  // Local directory header offsets
  MZ_ZIP_LDH_SIG_OFS = 0,
  MZ_ZIP_LDH_VERSION_NEEDED_OFS = 4,
  MZ_ZIP_LDH_BIT_FLAG_OFS = 6,
  MZ_ZIP_LDH_METHOD_OFS = 8,
  MZ_ZIP_LDH_FILE_TIME_OFS = 10,
  MZ_ZIP_LDH_FILE_DATE_OFS = 12,
  MZ_ZIP_LDH_CRC32_OFS = 14,
  MZ_ZIP_LDH_COMPRESSED_SIZE_OFS = 18,
  MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS = 22,
  MZ_ZIP_LDH_FILENAME_LEN_OFS = 26,
  MZ_ZIP_LDH_EXTRA_LEN_OFS = 28,
  // End of central directory offsets
  MZ_ZIP_ECDH_SIG_OFS = 0,
  MZ_ZIP_ECDH_NUM_THIS_DISK_OFS = 4,
  MZ_ZIP_ECDH_NUM_DISK_CDIR_OFS = 6,
  MZ_ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS = 8,
  MZ_ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS = 10,
  MZ_ZIP_ECDH_CDIR_SIZE_OFS = 12,
  MZ_ZIP_ECDH_CDIR_OFS_OFS = 16,
  MZ_ZIP_ECDH_COMMENT_SIZE_OFS = 20
};

enum {
  MZ_ZIP_MAX_IO_BUF_SIZE = 64*1024,
  MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE = 260,
  MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE = 256
};
//-------------------------------------------------------------------------------------


/** This class represents the contents of a central directory file header in a zip file.
 *
 *  The central directory is ...
 *  There are also local headers ...
 */
struct FileStat {

  /** Constructor
   *
   * @param buf The memory buffer for the zip file.
   * @param offset The offset in the buffer to the file header (in bytes). The offset is updated
   *               to the offset of the next file on exit from the constructor.
   * @param fileIndex The index of the current file header.
   */
  explicit FileStat(const uint8_t* buf, size_t &offset, uint32_t fileIndex);

  /** The name of the file */
  std::string filename() const;

  /** Compressed size of the file (bytes) */
  size_t compressedSize() const;

  /** Uncompressed size of the file (bytes) */
  size_t uncompressedSize() const;

  /** Offset (in bytes) of the local headers
   *
   *  The local headers contain ...
   */
  size_t offset() const;

  /** Size (in bytes) of the central directory header */
  uint32_t centralHeaderSize() const;

  // delete constructors
  FileStat() = delete;
  FileStat& operator=(const FileStat&) = delete;
  FileStat(const FileStat&) = default; // Needed to copy into an std::vector

private:

  uint32_t fileIndex_;
  uint16_t versionMadeBy_;
  uint16_t versionNeeded_;
  uint16_t bitFlag_;
  uint16_t method_; // compression method
  // time_t time_; // TODO
  uint16_t time_;
  uint16_t date_;
  uint32_t crc32_;
  uint64_t compSize_;
  uint64_t uncompSize_;
  uint16_t fileNameLength_;
  uint16_t extraFieldLength_;
  uint16_t commentLength_;
  uint16_t diskIndex_;
  uint16_t internalAttr_;
  uint32_t externalAttr_;
  uint32_t localHeaderOfs_;
  char filename_[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];
  char comment_[MZ_ZIP_MAX_ARCHIVE_FILE_COMMENT_SIZE];
};


struct Zipper {

  explicit Zipper(SgAsmGenericFile* gf);

  /** Listing of file names in the container */
  const std::vector<FileStat> & files() const;

  bool present(const std::string &name) const;
  size_t fileSize(const std::string &name) const;
  size_t offset(const char* file) const;

  uint8_t* decode(const std::string &name, size_t &nbytes);

// TODO: probably need the following?
// std::map<std::string,uint8_t*> classMap_/classDictionary/classes;
#if 0
  const uint8_t* data() const {
    return buffer_.data();
  }
#endif
    
  // delete constructors
  Zipper() = delete;
  Zipper(const Zipper &) = delete;
  Zipper &operator=(const Zipper&) = delete;

 private:
  SgAsmGenericFile* gf_;
  std::vector<uint8_t> buffer_;
  std::vector<FileStat> files_;
  std::map<std::string,size_t> offsetMap_;

  // End of central directory (CD) record
  struct ZipEnd {
    explicit ZipEnd(const SgFileContentList &buf);

    uint32_t numFiles() const;
    size_t cdirOffset() const;

  private:
    uint16_t diskNumber_; // number of this disk
    uint16_t diskNumberStart_; // number of disk on which CD record starts
    uint16_t numFiles_; // number of CD entries on this disk
    uint16_t numFilesTotal_; // number of total CD entries
    uint32_t sizeCD_; // size of the CD in bytes
    uint32_t offsetCD_; // offset of the start of the CD
    uint16_t lenComment_; // length of the comment (followed by the comment)

    // delete constructors
    ZipEnd() = delete;
    ZipEnd(const ZipEnd &) = delete;
    ZipEnd &operator=(const ZipEnd &) = delete;

  }; // ZipEnd

  struct LocalHeader {
    size_t localHeaderSize() const;

    explicit LocalHeader(const uint8_t* buf, size_t offset);

  private:
    uint16_t version_;
    uint16_t bitFlag_;
    uint16_t method_; // compression method
    // time_t time_; // TODO
    uint16_t time_;
    uint16_t date_;
    uint32_t crc32_;
    uint64_t compSize_;
    uint64_t uncompSize_;
    uint16_t fileNameLength_;
    uint16_t extraFieldLength_;
    char filename_[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE];

    // delete constructors
    LocalHeader() = delete;
    LocalHeader(const LocalHeader&) = delete;
    LocalHeader& operator=(const LocalHeader&) = delete;

  }; // LocalHeader

}; // Zipper

} // namespace ModulesJvm
} // namespace Partitioner2
} // namespace BinaryAnalysis
} // namespace Rose

#endif // ROSE_ENABLE_BINARY_ANALYSIS
#endif // ROSE_BinaryAnalysis_Partitioner2_ModulesJvm_H
