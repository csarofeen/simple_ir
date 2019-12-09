#pragma once
#include "IR.h"
#include <memory>


namespace Fuser{

class IRMutator {
public:
    IRMutator();
    virtual ~IRMutator();

    virtual Expr mutate(const Expr &expr);
    
protected:
    // ExprNode<> and StmtNode<> are allowed to call visit (to implement mutate_expr/mutate_stmt())
    template<typename T>
    friend struct ExprNode;
    
    #define VISIT_DEF(T) \
    virtual Expr visit(const T *);

    VISIT_DEF(IntImm)
    VISIT_DEF(Add)
    VISIT_DEF(Sub)
    VISIT_DEF(Mul)
    VISIT_DEF(Div)
    VISIT_DEF(Variable)

};

}