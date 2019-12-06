#include "IR.h"
#include "IRMutator.h"
#include "Visitors.h"
#include "IRVisitor.h"

namespace Fuser{

template<>
void ExprNode<IntImm>::accept(IRVisitor *v) const {
    v->visit((const IntImm *)this);
}

template<>
void ExprNode<Add>::accept(IRVisitor *v) const {
    v->visit((const Add *)this);
}

template<>
Expr ExprNode<IntImm>::mutate_expr(IRMutator *v) const {
    return v->visit((const IntImm *)this);
}

template<>
Expr ExprNode<Add>::mutate_expr(IRMutator *v) const {
    return v->visit((const Add *)this);
}

}