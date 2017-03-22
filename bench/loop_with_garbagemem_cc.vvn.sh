#!/bin/bash

cat <<EOF
fn loop
  var x = 1
  var a = []
  for x < 50000000
    x = x + 1
    if x / 50000 == 0
      a = "foo" + "bar"
    else
      a = []
    end
  end
end

loop()

EOF
