#pragma once
#include "IRVisitor.h"
#include <memory>

class PrintVisitor: public IRVisitor
{
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
    visit(node->extent.get()->as<Variable>());
    std::cout<<"]";
  }
  
  void visit(Variable* node){
    std::cout << node->name;
  }
  
};
