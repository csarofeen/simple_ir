#include "IRVisitor.h"

namespace Fuser{

#define IRVISITOR_CASE(T)   \
    case(IRNodeType::T):        \
      visit(node->as<T>()); \
      break;

  void IRVisitor::visit(const Expr* node){
    switch(node->node_type()){
    IRVISITOR_CASE(Add)
    IRVISITOR_CASE(Sub)
    IRVISITOR_CASE(Mul)
    IRVISITOR_CASE(Div)
    IRVISITOR_CASE(Mod)
    IRVISITOR_CASE(Set)
    IRVISITOR_CASE(LT)
    IRVISITOR_CASE(Variable)
    IRVISITOR_CASE(IntImm)
    IRVISITOR_CASE(For)
    IRVISITOR_CASE(If)
    IRVISITOR_CASE(Attr)
    IRVISITOR_CASE(TensorAccessor)
    IRVISITOR_CASE(Tensor)
    IRVISITOR_CASE(Thread)
      
    }
  }


#define EMPTY_VISIT(T) \
void IRVisitor::visit(const T*) {}

EMPTY_VISIT(IntImm)
EMPTY_VISIT(Variable)
EMPTY_VISIT(Thread)

#define BINARY_OP_VISIT(T) \
void IRVisitor::visit(const T *op) { \
    op->a.accept(this); \
    op->b.accept(this); \
}

BINARY_OP_VISIT(Add)
BINARY_OP_VISIT(Sub)
BINARY_OP_VISIT(Mul)
BINARY_OP_VISIT(Div)
BINARY_OP_VISIT(Mod)
BINARY_OP_VISIT(LT)
BINARY_OP_VISIT(Set)

void IRVisitor::visit(const Tensor *op){

    for(const auto shape : op->shapes)
        shape.accept(this);

    for(const auto stride : op->strides)
        stride.accept(this);
}

void IRVisitor::visit(const TensorAccessor *op){
    op->tensor.accept(this);
    for(const auto var : op->indexers)
        var.accept(this);
}

void IRVisitor::visit(const For *op){
    op->loop_var.accept(this);
    op->min.accept(this);
    op->extent.accept(this);
    op->body.accept(this);
}

void IRVisitor::visit(const If *op){
    op->pred.accept(this);
    op->body.accept(this);
}

void IRVisitor::visit(const Attr *op){
    op->body.accept(this);
    op->value.accept(this);
}

}

