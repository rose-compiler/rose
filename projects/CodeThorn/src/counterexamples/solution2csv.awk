#!/usr/bin/awk -f
BEGIN { n = 60; }
/^Formula is satisfied/ {
    n++;
    print n ",yes,9";
}
/^Formula is not satisfied/ {
    n++;
    print n ",no,9";
}
