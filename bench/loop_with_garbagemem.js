function loop() {
  var x = 1;
  var a = [];
  while (x < 50000000) {
    x = x + 1
    // if it doesn't survive a collection its not even fair
    a[x % 5000000] = "foo" + "bar";
  }

  return a;
}

loop();
