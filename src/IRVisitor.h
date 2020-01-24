#pragma once
namespace Fuser{

template<typename T>
class ExprNode;

struct Expr;

class IntImm;
class Add;
class Sub;
class Mul;
class Div;
class Mod;
class LT;
class Set;
class Variable;
class JITTensor;
class Tensor;
class TensorAccessor;
class TensorDomain;
class Merge;
class Split;
class Reorder;
class Range;
class For;
class If;
class Attr;
class Thread;
class Block;
class Null;

class IRVisitor {
public:
    IRVisitor(){}
    virtual ~IRVisitor(){}

    // ExprNode<> and StmtNode<> are allowed to call visit (to implement accept())
    template<typename T>
    friend struct ExprNode;

    //Generic dispatch
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
    virtual void visit(const JITTensor *);
    virtual void visit(const Tensor *);
    virtual void visit(const Merge *);
    virtual void visit(const Split *);
    virtual void visit(const Reorder *);
    virtual void visit(const TensorDomain *);
    virtual void visit(const TensorAccessor *);
    virtual void visit(const For *);
    virtual void visit(const If *);
    virtual void visit(const Range *);
    virtual void visit(const Attr *);
    virtual void visit(const Thread *);
    virtual void visit(const Block *);
    virtual void visit(const Null *);

};


}
