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

    cmdline.add_argument('--csv', metavar='<ltl.csv>', type=file,
                         help='LTL CSV data')

    cmdline.add_argument('--log', metavar='<counterexamples.log>', type=file,
                         help='qc log output')

    cmdline_args = cmdline.parse_args()
    if not cmdline_args.csv: print 'no csv input specified!'; exit(1)
    if not cmdline_args.log: print 'no log input specified!'; exit(1)

    filename = cmdline_args.log.name

    status = 0
    next_counterexample = -1
    for line in cmdline_args.csv.readlines()[1:]:
        idx, formula, result, confidence = line.split(';')
        n = int(idx)
        boundscheck(n)

        if next_counterexample < n:
            # skip to next coutnerexample
            while 1:
                qc_line = cmdline_args.log.readline()
                if qc_line == "": # EOF
                    exit(status)
                if qc_line.find("FALSE, found counterexample") <> -1:
                    next_counterexample = int(qc_line.split(' ')[0])
                    boundscheck(next_counterexample)
                    break

        if n == next_counterexample:
            if result == "YES":
                print "** INCONSISTENCY"
                print "  ", line
                print "  ", qc_line
            if result == "NO":
                print "%d consistent"%n
            if result == "UNKNOWN":
                print "%d UNKNOWN, but counterexample exists"%n
        
