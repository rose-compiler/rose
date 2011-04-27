{
  # rename our errors to match RTED's
  # s/Illegal Memory Overlap/memory overlap/

  s/^Violation: \(.*\) at .*(\([-0-9]\+\),.*/Vio\2/i
}

# Subtract 2 from the actual line number to compensate for the lines added by
# the instrumentation
s/^Vio\([0-9]\+\)$/echo $(( \1 - 2 ))/iep



# was:
#/Violation/I {
    ## rename our errors to match RTED's
    ## s/Illegal Memory Overlap/memory overlap/


    ## replace output with only two lines
    ##   error
    ##   line number
    # s/^Violation: \(.*\) at .*(\([-0-9]\+\),.*/\1\n\2/I


    ## Disabled for now -- just check the line numbers.  When we want to check
    ## the type of error as well, uncomment here and in extract-expected-error.sed
    ##
    ## also update the renaming (above) to include all of our errors

    ## print error and re-evaluate the line number so we can correct it
    ##P
    #D
#}

## Subtract 2 from the actual line number to compensate for the lines added by
## the instrumentation
#/^[0-9]\+$/I {
#    s/\(.*\)/echo $(( \1 - 2 ))/ep
#}
