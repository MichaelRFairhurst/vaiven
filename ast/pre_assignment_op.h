// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef VAIVEN_AST_HEADER_PRE_ASSIGNMENT_OP
#define VAIVEN_AST_HEADER_PRE_ASSIGNMENT_OP

namespace vaiven { namespace ast {

enum PreAssignmentOp {
  kPreAssignmentOpNone,
  kPreAssignmentOpAdd,
  kPreAssignmentOpSub,
  kPreAssignmentOpMul,
  kPreAssignmentOpDiv,
};

} }

#endif
