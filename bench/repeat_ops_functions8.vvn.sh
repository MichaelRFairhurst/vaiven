#!/bin/bash
cat <<EOF
fn ops of x is
  var x1 = x + 1;
  var x2 = x + 2;
  var x3 = x + 3;
  var x4 = x + 4;
  var x5 = x + 5;
  var x6 = x + 6;
  var x7 = x + 7;
  x + x - x * x / x;
  x1 + x1 - x1 * x1 / x1;
  x2 + x2 - x2 * x2 / x2;
  x3 + x3 - x3 * x3 / x3;
  x4 + x4 - x4 * x4 / x4;
  x5 + x5 - x5 * x5 / x5;
  x6 + x6 - x6 * x6 / x6;
  x7 + x7 - x7 * x7 / x7;
end
EOF

for i in `seq 1 6250`
do
  echo "ops(" $(($i * 8)) ");"
done

