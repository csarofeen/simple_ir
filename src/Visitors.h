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

  void visit(const Tensor *op){
    os<<op->name<<" [";
    for(const auto shape : op->shapes){
      IRVisitor::visit( (const Expr*) &shape);
      
      if(! shape.same_as(*(op->shapes.end()-1)))
        os<<", ";
    }
    os<<"] (";

    for(const auto stride : op->strides){
      IRVisitor::visit( (const Expr*) &stride);
      if(! stride.same_as(*(op->strides.end()-1)))
        os<<", ";
    }
    os<<") ";
  }


};



}//namespace Fuser