#include <sage3basic.h>
#include <BinaryConcolic.h>

template <class ExceptionType = std::runtime_error>
static inline
void
throw_ex(std::string arg1, const std::string& arg2 = "", const std::string& arg3 = "")
{
  arg1.append(arg2);
  arg1.append(arg3);

  throw ExceptionType(arg1);
}


namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

// class method
Specimen::Ptr
Specimen::instance(const boost::filesystem::path &executableName) {
    Ptr self = Ptr(new Specimen);
    if (!executableName.empty())
        self->open(executableName);
    return self;
}

Specimen::Ptr
Specimen::instance() {
    return Ptr(new Specimen);
}


void
Specimen::open(const boost::filesystem::path &executableName) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

    std::ifstream stream(executableName.string(), std::ios::in | std::ios::binary);

    if (!stream.good())
    {
      throw_ex<>("Unable to open ", executableName.string(), ".");
    }

    std::copy( std::istreambuf_iterator<char>(stream),
               std::istreambuf_iterator<char>(),
               std::back_inserter(content_)
             );

    empty_ = false;
}

void
Specimen::close() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

    content_.clear();
    empty_ = true;
}

bool
Specimen::isEmpty() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return content_.empty();
}

std::string
Specimen::name() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return name_;
}

void
Specimen::name(const std::string &s) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    name_ = s;
}

void
Specimen::content(const std::string& binary_string)
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  if (read_only_) throw_ex<std::logic_error>("write after read observed on specimen::content_");

  content_.clear();
  content_.reserve(binary_string.size());
  std::copy(binary_string.begin(), binary_string.end(), std::back_inserter(content_));
}

const std::vector<uint8_t>&
Specimen::content() const
{
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  read_only_ = true;
  return content_;
}


} // namespace
} // namespace
} // namespace
