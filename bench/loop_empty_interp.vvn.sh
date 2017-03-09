#!/bin/bash
cat <<EOF
var x = 1;
for x < 50000000 do
  x = x + 1;
end
EOF
