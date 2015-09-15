#ifndef ROSE_EditDistance_Levenshtein_H
#define ROSE_EditDistance_Levenshtein_H

namespace rose {
namespace EditDistance {

// Stack for Levenshtein distance. Used internally
template<typename T>
struct LevenshteinStack {
    typedef std::pair<T/*key*/, size_t/*value*/> KeyVal;
    typedef std::list<KeyVal> KeyValList;
    KeyValList pairs;

    void unique_push_zero(const T& key) {
        for (typename KeyValList::iterator pi=pairs.begin(); pi!=pairs.end(); ++pi) {
            if (pi->first==key)
                return;
        }
        pairs.push_front(KeyVal(key, 0));
    }

    size_t& operator[](const T& key) {
        for (typename KeyValList::iterator pi=pairs.begin(); pi!=pairs.end(); ++pi) {
            if (pi->first==key)
                return pi->second;
        }
        assert(!"not found");
        abort();
    }
};

/** Levenshtein edit distance.
 *
 *  Returns the Levenshtein edit distance of the specified vectors. The vectors may contain any type of element as long as they
 *  are both the same type and the element types define equality ("==" operator). */
template<typename T>
size_t
levenshteinDistance(const std::vector<T> &src, const std::vector<T> &tgt)
{
    // Implementation is cut-n-pasted from above, but removed the line for swaps and associated variables
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
        for (size_t j=1; j<=y; ++j) {
            if (src[i-1]==tgt[j-1]) {
                score[i+1][j+1] = score[i][j];
            } else {
                score[i+1][j+1] = std::min(score[i][j], std::min(score[i+1][j], score[i][j+1])) + 1;
            }
        }
        dict[src[i-1]] = i;
    }

    return score[x+1][y+1];
}


} // namespace
} // namespace

#endif
