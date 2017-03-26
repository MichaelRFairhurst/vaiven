function isEvenlyDivisible(val, div, lim) {
  if (div > lim) {
    return true;
  } else if (val % div != 0) { // is not evenly divisible
    return false;
  }

  return isEvenlyDivisible(val, div + 1, lim);
}

function test(lim) {
  var val = 10;
  while (!isEvenlyDivisible(val, 2, lim)) {
    val += 2;
  }

  return val;
}

console.log(test(1));
console.log(test(5));
console.log(test(10));
console.log(test(15));
console.log(test(20));
