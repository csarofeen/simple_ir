#pragma once

namespace Fuser{

template<typename T>
class ExprNode;
class IntImm;
class Add;

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
