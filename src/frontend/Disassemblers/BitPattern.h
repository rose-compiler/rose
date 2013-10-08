#ifndef ROSE_BitPattern_H
#define ROSE_BitPattern_H

#include "integerOps.h"
#include "StringUtility.h"

#include <iostream>
#include <vector>

/** Describes a pattern of bits in a finite number of words.
 *
 *  A BitPattern is a set of one or more alternative values for certain significant bits in a finite sequence of words. The
 *  words have type @p T and are numbered starting at zero.  The bits are numbered independently in each word so that the least
 *  significant bit of a word is bit number zero.
 *
 *  A BitPattern matches a sequence of value words if, for each pattern word that has significant bits, the value of those bits
 *  matches the corresponding bits in the value words.  A BitPattern that has no significant bits matches all sequences of
 *  value words.
 *
 *  A BitPattern may have multiple sets of pattern words that serve as alternative values to match.  A BitPattern will match a
 *  sequence of value words if any of the pattern word sets match.  All pattern word sets use the same significant bits.
 *
 *  Here are examples of creating a BitPattern for a single 32-bit word where the high order byte must be the value 0x33 and
 *  the low-order byte must be the value 0x66.
 *
 * @code
 *  BitPattern<uint32_t> bp1(0xff0000ff, 0x33000066, 0);
 *  BitPattern<uint32_t> bp2 = BitPattern<uint32_t>(0, 8, 0x66, 0) & BitPattern<uint32_t>(24, 32, 0x33, 0);
 * @endcode
 *
 *  The following example uses the same pattern, but treats the input as four separate 8-bit values:
 *
 * @code
 *  BitPattern<uint8_t> bp3 = BitPattern<uint8_t>(0xff, 0x66, 0) & BitPattern<uint32_t>(0xff, 0x33, 3);
 * @endcode
 *
 * Matching these patterns against actual data proceeds like follows. Since the 32-bit word pattern has only one word, we can
 * use the more convenient match_word() method.  On the other hand, we need to supply four words at a time for the 8-bit word
 * pattern.
 *
 * @code
 *  uint32_t w32 = ...;
 *  bool m1 = bp1.matches_word(w32);
 *  std::vector<uint8_t> w8a = ...;
 *  bool m2 = bp3.matches(w8a);
 *  uint8_t w8b[4] = {...};
 *  bool m3 = bp3.matches(w8b, 4);
 * @endcode
 */
template<typename T>
class BitPattern {
    typedef std::vector<T> Words;
    typedef std::vector<Words> Alternatives;
    Words mask;                                 // significant bits
    Alternatives patterns;                      // set of alternative patterns; each pattern has the same size as the mask

public:
    /** Creates a new, empty bit pattern. An empty pattern matches all values. */
    BitPattern() {}

    /** Creates a new bit pattern for a single word using a mask and value. The pattern is bit-wise AND'd with the mask, and
     *  none of its other bits should be set. */
    BitPattern(T msk, T pat, size_t wordnum) {
        insert(msk, pat, wordnum);
    }

    /** Create a new bit pattern for a single word using bit offsets.  The pattern is left shifted into the specified bits, and
     *  none of its higher order bits should be set. The range of significant bits is from @p lo_bit through @p hi_bit,
     *  inclusive. */
    BitPattern(size_t lo_bit, size_t hi_bit, T pat, size_t wordnum) {
        bits(lo_bit, hi_bit, pat, wordnum);
    }

    /** Verify internal consistency. This is asserted at the beginning and end of methods that modify a bit pattern. */
    bool is_consistent() const {
        for (size_t i=0; i<patterns.size(); ++i) {
            if (patterns[i].size()!=mask.size()) {
                std::cerr <<"BitPattern::is_consistent failed\n"
                          <<"    mask.size() = " <<mask.size() <<"\n"
                          <<"    patterns[" <<i <<"].size() = " <<patterns[i].size() <<"\n"
                          <<"    these two vectors should have been the same size\n";
                std::cerr <<"    this = " <<*this <<"\n"; // printing might fail
                return false;
            }
        }
        return true;
    }

    /** Returns the number of significant bits. Returns zero if the pattern is empty. */
    size_t nsignificant() const {
        size_t retval = 0;
        for (size_t i=0; i<mask.size(); ++i)
            retval += IntegerOps::countSet(mask[i]);
        return retval;
    }

    /** Returns the number of words in the pattern.  Although the final word of the pattern will have at least one significant
     *  bit, the previous pattern words might not be significant. */
    size_t nwords() const {
        return mask.size();
    }

    /** Returns the size of the pattern in bits.  If the pattern has no significant bits, then zero is returned. Otherwise the
     * return value is the one-origin position of the most significant bit of the pattern mask. */
    size_t width() const {
        if (0==mask.size())
            return 0;
        assert(mask.back()!=0);
        return 8*sizeof(T)*(mask.size()-1) + IntegerOps::msb_set(mask.back()).get() + 1;
    }
    
    /** Returns the number of alternatives. Returns zero if the pattern is empty. */
    size_t nalternatives() const {
        return patterns.size();
    }

    /** Determines if the specified bits conflict with a particular pattern alternative.  If the new pattern (the arguments)
     *  conflicts with an existing pattern, then a mask describing the bits in conflict is returned.  This function returns zero
     *  if there is no conflict with the specified alternative. */
    T conflict(T msk, T pat, size_t wordnum, size_t altnum) const {
        if (wordnum < mask.size()) {
            if (T overlap = mask[wordnum] & msk) {
                assert(altnum < patterns.size());
                if (T differ = (patterns[altnum][wordnum] & overlap) ^ (pat & overlap))
                    return differ;
            }
        }
        return 0;
    }
    
    /** Determines if the specified significant bits conflict with any of the existing pattern alternatives.  If the new
     *  pattern (the arguments) conflicts with an existing pattern, then a mask describing the bits in conflict is returned.
     *  If there is no conflict, then this function returns zero. */
    T conflict(T msk, T pat, size_t wordnum) const {
        if (wordnum < mask.size()) {
            for (size_t altnum=0; altnum<nalternatives(); ++altnum) {
                if (T differ = conflict(msk, pat, wordnum, altnum))
                    return differ;
            }
        }
        return 0;
    }

    /** Check that a pattern insertion does not conflict with an existing pattern.  If a conflict is present, print
     *  information about the conflict to stderr and abort. */
    void check_insertion(T msk, T pat, size_t wordnum) const {
        if (0!=msk && wordnum<mask.size()) {
            if (T differ = conflict(msk, pat, wordnum)) {
                std::cerr <<"BitPattern::insert(msk=" <<StringUtility::addrToString(msk, 8*sizeof(T))
                          <<", pat=" <<StringUtility::addrToString(pat, 8*sizeof(T))
                          <<", wordnum=" <<wordnum <<") conflicts with existing pattern " <<*this
                          <<" at bits " <<StringUtility::addrToString(differ, 8*sizeof(T)) <<"\n";
                assert(!"new bit pattern conflicts with existing pattern");
                abort();
            }
        }
    }

    /** Determines whether two patterns can match the same input.  Returns true if this pattern can match at least one of the
     *  same input values as the @p other BitPattern. */
    bool any_same(const BitPattern &other, std::pair<size_t, size_t> *alternatives=NULL) const {
        if (nwords()!=other.nwords())
            return false;
        for (size_t wordnum=0; wordnum<nwords(); ++wordnum) {
            if (mask[wordnum]!=other.mask[wordnum])
                return false;
        }
        for (size_t a1=0; a1<nalternatives(); ++a1) {
            for (size_t a2=0; a2<other.nalternatives(); ++a2) {
                bool are_same = true;
                for (size_t wordnum=0; are_same && wordnum<nwords(); ++wordnum)
                    are_same = patterns[a1][wordnum] == other.patterns[a2][wordnum];
                if (are_same) {
                    if (alternatives!=NULL)
                        *alternatives = std::make_pair(a1, a2);
                    return true;
                }
            }
        }
        return false;
    }
    
    /** Creates a new pattern by adding significant bits to all alternatives of a pattern. The @p msk and @p pat are a mask and
     *  bits for the specified word. */
    BitPattern& insert(T msk, T pat, size_t wordnum=0) {
        assert(is_consistent());
        assert(0 == (pat & ~msk));
        check_insertion(msk, pat, wordnum);
        if (msk != 0) {
            if (wordnum >= mask.size())
                mask.resize(wordnum+1, T(0));
            mask[wordnum] |= msk;
            if (patterns.empty()) {
                patterns.resize(1);
                patterns[0].resize(wordnum+1, T(0));
                patterns[0][wordnum] = pat;
            } else {
                for (size_t altnum=0; altnum<nalternatives(); ++altnum) {
                    if (wordnum >= patterns[altnum].size())
                        patterns[altnum].resize(wordnum+1, T(0));
                    patterns[altnum][wordnum] |= pat;
                }
            }
        }
        assert(is_consistent());
        return *this;
    }

    /** Adds significant bits to all alternatives of a pattern.  The pattern is over the inclusive range of bits for the
     *  specified word.  E.g., when called as "bits(8,11,0xc,1)", the pattern will match where bits 8 and 9 of word 1 are clear
     *  and bits 10 and 11 of word 1 are set.*/
    BitPattern& bits(size_t lo_bit, size_t hi_bit, T value, size_t wordnum) {
        T msk = IntegerOps::genMask<T>(lo_bit, hi_bit);
        value = IntegerOps::shiftLeft2(value, lo_bit);
        return insert(msk, value, wordnum);
    }

    /** Return a new BitPattern with the pattern bits shifted left by the indicated amount. */
    BitPattern shift_left(size_t nbits) const {
#if 0 /*DEBUGGING [Robb P. Matzke 2013-10-02]*/
        std::cerr <<"BitPattern::shift_left: nbits=" <<nbits <<"; this=" <<*this <<"\n";
#endif
        assert(is_consistent());
        if (0==nbits || patterns.empty())
            return *this;
        static const size_t word_size = 8*sizeof(T);
        size_t need_nbits = width() + nbits;
        size_t need_nwords = (need_nbits + word_size - 1) / word_size;
        size_t word_delta = nbits / word_size;
        size_t bit_delta_lt = nbits % word_size;
        size_t bit_delta_rt = word_size - bit_delta_lt;
        BitPattern retval;

        // shift the mask
        retval.mask.resize(need_nwords, 0);
        for (size_t i=0; i<mask.size(); ++i) {
            retval.mask[i+word_delta] |= IntegerOps::shiftLeft2(mask[i], bit_delta_lt);
            if (i+word_delta+1<need_nwords)
                retval.mask[i+word_delta+1] = IntegerOps::shiftRightLogical2(mask[i], bit_delta_rt);
        }

        // shift each pattern
        retval.patterns.resize(patterns.size());
        for (size_t i=0; i<patterns.size(); ++i) {
            retval.patterns[i].resize(need_nwords, 0);
            for (size_t j=0; j<patterns[i].size(); ++j) {
                retval.patterns[i][j+word_delta] |= IntegerOps::shiftLeft2(patterns[i][j], bit_delta_lt);
                if (j+word_delta+1<need_nwords)
                    retval.patterns[i][j+word_delta+1] = IntegerOps::shiftRightLogical2(patterns[i][j], bit_delta_rt);
            }
        }
        assert(is_consistent());
#if 0 /*DEBUGGING [Robb P. Matzke 2013-10-02]*/
        std::cerr <<"  shift_left result=" <<retval <<"\n";
#endif
        return retval;
    }
    
    /** Combines this BitPattern with another by forming their conjunction. After this operation, this BitPattern will match
     *  values where both original patterns matched the value.  The two original bit patterns need not have the same mask; in
     *  fact, it's most useful when their masks don't overlap. Either (or both) original patterns can have multiple
     *  alternatives; the number of alternatives in the result will be on the order of the product of the number of
     *  alternatives in the originals. */
    BitPattern& conjunction(const BitPattern &other) {
#if 0 /*DEBUGGING [Robb P. Matzke 2013-10-02]*/
        std::cerr <<"BitPattern::conjunction:\n"
                  <<"  this  = " <<*this <<"\n"
                  <<"  other = " <<other <<"\n";
#endif
        assert(is_consistent());
        assert(other.is_consistent());
        // check that the operation is possible
        for (size_t altnum=0; altnum<other.nalternatives(); ++altnum) {
            for (size_t wordnum=0; wordnum<other.nwords(); ++wordnum)
                check_insertion(other.mask[wordnum], other.patterns[altnum][wordnum], wordnum);
        }

        // merge masks
        size_t this_nwords = mask.size();
        size_t result_nwords = std::max(this_nwords, other.mask.size());
        mask.resize(result_nwords, T(0));
        for (size_t wordnum=0; wordnum<other.mask.size(); ++wordnum)
            mask[wordnum] |= other.mask[wordnum];

        // do the conjunction
        Alternatives retval;
        for (size_t a1=0; a1<patterns.size(); ++a1) {
            for (size_t a2=0; a2<other.patterns.size(); ++a2) {
                retval.push_back(Words(result_nwords, T(0)));
                for (size_t i=0; i<this_nwords; ++i)
                    retval.back()[i] = patterns[a1][i];
                for (size_t i=0; i<other.nwords(); ++i)
                    retval.back()[i] |= other.patterns[a2][i];

                // check for and remove duplicate pattern
                for (size_t a3=0; a3+1<retval.size(); ++a3) {
                    bool isdup = true;
                    for (size_t i=0; isdup && i<result_nwords; ++i)
                        isdup = retval[a3][i] == retval.back()[i];
                    if (isdup) {
                        retval.pop_back();
                        break;
                    }
                }
            }
        }
        patterns = retval;
        assert(is_consistent());
#if 0 /*DEBUGGING [Robb P. Matzke 2013-10-02]*/
        std::cerr <<"  conjunction result = " <<*this <<"\n";
#endif
        return *this;
    }

    /** Combines this BitPattern with another by forming their disjunction.  After the operation, this BitPattern will match
     *  values where either of the original patterns matched.  The two original bit patterns (and thus the result) must have the
     *  same set of significant bits. */
    BitPattern& disjunction(const BitPattern &other) {
        assert(is_consistent());
        assert(other.is_consistent());
        if (0==nalternatives()) {
            *this = other;
        } else if (0==other.nalternatives()) {
            // void
        } else {
            assert(nwords()==other.nwords());
            for (size_t wordnum=0; wordnum<nwords(); ++wordnum)
                assert(mask[wordnum]==other.mask[wordnum]);
            size_t na = nalternatives();
            for (size_t a1=0; a1<other.nalternatives(); ++a1) {
                bool isdup = false;
                for (size_t a2=0; !isdup && a2<na; ++a2) {
                    isdup = true;
                    for (size_t wordnum=0; isdup && wordnum<nwords(); ++wordnum)
                        isdup = patterns[a2][wordnum] == other.patterns[a1][wordnum];
                }
                if (!isdup)
                    patterns.push_back(other.patterns[a1]);
            }
        }
        assert(is_consistent());
        return *this;
    }

    /** Creates a new BitPattern that is the conjunction of two bit patterns. The new pattern matches wherever both of the
     *  original patterns matched.
     * @{ */
    BitPattern operator&(const BitPattern &other) const {
        BitPattern retval = *this;
        return retval.conjunction(other);
    }
    BitPattern& operator&=(const BitPattern &other) {
        return conjunction(other);
    }
    /** @} */

    /** Creates a new BitPattern that is the inclusive disjunction of two bit patterns.  The new pattern matches wherever
     *  either of the original patterns matched.
     * @{ */
    BitPattern operator|(const BitPattern &other) const {
        BitPattern retval = *this;
        return retval.disjunction(other);
    }
    BitPattern& operator|=(const BitPattern &other) {
        return disjunction(other);
    }
    /** @} */

    /** Returns true if this pattern matches the specified values.
     * @{ */
    bool matches(const std::vector<T> value_words) const {
        if (0==nalternatives())
            return true;
        if (value_words.size() < nwords())
            return false;
        for (size_t altnum=0; altnum<nalternatives(); ++altnum) {
            bool eq = true;
            for (size_t wordnum=0; eq && wordnum<nwords(); ++wordnum)
                eq = (value_words[wordnum] & mask[wordnum]) == (patterns[altnum][wordnum] & mask[wordnum]);
            if (eq)
                return true;
        }
        return false;
    }
    bool matches(const T *value_words, size_t sz) const {
        std::vector<T> vv;
        for (size_t i=0; i<sz; ++i)
            vv.push_back(value_words[i]);
        return matches(vv);
    }
    /** @} */

    /** Returns true if one word of this pattern matches the specified value. */
    bool matches_word(T value, size_t wordnum) const {
        if (0==nalternatives() || wordnum>=nwords())
            return true;
        for (size_t altnum=0; altnum<nalternatives(); ++altnum) {
            if ((value & mask[wordnum]) == (patterns[altnum][wordnum] & mask[wordnum]))
                return true;
        }
        return false;
    }

    /** Print one pattern alternative. */
    void print(std::ostream &o, size_t altnum, bool with_mask=true) const {
        assert(altnum<nalternatives());
        o <<(1==nwords()?"":"{");
        for (size_t wordnum=0; wordnum<nwords(); ++wordnum) {
            o <<(0==wordnum?"":",") <<StringUtility::addrToString(patterns[altnum][wordnum], 8*sizeof(T));
            if (with_mask)
                o <<"/" <<StringUtility::addrToString(mask[wordnum], 8*sizeof(T));
        }
        o <<(1==nwords()?"":"}");
    }

    /** Print all alternatives of the pattern. */
    void print(std::ostream &o) const {
        if (0==nwords()) {
            o <<"empty";
        } else {
            o <<(1==nalternatives()?"":"(");
            for (size_t altnum=0; altnum<nalternatives(); ++altnum) {
                o <<(0==altnum?"":" | ");
                print(o, altnum, false);
            }
            o <<(1==nalternatives()?"":")");
            o <<"/" <<(1==nwords()?"":"{");
            for (size_t wordnum=0; wordnum<nwords(); ++wordnum)
                o <<(wordnum>0?",":"") <<StringUtility::addrToString(mask[wordnum], 8*sizeof(T));
            o <<(1==nwords()?"":"}");
        }
    }
};

template<typename T>
std::ostream& operator<<(std::ostream &o, const BitPattern<T> &bp) 
{
    bp.print(o);
    return o;
}

#endif
