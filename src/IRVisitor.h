#pragma once
#include "IR.h"
#include <memory>

class IRVisitor
{
public:
  virtual void visit(Variable* ir){}
  
  virtual void visit(For* ir){
    visit(ir->range.get());
    visit(ir->loop_var.get());
  }

  virtual void visit(Range* ir){
    visit(ir->min.get());
    visit(ir->extent.get());
  }
  
  virtual void visit(IR* ir){
  }
  
};
