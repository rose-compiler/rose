// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Assert.h>
#include <Sawyer/LineVector.h>

using namespace Sawyer::Container;

static const size_t BIG_OFFSET = (size_t)(-1) - 10;

static void
test01() {
    {
        LineVector v1(0, "");
        ASSERT_always_require(v1.nLines() == 0);
    }

    {
        LineVector v2(0, "");
        ASSERT_always_require(v2.nCharacters() == 0);
    }

    {
        LineVector v3(0, "");
        ASSERT_always_require(v3.nCharacters(1) == 0);
        ASSERT_always_require(v3.nCharacters(BIG_OFFSET) == 0);

        ASSERT_always_require(v3.character(0) == EOF);
        ASSERT_always_require(v3.character(BIG_OFFSET) == EOF);
        ASSERT_always_require(v3.character(0, 0) == EOF);
        ASSERT_always_require(v3.character(BIG_OFFSET, 0) == EOF);
        ASSERT_always_require(v3.character(0, BIG_OFFSET) == EOF);
        ASSERT_always_require(v3.line(0) == NULL);
        ASSERT_always_require(v3.line(BIG_OFFSET) == NULL);
        ASSERT_always_require(v3.characterIndex(0) == 0);
        ASSERT_always_require(v3.characterIndex(BIG_OFFSET) == 0);
        ASSERT_always_require(v3.lineIndex(0) == 0);
        ASSERT_always_require(v3.lineIndex(BIG_OFFSET) == 0);
        ASSERT_always_require(v3.isLastLineTerminated() == false);
        ASSERT_always_require(v3.isEmpty() == true);
    }
}

static void
test02() {
    static const size_t n = 10;
    static const char *s = "a\nbc\nd\0f\n\nXXX";

    {
        LineVector v1(n, s);
        ASSERT_always_require(v1.nLines() == 4);
        ASSERT_always_require(v1.isEmpty() == false);
        ASSERT_always_require(v1.isLastLineTerminated() == true);
    }

    {
        LineVector v2(1, s);
        ASSERT_always_require(v2.nCharacters() == 1);
    }

    {
        LineVector v3(n, s);
        ASSERT_always_require(v3.nCharacters() == n);
    }

    {
        LineVector v4(n, s);
        ASSERT_always_require(v4.nCharacters(0) == 2);
        ASSERT_always_require(v4.nCharacters(2) == 4);
        ASSERT_always_require(v4.nCharacters(4) == 0);
        ASSERT_always_require(v4.nCharacters(3) == 1);
        ASSERT_always_require(v4.nCharacters(BIG_OFFSET) == 0);
    }

    {
        LineVector v5(n, s);
        ASSERT_always_require(v5.character(0) == 'a');
        ASSERT_always_require(v5.character(1) == '\n');
        ASSERT_always_require(v5.character(6) == '\0');
        ASSERT_always_require(v5.character(7) == 'f');
        ASSERT_always_require(v5.character(9) == '\n');
        ASSERT_always_require(v5.character(n) == EOF);
        ASSERT_always_require(v5.character(BIG_OFFSET) == EOF);
    }

    {
        LineVector v6(n, s);
        ASSERT_always_require(v6.character(2, 0) == 'd');
        ASSERT_always_require(v6.character(2, 1) == '\0');
        ASSERT_always_require(v6.character(2, 2) == 'f');
        ASSERT_always_require(v6.character(2, 3) == '\n');
        ASSERT_always_require(v6.character(2, 4) == '\0');
        ASSERT_always_require(v6.character(2, BIG_OFFSET) == '\0');
        ASSERT_always_require(v6.character(3, 0) == '\n');
        ASSERT_always_require(v6.character(3, 1) == '\0');
        ASSERT_always_require(v6.character(3, BIG_OFFSET) == '\0');
        ASSERT_always_require(v6.character(4, 0) == EOF);
        ASSERT_always_require(v6.character(4, BIG_OFFSET) == EOF);
        ASSERT_always_require(v6.character(BIG_OFFSET, 0) == EOF);
        ASSERT_always_require(v6.character(BIG_OFFSET, BIG_OFFSET) == EOF);
    }

    {
        LineVector v7(n, s);
        ASSERT_always_not_null(v7.line(0));
        ASSERT_always_require(0 == strncmp(v7.line(0), "a\n", 2));
        ASSERT_always_require(0 == strncmp(v7.line(3), "\n", 1));
        ASSERT_always_require(v7.line(4) == NULL);
        ASSERT_always_require(v7.line(BIG_OFFSET) == NULL);
    }

    {
        LineVector v8(n, s);
        ASSERT_always_require(v8.characterIndex(3) == 9);
        ASSERT_always_require(v8.characterIndex(4) == n);
        ASSERT_always_require(v8.characterIndex(BIG_OFFSET) == n);
    }

    {
        LineVector v9(n, s);
        ASSERT_always_require(v9.lineIndex(0) == 0);
        ASSERT_always_require(v9.lineIndex(1) == 0);
        ASSERT_always_require(v9.lineIndex(2) == 1);
        ASSERT_always_require(v9.lineIndex(9) == 3);
        ASSERT_always_require(v9.lineIndex(10) == 4);
        ASSERT_always_require(v9.lineIndex(11) == 4);
        ASSERT_always_require(v9.lineIndex(BIG_OFFSET) == 4);
    }
}

static void
test03() {
    static const size_t n = 11;
    static const char *s = "a\nbc\nd\0f\n\ngXX";

    {
        LineVector v1(n, s);
        ASSERT_always_require(v1.nLines() == 5);
        ASSERT_always_require(v1.isEmpty() == false);
        ASSERT_always_require(v1.isLastLineTerminated() == false);
    }

    {
        LineVector v2(1, s);
        ASSERT_always_require(v2.nCharacters() == 1);
    }

    {
        LineVector v3(n, s);
        ASSERT_always_require(v3.nCharacters() == n);
    }

    {
        LineVector v4(n, s);
        ASSERT_always_require(v4.nCharacters(0) == 2);
        ASSERT_always_require(v4.nCharacters(2) == 4);
        ASSERT_always_require(v4.nCharacters(4) == 1);
        ASSERT_always_require(v4.nCharacters(3) == 1);
        ASSERT_always_require(v4.nCharacters(5) == 0);
        ASSERT_always_require(v4.nCharacters(BIG_OFFSET) == 0);
    }

    {
        LineVector v5(n, s);
        ASSERT_always_require(v5.character(0) == 'a');
        ASSERT_always_require(v5.character(1) == '\n');
        ASSERT_always_require(v5.character(6) == '\0');
        ASSERT_always_require(v5.character(7) == 'f');
        ASSERT_always_require(v5.character(9) == '\n');
        ASSERT_always_require(v5.character(10) == 'g');
        ASSERT_always_require(v5.character(n) == EOF);
        ASSERT_always_require(v5.character(BIG_OFFSET) == EOF);
    }

    {
        LineVector v6(n, s);
        ASSERT_always_require(v6.character(2, 0) == 'd');
        ASSERT_always_require(v6.character(2, 1) == '\0');
        ASSERT_always_require(v6.character(2, 2) == 'f');
        ASSERT_always_require(v6.character(2, 3) == '\n');
        ASSERT_always_require(v6.character(2, 4) == '\0');
        ASSERT_always_require(v6.character(2, BIG_OFFSET) == '\0');
        ASSERT_always_require(v6.character(3, 0) == '\n');
        ASSERT_always_require(v6.character(3, 1) == '\0');
        ASSERT_always_require(v6.character(3, BIG_OFFSET) == '\0');
        ASSERT_always_require(v6.character(4, 0) == 'g');
        ASSERT_always_require(v6.character(4, 1) == '\0');
        ASSERT_always_require(v6.character(4, BIG_OFFSET) == '\0');
        ASSERT_always_require(v6.character(5, 0) == EOF);
        ASSERT_always_require(v6.character(5, BIG_OFFSET) == EOF);
        ASSERT_always_require(v6.character(BIG_OFFSET, 0) == EOF);
        ASSERT_always_require(v6.character(BIG_OFFSET, BIG_OFFSET) == EOF);
    }

    {
        LineVector v7(n, s);
        ASSERT_always_not_null(v7.line(0));
        ASSERT_always_require(0 == strncmp(v7.line(0), "a\n", 2));
        ASSERT_always_require(0 == strncmp(v7.line(3), "\n", 1));
        ASSERT_always_require(0 == strncmp(v7.line(4), "g", 1));
        ASSERT_always_require(v7.line(5) == NULL);
        ASSERT_always_require(v7.line(BIG_OFFSET) == NULL);
    }

    {
        LineVector v8(n, s);
        ASSERT_always_require(v8.characterIndex(3) == 9);
        ASSERT_always_require(v8.characterIndex(4) == 10);
        ASSERT_always_require(v8.characterIndex(5) == n);
        ASSERT_always_require(v8.characterIndex(BIG_OFFSET) == n);
    }

    {
        LineVector v9(n, s);
        ASSERT_always_require(v9.lineIndex(0) == 0);
        ASSERT_always_require(v9.lineIndex(1) == 0);
        ASSERT_always_require(v9.lineIndex(2) == 1);
        ASSERT_always_require(v9.lineIndex(9) == 3);
        ASSERT_always_require(v9.lineIndex(10) == 4);
        ASSERT_always_require(v9.lineIndex(11) == 5);
        ASSERT_always_require(v9.lineIndex(BIG_OFFSET) == 5);
    }
}

int
main() {
    test01();
    test02();
    test03();
}
