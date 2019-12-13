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
    void visit(const Add *op){
      std::cout<<"( ";
      op->a.accept(this);
      std::cout<<" + ";
      op->b.accept(this);
      std::cout<<" )"<<std::endl;
    }
};



} //namespace Fuser
