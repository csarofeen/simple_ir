#pragma once
#include "IRVisitor.h"
#include <memory>

class PrintVisitor: public IRVisitor
{

#define CASE(T) case(IRType::T):		\
  visit(node->as<T>());				\
  break; 

  void visit(Node* node){
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
