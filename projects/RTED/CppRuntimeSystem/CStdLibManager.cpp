#include "CStdLibManager.h"
#include "CppRuntimeSystem.h"

#include <sstream>
#include <cstring>


// ------------------ Util ----------------------------------------------------

// Check that the first num bytes of ptr1 and ptr2 do not overlap.
void CStdLibManager::
check_overlap(
        const void* ptr1,
        const void* ptr2,
        size_t size,
        const std::string& desc
) {
    return check_overlap( ptr1, size, ptr2, size, desc);
}

// check that the memory allocated at ptr1 and ptr2 doesn't overlap
void
CStdLibManager::check_allocation_overlap(const void* ptr1, const void* ptr2, const std::string&)
{
    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    MemoryType* p1_mem = mm->findContainingMem( memAddr(ptr1) );
    MemoryType* p2_mem = mm->findContainingMem( memAddr(ptr2) );

    // if either source or dest is NULL or missing, we'll catch it in subsequent
    // checks to write/read
    if( p1_mem && p2_mem )
        check_overlap(
            ptr1,
            p1_mem->getSize(),
            ptr2,
            p2_mem->getSize(),
            "in call to memcpy"
        );


}

void
CStdLibManager::check_overlap(const void* ptr1, size_t size1, const void* ptr2, size_t size2, const std::string& desc)
{
    RuntimeSystem* rts = RuntimeSystem::instance();

    if (ptr1 > ptr2)
    {
      std::swap(ptr1, ptr2);
      std::swap(size1, size2);
    }

    Address addr1 = memAddr(ptr1);
    Address addr2 = memAddr(ptr2);

    assert(addr1 < addr2);

    Address range1_high = addr1 + size1;
    Address range2_low = addr2;

    // range1 is the range that starts in the smaller region of memory.  The
    // only way for the high value (the end) of range1 to be greater than the
    // start (low value) of range2 is for the ranges to overlap.
    if(range2_low < range1_high) {
        rts->violationHandler( RuntimeViolation::INVALID_MEM_OVERLAP, desc);
    }
}


// Check that str is allocated, initialized and has a null terminator in
// allocated memory, and that the null terminator is in the same chunk of memory
// that str points to.
size_t CStdLibManager::check_string( const char* str) {

    RuntimeSystem* rts = RuntimeSystem::instance();
    MemoryManager* mm = rts->getMemManager();

    MemoryType* memory = mm->findContainingMem( memAddr(str));

    if( NULL == memory) {
        std::stringstream desc;
        desc    << "Trying to read from non-allocated MemoryRegion (Address "
                << "0x" << memAddr(str) << ")" << std::endl;
        rts->violationHandler( RuntimeViolation::INVALID_READ, desc.str());
        return 0;
    } else {
        // str points somewhere within allocated memory.
        // Now we make sure that there's a null terminator within that memory
        // chunk
        char* end = point_to<char>(memory->getAddress() + memory->getSize());
        for( char* p = (char*) str; p < end; ++p) {
            if( '\0' == *p) {
                // size includes null terminator
                size_t size = (p - str) + 1;
                mm->checkRead( memAddr(str), size);
                return size;
            }
        }

        std::stringstream desc;
        desc    << "Trying to read from std::string at " << memAddr(str)
                << " In memory chunk 0x" << memory->getAddress() << " .. "
                << (memory->getAddress() + memory->getSize())
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
    check_allocation_overlap( destination, source );

    // checkmem   dest .. dest + num
    mm->checkWrite( memAddr(destination), num);

    // checkmem   source .. source + num
    mm->checkRead( memAddr(source), num);
}


void CStdLibManager::
check_memmove( void* destination, const void* source, size_t num) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // checkmem   dest .. dest + num
    mm->checkWrite( memAddr(destination), num);

    // checkmem   source .. source + num
    mm->checkRead( memAddr(source), num);
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
    mm->checkWrite( memAddr(destination), len);
}


void CStdLibManager::
check_strncpy( char* destination, const char* source, size_t num) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // Don't need a \0 in first num chars, but memory must be readable
    mm->checkRead( memAddr(source), num);

    // check no-overlap
    //
    // DJH - doing check_allocation_overlap instead of check_overlap will raise
    // violations for copying substrings where the copied ranges don't overlap,
    // but the allocated ranges do (e.g. copying the first n characters of a
    // std::string to somewhere later in the std::string).
    //
    // the RTED tests imply that such a copy is indeed a violation, but this
    // isn't clear to me.
    check_allocation_overlap( destination, source, "in call to strncpy");

    // check that destination was allocated
    mm->checkWrite( memAddr(destination), num);
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
        // can't overlap with current destination std::string, or the bytes at the
        // end that we're going to write to.  -1 to avoid doublecounting the
        // null terminators.
        destination_len + source_len - 1,
        source,
        source_len,
        "in call to strcat"
    );

    // check that destination was allocated
    mm->checkWrite( memAddr(destination) + destination_len, source_len - 1);
}


void CStdLibManager::
check_strncat( char* destination, const char* source, size_t num) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // Don't need a \0 in first num chars, but memory must be readable
    mm->checkRead( memAddr(source), num);

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
    mm->checkWrite( memAddr(destination) + destination_len, num);
}


void CStdLibManager::
check_strchr( const char* str, int /* character */) {
    // std::strings must have null terminators in allocated memory
    check_string( str);
}


void CStdLibManager::
check_strpbrk( const char* str1, const char* str2) {
    // std::strings must have null terminators in allocated memory
    check_string( str1);
    check_string( str2);
}


void CStdLibManager::
check_strspn( const char* str1, const char* str2) {
    // std::strings must have null terminators in allocated memory
    check_string( str1);
    check_string( str2);
}


void CStdLibManager::
check_strstr( const char* str1, const char* str2) {
    // std::strings must have null terminators in allocated memory
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
