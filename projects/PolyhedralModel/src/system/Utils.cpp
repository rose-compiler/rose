/**
 * \file src/system/Utils.cpp
 * \brief Implementation of some "usefull" system call. (really poor for now XD)
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "system/Utils.hpp"

long getTotalSystemMemory() {
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}

