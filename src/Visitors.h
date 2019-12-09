#pragma once
#include <iostream>
#include "IRVisitor.h"
#
namespace Fuser{

class PrintVisitor : public IRVisitor {
public:

  void visit(const Variable* op){
    std::cout<<op->name;
  }

  void visit(const IntImm *val){
    std::cout<<val->value;
  };

  #define BINARY_PRINT(TYPE, STRING) \
  void visit(const TYPE *op){ \
    std::cout<<"( "; \
    IRVisitor::visit( (const Expr*) &(op->a) ); \
    std::cout<<" "<<STRING<<" "; \
    IRVisitor::visit( (const Expr*) &(op->b) ); \
    std::cout<<" )"<<std::endl; \
  }

  BINARY_PRINT(Add, "+")
  BINARY_PRINT(Sub, "-")
  BINARY_PRINT(Mul, "*")
  BINARY_PRINT(Div, "/")

};



}//namespace Fuser