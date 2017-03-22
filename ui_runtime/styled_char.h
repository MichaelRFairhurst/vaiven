#ifndef ENCASE_HEADER_STYLED_CHAR
#define ENCASE_HEADER_STYLED_CHAR

#include <string>
#include <vector>

using namespace std;

namespace vaiven {

// TODO use less memory
class StyledChar {
  public:
  // TODO support utf-8
  char theChar;
// TODO use less memory
  bool isBold;
  bool isUnderlined;
  bool isItalic;
  int colorCode;

  StyledChar(char theChar) :
      theChar(theChar), isBold(false), isUnderlined(false),
      isItalic(false), colorCode(0) {};

  StyledChar() :
      theChar(' '), isBold(false), isUnderlined(false),
      isItalic(false), colorCode(0) {};
};

vector<StyledChar> fromStr(const string& s);

}

#endif
