#include "IR.h"
#include "IRVisitor.h"

namespace Fuser{

#define ACCEPT(T)		\
  void T::accept(IRVisitor &v){ \
    v.visit(this);		\
  }

ACCEPT(For)
ACCEPT(Variable)
ACCEPT(Node)
ACCEPT(Range)
ACCEPT(Mul)
ACCEPT(Div)
ACCEPT(Add)
ACCEPT(Sub)
ACCEPT(Tensor)

int Tensor::tensor_name_count = 0;
}