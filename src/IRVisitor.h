#pragma once
#include "IR.h"
#include <memory>


namespace Fuser{
class IRVisitor
{
public:

#define IRVISITOR_CASE(T)   \
    case(IRType::T):        \
      visit(node->as<T>()); \
      break;

  virtual void visit(const Node* const node){
    switch(node->type){
      IRVISITOR_CASE(For)
      IRVISITOR_CASE(Range)
      IRVISITOR_CASE(Variable)
      IRVISITOR_CASE(Mul)
      IRVISITOR_CASE(Div)
      IRVISITOR_CASE(Add)
      IRVISITOR_CASE(Sub)
      IRVISITOR_CASE(Set)
      IRVISITOR_CASE(Tensor)
    }
  }
  
#define IRVISITOR_BIN_OP(OP)    \
  virtual void visit(const OP* const node){ \
    visit(node->LHS.get());     \
    visit(node->RHS.get());     \
  }

IRVISITOR_BIN_OP(Div)
IRVISITOR_BIN_OP(Mul)
IRVISITOR_BIN_OP(Add)
IRVISITOR_BIN_OP(Sub)
IRVISITOR_BIN_OP(Set)

  virtual void visit(const Variable* const node){}
  
  virtual void visit(const For* const node){
    visit(node->range.get());
    visit(node->loop_var.get());
  }

  virtual void visit(const Range* const node){
    visit(node->min.get());
    visit(node->extent.get());
  }
  
  virtual void visit(const Tensor* const node){
    for(int i=0; i<node->ndims; i++){
      visit(node->shape(i).get());
      visit(node->stride(i).get());
    }

  }

};
}