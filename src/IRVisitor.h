#pragma once
#include "IR.h"

namespace Fuser{

class IRVisitor {
public:
    IRVisitor(){}
    virtual ~IRVisitor(){}

    // ExprNode<> and StmtNode<> are allowed to call visit (to implement accept())
    template<typename T>
    friend struct ExprNode;

    //  To use this dispatch in a derived visitor you need to call:
    //  IRVisitor::visit( (const Expr *) op->some_Expr );
    virtual void visit(const Expr *);

    virtual void visit(const Add *);
    virtual void visit(const Sub *);
    virtual void visit(const Mul *);
    virtual void visit(const Div *);
    virtual void visit(const Set *);
    virtual void visit(const Mod *);
    virtual void visit(const LT *);
    virtual void visit(const IntImm *);
    virtual void visit(const Variable *);
    virtual void visit(const Tensor *);
    virtual void visit(const TensorAccessor *);
    virtual void visit(const For *);
    virtual void visit(const If *);
    virtual void visit(const Attr *);
    virtual void visit(const Thread *);


};


}