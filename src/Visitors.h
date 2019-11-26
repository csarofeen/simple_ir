#pragma once
#include "IRVisitor.h"
#include <memory>

class PrintVisitor: public IRVisitor
{
  void visit(For* node)
    {
      std::cout<<"For(";
      visit(node->min.get()->as<Variable>());
      std::cout<<", ";
      visit(node->extent.get());
      std::cout<<")"<<std::endl;
    }

  void visit(Variable* node)
    {
      std::cout << node->name;
    }
  
};
