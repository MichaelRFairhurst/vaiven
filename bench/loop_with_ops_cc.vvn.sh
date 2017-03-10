#!/bin/bash

cat <<EOF
fn loop
  var x = 1
  for x < 50000000
    x + x - x * x / x
    x = x + 1
  end
end

loop()

EOF
