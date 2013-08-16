#!/usr/bin/python

def boundscheck(n):
    if n < 1 or n > 100:
        print "** PARSE ERROR"
        exit(1)

# needs python 2.7?
import argparse

if __name__ == '__main__':
    # Command line argument handling
    cmdline = argparse.ArgumentParser(
        description='compare counterexample output with rers csv output',
        epilog='Please report bugs to <adrian@llnl.gov>.')

    cmdline.add_argument('--csv', metavar='<ltl.csv>', type=file, help='LTL CSV data')
    cmdline.add_argument('--log', metavar='<counterexamples.log>', type=file, help='qc log output')
    cmdline.add_argument('--verbose', action='store_true', help='verbose mode')

    args = cmdline.parse_args()
    if not args.csv: print 'no csv input specified!'; exit(1)
    if not args.log: print 'no log input specified!'; exit(1)

    filename = args.log.name

    next_counterexample = -1
    correct      = 0
    inconsistent = 0
    unverified   = 0
    unknown      = 0
    for line in args.csv.readlines(): # OLD FORMAT [1:]:
        # OLD FORMAT idx, formula, result, confidence = line.split(';')
        idx, result, confidence = line.split(',')
        n = int(idx)-60
        boundscheck(n)

        if next_counterexample < n:
            # skip to next counterexample
            while 1:
                qc_line = args.log.readline()
                if qc_line == "": # EOF
                    # no more counterexamples in the log file
                    next_counterexample = -1
                    break
                if qc_line.find("FALSE, found counterexample") <> -1:
                    next_counterexample = int(qc_line.split(' ')[0])
                    boundscheck(next_counterexample)
                    break

        if n == next_counterexample:
            if result == "yes":
                print "** INCONSISTENCY"
                print "  ", line
                print "  ", qc_line
                inconsistent += 1
            elif result == "no":
                if (args.verbose): print "%d consistent"%n
                correct += 1
            elif result == "unknown":
                print "%d UNKNOWN, but counterexample exists"%n
                unknown += 1
        else:   
            if (args.verbose): print "%d consistent, but unverified"%n
            if result == "unknown":
                unknown += 1
            else: 
                unverified += 1
    print correct,inconsistent,unverified,unknown,n
    assert(correct+inconsistent+unverified+unknown == n)
    red = '\033[31m' # MS: changed color code to standard 31-38 (instead of non-standard aixterm codes)
    green = '\033[32m'
    reset = '\033[39m'
    print
    print "Statistics"
    print "=========="
    if inconsistent > 0:
        inconsistent_color=red
    else:
        inconsistent_color=green
		
    print "%d/%d Consistent, %s%d/%d Inconsistent%s, %d/%d Unverified, %d/%d Unknown" % (
        correct, n, inconsistent_color, inconsistent, n, reset, unverified, n, unknown, n)
    
    if inconsistent > 0:
        exit(1)
