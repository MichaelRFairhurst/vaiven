#!/bin/bash

echo > bench_log

export TIMEFORMAT="%U"

BASELINE=0
COMPARISON=0

run_single_bench() {
  echo "[$1]"
  TIME=$( time (
    ./$1.vvn.sh | (../vvn 2>&1 >> bench_log_$1)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
    ./$1.vvn.sh | (../vvn 2>&1 > /dev/null)
  ) 2>&1)
  DIFFERENCE="$( echo $TIME - $BASELINE | bc -l )"
  echo "$DIFFERENCE over/under baseline of $BASELINE ($TIME)"
  
  if [[ "z$COMPARISON" != "z0" ]]
  then
    CHANGEX=$(printf %.2f $(echo "$COMPARISON / $DIFFERENCE" | bc -l))
    echo "Comparison ($COMPARISON / $DIFFERENCE): ${CHANGEX}X"
  fi
}

run_single_bench repeat_ops_parser
BASELINE="$TIME"
run_single_bench repeat_ops_interp

RECURSE_COMPARISON="$DIFFERENCE"
BASELINE=0

run_single_bench repeat_ops_functions_parse
BASELINE="$TIME"
COMPARISON="$RECURSE_COMPARISON"
run_single_bench repeat_ops_functions

COMPARISON=0
BASELINE=0
run_single_bench repeat_ops_functions8_parse
BASELINE="$TIME"
COMPARISON="$RECURSE_COMPARISON"
run_single_bench repeat_ops_functions8

COMPARISON=0
BASELINE=0
run_single_bench repeat_ops_functions50_parse
BASELINE="$TIME"
COMPARISON="$RECURSE_COMPARISON"
run_single_bench repeat_ops_functions50

BASELINE=0
COMPARISON=0
run_single_bench repeat_ops_recurse_parse
BASELINE="$TIME"
COMPARISON="$RECURSE_COMPARISON"
run_single_bench repeat_ops_recurse

BASELINE=0
COMPARISON="$RECURSE_COMPARISON"
run_single_bench repeat_functions
