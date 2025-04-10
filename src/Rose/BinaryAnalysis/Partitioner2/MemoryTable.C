#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/MemoryTable.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/NumberToString.h>

using namespace Sawyer::Message::Common;
using Rose::StringUtility::addrToString;
using Rose::StringUtility::plural;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MemoryTable::Entries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemoryTable::Entries::~Entries() {}

MemoryTable::Entries::Entries() {}

bool
MemoryTable::Entries::empty() const {
    return size() == 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MemoryTable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemoryTable::~MemoryTable() {}

MemoryTable::MemoryTable(const AddressInterval &tableLimits, const size_t bytesPerEntry, const Entries::Ptr &entries)
    : tableLimits_(tableLimits), bytesPerEntry_(bytesPerEntry), entries_(notnull(entries)) {}

MemoryTable::Ptr
MemoryTable::instance(const AddressInterval &tableLimits, const size_t bytesPerEntry, const Entries::Ptr &entries) {
    return Ptr(new MemoryTable(tableLimits, bytesPerEntry, entries));
}

size_t
MemoryTable::maxInitialSkip() const {
    return maxSkippable_;
}

void
MemoryTable::maxInitialSkip(const size_t n) {
    maxSkippable_ = n;
}

size_t
MemoryTable::maxPreEntries() const {
    return maxPreEntries_;
}

void
MemoryTable::maxPreEntries(const size_t n) {
    maxPreEntries_ = n;
}

AddressInterval
MemoryTable::tableLimits() const {
    return tableLimits_;
}

void
MemoryTable::tableLimits(const AddressInterval &limits) {
    tableLimits_ = limits;
}

size_t
MemoryTable::bytesPerEntry() const {
    return bytesPerEntry_;
}

MemoryTable::Entries::Ptr
MemoryTable::entries() const {
    return entries_;
}

AddressInterval
MemoryTable::scan(const MemoryMap::Constraints &map, const Address probableTableAddr) {
    ASSERT_require(bytesPerEntry_ > 0);

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    if (debug) {
        debug <<"scan memory table within " <<addrToString(tableLimits_) <<" with " <<bytesPerEntry_ <<"-byte entries\n";
        debug <<"  probable start address: " <<addrToString(probableTableAddr) <<"\n";
        debug <<"  max initial invalid entries: " <<maxSkippable_ <<"\n";
    }
    if (!tableLimits_.contains(probableTableAddr)) {
        SAWYER_MESG(debug) <<"  start address is outside table limits\n";
        return {};
    }

    // Parse forward from probable start of table
    size_t tableAddr = probableTableAddr;               // start here, and adjust as we learn more
    size_t nSkipped = 0;
    while (1) {
        // Entry location
        const Address entryAddr = tableAddr + entries_->size() * bytesPerEntry_;
        const auto entryLoc = AddressInterval::baseSize(entryAddr, bytesPerEntry_);
        SAWYER_MESG(debug) <<"  entry at " <<addrToString(entryLoc) <<"\n";
        if (!tableLimits_.contains(entryLoc)) {
            SAWYER_MESG(debug) <<"    entry falls outside table limits\n";
            break;
        }

        // Read bytes for table entry
        std::vector<uint8_t> buffer(bytesPerEntry_, 0);
        const AddressInterval read = map.within(tableLimits_).at(entryAddr).read(buffer);
        if (read.size() != bytesPerEntry_) {
            SAWYER_MESG(debug) <<"    entry data read failed (got " <<plural(read.size(), "bytes") <<")\n";
            break;
        }

        // Parse and insert, or maybe skip initial entries
        tableLocation_ = AddressInterval::baseSize(tableAddr, (entries_->size() + 1) * bytesPerEntry_);
        if (entries_->maybeAppend(this, entryAddr, buffer)) {
            SAWYER_MESG(debug) <<"    entry parsed and appended to table\n";
        } else {
            if (entries_->size() == 0) {
                if (++nSkipped > maxSkippable_) {
                    SAWYER_MESG(debug) <<"    entry failed to parse (skip limit reached)\n";
                    break;
                } else {
                    SAWYER_MESG(debug) <<"    entry failed to parse (skipped); table address now " <<addrToString(tableAddr) <<"\n";
                    tableAddr += bytesPerEntry_;
                }
            } else {
                SAWYER_MESG(debug) <<"    entry failed to parse (reached end of table)\n";
                break;
            }
        }
    }

    // Parse backward from probable start of table
    nPreEntries_ = 0;
    while (0 == nSkipped && tableAddr >= bytesPerEntry_ && nPreEntries_ < maxPreEntries_) {
        const Address entryAddr = tableAddr - bytesPerEntry_;
        const auto entryLoc = AddressInterval::baseSize(entryAddr, bytesPerEntry_);
        SAWYER_MESG(debug) <<"  entry at " <<addrToString(entryLoc) <<" (before start of table)\n";
        if (!tableLimits_.contains(entryLoc)) {
            SAWYER_MESG(debug) <<"    entry falls outside table limits\n";
            break;
        }

        // Read bytes for table entry
        std::vector<uint8_t> buffer(bytesPerEntry_, 0);
        const AddressInterval read = map.within(tableLimits_).at(entryAddr).read(buffer);
        if (read.size() != bytesPerEntry_) {
            SAWYER_MESG(debug) <<"    entry data read failed (got " <<plural(read.size(), "bytes") <<")\n";
            break;
        }

        // Parse and insert if possible
        tableLocation_ = AddressInterval::baseSize(entryAddr, (entries_->size() + 1) * bytesPerEntry_);
        if (entries_->maybePrepend(this, entryAddr, buffer)) {
            SAWYER_MESG(debug) <<"    entry parsed and prepended to table\n";
            tableAddr = entryAddr;
            ++nPreEntries_;
        } else {
            SAWYER_MESG(debug) <<"    entry failed to parse (reached beginning of table)\n";
            break;
        }
    }

    tableLocation_ = AddressInterval::baseSize(tableAddr, entries_->size() * bytesPerEntry_);
    if (debug) {
        debug <<"  table at " <<addrToString(tableLocation_) <<" with " <<plural(entries_->size(), "entries") <<"\n";
        debug <<"    entries skipped at " <<addrToString(probableTableAddr) <<": " <<nSkipped <<"\n";
        debug <<"    entries found at or after " <<addrToString(probableTableAddr) <<": " <<(entries_->size() - nPreEntries_) <<"\n";
        debug <<"    entries found before " <<addrToString(probableTableAddr) <<": " <<nPreEntries_ <<"\n";
    }
    return tableLocation_;
}

AddressInterval
MemoryTable::location() const {
    return tableLocation_;
}

size_t
MemoryTable::nPreEntries() const {
    return nPreEntries_;
}

bool
MemoryTable::empty() const {
    return entries_->empty();
}

size_t
MemoryTable::nEntries() const {
    return entries_->size();
}

} // namespace
} // namespace
} // namespace

#endif
