#pragma once
#include "IR.h"
#include <memory>

class IRVisitor
{
public:

#define IRVISITOR_CASE(T)   \
    case(IRType::T):        \
      visit(node->as<T>()); \
      break;

  virtual void visit(Node* node){
    switch(node->type){
      IRVISITOR_CASE(For)
      IRVISITOR_CASE(Range)
      IRVISITOR_CASE(Variable)
      IRVISITOR_CASE(Mul)
      IRVISITOR_CASE(Div)
      IRVISITOR_CASE(Add)
      IRVISITOR_CASE(Sub)
    }
  }
  
#define IRVISITOR_BIN_OP(OP)    \
  virtual void visit(OP* node){ \
    visit(node->LHS.get());     \
    visit(node->RHS.get());     \
  }

IRVISITOR_BIN_OP(Div)
IRVISITOR_BIN_OP(Mul)
IRVISITOR_BIN_OP(Add)
IRVISITOR_BIN_OP(Sub)

  virtual void visit(Variable* node){}
  
  virtual void visit(For* node){
    visit(node->range.get());
    visit(node->loop_var.get());
  }

  virtual void visit(Range* node){
    visit(node->min.get());
    visit(node->extent.get());
  }
  
};
