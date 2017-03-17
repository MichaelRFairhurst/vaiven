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

echo
echo V8/dart comparison
echo

run_cmd_bench fibvvn "./fib_45.vvn.sh | ../vvn"
run_cmd_bench fibjs "js fib_45.js"
run_cmd_bench fibdart "dart fib_45.dart"
compare fibvvn fibdart
compare fibvvn fibjs

echo
echo interp/cc/hot comparison
echo

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

echo
echo recursion comparison
echo

BASELINE=0
run_vvn_bench recurse_with_ops
compare recurse_with_ops loop_with_ops_interp
compare recurse_with_ops loop_with_ops_cc
compare recurse_with_ops loop_with_ops_hot

echo
echo garbage comparison
echo

baseline loop_empty_cc
run_vvn_bench loop_with_garbagemem_cc
compare loop_with_garbagemem_cc loop_empty_cc
BASELINE=0
run_cmd_bench loop_empty_js "js loop_empty.js"
baseline loop_empty_js
run_cmd_bench loop_with_garbagemem_js "js loop_with_garbagemem.js"
compare loop_with_garbagemem_js loop_with_garbagemem_cc

echo
echo compilation comparison
echo

BASELINE=0
run_vvn_bench repeat_functions
compare loop_with_ops_interp repeat_functions
