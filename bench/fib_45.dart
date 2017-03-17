int fib(int x) {
  if (x == 0) {
    return 0;
  } else if (x == 1) {
    return 1;
  } else {
    return fib(x - 1) + fib(x - 2);
  }
}

main() {
  print(fib(45));
}
