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


# Store a special number passed to the script.  This is intended to be done by
# calling wc -l on the input file and passing this to sed as stdin.  It is
# stored in the hold space in case it's needed, e.g. if we want to say the error
# line is the last line of the program.
/===\(.*\)===/ {
    s/===\([0-9]\+\).*===/\1/
    h
}

/upon exiting the program/ {
    x
    P
}


