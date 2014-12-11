// Test code for string utility library
#include <rose_config.h>

#include "string_functions.h"
#include "Combinatorics.h"
#include <EditDistance/Levenshtein.h>
#include <EditDistance/DamerauLevenshtein.h>

#include <iostream>

using namespace rose;

static bool
test_isLineTerminated()
{
    using namespace StringUtility;

    bool batch1 = (true == isLineTerminated("hello world\n") &&
                   true == isLineTerminated("hello world\r") &&
                   true == isLineTerminated("hello world\r\n") &&
                   true == isLineTerminated("hello world\n\r"));
    assert(batch1);

    bool batch2 = (true == isLineTerminated("\n") &&
                   true == isLineTerminated("\r") &&
                   true == isLineTerminated("\r\n") &&
                   true == isLineTerminated("\n\r"));
    assert(batch2);

    bool batch3 = (false== isLineTerminated("\nhello world") &&
                   false== isLineTerminated("\rhello world") &&
                   false== isLineTerminated("\r\nhello world") &&
                   false== isLineTerminated("\n\rhello world"));
    assert(batch3);

    bool batch4 = (false== isLineTerminated("hello\nworld") &&
                   false== isLineTerminated("hello\rworld") &&
                   false== isLineTerminated("hello\r\nworld") &&
                   false== isLineTerminated("hello\n\rworld"));
    assert(batch4);

    bool batch5 = (false== isLineTerminated("") &&
                   false== isLineTerminated(std::string("")+'\0') &&
                   false== isLineTerminated(std::string("\n")+'\0') &&
                   false== isLineTerminated(std::string("\r")+'\0') &&
                   false== isLineTerminated(std::string("\r\n")+'\0') &&
                   false== isLineTerminated(std::string("\n\r")+'\0'));
    assert(batch5);

    bool batch6 = (true == isLineTerminated('\0'+std::string("\n")) &&
                   true == isLineTerminated('\0'+std::string("\r")) &&
                   true == isLineTerminated('\0'+std::string("\r\n")) &&
                   true == isLineTerminated('\0'+std::string("\n\r")));
    assert(batch6);

    return batch1 && batch2 && batch3 && batch4 && batch5 && batch6;
}

static bool
test_prefixLines()
{
    using namespace StringUtility;
    bool retval = false;

    // Tests using an empty string as input. Empty lines are never modified.
    bool test00 = 0 == (prefixLines("", "L", false, false)
                        .compare(""));
    assert(test00);
    bool test01 = 0 == (prefixLines("", "L", true, false)
                        .compare(""));
    assert(test01);
    bool test02 = 0 == (prefixLines("", "L", false, true)
                        .compare(""));
    assert(test02);
    bool test03 = 0 == (prefixLines("", "L", true, true)
                        .compare(""));
    assert(test03);
    retval = retval && test00 && test01 && test02 && test03;


    // Tests without internal line termination.
    bool test10 = 0 == (prefixLines("aaa", "L", false, false)
                        .compare("aaa"));
    assert(test10);
    bool test11 = 0 == (prefixLines("aaa", "L", true, false)
                        .compare("Laaa"));
    assert(test11);
    bool test12 = 0 == (prefixLines("aaa", "L", false, true)
                        .compare("aaa"));
    assert(test12);
    bool test13 = 0 == (prefixLines("aaa", "L", true, true)
                        .compare("Laaa"));
    assert(test13);
    retval = retval && test10 && test11 && test12 && test13;


    // Tests with trailing line termination.
    bool test20 = 0 == (prefixLines("aaa\n", "L", false, false)                 // LF (unix)
                        .compare("aaa\n"));
    assert(test20);
    bool test21 = 0 == (prefixLines("aaa\n", "L", true, false)
                        .compare("Laaa\n"));
    assert(test21);
    bool test22 = 0 == (prefixLines("aaa\n", "L", false, true)
                        .compare("aaa\nL"));
    assert(test22);
    bool test23 = 0 == (prefixLines("aaa\n", "L", true, true)
                        .compare("Laaa\nL"));
    assert(test23);
    retval = retval && test20 && test21 && test22 && test23;

    bool test24 = 0 == (prefixLines("aaa\r", "L", false, false)                 // CR only
                        .compare("aaa\r"));
    assert(test24);
    bool test25 = 0 == (prefixLines("aaa\r", "L", true, false)
                        .compare("Laaa\r"));
    assert(test25);
    bool test26 = 0 == (prefixLines("aaa\r", "L", false, true)
                        .compare("aaa\rL"));
    assert(test26);
    bool test27 = 0 == (prefixLines("aaa\r", "L", true, true)
                        .compare("Laaa\rL"));
    assert(test27);

    bool test28 = 0 == (prefixLines("aaa\r\n", "L", false, false)               // CR-LF (DOS/Windows)
                        .compare("aaa\r\n"));
    assert(test28);
    bool test29 = 0 == (prefixLines("aaa\r\n", "L", true, false)
                        .compare("Laaa\r\n"));
    assert(test29);
    bool test2a = 0 == (prefixLines("aaa\r\n", "L", false, true)
                        .compare("aaa\r\nL"));
    assert(test2a);
    bool test2b = 0 == (prefixLines("aaa\r\n", "L", true, true)
                        .compare("Laaa\r\nL"));
    assert(test2b);
    retval = retval &&
             test20 && test21 && test22 && test23 &&
             test24 && test25 && test26 && test27 &&
             test28 && test29 && test2a && test2b;


    // Tests with internal line termination.
    bool test30 = 0 == (prefixLines("aaa\nbbb", "L", false, false)              // LF (unix)
                        .compare("aaa\nLbbb"));
    assert(test30);
    bool test31 = 0 == (prefixLines("aaa\nbbb", "L", true, false)
                        .compare("Laaa\nLbbb"));
    assert(test31);
    bool test32 = 0 == (prefixLines("aaa\nbbb", "L", false, true)
                        .compare("aaa\nLbbb"));
    assert(test32);
    bool test33 = 0 == (prefixLines("aaa\nbbb", "L", true, true)
                        .compare("Laaa\nLbbb"));
    assert(test33);

    bool test34 = 0 == (prefixLines("aaa\rbbb", "L", false, false)              // CR only
                        .compare("aaa\rLbbb"));
    assert(test34);
    bool test35 = 0 == (prefixLines("aaa\rbbb", "L", true, false)
                        .compare("Laaa\rLbbb"));
    assert(test35);
    bool test36 = 0 == (prefixLines("aaa\rbbb", "L", false, true)
                        .compare("aaa\rLbbb"));
    assert(test36);
    bool test37 = 0 == (prefixLines("aaa\rbbb", "L", true, true)
                        .compare("Laaa\rLbbb"));
    assert(test37);

    bool test38 = 0 == (prefixLines("aaa\r\nbbb", "L", false, false)            // CR-LF (DOS/Windows)
                        .compare("aaa\r\nLbbb"));
    assert(test38);
    bool test39 = 0 == (prefixLines("aaa\r\nbbb", "L", true, false)
                        .compare("Laaa\r\nLbbb"));
    assert(test39);
    bool test3a = 0 == (prefixLines("aaa\r\nbbb", "L", false, true)
                        .compare("aaa\r\nLbbb"));
    assert(test3a);
    bool test3b = 0 == (prefixLines("aaa\r\nbbb", "L", true, true)
                        .compare("Laaa\r\nLbbb"));
    assert(test3b);
    retval = retval &&
             test30 && test31 && test32 && test33 &&
             test34 && test35 && test36 && test37 &&
             test38 && test39 && test3a && test3b;


    // Tests for multiple lines
    bool test40 = 0 == (prefixLines("aaa\nbbb\nccc\n", "L", false, false)       // LF (unix)
                        .compare("aaa\nLbbb\nLccc\n"));
    assert(test40);
    bool test41 = 0 == (prefixLines("aaa\nbbb\nccc\n", "L", true, false)
                        .compare("Laaa\nLbbb\nLccc\n"));
    assert(test41);
    bool test42 = 0 == (prefixLines("aaa\nbbb\nccc\n", "L", false, true)
                        .compare("aaa\nLbbb\nLccc\nL"));
    assert(test42);
    bool test43 = 0 == (prefixLines("aaa\nbbb\nccc\n", "L", true, true)
                        .compare("Laaa\nLbbb\nLccc\nL"));
    assert(test43);

    bool test44 = 0 == (prefixLines("aaa\rbbb\rccc\r", "L", false, false)       // CR only
                        .compare("aaa\rLbbb\rLccc\r"));
    assert(test44);
    bool test45 = 0 == (prefixLines("aaa\rbbb\rccc\r", "L", true, false)
                        .compare("Laaa\rLbbb\rLccc\r"));
    assert(test45);
    bool test46 = 0 == (prefixLines("aaa\rbbb\rccc\r", "L", false, true)
                        .compare("aaa\rLbbb\rLccc\rL"));
    assert(test46);
    bool test47 = 0 == (prefixLines("aaa\rbbb\rccc\r", "L", true, true)
                        .compare("Laaa\rLbbb\rLccc\rL"));
    assert(test47);

    bool test48 = 0 == (prefixLines("aaa\r\nbbb\r\nccc\r\n", "L", false, false) // CR-LF (DOS/Windows)
                        .compare("aaa\r\nLbbb\r\nLccc\r\n"));
    assert(test48);
    bool test49 = 0 == (prefixLines("aaa\r\nbbb\r\nccc\r\n", "L", true, false)
                        .compare("Laaa\r\nLbbb\r\nLccc\r\n"));
    assert(test49);
    bool test4a = 0 == (prefixLines("aaa\r\nbbb\r\nccc\r\n", "L", false, true)
                        .compare("aaa\r\nLbbb\r\nLccc\r\nL"));
    assert(test4a);
    bool test4b = 0 == (prefixLines("aaa\r\nbbb\r\nccc\r\n", "L", true, true)
                        .compare("Laaa\r\nLbbb\r\nLccc\r\nL"));
    assert(test4b);
    retval = retval &&
             test40 && test41 && test42 && test43 &&
             test44 && test45 && test46 && test47 &&
             test48 && test49 && test4a && test4b;


    // Tests for default arguments
    bool test50 = 0 == (prefixLines("aaa\n", "L")
                        .compare("Laaa\n"));
    assert(test50);
    bool test51 = 0 == (prefixLines("aaa\nbbb\nccc\n", "L")
                        .compare("Laaa\nLbbb\nLccc\n"));
    assert(test51);
    retval = retval && test50 && test51;

    // Test longer prefixes
    bool test60 = 0 == (prefixLines("line1\nline2\nline3\n", "___", false, false)
                        .compare("line1\n___line2\n___line3\n"));
    assert(test60);
    bool test61 = 0 == (prefixLines("line1\nline2\nline3\n", "___", false, true)
                        .compare("line1\n___line2\n___line3\n___"));
    assert(test61);
    bool test62 = 0 == (prefixLines("line1\nline2\nline3\n", "___", true, false)
                        .compare("___line1\n___line2\n___line3\n"));
    assert(test62);
    bool test63 = 0 == (prefixLines("line1\nline2\nline3\n", "___", true, true)
                        .compare("___line1\n___line2\n___line3\n___"));
    assert(test63);
    retval = retval && test60 && test61 && test62 && test63;

    return retval;
}

static bool
test_makeOneLine()
{
    using namespace StringUtility;
    std::string s;

    // These test that the input does not change if it is only one line to start with.
    s = makeOneLine("hello world");
    bool test_a1 = 0 == s.compare("hello world");
    assert(test_a1);

    s = makeOneLine("   hello world");
    bool test_a2 = 0 == s.compare("   hello world");
    assert(test_a2);

    s = makeOneLine("hello world   ");
    bool test_a3 = 0 == s.compare("hello world   ");
    assert(test_a3);

    s = makeOneLine("hello   world");
    bool test_a4 = 0 == s.compare("hello   world");
    assert(test_a4);

    bool batch_a = test_a1 && test_a2 && test_a3 && test_a4;

    // These test cases with internal line termination and no white space around the line termination
    s = makeOneLine("hello\nworld");
    bool test_b1 = 0 == s.compare("hello world");
    assert(test_b1);

    s = makeOneLine("hello\n\nworld");
    bool test_b2 = 0 == s.compare("hello world");
    assert(test_b2);

    s = makeOneLine("hello\rworld");
    bool test_b3 = 0 == s.compare("hello world");
    assert(test_b3);

    s = makeOneLine("hello\n\rworld");
    bool test_b4 = 0 == s.compare("hello world");
    assert(test_b4);

    bool batch_b = test_b1 && test_b2 && test_b3 && test_b4;

    // These test line termination at the beginning and end of the string.
    s = makeOneLine("\nhello world");
    bool test_c1 = 0 == s.compare("hello world");
    assert(test_c1);

    s = makeOneLine("  \n  hello world");
    bool test_c2 = 0 == s.compare("hello world");
    assert(test_c2);

    s = makeOneLine("\n\nhello world");
    bool test_c3 = 0 == s.compare("hello world");
    assert(test_c3);

    s = makeOneLine("hello world\n");
    bool test_c4 = 0 == s.compare("hello world");
    assert(test_c4);

    s = makeOneLine("hello world  \n  ");
    bool test_c5 = 0 == s.compare("hello world");
    assert(test_c5);

    s = makeOneLine("hello world\n\n");
    bool test_c6 = 0 == s.compare("hello world");
    assert(test_c6);

    bool batch_c = test_c1 && test_c2 && test_c3 && test_c4 && test_c5 && test_c6;

    // These test multi-line cases with surrounding white space
    s = makeOneLine("hello\n  world");
    bool test_d1 = 0 == s.compare("hello world");
    assert(test_d1);

    s = makeOneLine("hello  \nworld");
    bool test_d2 = 0 == s.compare("hello world");
    assert(test_d2);

    s = makeOneLine("hello  \n  world");
    bool test_d3 = 0 == s.compare("hello world");
    assert(test_d3);

    s = makeOneLine("hello  \n\n world");
    bool test_d4 = 0 == s.compare("hello world");
    assert(test_d4);

    bool batch_d = test_d1 && test_d2 && test_d3 & test_d4;

    // These test blank lines (lines with just white space)
    s = makeOneLine("hello\n   \nworld");
    bool test_e1 = 0 == s.compare("hello world");
    assert(test_e1);

    s = makeOneLine("hello  \n  \n  world");
    bool test_e2 = 0 == s.compare("hello world");
    assert(test_e2);

    bool batch_e = test_e1 && test_e2;

    // These test user-defined replacement strings
    s = makeOneLine("hello\nworld", "[]");
    bool test_f1 = 0 == s.compare("hello[]world");
    assert(test_f1);

    s = makeOneLine("\nhello\n  \n  world\n", "[]");
    bool test_f2 = 0 == s.compare("hello[]world");
    assert(test_f2);

    s = makeOneLine("hello\n\n\n\n\nworld\n\n\n", "\n");
    bool test_f3 = 0 == s.compare("hello\nworld");
    assert(test_f3);

    bool batch_f = test_f1 && test_f2 && test_f3;

    return batch_a && batch_b && batch_c && batch_d && batch_e && batch_f;
}

// WARNING: This "test" doesn't really test anything. It just calls the functions and spits out results without checking that
//          the results are valid.
static bool
stringTest(std::string s)
{
    std::string s_filename = StringUtility::stripPathFromFileName(s);
    std::string s_path     = StringUtility::getPathFromFileName(s);
    std::string s_nosuffix = StringUtility::stripFileSuffixFromFileName(s);
    printf("s = %s s_filename = %s \n",s.c_str(),s_filename.c_str());
    printf("s = %s s_path     = %s \n",s.c_str(),s_path.c_str());
    printf("s = %s s_nosuffix = %s \n",s.c_str(),s_nosuffix.c_str());
    printf("\n");
    return true;
}

// WARNING: This "test" doesn't really test anything. It just calls the functions and spits out results without checking that
//          the results are valid.
static bool
test_removePseudoRedundentSubstrings() // sic
{
    std::string X = "ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D6(A);";
    printf("X = \n%s\n",X.c_str());
    std::string Y = StringUtility::removePseudoRedundentSubstrings ( X );
    printf("Y = \n%s\n",Y.c_str());
    return true;
}

// Here's another implementation of Damerau-Levenshtein edit distance which we can use to test the one in ROSE.
// https://github.com/ugexe/Text--Levenshtein--Damerau--XS/blob/master/damerau-int.c
namespace DamerauLevenshtein2 {
struct dictionary {
    unsigned int key;
    unsigned int value;
  struct dictionary* next;
};
typedef struct dictionary item;

static __inline item* push(unsigned int key,item* curr) {
    item* head;
    head = new item;
    head->key = key;
    head->value = 0;
    head->next = curr;
    return head;
}

static __inline item* find(item* head,unsigned int key) {
    item* iterator = head;
    while (iterator) {
        if (iterator->key == key) {
            return iterator;
        }
        iterator = iterator->next;
    }
    return NULL;
}

static __inline item* uniquePush(item* head,unsigned int key) {
    item* iterator = head;
    while (iterator) {
        if (iterator->key == key) {
            return head;
        }
        iterator = iterator->next;
    }
    return push(key,head);
}

static void dict_free(item* head) {
    item* iterator = head;
    while (iterator) {
        item* temp = iterator;
        iterator = iterator->next;
        delete temp;
    }
    head = NULL;
}

static int distance(const unsigned int *src, const unsigned int *tgt, unsigned int x, unsigned int y)
{
    item *head = NULL;
    unsigned int swapCount, swapScore, targetCharCount, i, j;
    unsigned int *scores = new unsigned int[(x+2)*(y+2)];
    unsigned int score_ceil = x + y;
 
    /* intialize matrix start values */
    scores[0] = score_ceil;
    scores[1 * (y + 2) + 0] = score_ceil;
    scores[0 * (y + 2) + 1] = score_ceil;
    scores[1 * (y + 2) + 1] = 0;
    head = uniquePush(uniquePush(head,src[0]),tgt[0]);

    /* work loops */
    /* i = src index */
    /* j = tgt index */
    for (i=1;i<=x;i++) {
        head = uniquePush(head,src[i]);
        scores[(i+1) * (y + 2) + 1] = i;
        scores[(i+1) * (y + 2) + 0] = score_ceil;
        swapCount = 0;

        for (j=1;j<=y;j++) {
            if (i == 1) {
                head = uniquePush(head,tgt[j]);
                scores[1 * (y + 2) + (j + 1)] = j;
                scores[0 * (y + 2) + (j + 1)] = score_ceil;
            }

            targetCharCount = find(head,tgt[j-1])->value;
            swapScore = scores[targetCharCount * (y + 2) + swapCount] + i - targetCharCount - 1 + j - swapCount;

            if (src[i-1] != tgt[j-1]) {
                scores[(i+1) * (y + 2) + (j + 1)] = std::min(swapScore,
                                                             std::min(scores[i * (y + 2) + j],
                                                                      std::min(scores[(i+1) * (y + 2) + j],
                                                                               scores[i * (y + 2) + (j + 1)])) + 1);
            } else {
                swapCount = j;
                scores[(i+1) * (y + 2) + (j + 1)] = std::min(scores[i * (y + 2) + j], swapScore);
            }
        }
        find(head,src[i-1])->value = i;
    }

    unsigned int score = scores[(x+1) * (y + 2) + (y + 1)];
    dict_free(head);
    delete[] scores;
    return score;
}
} // namespace

// Here's another implementation of Levenshtein edit distance so we can test the one in ROSE.
namespace Levenshtein2 {
template <class T>
unsigned int edit_distance(const T& s1, const T& s2)
{
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<unsigned int> > d(len1 + 1, std::vector<unsigned int>(len2 + 1));
 
    d[0][0] = 0;
    for (unsigned int i = 1; i <= len1; ++i)
        d[i][0] = i;
    for (unsigned int i = 1; i <= len2; ++i)
        d[0][i] = i;
 
    for (unsigned int i = 1; i <= len1; ++i) {
        for (unsigned int j = 1; j <= len2; ++j) {
            d[i][j] = std::min(std::min(d[i - 1][j] + 1,d[i][j - 1] + 1),
                               d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
        }
    }
    return d[len1][len2];
}
} // namespace

static bool
test_edit_distance()
{
    static const size_t sz_delta = 5;                   // max difference in lengths of vectors (v1 is longer than v2)
    static const size_t sz_max = 20;                    // maximum length of vectors
    static const unsigned int elmt_modulo = 8;          // modulo for choosing elements for the two vectors
    static const bool show_every_run = true;            // set if you want to see every test
    LinearCongruentialGenerator random;                 // source of pseudo-random numbers
    size_t nfailures = 0;

    // The first of two vectors
    for (size_t sz1=sz_delta; sz1<sz_max; ++sz1) {
        std::vector<unsigned int> v1(sz1, 0);
        for (size_t i=0; i<sz1; ++i)
            v1[i] = random() % elmt_modulo;

        // The second of two vectors.
        for (size_t sz2=sz1-sz_delta; sz2<sz1; ++sz2) {
            std::vector<unsigned int> v2(sz2, 0);
            for (size_t i=0; i<sz2; ++i)
                v2[i] = v1[i];
            Combinatorics::shuffle(v2);

            if (show_every_run) {
                std::cerr <<"using these vectors:\n"
                          <<"    v1[" <<sz1 <<"] = {";
                for (size_t i=0; i<sz1; ++i)
                    std::cerr <<" " <<v1[i];
                std::cerr <<"}\n"
                          <<"    v2[" <<sz2 <<"] = {";
                for (size_t i=0; i<sz2; ++i)
                    std::cerr <<" " <<v2[i];
                std::cerr <<"}\n";
            }
                

            for (size_t algo=0; algo<2; ++algo) {
                size_t d1, d2;
                const char *name;
                switch (algo) {
                    case 0:
                        name = "Levenshtein";
                        d1 = EditDistance::levenshteinDistance(v1, v2);
                        d2 = Levenshtein2::edit_distance(v1, v2);
                        break;
                    case 1:
                        name = "Damerau-Levenshtein";
                        d1 = EditDistance::damerauLevenshteinDistance(v1, v2);
                        if (v1.empty())
                            v1.push_back(911); // we need a pointer, and &v1[0] won't cut it if v1 is empty
                        if (v2.empty())
                            v2.push_back(911);
                        d2 = DamerauLevenshtein2::distance(&v1[0], &v2[0], sz1, sz2);
                        v1.resize(sz1);
                        v2.resize(sz2);
                        break;
                }

                if (d1!=d2) {
                    std::cerr <<"failure for " <<name <<" edit distance:\n"
                              <<"    v1[" <<sz1 <<"] = {";
                    for (size_t i=0; i<sz1; ++i)
                        std::cerr <<" " <<v1[i];
                    std::cerr <<"}\n"
                              <<"    v2[" <<sz2 <<"] = {";
                    for (size_t i=0; i<sz2; ++i)
                        std::cerr <<" " <<v2[i];
                    std::cerr <<"}\n"
                              <<"    rose implementation: distance=" <<d1 <<"\n"
                              <<"    test implementation: disatnce=" <<d2 <<"\n";
                    ++nfailures;
                } else if (show_every_run) {
                    std::cerr <<"    " <<name <<"\n"
                              <<"        ROSE=" <<d1 <<", alternate=" <<d2 <<"\n";
                }
            }
        }
    }
    return 0==nfailures;
}


            


int
main()
{
    size_t nfailures = 0;

    nfailures += test_removePseudoRedundentSubstrings() ? 0 : 1;

    nfailures += stringTest("foo.h") ? 0 : 1;
    nfailures += stringTest("/foo.h") ? 0 : 1;
    nfailures += stringTest("//foo.h") ? 0 : 1;
    nfailures += stringTest("///foo.h") ? 0 : 1;
    nfailures += stringTest("////foo.h") ? 0 : 1;
    nfailures += stringTest("./foo.h") ? 0 : 1;
    nfailures += stringTest("../foo.h") ? 0 : 1;
    nfailures += stringTest("//foo.h") ? 0 : 1;
    nfailures += stringTest("path/foo.h") ? 0 : 1;
    nfailures += stringTest("/path/foo.h") ? 0 : 1;
    nfailures += stringTest("/pathA/pathB/foo.h") ? 0 : 1;
    nfailures += stringTest("foo") ? 0 : 1;
    nfailures += stringTest("/path/foo") ? 0 : 1;

    nfailures += test_isLineTerminated() ? 0 : 1;

    nfailures += test_prefixLines() ? 0 : 1;

    nfailures += test_makeOneLine() ? 0 : 1;

    nfailures += test_edit_distance() ? 0 : 1;

    return 0==nfailures;
}





