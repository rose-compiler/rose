#ifndef ROSE_BinaryAnalysis_Concolic_io_utility_H
#define ROSE_BinaryAnalysis_Concolic_io_utility_H
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

//! Utility functions to access binary and text files.

#include <fstream>
#include <streambuf>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Loads a file @ref filename and stores the raw char data in a sequence of
 *  type Container.
 *
 *  @tparam Container a sequence container
 *  @param  filename  the file to load
 *  @param  mode      opening mode
 */
template <class Container>
Container
loadFile(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
{
  typedef std::istreambuf_iterator<char> stream_iterator;

  std::ifstream stream(filename.c_str(), mode);

  if (!stream.good())
    throw std::runtime_error("Unable to open " + filename + ".");

  Container     res;

  // \todo reserve capacity in res
  std::copy(stream_iterator(stream), stream_iterator(), std::back_inserter(res));
  return res;
}


/** An output iterator for streams that store binary data.
 *
 * @note https://stackoverflow.com/questions/31131907/writing-into-binary-file-with-the-stdostream-iterator
 */
template <class T, class CharT = char, class Traits = std::char_traits<CharT> >
struct OStreamBinaryIterator : std::iterator<std::output_iterator_tag, void, void, void, void>
{
    typedef std::basic_ostream<CharT, Traits> ostream_type;
    typedef Traits                            traits_type;
    typedef CharT                             char_type;

    /** creates a new OStreamBinaryIterator around a basic_ostream object. */
    explicit
    OStreamBinaryIterator(ostream_type& s) : stream(s) { }

    /** writes a character to the output stream. */
    OStreamBinaryIterator& operator=(const T& value)
    {
      stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
      return *this;
    }

    /** iterator compatibility functions
     *  @{
     */
    OStreamBinaryIterator& operator*()     { return *this; }
    OStreamBinaryIterator& operator++()    { return *this; }
    OStreamBinaryIterator& operator++(int) { return *this; }
    /** @} */

  private:
    ostream_type& stream;

    OStreamBinaryIterator();
    OStreamBinaryIterator& operator=(const OStreamBinaryIterator&);
};

/** Implements an abstraction of storing data to a stream. This class
 *  streams to a C++ output stream.
 *
 *  @note class could be replaced by other classes streaming efficiently
 *        to files (e.g., memory mapped files).
 */
template <class T>
struct FileSink
{
    typedef OStreamBinaryIterator<T> insert_iterator;

    /** creates a new sink object. */
    explicit
    FileSink(std::ostream& stream)
    : datastream(stream)
    {}

    /** reserves a number of bytes on the stream. */
    void reserve(size_t) {}

    /** returns an inserter to the storage. */
    insert_iterator
    inserter()
    {
      return insert_iterator(datastream);
    }

  private:
    std::ostream& datastream;
};


/** copies to content of a sequence container @ref Container to a file
 *  @ref filename.
 *
 *  @tparam Container a sequence container type.
 *  @param  data      a sequence container
 *  @param  filename  output file
 *  @param  mode      file opening mode
 */
template <class Container>
void
storeFile(const Container& data, const std::string& filename, std::ios_base::openmode mode = std::ios_base::out)
{
  std::ofstream stream(filename.c_str(), mode);

  if (!stream.good())
    throw std::runtime_error("Unable to open " + filename + ".");

  FileSink<char> sink(stream);

  sink.reserve(data.size());
  std::copy(data.begin(), data.end(), sink.inserter());
}


namespace
{
  /** Loads a text file into a string.
   *
   * Throws a std::runtime_error if the file cannot be opened.
   */
  inline
  std::string
  loadTextFile(const boost::filesystem::path& path)
  {
    return loadFile<std::string>(path.string());
  }


  /** Stores a string into a text file.
   *
   * Throws a std::runtime_error if the file cannot be opened.
   */
  inline
  void
  storeTextFile(const std::string& data, const boost::filesystem::path& path)
  {
    storeFile(data, path.string());
  }


  /** Loads a binary file.
   *
   * Throws a std::runtime_error if the file cannot be opened.
   */
  inline
  std::vector<uint8_t>
  loadBinaryFile(const boost::filesystem::path& path)
  {
    return loadFile<std::vector<uint8_t> >(path.string(), std::ios::in | std::ios::binary);
  }

  /** Stores a binary file.
   *
   * Throws a std::runtime_error if the file cannot be opened.
   */
  inline
  void
  storeBinaryFile(const std::vector<uint8_t>& data, const boost::filesystem::path& path)
  {
    storeFile(data, path.string(), std::ofstream::binary);
  }
} // anonymous namespace

} // namespace Concolic
} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif
