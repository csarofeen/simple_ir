#pragma once
#include "IR.h"
#include <memory>

class IRVisitor
{
public:
  virtual void visit(Variable* ir){}
  virtual void visit(For* ir){}
  virtual void visit(IR* ir){}
};
