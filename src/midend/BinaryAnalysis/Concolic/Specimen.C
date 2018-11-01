#include <sage3basic.h>
#include <BinaryAnalysis/Concolic.h>

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

void
Specimen::open(const boost::filesystem::path &executableName) {
    if (!isEmpty())
        throw Exception("specimen object is non-empty");

    
    

} // namespace
} // namespace
} // namespace
