#include "CStdLibManager.h"
#include "CppRuntimeSystem.h"

#include <sstream>
#include <cstring>


using namespace std;


// ------------------ Util ----------------------------------------------------

// Check that the first num bytes of ptr1 and ptr2 do not overlap.
void CStdLibManager::
check_overlap(
        const void* ptr1, 
        const void* ptr2, 
        size_t size, 
        const string& desc
) {
    return check_overlap( ptr1, size, ptr2, size, desc);
}

void CStdLibManager::
check_overlap(
        const void* ptr1, 
        size_t size1,
        const void* ptr2, 
        size_t size2, 
        const string& desc
) {

    RuntimeSystem* rts = RuntimeSystem::instance();

    addr_type   addr1 = (addr_type) ptr1,
                addr2 = (addr_type) ptr2; 

    addr_type range1_high, range2_low;
    if( addr2 > addr1) {
        range1_high = addr1 + size1;
        range2_low = addr2;
    } else {
        range1_high = addr2 + size2;
        range2_low = addr1;
    }

    // range1 is the range that starts in the smaller region of memory.  The
    // only way for the high value (the end) of range1 to be greater than the
    // start (low value) of range2 is for the ranges to overlap.
    if( range1_high > range2_low) {
        rts->violationHandler( RuntimeViolation::INVALID_MEM_OVERLAP, desc);
    }
}


// Check that str is allocated, initialized and has a null terminator in
// allocated memory, and that the null terminator is in the same chunk of memory
// that str points to.
size_t CStdLibManager::check_string( const char* str) {

    RuntimeSystem* rts = RuntimeSystem::instance();
    MemoryManager* mm = rts->getMemManager();

    MemoryType* memory = mm->findContainingMem( (addr_type) str);

    if( NULL == memory) {
        stringstream desc;
        desc    << "Trying to read from non-allocated MemoryRegion (Address " 
                << "0x" << hex << (addr_type) str << ")" << endl;
        rts->violationHandler( RuntimeViolation::INVALID_READ, desc.str());
        return 0;
    } else {
        // str points somewhere within allocated memory.
        // Now we make sure that there's a null terminator within that memory
        // chunk
        char* end = (char*) (memory->getAddress() + memory->getSize());
        for( char* p = (char*) str; p < end; ++p) {
            if( '\0' == *p) {
                // size includes null terminator
                size_t size = (p - str) + 1;
                mm->checkRead( (addr_type) str, size);
                return size;
            }
        }

        stringstream desc;
        desc    << "Trying to read from string at " << (addr_type) str
                << " In memory chunk 0x" << hex << memory->getAddress() << " .. "
                << hex << memory->getAddress() + memory->getSize()
                << " But there is no null terminator from the pointer to the"
                << " end of the chunk.";
        rts->violationHandler( RuntimeViolation::INVALID_READ, desc.str());
        return 0;
    }
}



// ------------------ CStdLib -------------------------------------------------

void CStdLibManager::
check_memcpy( void* destination, const void* source, size_t num) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // check no-overlap
    check_overlap( destination, source, num, "in call to memcpy");

    // checkmem   dest .. dest + num
    mm->checkWrite( (addr_type) destination, num);

    // checkmem   source .. source + num
    mm->checkRead( (addr_type) source, num);
}


void CStdLibManager::
check_memmove( void* destination, const void* source, size_t num) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // checkmem   dest .. dest + num
    mm->checkWrite( (addr_type) destination, num);

    // checkmem   source .. source + num
    mm->checkRead( (addr_type) source, num);
}


void CStdLibManager::
check_strcpy( char* destination, const char* source) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // source must have null terminators in allocated memory
    size_t len = check_string( source);
    if( !len ) return;

    // check no-overlap
    check_overlap( destination, source, len, "in call to strcpy");

    // check that destination was allocated
    mm->checkWrite( (addr_type) destination, len);
}


void CStdLibManager::
check_strncpy( char* destination, const char* source, size_t num) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // Don't need a \0 in first num chars, but memory must be readable
    mm->checkRead( (addr_type) source, num);

    // check no-overlap
    check_overlap( destination, source, num, "in call to strncpy");

    // check that destination was allocated
    mm->checkWrite( (addr_type) destination, num);
}


void CStdLibManager::
check_strcat( char* destination, const char* source) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // source must have null terminators in allocated memory
    size_t source_len = check_string( source);
    if( !source_len ) return;

    // dest must also have null terminator in allocated memory
    size_t destination_len = check_string( destination );
    if( !destination_len ) return;

    // check no-overlap
    check_overlap(
        destination,
        // can't overlap with current destination string, or the bytes at the
        // end that we're going to write to.  -1 to avoid doublecounting the
        // null terminators. 
        destination_len + source_len - 1,
        source, 
        source_len,
        "in call to strcat"
    );

    // check that destination was allocated
    mm->checkWrite( (addr_type) destination + destination_len, source_len - 1);
}


void CStdLibManager::
check_strncat( char* destination, const char* source, size_t num) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // Don't need a \0 in first num chars, but memory must be readable
    mm->checkRead( (addr_type) source, num);

    // dest, however, must have null terminator in allocated memory
    size_t destination_len = check_string( destination );
    if( !destination_len ) return;

    // check no-overlap
    check_overlap(
        destination,
        destination_len + num,
        source, 
        num, 
        "in call to strncat"
    );

    // check that destination was allocated
    mm->checkWrite( (addr_type) destination + destination_len, num);
}


void CStdLibManager::
check_strchr( const char* str, int character) {
    // strings must have null terminators in allocated memory
    check_string( str);
}


void CStdLibManager::
check_strpbrk( const char* str1, const char* str2) {
    // strings must have null terminators in allocated memory
    check_string( str1);
    check_string( str2);
}


void CStdLibManager::
check_strspn( const char* str1, const char* str2) {
    // strings must have null terminators in allocated memory
    check_string( str1);
    check_string( str2);
}


void CStdLibManager::
check_strstr( const char* str1, const char* str2) {
    // strings must have null terminators in allocated memory
    check_string( str1);
    check_string( str2);
}


void CStdLibManager::
check_strlen( const char* str) {
    // strings must have null terminators in allocated memory
    check_string( str);
}

// ----------------------------------------------------------------------------






// vim:sw=4 ts=4 tw=80 et sta:
