#pragma once
#include "IR.h"
#include <memory>

class IRVisitor
{
public:

#define CASE(T)					\
    case(IRType::T):				\
      visit(node->as<T>());			\
      break;

  virtual void visit(Node* node){
    switch(node->type){
      CASE(For)
      CASE(Range)
      CASE(Variable)
      CASE(Mul)
      CASE(Div)
      CASE(Add)
      CASE(Sub)
    }
  }
  
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
