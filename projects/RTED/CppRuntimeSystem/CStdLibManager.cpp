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
        size_t num, 
        const string& desc
) {

    RuntimeSystem* rts = RuntimeSystem::instance();

    addr_type   addr1 = (addr_type) ptr1,
                addr2 = (addr_type) ptr2; 
    if( max(addr1, addr2) - min(addr1, addr2) < num) {
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
                << (addr_type) str << ")" << endl;
        rts->violationHandler( RuntimeViolation::INVALID_READ, desc.str());
        return -1;
    } else {
        // str points somewhere within allocated memory.
        // Now we make sure that there's a null terminator within that memory
        // chunk
        char* end = (char*) (memory->getAddress() + memory->getSize());
        for( char* p = (char*) str; p < end; ++p) {
            if( '\0' == *p) {
                mm->checkRead( (addr_type) str, (size_t) (p - str));
                return (size_t) (p - str);
            }
        }

        stringstream desc;
        desc    << "Trying to read from string at " << (addr_type) str
                << " In memory chunk " << memory->getAddress() << " .. "
                << memory->getAddress() + memory->getSize()
                << " But there is no null terminator from the pointer to the"
                << " end of the chunk.";
        rts->violationHandler( RuntimeViolation::INVALID_READ, desc.str());
        return -1;
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

    // check no-overlap
    check_overlap( destination, source, len, "in call to strcpy");

    if( len != -1)
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
    size_t len = check_string( source);

    // check no-overlap
    check_overlap( destination, source, len, "in call to strcat");

    if( len != -1)
        // check that destination was allocated
        mm->checkWrite( (addr_type) destination, len);
}


void CStdLibManager::
check_strncat( char* destination, const char* source, size_t num) {

    MemoryManager* mm = RuntimeSystem::instance()->getMemManager();

    // Don't need a \0 in first num chars, but memory must be readable
    mm->checkRead( (addr_type) source, num);

    // check no-overlap
    check_overlap( destination, source, num, "in call to strncat");

    // check that destination was allocated
    mm->checkWrite( (addr_type) destination, num);
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
