#!/bin/python

import argparse

if __name__ == '__main__':
    # Command line argument handling                                                                                             
    cmdline = argparse.ArgumentParser(
        description='compare csv reference solution with csv file',
        epilog='Please report bugs to <adrian@complang.tuwien.ac.at>.')

    cmdline.add_argument('--ref', metavar='<solution.csv>', type=file, help='LTL CSV data')
    cmdline.add_argument('--mine', metavar='<mine.csv>', type=file, help='codethorn ltl output')
    cmdline.add_argument('--verbose', action='store_true', help='verbose mode')
    cmdline.add_argument('--ltlstart', default=60, type=int, help='ltl start')

    args = cmdline.parse_args()
    if not args.ref: print 'no reference input specified!'; exit(1)
    if not args.mine: print 'no codethorn input specified!'; exit(1)

    correct      = 0
    inconsistent = 0
    unknown      = 0
    for line in args.ref.readlines():
        idx, result, confidence = line.split(',')
        n = int(idx)-args.ltlstart
        if n <= 0:
            # skip over assertions
            continue

        line = args.mine.readline()
        my_idx, my_result, _ = line.split(',')

        assert(idx == my_idx)
        if my_result == result:
            correct += 1
        elif my_result == 'unknown':
            unknown += 1
        else:
            if args.verbose:
                print '** LTL %d:'%(n-1), line, 'should be', result
            inconsistent += 1

    print correct,inconsistent,unknown,n
    assert(correct+inconsistent+unknown == n)
    red = '\033[31m'
    green = '\033[32m'
    reset = '\033[39m'
    print
    print "Statistics"
    print "=========="
    if inconsistent > 0:
        inconsistent_color=red
    else:
        inconsistent_color=green

    print "%d/%d Consistent, %s%d/%d Inconsistent%s, %d/%d Unknown" % (
        correct, n, inconsistent_color, inconsistent, n, reset, unknown, n)

    if inconsistent > 0:
        exit(1)

