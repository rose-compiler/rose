#ifndef ROSE_EditDistance_DamerauLevenshtein_H
#define ROSE_EditDistance_DamerauLevenshtein_H

#include <EditDistance/Levenshtein.h>

namespace rose {
namespace EditDistance {

/** Damerau-Levenshtein edit distance.
 *
 *  Returns the true Damerau-Levenshtein edit distance of vectors with adjacent transpositions.  The arguments may be vectors
 *  of any type that defines an equality operation. */
template<typename T>
size_t
damerauLevenshteinDistance(const std::vector<T> &src, const std::vector<T> &tgt)
{
    // Based on the C# implementation on the wikipedia page
    if (src.empty() || tgt.empty())
        return std::max(src.size(), tgt.size());

    const size_t x = src.size();
    const size_t y = tgt.size();
    std::vector<std::vector<size_t> > score(x+2, std::vector<size_t>(y+2, 0));
    size_t score_ceil = x + y;
    score[0][0] = score_ceil;
    for (size_t i=0; i<=x; ++i) {
        score[i+1][1] = i;
        score[i+1][0] = score_ceil;
    }
    for (size_t j=0; j<=y; ++j) {
        score[1][j+1] = j;
        score[0][j+1] = score_ceil;
    }
    
    LevenshteinStack<T> dict;
    for (size_t i=0; i<x; ++i)
        dict.unique_push_zero(src[i]);
    for (size_t j=0; j<y; ++j)
        dict.unique_push_zero(tgt[j]);

    for (size_t i=1; i<=x; ++i) {
        size_t db = 0;
        for (size_t j=1; j<=y; ++j) {
            size_t i1 = dict[tgt[j-1]];
            size_t j1 = db;
            if (src[i-1]==tgt[j-1]) {
                score[i+1][j+1] = score[i][j];
                db = j;
            } else {
                score[i+1][j+1] = std::min(score[i][j], std::min(score[i+1][j], score[i][j+1])) + 1;
            }
            // swaps
            score[i+1][j+1] = std::min(score[i+1][j+1], score[i1][j1] + (i-i1-1) + 1 + (j-j1-1));
        }
        dict[src[i-1]] = i;
    }

    return score[x+1][y+1];
}

} // namespace
} // namespace
#endif
