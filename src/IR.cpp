#include "IR.h"
#include "IRMutator.h"
#include "Visitors.h"
#include "IRVisitor.h"

namespace Fuser{


const IntImm* IntImm::make(int64_t value){//Type t, int64_t value) {
    IntImm *node = new IntImm;
    node->value = value;
    return node;
}

Expr Add::make(Expr a, Expr b) {
  Add *node = new Add;
  node->a = std::move(a);
  node->b = std::move(b);
  return node;
}

}
