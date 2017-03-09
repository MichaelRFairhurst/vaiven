#!/bin/bash
for i in `seq 1 500000`
do
  echo "fn func$i of x is x + x - x * x / x; end"
  echo "func$i($i);"
done


