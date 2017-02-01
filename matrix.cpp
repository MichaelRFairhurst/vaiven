#include "matrix.h"

namespace vaiven {

void Matrix::move(int col, int row) {
  cursor = row * cols + col;
}

StyledChar& Matrix::get(int col, int row) {
  return data[row * cols + col];
}

vector<StyledChar> Matrix::getMany(int col, int row, int count) {
  vector<StyledChar> result(count);
  for(int i = 0; i < count; ++i) {
    result[i] = data[row * cols + col + i];
  }
  return result;
}

void Matrix::write(StyledChar& theChar) {
  if (cursor >= cols * rows) {
    return;
  }
  data[cursor] = theChar;
}

void Matrix::print(StyledChar& theChar) {
  if (cursor >= cols * rows) {
    return;
  }
  data[cursor++] = theChar;
}

void Matrix::writeMany(vector<StyledChar> theChars) {
  for(int i = 0; i < theChars.size(); ++i) {
    if (cursor + i >= cols * rows) {
      return;
    }
    data[cursor + i] = theChars[i];
  }
}

void Matrix::printMany(vector<StyledChar> theChars) {
  for(vector<StyledChar>::iterator it = theChars.begin(); it != theChars.end(); ++it) {
    if (cursor >= cols * rows) {
      return;
    }
    data[cursor++] = *it;
  }
}

void Matrix::put(StyledChar& theChar, int col, int row) {
  if (col > cols || row > rows) {
    return;
  }
  data[row * cols + col] = theChar;
}

void Matrix::putMany(vector<StyledChar> theChars, int col, int row) {
  if (row > rows) {
    return;
  }

  for(int i = 0; i < theChars.size(); ++i) {
    if (col + i > cols) {
      return;
    }

    data[row * cols + col + i] = theChars[i];
  }
}

}
