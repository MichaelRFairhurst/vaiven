#!/bin/bash

cat <<EOF
fn loop
  var x = 1
  var a = 0
  for x < 50000000
    x = x + 1
    if x / 50000 == 0
      a = append("foo", "bar")
    else
      a = listWith(a)
    end
  end
end

loop()

EOF
