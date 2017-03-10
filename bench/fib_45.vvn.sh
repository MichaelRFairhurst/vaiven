#!/bin/bash
cat <<EOF
fn fib of x is
  if x == 0
    ret 0
  end
  if x == 1
    ret 1
  end
  ret fib(x - 1) + fib(x - 2)
end

fib(45)
EOF
