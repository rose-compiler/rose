#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinarySourceLocations.h>

namespace Rose {
namespace BinaryAnalysis {

SourceLocations::SourceLocations(const SourceLocations &other) {
    SAWYER_THREAD_TRAITS::LockGuard lock(other.mutex_);
    srcToAddr_ = other.srcToAddr_;
    addrToSrc_ = other.addrToSrc_;
}

SourceLocations&
SourceLocations::operator=(const SourceLocations &other) {
    SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other.mutex_);
    srcToAddr_ = other.srcToAddr_;
    addrToSrc_ = other.addrToSrc_;
    return *this;
}

void
SourceLocations::insert(const SourceLocation &src, rose_addr_t va) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    eraseNS(va);
    if (!src.isEmpty()) {
        srcToAddr_.insertMaybeDefault(src).insert(va);
        addrToSrc_.insert(va, src);
    }
}

void
SourceLocations::insert(const SourceLocation &src, const AddressInterval &vas) {
    // No lock necessary since we call synchronized insert.
    // FIXME[Robb Matzke 2020-02-07]: this could be more efficient instead of doing one at a time
    BOOST_FOREACH (rose_addr_t va, vas)
        insert(src, va);
}

void
SourceLocations::insert(const SourceLocation &src, const AddressIntervalSet &vas) {
    // No lock necessary since we call synchronized insert.
    BOOST_FOREACH (const AddressInterval &interval, vas.intervals())
        insert(src, interval);
}

void
SourceLocations::erase(const SourceLocation &src, rose_addr_t va) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    eraseNS(src, va);
}

void
SourceLocations::eraseNS(const SourceLocation &src, rose_addr_t va) {
    if (src.isEmpty() || addrToSrc_.getOrDefault(va) != src)
        return;
    addrToSrc_.erase(va);
    srcToAddr_[src].erase(va);

    // This step is optional, but it keeps things tidy
    if (srcToAddr_[src].isEmpty())
        srcToAddr_.erase(src);
}

void
SourceLocations::erase(const SourceLocation &src, const AddressInterval &vas) {
    // No lock necessary since we call synchronized erase.
    AddressIntervalSet set;
    set |= vas;
    erase(src, set);
}

void
SourceLocations::erase(const SourceLocation &src, const AddressIntervalSet &vas) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    if (src.isEmpty())
        return;
    AddressIntervalSet empty;
    AddressIntervalSet &addrs = srcToAddr_.getOrElse(src, empty);
    AddressIntervalSet toErase = addrs & vas;           // links to be erased
    addrs -= vas;
    BOOST_FOREACH (const AddressInterval &interval, toErase.intervals())
        addrToSrc_.erase(interval);
}

void
SourceLocations::erase(rose_addr_t va) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    eraseNS(va);
}

void
SourceLocations::eraseNS(rose_addr_t va) {
    SourceLocation src = addrToSrc_.getOrDefault(va);
    if (!src.isEmpty()) {
        srcToAddr_[src].erase(va);
        addrToSrc_.erase(va);

        // This step is optional, but it keeps things tidy
        if (srcToAddr_[src].isEmpty())
            srcToAddr_.erase(src);
    }
}

void
SourceLocations::erase(const AddressInterval &vas) {
    // No lock necessary since we call synchronized erase.
    // FIXME[Robb Matzke 2020-02-07]: This could be more efficient instead of doing one at a time
    BOOST_FOREACH (rose_addr_t va, vas)
        erase(va);
}

void
SourceLocations::erase(const AddressIntervalSet &vas) {
    // No lock necessary since we call synchronized erase.
    BOOST_FOREACH (const AddressInterval &interval, vas.intervals())
        erase(interval);
}

void
SourceLocations::erase(const SourceLocation &src) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    if (!src.isEmpty()) {
        AddressIntervalSet empty;
        BOOST_FOREACH (const AddressInterval &interval, srcToAddr_.getOrElse(src, empty).intervals())
            addrToSrc_.erase(interval);
        srcToAddr_.erase(src);
    }
}

void
SourceLocations::clear() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    srcToAddr_.clear();
    addrToSrc_.clear();
}

void
SourceLocations::insertFromDebug(SgNode *ast) {
    // No lock necessary since we call synchronized insert.
    struct T: AstSimpleProcessing {
        SourceLocations *self;
        T(SourceLocations *self): self(self) {}
        void visit(SgNode *node) ROSE_OVERRIDE {
            if (SgAsmDwarfLineList *ll = isSgAsmDwarfLineList(node)) {
                BOOST_FOREACH (SgAsmDwarfLine *line, ll->get_line_list()) {
                    SourceLocation src(Sg_File_Info::getFilenameFromID(line->get_file_id()), line->get_line());
                    self->insert(src, line->get_address());
                }
            }
        }
    } visitor(this);
    visitor.traverse(ast, preorder);
}

void
SourceLocations::fillHoles(size_t maxHoleSize) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    AddressToSource fill;                               // new entries that fill in holes
    typedef AddressToSource::ConstNodeIterator Iter;
    for (Iter left = addrToSrc_.nodes().begin(); left != addrToSrc_.nodes().end(); ++left) {
        Iter right = left; ++right;

        // Find the hole
        AddressInterval hole;
        if (left->key().greatest() == AddressInterval::whole().greatest()) {
            break;
        } else if (right == addrToSrc_.nodes().end()) {
            hole = AddressInterval::hull(left->key().greatest() + 1, AddressInterval::whole().greatest());
        } else if (left->key().greatest() < right->key().least()) {
            hole = AddressInterval::hull(left->key().greatest() + 1, right->key().least() - 1);
        }

        // Fill the hole (delayed until we're done iterating
        if (!hole.isEmpty() && hole.size() <= maxHoleSize)
            fill.insert(hole, left->value());
    }

    // Delayed filling of holes
    addrToSrc_.insertMultiple(fill);
    BOOST_FOREACH (const AddressToSource::Node &node, fill.nodes())
        srcToAddr_[node.value()] |= node.key();
}

SourceLocation
SourceLocations::get(rose_addr_t va) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return addrToSrc_.getOrDefault(va);
}

AddressIntervalSet
SourceLocations::get(const SourceLocation &src) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return srcToAddr_.getOrDefault(src);
}

Sawyer::Optional<rose_addr_t>
SourceLocations::firstAddress(const SourceLocation &src) const {
    // No lock necessary since we call synchronized get.
    const AddressIntervalSet &vas = get(src);
    if (vas.isEmpty())
        return Sawyer::Nothing();
    return *vas.scalars().begin();
}

std::set<boost::filesystem::path>
SourceLocations::allFileNames() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    std::set<boost::filesystem::path> retval;
    BOOST_FOREACH (const SourceLocation &src, srcToAddr_.keys())
        retval.insert(src.fileName());
    return retval;
}

SourceLocation
SourceLocations::nextSourceLocation(const SourceLocation &current) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    SourceToAddress::ConstNodeIterator next = srcToAddr_.nodes().end();
    if (current.isEmpty()) {
        next = srcToAddr_.nodes().begin();
    } else {
        next = srcToAddr_.find(current);
        if (next != srcToAddr_.nodes().end())
            ++next;
    }

    if (next == srcToAddr_.nodes().end())
        return SourceLocation();
    return next->key();
}

void
SourceLocations::printSrcToAddr(std::ostream &out, const std::string &prefix) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    BOOST_FOREACH (const SourceToAddress::Node &node, srcToAddr_.nodes()) {
        out <<prefix <<node.key() <<":";
        BOOST_FOREACH (rose_addr_t va, node.value().scalars())
            out <<" " <<StringUtility::addrToString(va) <<"\n";
    }
}

void
SourceLocations::printAddrToSrc(std::ostream &out, const std::string &prefix) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    BOOST_FOREACH (const AddressToSource::Node &node, addrToSrc_.nodes())
        out <<prefix <<StringUtility::addrToString(node.key()) <<": " <<node.value() <<"\n";
}

void
SourceLocations::print(std::ostream &out, const std::string &prefix) const {
    // No lock necessary since we call synchronized printSrcToAddr.
    printSrcToAddr(out, prefix);
}

std::ostream&
operator<<(std::ostream &out, const SourceLocations &x) {
    x.print(out);
    return out;
}

} // namespace
} // namespace

#endif
