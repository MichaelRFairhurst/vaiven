#!/bin/bash

cat <<EOF
fn ops of x is
  x + x - x * x / x
end

var x = 1

for x < 50000000
  ops(x)
  x = x + 1
end

EOF
