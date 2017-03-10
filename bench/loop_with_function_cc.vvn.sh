#!/bin/bash

cat <<EOF
fn ops of x is
  x + x - x * x / x
end

fn loop
  var x = 1
  
  for x < 50000000
    ops(x)
    x = x + 1
  end
end

loop()

EOF
