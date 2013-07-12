#!/usr/bin/awk -f
/^[0-9]+ FALSE, found counterexample/ {
    print $1+60 ",no,9"
}
