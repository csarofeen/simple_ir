#pragma once
#include <iostream>
#include "IRVisitor.h"
#
namespace Fuser{

class PrintVisitor : public IRVisitor {
public:
    /** These methods should call 'include' on the children to only
     * visit them if they haven't been visited already. */
    // @{
    void visit(const IntImm *val){
      std::cout<<val->value;
    };

    #define BINARY_PRINT(TYPE, STRING) \
    void visit(const TYPE *op){ \
      std::cout<<"( "; \
      visit(op->a.as<IntImm>()); \
      std::cout<<" "<<STRING<<" "; \
      visit(op->b.as<IntImm>()); \
      std::cout<<" )"<<std::endl; \
    }

  BINARY_PRINT(Add, "+")
  BINARY_PRINT(Sub, "-")
  BINARY_PRINT(Mul, "*")
  BINARY_PRINT(Div, "/")

};



}//namespace Fuser