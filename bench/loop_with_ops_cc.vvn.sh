#!/bin/bash

cat <<EOF
fn loop is
  var x = 1;
  for x < 50000000 do
    x + x - x * x / x;
    x = x + 1;
  end
end

loop();

EOF
