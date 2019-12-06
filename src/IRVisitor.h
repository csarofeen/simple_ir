#pragma once
#include "IR.h"

namespace Fuser{

class IRVisitor {
public:
    IRVisitor();
    virtual ~IRVisitor();

protected:
    // ExprNode<> and StmtNode<> are allowed to call visit (to implement accept())
    template<typename T>
    friend struct ExprNode;

    virtual void visit(const Add *);
    virtual void visit(const IntImm *);

};


}