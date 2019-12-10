#include "IRVisitor.h"

namespace Fuser{

#define IRVISITOR_CASE(T)   \
    case(IRNodeType::T):        \
      visit(node->as<T>()); \
      break;

  void IRVisitor::visit(const Expr* node){
    switch(node->node_type()){
      IRVISITOR_CASE(Mul)
      IRVISITOR_CASE(Div)
      IRVISITOR_CASE(Add)
      IRVISITOR_CASE(Sub)
      IRVISITOR_CASE(Variable)
      IRVISITOR_CASE(IntImm)
      IRVISITOR_CASE(Tensor)
    }
  }


#define EMPTY_VISIT(T) \
void IRVisitor::visit(const T *) {}

EMPTY_VISIT(IntImm)

void IRVisitor::visit(const Variable* var){
    std::cout<<"IRVisitor visit variable."<<std::endl;
}

#define BINARY_OP_VISIT(T) \
void IRVisitor::visit(const T *op) { \
    op->a.accept(this); \
    op->b.accept(this); \
}

BINARY_OP_VISIT(Add)
BINARY_OP_VISIT(Sub)
BINARY_OP_VISIT(Mul)
BINARY_OP_VISIT(Div)

void IRVisitor::visit(const Tensor *op){
    
    for(const auto shape : op->shapes)
        shape.accept(this);

    for(const auto stride : op->strides)
        stride.accept(this);
}

}

