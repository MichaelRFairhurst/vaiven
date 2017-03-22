#include "matrix.h"

#include "stdio.h"

using namespace vaiven;

int main() {
  Matrix mainWindow(10, 3);
  mainWindow.printMany(fromStr("Hello world!"));
  mainWindow.move(0, 2);
  StyledChar a('a');
  StyledChar b('b');
  mainWindow.print(a);
  mainWindow.write(a);
  mainWindow.print(b);
  mainWindow.writeMany(fromStr("Hello world again!"));
  mainWindow.printMany(fromStr("overwrite all but"));
  mainWindow.cursor += 5;
  mainWindow.writeMany(fromStr("Goodbye world!"));
  
  for(int i = 0; i < mainWindow.cols * mainWindow.rows; ++i) {
    putchar(mainWindow.data[i].theChar);
    if (i % mainWindow.cols == mainWindow.cols - 1) {
      putchar('\n');
    }
  }
}
