#include "styled_char.h"

namespace vaiven {

vector<StyledChar> fromStr(const string& s) {
  vector<StyledChar> result(s.size());
  for(int i = 0; i < s.size(); ++i) {
    result.at(i) = StyledChar(s[i]);
  }

  return result;
}

}
