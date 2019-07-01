#include <sage3basic.h>
#include <BinaryConcolic.h>

#include "io-utility.h"

namespace Rose {
namespace BinaryAnalysis {

namespace Concolic {

// class method
Specimen::Ptr
Specimen::instance(const boost::filesystem::path &executableName) {
    Ptr self = Ptr(new Specimen);

    if (!executableName.empty())
        self->open(executableName);

    self->name(executableName.string());
    return self;
}

Specimen::Ptr
Specimen::instance() {
    return Ptr(new Specimen);
}


void
Specimen::open(const boost::filesystem::path &executableName) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

    content_ = loadBinaryFile(executableName);
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

void Specimen::content(std::vector<uint8_t> binary_data)
{
  content_ = binary_data;
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
