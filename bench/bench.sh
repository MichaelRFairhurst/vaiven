#!/bin/bash

echo > bench_log

export TIMEFORMAT="%U"

BASELINE=0
COMPARISON=0

run_vvn_bench() {
  run_cmd_bench $1 "./$1.vvn.sh | ../vvn"
}

run_cmd_bench() {
  echo -n "[$1] "
  TIME=$( time (
    (eval "$2") 2>&1 > bench_log_$1
  ) 2>&1)
  DIFFERENCE="$( echo $TIME - $BASELINE | bc -l )"
  echo "$DIFFERENCE over/under baseline of $BASELINE ($TIME)"
  eval "TIME_$1=$TIME"
  eval "DIFFERENCE_$1=$DIFFERENCE"
}

compare() {
  AVARNAME="TIME_$1"
  BVARNAME="TIME_$2"
  A="${!AVARNAME}"
  B="${!BVARNAME}"
  CHANGEX=$(printf %.2f $(echo "$B / $A" | bc -l))
  echo ":: $1 ${CHANGEX}X faster than $2 ($B / $A)"
}

compare_adjust_baseline() {
  AVARNAME="DIFFERENCE_$1"
  BVARNAME="DIFFERENCE_$2"
  A="${!AVARNAME}"
  B="${!BVARNAME}"
  CHANGEX=$(printf %.2f $(echo "$B / $A" | bc -l))
  echo ":: $1 ${CHANGEX}X faster than $2 ($B / $A)"
}

baseline() {
  BASELINEVARNAME="TIME_$1"
  BASELINE="${!BASELINEVARNAME}"
}

run_cmd_bench fibjs "node fib_45.js"
run_cmd_bench fibvvn "./fib_45.vvn.sh | ../vvn"
compare fibvvn fibjs

run_vvn_bench loop_empty_interp
run_vvn_bench loop_empty_cc
run_vvn_bench loop_empty_hot

baseline loop_empty_interp
run_vvn_bench loop_with_ops_interp
run_vvn_bench loop_with_function_interp
compare_adjust_baseline loop_with_function_interp loop_with_ops_interp

baseline loop_empty_cc
run_vvn_bench loop_with_ops_cc
compare loop_with_ops_cc loop_with_ops_interp
run_vvn_bench loop_with_function_cc
compare_adjust_baseline loop_with_function_cc loop_with_ops_cc

baseline loop_empty_hot
run_vvn_bench loop_with_ops_hot
compare loop_with_ops_hot loop_with_ops_interp
run_vvn_bench loop_with_function_hot
compare loop_with_function_hot loop_with_ops_hot
compare loop_with_function_hot loop_with_ops_interp

BASELINE=0
run_vvn_bench recurse_with_ops
compare recurse_with_ops loop_with_ops_interp
compare recurse_with_ops loop_with_ops_cc
compare recurse_with_ops loop_with_ops_hot

BASELINE=0
run_vvn_bench repeat_functions
compare loop_with_ops_interp repeat_functions
