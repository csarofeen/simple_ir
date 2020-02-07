#pragma once
#include <memory>

namespace Fuser{

template<typename T>
class ExprNode;

struct Expr;

class IntImm;
class BinaryOp;
class Variable;
class Tensor;
class Range;
class JITTensor;
class TensorAccessor;
class TensorDomain;
class Merge;
class Split;
class Reorder;
class For;
class If;
class Attr;
class Thread;
class Block;
class Null;

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
    virtual Expr visit(const BinaryOp *);
    virtual Expr visit(const Variable *);
    virtual Expr visit(const JITTensor *);
    virtual Expr visit(const Tensor *);
    virtual Expr visit(const Range *);
    virtual Expr visit(const Split *);
    virtual Expr visit(const Merge *);
    virtual Expr visit(const Reorder *);
    virtual Expr visit(const TensorDomain *);
    virtual Expr visit(const TensorAccessor *);
    virtual Expr visit(const For *);
    virtual Expr visit(const If *);
    virtual Expr visit(const Attr *);
    virtual Expr visit(const Thread *);
    virtual Expr visit(const Block *);
    virtual Expr visit(const Null *);
};

}
