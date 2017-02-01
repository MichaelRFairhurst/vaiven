#ifndef ENCASE_HEADER_MATRIX
#define ENCASE_HEADER_MATRIX

#include <vector>
#include "styled_char.h"

namespace vaiven {

using namespace std;

class Matrix {
  public:
  int cols;
  int rows;

  Matrix(int cols, int rows) : cols(cols), rows(rows), cursor(0), data(cols * rows) {
  }

  int cursor;

  vector<StyledChar> data;

  void move(int col, int row);
  StyledChar& get(int col, int row);
  vector<StyledChar> getMany(int col, int row, int count);
  void write(StyledChar& theChar);
  void print(StyledChar& theChar);
  void writeMany(vector<StyledChar> theChars);
  void printMany(vector<StyledChar> theChars);
  void writeManyDown(vector<StyledChar> theChars);
  void printManyDown(vector<StyledChar> theChars);
  void writeManyUp(vector<StyledChar> theChars);
  void printManyUp(vector<StyledChar> theChars);
  void writeManyLeft(vector<StyledChar> theChars);
  void printManyLeft(vector<StyledChar> theChars);
  void put(StyledChar& theChar, int col, int row);
  void putMany(vector<StyledChar> theChars, int col, int row);
};

}

#endif
