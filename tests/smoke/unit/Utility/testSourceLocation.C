#include <rose.h>
#include <SourceLocation.h>

using namespace Rose;

int main() {
    // Basic construction, property access, and comparison
    SourceLocation a("/dev/null", 100);
    ASSERT_always_require(a.fileName() == "/dev/null");
    ASSERT_always_require(a.line() == 100);
    ASSERT_always_forbid(a.column());
    ASSERT_always_require(a.toString() == "/dev/null:100");
    ASSERT_always_require(a.printableName() == "\"/dev/null\":100");

    SourceLocation b("/dev/null", 100);
    ASSERT_always_require(a == b);
    ASSERT_always_require(b == a);
    ASSERT_always_forbid(a != b);
    ASSERT_always_forbid(b != a);
    ASSERT_always_require(a >= b);
    ASSERT_always_require(a <= b);
    ASSERT_always_require(b >= a);
    ASSERT_always_require(b <= a);

    SourceLocation c("/dev/null", 101);
    ASSERT_always_require(a != c);
    ASSERT_always_require(c != a);
    ASSERT_always_forbid(a == c);
    ASSERT_always_forbid(c == a);
    ASSERT_always_forbid(a >= c && a <= c);
    ASSERT_always_forbid(c >= a && c <= a);
    ASSERT_always_require(a < c || a > c);
    ASSERT_always_require(c < a || c > a);

    SourceLocation d("/dev/null", 100, 0);
    ASSERT_always_require(d.column());
    ASSERT_always_require(*d.column() == 0);
    ASSERT_always_require(a != d);
    ASSERT_always_require(d != a);
    ASSERT_always_forbid(a == d);
    ASSERT_always_forbid(d == a);
    ASSERT_always_forbid(a >= d && a <= d);
    ASSERT_always_forbid(d >= a && d <= a);
    ASSERT_always_require(a < d || a > d);
    ASSERT_always_require(d < a || d > a);
    ASSERT_always_require(d.toString() == "/dev/null:100:0");
    ASSERT_always_require(d.printableName() == "\"/dev/null\":100:0");

    SourceLocation e("/dev/random", 100);
    ASSERT_always_require(a.fileName() == "/dev/null");
    ASSERT_always_require(e.fileName() == "/dev/random");
    ASSERT_always_require(a != e);
    ASSERT_always_require(e != a);
    ASSERT_always_forbid(a == e);
    ASSERT_always_forbid(e == a);
    ASSERT_always_forbid(a >= e && a <= e);
    ASSERT_always_forbid(e >= a && e <= a);
    ASSERT_always_require(a < e || a > e);
    ASSERT_always_require(e < a || e > a);
    
    // Destruction with same name as before
    {
        SourceLocation f("/dev/null", 100);
        ASSERT_always_require(f.fileName() == "/dev/null");
    }
    ASSERT_always_require(a.fileName() == "/dev/null");
}
