# Print out the error type
/^ERROR/I {
    s/^ERROR: \(.*\)$/\1/I
    # Disabled for now -- just check the line numbers.  When we want to check
    # the type of error as well, uncomment here and in extract-actual-error.sed
    #P
}

# Print out the line number
/at line [0-9]\+/I {
    s/^.*at line \([0-9]\+\).*$/\1/I
    P
}


