#!/bin/bash

cat <<EOF
fn ops of x is
  x + x - x * x / x;
end

fn loop of x is
  for x < 50000000 do
    ops(x);
    x = x + 1;
  end
end

var primings = 10;
for primings > 0 do
  loop(50000000);
  primings = primings - 1;
end

loop(1);

EOF
