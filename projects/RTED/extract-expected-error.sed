

# Store a special number passed to the script.  This is intended to be done by
# calling wc -l on the input file and passing this to sed as stdin.  It is
# stored in the hold space in case it's needed, e.g. if we want to say the error
# line is the last line of the program.
/===\(.*\)===/ {
    s/===\([0-9]\+\).*===/\1/
    h
}


# At the moment, we only track memory leaks at the end of the problem.  If we're
# ever more precise, then this rule is unnecessary, i.e. memory leak errors
# other than ones "upon exiting the program," should have their proper line
# number extracted.
/ERROR: memory block not freed/,$ {
    x
    P
	q
}
/upon exiting the program/ {
    x
    P
}


# Print out the error type
/^ERROR/I {
    s/^ERROR: \(.*\)$/\1/I
    # Disabled for now -- just check the line numbers.  When we want to check
    # the type of error as well, uncomment here and in extract-actual-error.sed
    #P
}

# Print out the line number
/at line [0-9]\+/I {

	########################################################################
    # Special cases where we disagree with RTED about exactly where the error
	# line is
    s/^.*at line \([0-9]\+\).*c_K_1_d.*$/49/I
	# Actually here I think the ok_msg is just completely wrong.  It conflicts
	# with the comments in the .c file, which are actually correct.
    s/^.*at line \([0-9]\+\).*c_J_6_a.*$/34/I
	########################################################################

	########################################################################
	# Special cases where we report the error at the end of a range (e.g. "at
	# line x|y")
    s/^.*at line \([0-9]\+|\)\?\([0-9]\+\).*c_A_9_e.*$/\2/I
	########################################################################


    # In the normal case we just output the line number.
    s/^.*at line \([0-9]\+\).*$/\1/I
    P
}

