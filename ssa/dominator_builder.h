#ifndef VAIVEN_VISITOR_HEADER_SSA_DOMINATOR_BUILDER
#define VAIVEN_VISITOR_HEADER_SSA_DOMINATOR_BUILDER
#include "forward_visitor.h"

#include <map>
#include <set>

using std::map;
using std::set;

namespace vaiven { namespace ssa {

class DominatorBuilder {

  public:
  void firstBuild(Block& firstBlock);
  void rebuild(Block& firstBlock);

  void reset(Block& firstBlock);
};

}}

#endif
