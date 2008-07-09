#ifndef ROSE_PROCESSSUPPORT_H
#define ROSE_PROCESSSUPPORT_H

#include <vector>
#include <string>
#include <stdio.h>

int systemFromVector(const std::vector<std::string>& argv);
FILE* popenReadFromVector(const std::vector<std::string>& argv);
// Assumes there is only one child process
int pcloseFromVector(FILE* f);

#endif // ROSE_PROCESSSUPPORT_H
