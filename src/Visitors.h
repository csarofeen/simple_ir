#pragma once
#include <ostream>
#include "IRVisitor.h"

namespace Fuser{

class PrintVisitor : public IRVisitor {
std::ostream& os;

public:
PrintVisitor(std::ostream& os):os(os){}

  void visit(const Variable* op){
    os<<op->name;
  }

  void visit(const IntImm *val){
    os<<val->value;
  };

  #define BINARY_PRINT(TYPE, STRING) \
  void visit(const TYPE *op){ \
    os<<"( "; \
    IRVisitor::visit( (const Expr*) &(op->a) ); \
    os<<" "<<STRING<<" "; \
    IRVisitor::visit( (const Expr*) &(op->b) ); \
    os<<" )"<<std::endl; \
  }

  BINARY_PRINT(Add, "+")
  BINARY_PRINT(Sub, "-")
  BINARY_PRINT(Mul, "*")
  BINARY_PRINT(Div, "/")

};



}//namespace Fuser