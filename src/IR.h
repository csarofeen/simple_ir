#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>

#include "IntrusivePtr.h"

namespace Fuser{

class IRMutator;
class IRVisitor;

enum class IRNodeType {
    // Exprs, in order of strength
    Add,
    IntImm
};

/** The abstract base classes for a node in the Halide IR. */
struct IRNode {

    /** We use the visitor pattern to traverse IR nodes throughout the
     * compiler, so we have a virtual accept method which accepts
     * visitors.
     */
    virtual void accept(IRVisitor *v) const = 0;
    IRNode(IRNodeType t)
        : node_type(t) {
    }
    virtual ~IRNode() = default;

    /** These classes are all managed with intrusive reference
     * counting, so we also track a reference count. It's mutable
     * so that we can do reference counting even through const
     * references to IR nodes.
     */
    mutable RefCount ref_count;

    /** Each IR node subclass has a unique identifier. We can compare
     * these values to do runtime type identification. We don't
     * compile with rtti because that injects run-time type
     * identification stuff everywhere (and often breaks when linking
     * external libraries compiled without it), and we only want it
     * for IR nodes. One might want to put this value in the vtable,
     * but that adds another level of indirection, and for Exprs we
     * have 32 free bits in between the ref count and the Type
     * anyway, so this doesn't increase the memory footprint of an IR node.
     */
    IRNodeType node_type;
};


template<>
inline RefCount &ref_count<IRNode>(const IRNode *t) noexcept {
    return t->ref_count;
}

template<>
inline void destroy<IRNode>(const IRNode *t) {
    delete t;
}
class Expr; 

struct BaseExprNode : public IRNode {
    BaseExprNode(IRNodeType t)
        : IRNode(t) {
    }
    virtual Expr mutate_expr(IRMutator *v) const = 0;
    //Type type;
};

template<typename T>
struct ExprNode : public BaseExprNode {
    void accept(IRVisitor *v) const override;
    Expr mutate_expr(IRMutator *v) const override;
    ExprNode()
        : BaseExprNode(T::_node_type) {
    }
    virtual ~ExprNode() = default;
};

struct IRHandle : public IntrusivePtr<const IRNode> {
    IRHandle() = default;

    IRHandle(const IRNode *p)
        : IntrusivePtr<const IRNode>(p) {
    }

    /** Dispatch to the correct visitor method for this node. E.g. if
     * this node is actually an Add node, then this will call
     * IRVisitor::visit(const Add *) */
    void accept(IRVisitor *v) const {
        ptr->accept(v);
    }

    /** Downcast this ir node to its actual type (e.g. Add, or
     * Select). This returns nullptr if the node is not of the requested
     * type. Example usage:
     *
     * if (const Add *add = node->as<Add>()) {
     *   // This is an add node
     * }
     */
    template<typename T>
    const T *as() const {
        if (ptr && ptr->node_type == T::_node_type) {
            return (const T *)ptr;
        }
        return nullptr;
    }

    IRNodeType node_type() const {
        return ptr->node_type;
    }
};

struct Expr : public IRHandle {

  //Did not take the type system from Halide, it is there.

    /** Make an undefined expression */
    Expr() = default;

    /** Make an expression from a concrete expression node pointer (e.g. Add) */
    Expr(const BaseExprNode *n)
        : IRHandle(n) {
    }

        /** Override get() to return a BaseExprNode * instead of an IRNode * */
    const BaseExprNode *get() const {
        return (const BaseExprNode *)ptr;
    }

};

struct ExprCompare {
    bool operator()(const Expr &a, const Expr &b) const {
        return a.get() < b.get();
    }
};

struct IntImm : public ExprNode<IntImm> {
    int64_t value;

    static const IntImm *make(int64_t value){//Type t, int64_t value) {
        IntImm *node = new IntImm;
        node->value = value;
        return node;
    }

    static const IRNodeType _node_type = IRNodeType::IntImm;
};

/** The sum of two expressions */
struct Add : public ExprNode<Add> {
    Expr a, b;

    static Expr make(Expr a, Expr b) {
      Add *node = new Add;
      node->a = std::move(a);
      node->b = std::move(b);
      return node;
}

    static const IRNodeType _node_type = IRNodeType::Add;
};

}//Fuser namspace