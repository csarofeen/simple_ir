#pragma once
#include <memory>

namespace Fuser{

template<typename T>
class ExprNode;

class Expr;
class IntImm;
class Add;

class IRMutator {
public:
    IRMutator();
    virtual ~IRMutator();

    virtual Expr mutate(const Expr &expr);
    
protected:
    // ExprNode<> and StmtNode<> are allowed to call visit (to implement mutate_expr/mutate_stmt())
    template<typename T>
    friend struct ExprNode;
    
    virtual Expr visit(const IntImm *);
    virtual Expr visit(const Add *);
  
};

}
