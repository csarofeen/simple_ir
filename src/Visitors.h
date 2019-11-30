#pragma once
#include "IRVisitor.h"
#include <memory>

class PrintVisitor: public IRVisitor
{

#define PRINTVISITOR_CASE(T) \
  case(IRType::T):           \
    visit(node->as<T>());    \
    break; 

  void visit(Node* node){
    switch(node->type){
      PRINTVISITOR_CASE(For)
      PRINTVISITOR_CASE(Range)
      PRINTVISITOR_CASE(Variable)
      PRINTVISITOR_CASE(Mul)
      PRINTVISITOR_CASE(Div)
      PRINTVISITOR_CASE(Add)
      PRINTVISITOR_CASE(Sub)
    }
  }
  
#define PRINTVISITOR_BIN_OP(OP, OP_STR) \
  virtual void visit(OP* node){         \
    std::cout<<"(";                     \
    visit(node->LHS.get());             \
    std::cout<<" "<<OP_STR<<" ";        \
    visit(node->RHS.get());             \
    std::cout<<")";                     \
  }

  PRINTVISITOR_BIN_OP(Div, "/")
  PRINTVISITOR_BIN_OP(Mul, "*")
  PRINTVISITOR_BIN_OP(Add, "+")
  PRINTVISITOR_BIN_OP(Sub, "-")

  void visit(For* node){
    std::cout<<"For( ";
    visit(node->loop_var.get()->as<Variable>());
    std::cout<<" in ";
    visit(node->range.get()->as<Range>());
    std::cout<<" )"<<std::endl;
  }

  void visit(Range* node){
    std::cout<<"[";
    visit(node->min.get()->as<Variable>());
    std::cout<<", ";
    visit(node->extent.get());
    std::cout<<"]";
  }
  
  void visit(Variable* node){
    std::cout << node->name;
  }
  
};
