tokens = [
  'fn',
  'foo',
  'of',
  'x',
  ',',
  'y',
  ' ',
  'is',
  '\n',
  '\t',
  'ret',
  '1',
  '+',
  '2',
  '*',
  '3',
  '/',
  '4',
  '==',
  '5',
  '>',
  '6',
  '<',
  '(',
  'z',
  '=',
  '7',
  ')',
  ';',
  'for',
  'if',
  'do',
  '"foo"',
  'true',
  'false',
  'end'];

var out = "";
for (var x = 0; x < 5000; ++x) {
  var rand = Math.floor(Math.random() * tokens.length);
  out += tokens[rand];
  x++;
  if (Math.random() > .5) {
    out += " ";
  }
}

console.log(out);
