#include "IR.h"
#include "IRMutator.h"
#include "Visitors.h"
#include "IRVisitor.h"

namespace Fuser{

#define SIMPLE_ACCEPT(TYPE) \
template<> \
void ExprNode<TYPE>::accept(IRVisitor *v) const { \
    v->visit((const TYPE *)this); \
}

SIMPLE_ACCEPT(IntImm)
SIMPLE_ACCEPT(Add)
SIMPLE_ACCEPT(Sub)
SIMPLE_ACCEPT(Mul)
SIMPLE_ACCEPT(Div)
SIMPLE_ACCEPT(Variable)
SIMPLE_ACCEPT(Tensor)

#define SIMPLE_MUTATE_EXPR(TYPE) \
template<> \
Expr ExprNode<TYPE>::mutate_expr(IRMutator *v) const { \
    return v->visit((const TYPE *)this); \
}

SIMPLE_MUTATE_EXPR(IntImm)
SIMPLE_MUTATE_EXPR(Add)
SIMPLE_MUTATE_EXPR(Sub)
SIMPLE_MUTATE_EXPR(Mul)
SIMPLE_MUTATE_EXPR(Div)
SIMPLE_MUTATE_EXPR(Variable)
SIMPLE_MUTATE_EXPR(Tensor)

//For tensor names if one isn't provided, probably want the same for Variables.
int Tensor::tensor_name_count = 0;

}//Fuser namespace