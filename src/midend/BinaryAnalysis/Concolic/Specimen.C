#include <sage3basic.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/lexical_cast.hpp>
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

std::string
Specimen::printableName(const Database::Ptr &db) {
    std::string retval = "specimen";
    if (db) {
        if (SpecimenId id = db->id(sharedFromThis(), Update::NO))
            retval += " " + boost::lexical_cast<std::string>(*id);
    }
    if (!name().empty())
        retval += " \"" + StringUtility::cEscape(name()) + "\"";
    return retval;
}

void Specimen::content(std::vector<uint8_t> binary_data) {
  content_ = binary_data;
  read_only_ = empty_ = false;
}

const std::vector<uint8_t>&
Specimen::content() const {
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

  read_only_ = true;
  return content_;
}


} // namespace
} // namespace
} // namespace

#endif
