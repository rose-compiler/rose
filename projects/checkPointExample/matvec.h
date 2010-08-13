#ifndef MATVEC_H
#define MATVEC_H

#include <vector>

typedef void (*matvec_ptr)(std::vector<double>& y, 
                           const std::vector<double>& A,
			   const std::vector<double>& x);

extern "C"
void matvec(std::vector<double>& y, const std::vector<double>& A, 
            const std::vector<double>& x);

#endif // MATVEC_H
