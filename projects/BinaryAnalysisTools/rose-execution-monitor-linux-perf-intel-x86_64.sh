#!/usr/bin/env bash

helpmsg() {
  echo "Monitors the execution of a program using Linux perf together with"
  echo "Intel x86 monitoring capabilities and produces a coverage number. "
  echo "The coverage number is an estimate how many unique instructions were executed. "
  echo "To this end, we gather unique code intervals from perf"
  echo ""
  echo "Usage: rose-execution-monitor-linux-perf-intel-x76_64.sh -o outfile -- specimen arguments"
  echo "         -o outfile       a file containing two lines:"
  echo "     or  --output=outfile (1) a human-readable integer value,"
  echo "                              the exit code of the child process;"
  echo "                          (2) a human-readable floating point value,"
  echo "                              the quality score of the execution."
  echo "         --               separator between arguments to execmon"
  echo "                          and test specification."
  echo "         specimen         the tested specimen"
  echo "         arguments..      an arbitrary long argument list passed"
  echo "                          to specimen."     
}

# echo "Your command line contains $# arguments: $@"

while [[ $# -gt 0 ]] && [[ "$1" != "--" ]]; do
  case "$1" in
    -o) 
      outfile="$2"
      shift 2
      ;;
    --output=*)
      outfile=${1#*=}
      shift
      ;;
    -h|--help)
      helpmsg
      exit 0
      ;;
    *)
      echo "unknown argument: $1"
      exit 1
      ;;
  esac
done

if [ $# -eq 0 ]; then
  echo "no specimen provided"
  echo "rose-execution-monitor-linux-perf-intel-x86_64.sh options -- specimen testargs.."
  echo "use -h or --help for more information."
  exit 1;
fi

# shift out --
shift

specimen="$1"
shift

#set -o xtrace

perf record -e intel_pt//u rose-perf-execution-wrapper "$outfile" "$specimen" "$@"

echo "$?" > "$outfile"

# Not working alternative: should filter startup code
# perf record -e intel_pt//u --filter 'start main @ $specimen' "$specimen $@"
perf script --itrace=igb -F brstack,ip,addr,dso,flags | ./rose-perf-analyzer >> "$outfile" 

