#ifndef _IO_UTILITY_H
#define _IO_UTILITY_H 1

//! Utility functions to access binary and text files.

#include <fstream>
#include <streambuf>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

template <class Container>
Container
loadFile(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
{
  typedef std::istreambuf_iterator<char> stream_iterator;

  std::ifstream stream(filename.c_str(), mode);

  if (!stream.good())
    throw std::runtime_error("Unable to open " + filename + ".");

  Container     res;

  // \todo reserve capacity of res
  std::copy(stream_iterator(stream), stream_iterator(), std::back_inserter(res));
  return res;
}


// https://stackoverflow.com/questions/31131907/writing-into-binary-file-with-the-stdostream-iterator
template <class T, class CharT = char, class Traits = std::char_traits<CharT> >
struct ostreambin_iterator : std::iterator<std::output_iterator_tag, void, void, void, void>
{
  typedef std::basic_ostream<CharT, Traits> ostream_type;
  typedef Traits                            traits_type;
  typedef CharT                             char_type;

  explicit
  ostreambin_iterator(ostream_type& s) : stream(s) { }

  ostreambin_iterator& operator=(const T& value)
  {
    stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return *this;
  }

  ostreambin_iterator& operator*()     { return *this; }
  ostreambin_iterator& operator++()    { return *this; }
  ostreambin_iterator& operator++(int) { return *this; }

  ostream_type& stream;
};

template <class T>
struct FileSink
{
  typedef ostreambin_iterator<T> insert_iterator;

  std::ostream& datastream;

  explicit
  FileSink(std::ostream& stream)
  : datastream(stream)
  {}

  void reserve(size_t) {}

  insert_iterator
  inserter()
  {
    return insert_iterator(datastream);
  }
};

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

#endif /* _IOUTILITY_H */
